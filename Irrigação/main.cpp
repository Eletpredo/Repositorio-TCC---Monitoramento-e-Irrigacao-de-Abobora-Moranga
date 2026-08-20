#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define RELE 23          // Relé ligado no GPIO 23
#define SENSOR_SOLO 34   // Pino ADC do ESP32 (somente leitura)

#define DHTPIN 26        // Pino digital do DHT11
#define DHTTYPE DHT11

// VALORES DE CALIBRAÇÃO DO SENSOR CAPACITIVO (ESP32 - ADC 12 bits)
// Faça o teste no monitor serial para calibrar com o seu sensor:
const int VALOR_SECO = 3000;  // Valor com o sensor totalmente no AR (0% de umidade)
const int VALOR_UMIDO = 1350; // Valor com o sensor submerso em ÁGUA (100% de umidade)

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin(21, 22);    // SDA = 21, SCL = 22
  
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  dht.begin();

  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, HIGH); // Relé desligado por padrão (lógica invertida comum)
}

void loop() {
  // Leituras do DHT11 (Ar)
  float umidade_ar = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Leitura do Sensor Capacitivo (Solo)
  int leituraBruta = analogRead(SENSOR_SOLO);
  
  // Converte a leitura analógica invertida para porcentagem de 0 a 100%
  int umidadeSolo = map(leituraBruta, VALOR_SECO, VALOR_UMIDO, 0, 100);
  umidadeSolo = constrain(umidadeSolo, 0, 100); // Garante que fique entre 0% e 100%

  // Exibição no Monitor Serial
  Serial.print(F("Bruto Solo: "));
  Serial.print(leituraBruta);
  Serial.print(F(" | Umidade Solo: "));
  Serial.print(umidadeSolo);
  Serial.print(F("% | Umidade Ar: "));
  Serial.print(isnan(umidade_ar) ? 0 : umidade_ar);
  Serial.print(F("% | Temp: "));
  Serial.print(isnan(temperatura) ? 0 : temperatura);
  Serial.println(F("°C"));

  // --- TELA 1: Umidade do Ar ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Umidade do ar:");
  lcd.setCursor(0, 1);
  if (isnan(umidade_ar)) {
    lcd.print("Erro DHT11");
  } else {
    lcd.print(umidade_ar, 1);
    lcd.print("%");
  }
  delay(2500);

  // --- TELA 2: Temperatura ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0, 1);
  if (isnan(temperatura)) {
    lcd.print("Erro DHT11");
  } else {
    lcd.print(temperatura, 1);
    lcd.print(" C");
  }
  delay(2500);

  // --- TELA 3: Status do Solo e Ação do Relé ---
  lcd.clear();
  lcd.setCursor(0, 0);

  if (umidadeSolo < 30) { 
    // Solo Seco -> Liga a irrigação
    lcd.print("Solo Seco :(");
    lcd.setCursor(0, 1);
    lcd.print("Umidade: ");
    lcd.print(umidadeSolo);
    lcd.print("%");

    digitalWrite(RELE, LOW);  // Liga o relé
    delay(3000);               // Tempo de irrigação
    digitalWrite(RELE, HIGH); // Desliga o relé

  } else if (umidadeSolo >= 30 && umidadeSolo <= 70) {
    // Solo com umidade ideal
    lcd.print("Umidade Boa :D");
    lcd.setCursor(0, 1);
    lcd.print("Umidade: ");
    lcd.print(umidadeSolo);
    lcd.print("%");

    digitalWrite(RELE, HIGH); // Relé desligado
    delay(3000);

  } else {
    // Solo muito úmido / Encharcado
    lcd.print("Umidade Alta :o");
    lcd.setCursor(0, 1);
    lcd.print("Umidade: ");
    lcd.print(umidadeSolo);
    lcd.print("%");

    digitalWrite(RELE, HIGH); // Relé desligado
    delay(3000);
  }

  delay(1000);
}

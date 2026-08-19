#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define RELE 23          // ESP32 não tem pino D3 como Arduino. Ajustei para GPIO 23.
#define SENSOR 34        // Pino ADC do ESP32 (somente leitura)

#define DHTPIN 26     // Pino digital onde o sensor está conectado
#define DHTTYPE DHT11 // Define o modelo do sensor

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin(21, 22);    // SDA = 21, SCL = 22 (padrão do ESP32)
  
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

dht.begin();

  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, HIGH); // relé desligado (depende do módulo)
}

void loop() {
  float umidade_ar = dht.readHumidity();
  float temperatura = dht.readTemperature(); // Em Celsius
  
  int umidade = analogRead(SENSOR); // ESP32: 0 a 4095
  Serial.println(umidade);

  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.print(F("Umidade: "));
  Serial.print(umidade_ar);
  Serial.print(F("%  |  Temperatura: "));
  Serial.print(temperatura);
  Serial.println(F("°C"));

  // Ajuste dos limites (antes era 0–1023, agora é 0–4095)
  if (umidade <= 800) {  // equivalente a ~200 do Arduino
   lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Umidade do ar:");
     lcd.setCursor(0, 1);
     lcd.print(umidade_ar);
    delay(3000);
     lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Temperatura em C");
     lcd.setCursor(0, 1);
     lcd.print(temperatura);
     delay(3000);
    lcd.print("Umidade Acima :o");
    delay(5000);
    digitalWrite(RELE, LOW);

  } else if (umidade >= 800 && umidade <= 3600) { // equivalente 200–900
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Umidade do ar:");
     lcd.setCursor(0, 1);
     lcd.print(umidade_ar);
    delay(3000);
     lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Temperatura em C");
     lcd.setCursor(0, 1);
     lcd.print(temperatura);
     delay(3000);
    lcd.print("Umidade Boa :D");
    delay(5000);
    digitalWrite(RELE, LOW);

  } else {
    if (umidade >= 3600) { // equivalente a 900+
lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Umidade do ar:");
     lcd.setCursor(0, 1);
     lcd.print(umidade_ar);
    delay(3000);
     lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Temperatura em C");
     lcd.setCursor(0, 1);
     lcd.print(temperatura);
     delay(3000);
    lcd.print("Solo Seco :(");
    delay(5000);
      digitalWrite(RELE, HIGH);
      delay(1500);
      digitalWrite(RELE, LOW);
    }
  }

  delay(1000);
}
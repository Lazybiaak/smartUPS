#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

#define DHTPIN 13
#define DHTTYPE DHT11
#define STATUS_LED 2

DHT dht(DHTPIN, DHTTYPE);
SFE_MAX1704X fuelGauge;

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin();
  if (!fuelGauge.begin(Wire)) {
    Serial.println("MAX17048 not detected.");
  }

  pinMode(STATUS_LED, OUTPUT);
}

void loop() {
  float temperature = dht.readTemperature();
  float voltage = fuelGauge.getVoltage();

  Serial.printf("Temp: %.1f°C | Battery: %.2f V\n", temperature, voltage);
  digitalWrite(STATUS_LED, temperature > 30 ? HIGH : LOW);
  delay(2000);
}

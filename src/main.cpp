#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <RemoteOTA.h>

// Pin Definitions
#define DHTPIN     13
#define DHTTYPE    DHT11
#define LED1_PIN   4
#define LED2_PIN   5
#define LED3_PIN   6
#define FAN_PIN    7

DHT dht(DHTPIN, DHTTYPE);
SFE_MAX1704X fuelGauge;
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Adjust these for your GitHub Pages links
RemoteOTA ota(
    "0.0.1",
    "https://lazybiaak.github.io/smartUPS/ota/version.txt",
    "https://lazybiaak.github.io/smartUPS/ota/firmware.bin"
);


void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin();
  ota.begin(ssid, password);
  if (!fuelGauge.begin(Wire)) {
    Serial.println("MAX17048 not detected.");
  }

  // Initialize output pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  ota.check();  // Only check once on boot
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  float volt = fuelGauge.getVoltage();

  Serial.printf("Temp: %.1f°C | Humidity: %.1f%% | Voltage: %.2fV\n", temp, hum, volt);

  // LED1 = Overheat warning (Temp > 30°C)
  digitalWrite(LED1_PIN, temp > 30 ? HIGH : LOW);

  // LED2 = Battery low warning (Voltage < 3.5V)
  digitalWrite(LED2_PIN, volt < 3.5 ? HIGH : LOW);

  // LED3 = System OK (blinks every 0.5s)
  digitalWrite(LED3_PIN, millis() / 500 % 2 == 0 ? HIGH : LOW);

  // FAN control logic (turn on if temp > 35°C)
  digitalWrite(FAN_PIN, temp > 35 ? HIGH : LOW);

  delay(500);
}

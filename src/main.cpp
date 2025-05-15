// ============================
// Smart UPS Firmware
// by lazybiaak
// github.com/lazybiaak/SmartUPS
// ============================

#define BLYNK_TEMPLATE_ID "TMPL6T0AuvhpP"
#define BLYNK_TEMPLATE_NAME "Smart Cradle"
#define BLYNK_AUTH_TOKEN "XgpNxiG07kYwcghBjQtaT3HPPyYY1AQM"

#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <RemoteOTA.h>
#include <SmartBlynk.h>

// ==== Pin Definitions ====
constexpr uint8_t DHTPIN = 13;
constexpr uint8_t DHTTYPE = DHT11;
constexpr uint8_t LED1_PIN = 4;
constexpr uint8_t LED2_PIN = 5;
constexpr uint8_t LED3_PIN = 6;
constexpr uint8_t FAN_PIN  = 7;

// ==== Global Objects ====
DHT dht(DHTPIN, DHTTYPE);
SFE_MAX1704X fuelGauge;

const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

RemoteOTA ota(
    "0.0.1",
    "https://lazybiaak.github.io/smartUPS/ota/version.txt",
    "https://lazybiaak.github.io/smartUPS/ota/firmware.bin"
);

SmartBlynk blynk(BLYNK_AUTH_TOKEN, ssid, password, &dht, &fuelGauge, FAN_PIN, LED1_PIN);

// ==== Initialization ====
void initializeHardware() {
  dht.begin();
  Wire.begin();

  if (!fuelGauge.begin(Wire)) {
    Serial.println("MAX17048 not detected.");
  }

  for (uint8_t pin : {LED1_PIN, LED2_PIN, LED3_PIN, FAN_PIN}) {
    pinMode(pin, OUTPUT);
  }
}

void updateIndicators(float temp, float volt) {
  digitalWrite(LED1_PIN, temp > 30);              // Overheat
  digitalWrite(LED2_PIN, volt < 3.5);             // Low Battery
  digitalWrite(LED3_PIN, millis() / 500 % 2);     // System Heartbeat
  digitalWrite(FAN_PIN, temp > 35);               // Fan Control
}

// ==== Arduino Setup ====
void setup() {
  Serial.begin(115200);
  initializeHardware();

  ota.begin(ssid, password);
  ota.check();  // OTA check once at boot

  blynk.begin();
}

// ==== Main Loop ====
void loop() {
  Blynk.run();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  float volt = fuelGauge.getVoltage();

  Serial.printf("Temp: %.1f°C | Humidity: %.1f%% | Voltage: %.2fV\n", temp, hum, volt);

  updateIndicators(temp, volt);
  blynk.update();

  delay(500);
}

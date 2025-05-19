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
#include <BlynkSimpleEsp32.h>

// ==== Pin Definitions ====
constexpr uint8_t DHTPIN = 13;
constexpr uint8_t DHTTYPE = DHT11;
constexpr uint8_t LED1_PIN = 4;
constexpr uint8_t LED2_PIN = 5;
constexpr uint8_t LED3_PIN = 6;
constexpr uint8_t FAN_PIN  = 7;

// ==== Config ====
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";
#define VOLTAGE_LOW_THRESHOLD 3.4

// ==== Global Objects ====
DHT dht(DHTPIN, DHTTYPE);
SFE_MAX1704X fuelGauge;
RemoteOTA ota(
  "0.0.1",
  "https://lazybiaak.github.io/smartUPS/ota/version.txt",
  "https://lazybiaak.github.io/smartUPS/ota/firmware.bin"
);

// ==== Blynk State ====
bool fanOverride = false;
bool lowBatteryNotified = false;

// ==== Blynk Virtual Pins ====
#define VPIN_TEMP  V0
#define VPIN_VOLT  V1
#define VPIN_FAN   V2
#define VPIN_OVERHEAT V3
#define VPIN_OVERRIDE V5

// ==== Fan Override Control ====
BLYNK_WRITE(VPIN_OVERRIDE) {
  fanOverride = param.asInt();
}

// ==== Initialization ====
void initializeHardware() {
  dht.begin();
  Wire.begin();

  if (!fuelGauge.begin(Wire)) {
    Serial.println("⚠️  MAX17048 not detected.");
  }

  for (uint8_t pin : {LED1_PIN, LED2_PIN, LED3_PIN, FAN_PIN}) {
    pinMode(pin, OUTPUT);
  }
}

void updateIndicators(float temp, float volt) {
  bool fanState = fanOverride || temp > 35;
  bool overheat = temp > 30;

  digitalWrite(LED1_PIN, overheat);              // Overheat LED
  digitalWrite(LED2_PIN, volt < 3.5);            // Low Battery LED
  digitalWrite(LED3_PIN, millis() / 500 % 2);    // System heartbeat
  digitalWrite(FAN_PIN, fanState);               // Fan control

  Serial.printf("🌀 Fan: %s\n", fanState ? "ON" : "OFF");

  // Push to Blynk
  Blynk.virtualWrite(VPIN_TEMP, temp);
  Blynk.virtualWrite(VPIN_VOLT, volt);
  Blynk.virtualWrite(VPIN_FAN, fanState);
  Blynk.virtualWrite(VPIN_OVERHEAT, overheat);
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  initializeHardware();

  ota.begin(ssid, password);
  ota.check();  // Check OTA on boot

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

// ==== Loop ====
void loop() {
  Blynk.run();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  float volt = fuelGauge.getVoltage();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("⚠️  DHT read failed.");
    delay(500);
    return;
  }

  Serial.printf("🌡️ Temp: %.1f°C | 💧 Hum: %.1f%% | 🔋 Volt: %.2fV\n", temp, hum, volt);

  // Battery Alert
  if (volt < VOLTAGE_LOW_THRESHOLD && !lowBatteryNotified) {
    Blynk.logEvent("low_battery", "Battery voltage is low!");
    lowBatteryNotified = true;
  } else if (volt >= VOLTAGE_LOW_THRESHOLD) {
    lowBatteryNotified = false;
  }

  updateIndicators(temp, volt);

  delay(500);
}

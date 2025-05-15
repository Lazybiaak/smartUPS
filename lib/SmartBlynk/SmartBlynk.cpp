#include "SmartBlynk.h"

SmartBlynk::SmartBlynk(const char* auth, const char* ssid, const char* pass,
                       DHT* dhtSensor, SFE_MAX1704X* batterySensor,
                       uint8_t fanPin, uint8_t ledPin)
  : _auth(auth), _ssid(ssid), _pass(pass),
    _dht(dhtSensor), _battery(batterySensor),
    _fanPin(fanPin), _ledPin(ledPin) {}

void SmartBlynk::begin() {
    Blynk.begin(_auth, _ssid, _pass);
}

void SmartBlynk::update() {
    float temperature = _dht->readTemperature();
    float voltage = _battery->getVoltage();
    bool fanState = digitalRead(_fanPin);
    bool ledState = digitalRead(_ledPin);

    // Push data to Blynk dashboard
    Blynk.virtualWrite(VPIN_TEMP, temperature);
    Blynk.virtualWrite(VPIN_VOLT, voltage);
    Blynk.virtualWrite(VPIN_FAN, fanState);
    Blynk.virtualWrite(VPIN_LED, ledState);
}

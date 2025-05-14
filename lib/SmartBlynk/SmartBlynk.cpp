#include "SmartBlynk.h"

SmartBlynk::SmartBlynk(const char* auth, const char* ssid, const char* pass,
                       DHT* dhtSensor, SFE_MAX1704X* batterySensor,
                       int fanPin, int ledPin)
  : _auth(auth), _ssid(ssid), _pass(pass),
    _dht(dhtSensor), _battery(batterySensor),
    _fanPin(fanPin), _ledPin(ledPin) {}

void SmartBlynk::begin() {
    Blynk.begin(_auth, _ssid, _pass);
    pinMode(_fanPin, OUTPUT);
    pinMode(_ledPin, OUTPUT);
}

void SmartBlynk::update() {
    float temp = _dht->readTemperature();
    float volt = _battery->getVoltage();
    bool fanState = digitalRead(_fanPin);
    bool ledState = digitalRead(_ledPin);

    Blynk.virtualWrite(V0, temp);
    Blynk.virtualWrite(V1, volt);
    Blynk.virtualWrite(V2, fanState);
    Blynk.virtualWrite(V3, ledState);
}

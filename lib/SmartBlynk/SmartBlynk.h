#ifndef SMART_BLYNK_H
#define SMART_BLYNK_H

#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

class SmartBlynk {
public:
    SmartBlynk(const char* auth, const char* ssid, const char* pass,
               DHT* dhtSensor, SFE_MAX1704X* batterySensor,
               int fanPin, int ledPin);

    void begin();
    void update(); // Push sensor data to Blynk

private:
    const char* _auth;
    const char* _ssid;
    const char* _pass;

    DHT* _dht;
    SFE_MAX1704X* _battery;
    int _fanPin;
    int _ledPin;
};

#endif

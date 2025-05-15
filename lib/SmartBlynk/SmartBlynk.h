#ifndef SMART_BLYNK_H
#define SMART_BLYNK_H

#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

// Virtual pin mappings (adjust as needed)
#define VPIN_TEMP     V0
#define VPIN_VOLT     V1
#define VPIN_FAN      V2
#define VPIN_LED      V3

class SmartBlynk {
public:
    SmartBlynk(const char* auth, const char* ssid, const char* pass,
               DHT* dhtSensor, SFE_MAX1704X* batterySensor,
               uint8_t fanPin, uint8_t ledPin);

    void begin();
    void update(); // Push sensor data to Blynk

private:
    const char* _auth;
    const char* _ssid;
    const char* _pass;

    DHT* _dht;
    SFE_MAX1704X* _battery;
    uint8_t _fanPin;
    uint8_t _ledPin;
};

#endif

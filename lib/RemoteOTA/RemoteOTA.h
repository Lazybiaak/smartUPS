#ifndef REMOTE_OTA_H
#define REMOTE_OTA_H

#include <WiFi.h>

class RemoteOTA {
public:
    RemoteOTA(const char* currentVersion,
              const char* versionURL,
              const char* firmwareURL);

    void begin(const char* ssid, const char* password);
    void check();

private:
    const char* _ssid;
    const char* _password;
    const char* _currentVersion;
    const char* _versionURL;
    const char* _firmwareURL;
};

#endif

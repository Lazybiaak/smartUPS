#include "RemoteOTA.h"
#include <HTTPClient.h>
#include <Update.h>

RemoteOTA::RemoteOTA(const char* currentVersion,
                     const char* versionURL,
                     const char* firmwareURL)
    : _currentVersion(currentVersion),
      _versionURL(versionURL),
      _firmwareURL(firmwareURL) {}

void RemoteOTA::begin(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;

    WiFi.begin(_ssid, _password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void RemoteOTA::check() {
    HTTPClient http;
    http.begin(_versionURL);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String newVersion = http.getString();
        newVersion.trim();
        Serial.printf("Available version: %s\n", newVersion.c_str());

        if (newVersion != _currentVersion) {
            Serial.println("New version found! Starting OTA...");
            http.end();

            http.begin(_firmwareURL);
            int code = http.GET();

            if (code == 200) {
                int len = http.getSize();
                WiFiClient* stream = http.getStreamPtr();

                if (!Update.begin(len)) {
                    Serial.println("Update.begin() failed");
                    return;
                }

                size_t written = Update.writeStream(*stream);
                if (written == len) {
                    Serial.println("Firmware written successfully");
                } else {
                    Serial.printf("Written only %d/%d bytes\n", (int)written, len);
                }

                if (Update.end()) {
                    if (Update.isFinished()) {
                        Serial.println("OTA complete. Rebooting...");
                        ESP.restart();
                    } else {
                        Serial.println("Update not finished.");
                    }
                } else {
                    Serial.printf("Update error: %s\n", Update.errorString());
                }
            } else {
                Serial.printf("Firmware download failed: %d\n", code);
            }
        } else {
            Serial.println("Firmware is up to date.");
        }
    } else {
        Serial.printf("Version check failed: %d\n", httpCode);
    }

    http.end();
}

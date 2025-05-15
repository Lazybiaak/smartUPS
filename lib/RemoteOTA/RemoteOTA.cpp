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
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - start > 15000) {  // timeout after 15s
            Serial.println("\nWiFi connection failed.");
            return;
        }
    }

    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void RemoteOTA::check(bool forceUpdate) {
    String remoteVersion = fetchRemoteVersion();

    if (remoteVersion.isEmpty()) {
        Serial.println("⚠️  Version check failed.");
        return;
    }

    Serial.printf("Current: %s | Remote: %s\n", _currentVersion, remoteVersion.c_str());

    if (remoteVersion != _currentVersion || forceUpdate) {
        Serial.println("🔁 Starting OTA update...");
        if (downloadAndUpdate()) {
            Serial.println("✅ OTA update successful. Restarting...");
            ESP.restart();
        } else {
            Serial.println("❌ OTA update failed.");
        }
    } else {
        Serial.println("✅ Firmware is up-to-date.");
    }
}

String RemoteOTA::fetchRemoteVersion() {
    HTTPClient http;
    http.begin(_versionURL);
    int code = http.GET();

    if (code == 200) {
        String version = http.getString();
        version.trim();
        http.end();
        return version;
    }

    Serial.printf("HTTP error while checking version: %d\n", code);
    http.end();
    return "";
}

bool RemoteOTA::downloadAndUpdate() {
    HTTPClient http;
    http.begin(_firmwareURL);
    int code = http.GET();

    if (code != 200) {
        Serial.printf("Failed to download firmware: HTTP %d\n", code);
        http.end();
        return false;
    }

    int len = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (!Update.begin(len)) {
        Serial.println("Update.begin() failed.");
        http.end();
        return false;
    }

    size_t written = Update.writeStream(*stream);
    if (written != len) {
        Serial.printf("Only %u of %u bytes written\n", (unsigned)written, len);
        http.end();
        return false;
    }

    if (!Update.end()) {
        Serial.printf("Update error: %s\n", Update.errorString());
        http.end();
        return false;
    }

    return Update.isFinished();
}

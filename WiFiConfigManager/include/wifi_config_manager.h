#ifndef WIFI_CONFIG_MANAGER_H
#define WIFI_CONFIG_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiConfigManager {
public:
    WiFiConfigManager(const String& defaultSSID, const String& defaultPassword, bool shouldClearPreferences);
    void begin();
    bool isConnected();
private:
    const String defaultSSID;
    const String defaultPassword;
    const bool shouldClearPreferences;
    void attemptWiFiConnections();
    void handleWiFiEvent(WiFiEvent_t event);
    bool connectToWiFi(const String& ssid, const String& password);
    void startSmartConfig();
    void saveCredentials(const String& ssid, const String& password);
    void loadCredentials(String& ssid, String& password);
    void clearPreferences();
};

#endif /* SMARTCONFIG_LIB_H */
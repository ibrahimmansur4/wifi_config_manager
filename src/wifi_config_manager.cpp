#include "wifi_config_manager.h"
#include <Preferences.h>
#include <nvs_flash.h>

#define SMART_CONFIG_TIMEOUT 60000 // 1 minute in milliseconds

Preferences preferences;

WiFiConfigManager::WiFiConfigManager(const String& defaultSSID, const String& defaultPassword, bool shouldClearPrefs)
    : defaultSSID(defaultSSID), defaultPassword(defaultPassword), shouldClearPreferences(shouldClearPrefs) {}

void WiFiConfigManager::begin() {
    WiFi.mode(WIFI_AP_STA);
    clearPreferences();
    attemptWiFiConnections();
}


bool WiFiConfigManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiConfigManager::attemptWiFiConnections() {
    String storedSSID;
    String storedPassword;

    loadCredentials(storedSSID, storedPassword);

    if (!connectToWiFi(storedSSID, storedPassword)) {
        Serial.println("Stored credentials failed, entering SmartConfig.");
        startSmartConfig();

        unsigned long startTime = millis();
        while (!WiFi.smartConfigDone() && millis() - startTime < SMART_CONFIG_TIMEOUT) {
            delay(500);
        }

        if (WiFi.smartConfigDone()) {
            Serial.println("\nSmartConfig Success.");
            while (!isConnected()) {
                delay(500);
                Serial.print("+");
            }
            saveCredentials(WiFi.SSID(), WiFi.psk());
        } else {
            Serial.println("\nSmartConfig Failed/Timeout. Attempting default credentials.");
            if (!connectToWiFi(defaultSSID, defaultPassword)) {
                Serial.println("Failed to connect using default credentials.");
            }
        }
    }
}

bool WiFiConfigManager::connectToWiFi(const String& ssid, const String& password) {
    if (ssid.length() == 0 || password.length() == 0) {
        Serial.println("SSID or password is empty.");
        return false;
    }

    WiFi.disconnect(true);
    delay(1000); // Wait for WiFi disconnect
    Serial.println("Attempting to connect to WiFi network: " + ssid);

    WiFi.begin(ssid.c_str(), password.c_str());
    for (int attempts = 0; attempts < 20; attempts++) {
        if (isConnected()) {
            Serial.println("Connected to WiFi!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            return true;
        }
        delay(500);
    }
    Serial.println("Failed to connect!");
    return false;
}

void WiFiConfigManager::startSmartConfig() {
    Serial.println("Starting SmartConfig...");
    WiFi.beginSmartConfig();
}

void WiFiConfigManager::saveCredentials(const String& ssid, const String& password) {
    Serial.println("Saving WiFi credentials to Preferences.");
    preferences.begin("credentials", false); // Open the preferences storage
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();
}

void WiFiConfigManager::loadCredentials(String& ssid, String& password) {
    preferences.begin("credentials", false); // Open the preferences storage
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end();
}

void WiFiConfigManager::clearPreferences() {
    if (shouldClearPreferences)
    {
        nvs_flash_erase(); // erase the NVS partition and...
        nvs_flash_init(); // initialize the NVS partition.
    }
    else
    {
        preferences.begin("credentials", false); // Keep existing preferences
        preferences.end();
    }
}

void WiFiConfigManager::handleWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to AP successfully!");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            Serial.println("Trying to Reconnect");
            WiFi.begin();
            break;
    }
}
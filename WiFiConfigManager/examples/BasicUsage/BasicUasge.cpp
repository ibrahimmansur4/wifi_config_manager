#include "wifi_config_manager.h"

                      //////This part is for PREFERENCES.h library////////////
const String DEFAULT_SSID = "Default_SSID";
const String DEFAULT_PASSWORD = "Default_Password";
const bool SHOULD_CLEAR_PREFERENCES = true; // Set to true to clear preferences

WiFiConfigManager wifiManager(DEFAULT_SSID, DEFAULT_PASSWORD, SHOULD_CLEAR_PREFERENCES);

void setup() {
    Serial.begin(115200);
    wifiManager.begin();
}

void loop() {
    //empty
}
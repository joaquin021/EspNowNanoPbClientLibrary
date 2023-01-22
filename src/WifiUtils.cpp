#include "WifiUtils.hpp"

void connectToWiFi(WiFiConfig wifiConfig) {
    logDebugln("Trying to connect WiFi.");
    WiFi.config(wifiConfig.getSaticIp(), wifiConfig.getGateway(), wifiConfig.getSubnet());
    WiFi.begin(wifiConfig.getSsid(), wifiConfig.getPassword());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        logTrace(".");
    }
    logDebugln("");
    logInfoln("WiFi connected.");
    logInfo("IP address: ");
    logInfoln(WiFi.localIP());
    logInfoln("Mac: " + WiFi.macAddress());
}

void setupWiFiForEspNow() {
    WiFi.mode(WIFI_STA);
    logInfoln("Mac: " + WiFi.macAddress());
}
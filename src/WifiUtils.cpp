#include "WifiUtils.hpp"

void connectToWiFi(WiFiConfig wifiConfig) {
    logDebugln("WifiUtils\t\tTrying to connect WiFi.");
    WiFi.config(wifiConfig.getSaticIp(), wifiConfig.getGateway(), wifiConfig.getSubnet());
    WiFi.begin(wifiConfig.getSsid(), wifiConfig.getPassword());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        logTrace(".");
    }
    logDebugln("");
    logInfoln("WifiUtils\t\tWiFi connected.");
    logInfo("WifiUtils\t\tIP address: ");
    logInfoln(WiFi.localIP());
    logInfoln("WifiUtils\t\tMac: " + WiFi.macAddress());
}

void setupWiFiForEspNow() {
    WiFi.mode(WIFI_STA);
    logInfoln("WifiUtils\t\tMac: " + WiFi.macAddress());
}
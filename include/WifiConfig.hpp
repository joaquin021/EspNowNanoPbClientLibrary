#ifndef __WIFI_CONFIG_HPP
#define __WIFI_CONFIG_HPP

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

class WiFiConfig {
   private:
    IPAddress staticIP;
    IPAddress gateway;
    IPAddress subnet;
    const char* ssid;
    const char* password;

   public:
    WiFiConfig();
    WiFiConfig(IPAddress staticIP, IPAddress gateway, IPAddress subnet, const char* ssid, const char* password);
    IPAddress getSaticIp();
    IPAddress getGateway();
    IPAddress getSubnet();
    const char* getSsid();
    const char* getPassword();
};

#endif
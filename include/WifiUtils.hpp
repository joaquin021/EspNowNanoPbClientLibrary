#ifndef __WIFI_UTILS_HPP
#define __WIFI_UTILS_HPP

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include "Commons.hpp"
#include "WifiConfig.hpp"

void connectToWiFi(WiFiConfig wifiConfig);
void setupWiFiForEspNow();

#endif
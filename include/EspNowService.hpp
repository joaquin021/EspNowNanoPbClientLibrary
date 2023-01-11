#ifndef __ESP_NOW_SERVICE_HPP
#define __ESP_NOW_SERVICE_HPP

#include <Arduino.h>
#ifdef ESP32
#include <esp_now.h>
#else
#include <espnow.h>
#endif

#include "Commons.hpp"

#ifdef ESP32
void espNowSendCallBackDummy(const uint8_t *mac_addr, esp_now_send_status_t status);
void espNowRecvCallBackDummy(const uint8_t *mac_addr, const uint8_t *data, int data_len);
#else
void espNowSendCallBackDummy(u8 *mac_addr, u8 status);
void espNowRecvCallBackDummy(u8 *mac_addr, u8 *data, u8 len);
#endif

class EspNowService {
   private:
   #ifdef ESP32
    void pair(const uint8_t *mac);
    #else
    void pair(u8 *mac);
    #endif

   public:
    EspNowService() {}
    void setup(esp_now_send_cb_t esp_now_send_cb = espNowSendCallBackDummy, esp_now_recv_cb_t esp_now_recv_cb = espNowRecvCallBackDummy);
    #ifdef ESP32
    void send(const uint8_t *mac, const uint8_t *outputData, int len);
    #else
    void send(u8 *mac, u8 *outputData, int len);
    #endif
};

#endif
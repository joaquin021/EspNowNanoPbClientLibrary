#ifndef __ESP_NOW_SERVICE_HPP
#define __ESP_NOW_SERVICE_HPP

#include <Arduino.h>
#ifdef ESP32
#include <esp_now.h>
#else
#include <espnow.h>
#endif

#include "Commons.hpp"
#include "RequestUtils.hpp"
#include "ResponseUtils.hpp"
#include "messages.pb.h"

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
    int send(const uint8_t *mac, const uint8_t *outputData, int len);
    int sendRequest(const uint8_t *mac, request *request);
    int sendResponse(const uint8_t *mac, response *response);
#else
    int send(u8 *mac, u8 *outputData, int len);
    int sendRequest(u8 *mac, request *request);
    int sendResponse(u8 *mac, response *response);
#endif
};

#endif
#include "EspNowService.hpp"

void EspNowService::setup(esp_now_send_cb_t esp_now_send_cb, esp_now_recv_cb_t esp_now_recv_cb) {
    if (esp_now_init() != 0) {
        logErrorln("There was an error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(esp_now_send_cb);
    esp_now_register_recv_cb(esp_now_recv_cb);
}

#ifdef ESP32
void espNowSendCallBackDummy(const uint8_t *mac_addr, esp_now_send_status_t status) {}
void espNowRecvCallBackDummy(const uint8_t *mac_addr, const uint8_t *data, int data_len) {}

void EspNowService::pair(const uint8_t *mac) {
    bool existPeer = esp_now_is_peer_exist(mac);
    if (!existPeer) {
        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != 0) {
            logErrorln("Failed to add peer");
        }
    }
}
#else
void espNowSendCallBackDummy(u8 *mac_addr, u8 status) {}
void espNowRecvCallBackDummy(u8 *mac_addr, u8 *data, u8 len) {}

void EspNowService::pair(u8 *mac) {
    bool existPeer = esp_now_is_peer_exist(mac);
    if (!existPeer) {
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
        if (esp_now_add_peer(mac, ESP_NOW_ROLE_COMBO, 0, NULL, 0) != 0) {
            logErrorln("Failed to add peer");
        }
    }
}
#endif

#ifdef ESP32
void EspNowService::send(const uint8_t *mac, const uint8_t *outputData, int len) {
#else
void EspNowService::send(u8 *mac, u8 *outputData, int len) {
#endif
    pair(mac);
    int result = esp_now_send(mac, outputData, len);
    if (result == 0) {
        logTraceln("The message was sent sucessfully via ESP-NOW.");
    } else {
        logErrorln("There was an error sending via ESP-NOW.");
    }
}

#ifdef ESP32
void EspNowService::sendRequest(const uint8_t *mac, request *request) {
#else
void EspNowService::sendRequest(u8 *mac, request *request) {
#endif
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int serializedLen = RequestUtils::getInstance().serialize(serializedBuffer, request);
    send(mac, serializedBuffer, serializedLen);
}

#ifdef ESP32
void EspNowService::sendResponse(const uint8_t *mac, response *response) {
#else
void EspNowService::sendResponse(u8 *mac, response *response) {
#endif
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int serializedLen = ResponseUtils::getInstance().serialize(serializedBuffer, response);
    send(mac, serializedBuffer, serializedLen);
}
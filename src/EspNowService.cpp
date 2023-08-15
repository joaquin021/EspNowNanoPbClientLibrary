#include "EspNowService.hpp"

void EspNowService::setup(esp_now_send_cb_t esp_now_send_cb, esp_now_recv_cb_t esp_now_recv_cb) {
    if (esp_now_init() != 0) {
        logErrorln("EspNowService\t\tThere was an error initializing ESP-NOW");
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
        memset(&peerInfo, 0, sizeof(peerInfo));
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != 0) {
            logErrorln("EspNowService\t\tFailed to add peer");
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
            logErrorln("EspNowService\t\tFailed to add peer");
        }
    }
}
#endif

#ifdef ESP32
int EspNowService::send(const uint8_t *mac, const uint8_t *outputData, int len) {
#else
int EspNowService::send(u8 *mac, u8 *outputData, int len) {
#endif
    pair(mac);
    int result = esp_now_send(mac, outputData, len);
    if (result == 0) {
        logTraceln("EspNowService\t\tThe message was sent sucessfully via ESP-NOW.");
    } else {
        logErrorln("EspNowService\t\tThere was an error sending via ESP-NOW.");
    }
    return result;
}

int EspNowService::sendRequest(request *request) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int serializedLen = RequestUtils::getInstance().serialize(serializedBuffer, request);
    return send(request->to_mac, serializedBuffer, serializedLen);
}

int EspNowService::sendResponse(response *response) {
    uint8_t serializedBuffer[ESPNOW_BUFFERSIZE];
    int serializedLen = ResponseUtils::getInstance().serialize(serializedBuffer, response);
    return send(response->to_mac, serializedBuffer, serializedLen);
}
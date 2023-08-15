#include "Commons.hpp"

void printMacAndLenPacket(const uint8_t *mac, int len, const char *debugMessage) {
    char macStr[18 + 1 + 4];
    logTrace("Commons\t\t");
    logTrace(debugMessage);
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x %db",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len);
    logTraceln(macStr);
}

void printFromToMacAndLenPacket(const uint8_t *fromMac, const uint8_t *toMac, int len) {
    logTracef("Commons\t\tPacket received from: %02x:%02x:%02x:%02x:%02x:%02x, to: %02x:%02x:%02x:%02x:%02x:%02x %db\n",
              fromMac[0], fromMac[1], fromMac[2], fromMac[3], fromMac[4], fromMac[5],
              toMac[0], toMac[1], toMac[2], toMac[3], toMac[4], toMac[5],
              len);
}
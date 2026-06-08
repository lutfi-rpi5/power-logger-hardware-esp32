#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

// ============================================================
// Async WiFi manager — non-blocking connect from saved networks
// ============================================================
// Device runs immediately without network. WiFi connects in
// background. Known networks from NVS (with hardcoded fallback).
// ============================================================

enum WiFiState {
    WIFI_IDLE,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_FAILED
};

class WiFiManager {
public:
    WiFiManager();

    void setKnownNetworks(int count, const char** ssids, const char** passwords);
    void begin();
    void update();  // call every loop iteration

    bool beginConnect();  // start async connect (called after boot)
    void disconnect();
    bool isConnected() const;
    WiFiState getState() const;
    int8_t getRSSI() const;
    const char* getSSID() const;

    // AP mode
    void startAP(const char* ssid, const char* pass);
    void stopAP();
    bool isAPActive() const;

private:
    WiFiState _state;
    unsigned long _lastAttemptMs;
    int _scanCount;
    int _currentAttempt;

    // Hardcoded fallback from credentials.h
    int _hcCount;
    const char** _hcSSIDs;
    const char** _hcPasswords;

    void _tryNext();
    static void _onEvent(WiFiEvent_t event, WiFiEventInfo_t info);
};

#endif

#include "wifi_manager.h"
#include "config.h"
#include "diagnostics.h"

WiFiManager::WiFiManager()
    : _state(WIFI_IDLE)
    , _lastAttemptMs(0)
    , _scanCount(0)
    , _currentAttempt(0)
    , _hcCount(0)
    , _hcSSIDs(nullptr)
    , _hcPasswords(nullptr)
{}

void WiFiManager::setKnownNetworks(int count, const char** ssids, const char** passwords) {
    _hcCount = count;
    _hcSSIDs = ssids;
    _hcPasswords = passwords;
}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(_onEvent);
    diag.info("WIFI", "WiFi manager initialized");
}

void WiFiManager::update() {
    // Non-blocking retry
    if (_state == WIFI_FAILED) {
        unsigned long now = millis();
        if (now - _lastAttemptMs >= WIFI_RECONNECT_INTERVAL_MS) {
            diag.info("WIFI", "Retrying WiFi connect...");
            beginConnect();
        }
    }
}

bool WiFiManager::beginConnect() {
    if (_state == WIFI_CONNECTING) {
        diag.warn("WIFI", "Already connecting, skipping");
        return false;
    }

    // Scan available networks
    diag.info("WIFI", "Scanning networks...");
    _scanCount = WiFi.scanNetworks();
    if (_scanCount == 0) {
        diag.warn("WIFI", "No networks found in scan");
        _state = WIFI_FAILED;
        _lastAttemptMs = millis();
        return false;
    }

    _currentAttempt = 0;
    _state = WIFI_CONNECTING;
    _tryNext();
    return true;
}

void WiFiManager::_tryNext() {
    // Try hardcoded credentials first (from credentials.cpp)
    if (_hcSSIDs != nullptr && _currentAttempt < _hcCount) {
        const char* ssid = _hcSSIDs[_currentAttempt];
        const char* pass = _hcPasswords[_currentAttempt];

        // Check if this network is visible
        bool found = false;
        for (int i = 0; i < _scanCount; i++) {
            if (strcmp(WiFi.SSID(i).c_str(), ssid) == 0) {
                found = true;
                break;
            }
        }

        if (found) {
            diag.info("WIFI", "Connecting to %s...", ssid);
            WiFi.begin(ssid, pass);
        } else {
            _currentAttempt++;
            _tryNext();
        }
        return;
    }

    // No more networks to try
    diag.warn("WIFI", "No known networks available");
    _state = WIFI_FAILED;
    _lastAttemptMs = millis();
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    _state = WIFI_IDLE;
}

bool WiFiManager::isConnected() const {
    return _state == WIFI_CONNECTED;
}

WiFiState WiFiManager::getState() const {
    return _state;
}

int8_t WiFiManager::getRSSI() const {
    return WiFi.RSSI();
}

const char* WiFiManager::getSSID() const {
    return WiFi.SSID().c_str();
}

void WiFiManager::startAP(const char* ssid, const char* pass) {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, pass, AP_CHANNEL, 0, AP_MAX_CLIENTS);
    diag.info("WIFI", "AP started: SSID=%s IP=%s", ssid, WiFi.softAPIP().toString().c_str());
}

void WiFiManager::stopAP() {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    diag.info("WIFI", "AP stopped");
}

bool WiFiManager::isAPActive() const {
    return WiFi.getMode() == WIFI_AP_STA || WiFi.getMode() == WIFI_AP;
}

void WiFiManager::_onEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            diag.info("WIFI", "Connected! IP=%s", WiFi.localIP().toString().c_str());
            // State is updated externally via isConnected()
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            diag.warn("WIFI", "Disconnected, reason=%d", info.wifi_sta_disconnected.reason);
            break;
        default:
            break;
    }
}

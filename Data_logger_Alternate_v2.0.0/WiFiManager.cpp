/******************************************************
 * File      : WiFiManager.cpp
 ******************************************************/

#include "WiFiManager.h"

WiFiManager::WiFiManager(StorageManager& storage, unsigned long connectTimeoutMs)
    : _storage(storage), _connectTimeoutMs(connectTimeoutMs)
{}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
    Serial.println("[WiFi] Manager initialized. Starting first scan...");
    _startBestConnection();
}

void WiFiManager::tick() {
    bool connected = (WiFi.status() == WL_CONNECTED);

    // Rising edge: STA baru tersambung
    if (!_prevConnected && connected) {
        Serial.printf("[WiFi] Connected: %s | IP: %s | RSSI: %d dBm\n",
            WiFi.SSID().c_str(),
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI());
    }
    _prevConnected = connected;

    // Update shared state
    updateState([&](SystemState& s) {
        s.wifiConnected = connected;
        s.wifiRSSI      = connected ? WiFi.RSSI() : 0;
    });

    if (connected) {
        _attemptInProgress = false;
        return;
    }

    // Check pending connection
    if (_attemptInProgress) {
        _checkPendingConnection();
        return;
    }

    // Periodic retry
    unsigned long now = millis();
    if (now - _lastAttemptMs >= WIFI_RECONNECT_INTERVAL_MS) {
        _startBestConnection();
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

// ─── Internal: Start Connection ───────────────────────────

void WiFiManager::_startBestConnection() {
    _lastAttemptMs = millis();

    Serial.println("[WiFi] Scanning networks...");
    int n = WiFi.scanNetworks(/*async=*/false, /*show_hidden=*/false);

    if (n <= 0) {
        Serial.println("[WiFi] No networks found.");
        return;
    }

    int bestIdx = _findBestNetwork(n);
    if (bestIdx < 0) {
        Serial.println("[WiFi] No known network in range.");
        WiFi.scanDelete();
        return;
    }

    WiFiEntry entry = _storage.getWifiEntry(bestIdx);
    Serial.printf("[WiFi] Connecting to: %s (list[%d])\n", entry.ssid, bestIdx);
    WiFi.scanDelete();
    WiFi.begin(entry.ssid, entry.pass);

    _attemptInProgress = true;
    _attemptStartMs    = millis();
}

// ─── Internal: Check Pending ──────────────────────────────

void WiFiManager::_checkPendingConnection() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        _attemptInProgress = false;
        return;
    }

    unsigned long elapsed = millis() - _attemptStartMs;
    if (elapsed >= _connectTimeoutMs) {
        Serial.printf("[WiFi] Connection timed out after %lu ms.\n", elapsed);
        WiFi.disconnect(true);
        _attemptInProgress = false;
    }
}

// ─── Internal: Priority Scan ──────────────────────────────

int WiFiManager::_findBestNetwork(int scannedCount) const {
    int storedCount = _storage.getWifiCount();
    for (int s = 0; s < storedCount; s++) {
        WiFiEntry entry = _storage.getWifiEntry(s);
        if (strlen(entry.ssid) == 0) continue;
        for (int n = 0; n < scannedCount; n++) {
            if (WiFi.SSID(n) == String(entry.ssid)) {
                Serial.printf("[WiFi] Found known network: %s (RSSI %d)\n",
                    entry.ssid, WiFi.RSSI(n));
                return s;
            }
        }
    }
    return -1;
}

void WiFiManager::printStatus() const {
    if (isConnected()) {
        Serial.printf("[WiFi] Active: %s | IP: %s | RSSI: %d dBm\n",
            WiFi.SSID().c_str(),
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI());
    } else {
        Serial.println("[WiFi] Not connected.");
    }
}

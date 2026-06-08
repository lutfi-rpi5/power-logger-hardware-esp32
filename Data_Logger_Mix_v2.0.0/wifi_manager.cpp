#include "wifi_manager.h"
#include "diagnostics.h"

WiFiManager::WiFiManager(StorageManager& storage, unsigned long connectTimeoutMs)
    : _storage(storage), _connectTimeoutMs(connectTimeoutMs)
{}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
    diag.info("WIFI", "Manager initialized. Starting first scan...");
    _startBestConnection();
}

void WiFiManager::tick() {
    bool connected = (WiFi.status() == WL_CONNECTED);

    if (!_prevConnected && connected) {
        diag.info("WIFI", "Connected: %s | IP: %s | RSSI: %d dBm",
            WiFi.SSID().c_str(),
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI());
    }
    _prevConnected = connected;

    updateState([&](SystemState& s) {
        s.wifiConnected = connected;
        s.wifiRSSI      = connected ? WiFi.RSSI() : 0;
    });

    if (connected) {
        _attemptInProgress = false;
        return;
    }

    if (_attemptInProgress) {
        _checkPendingConnection();
        return;
    }

    unsigned long now = millis();
    if (now - _lastAttemptMs >= WIFI_RECONNECT_INTERVAL_MS) {
        _startBestConnection();
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::_startBestConnection() {
    _lastAttemptMs = millis();

    diag.info("WIFI", "Scanning networks...");
    int n = WiFi.scanNetworks(false, false);

    if (n <= 0) {
        diag.info("WIFI", "No networks found.");
        return;
    }

    int bestIdx = _findBestNetwork(n);
    if (bestIdx < 0) {
        diag.info("WIFI", "No known network in range.");
        WiFi.scanDelete();
        return;
    }

    WiFiEntry entry = _storage.getWifiEntry(bestIdx);
    diag.info("WIFI", "Connecting to: %s (list[%d])", entry.ssid, bestIdx);
    WiFi.scanDelete();
    WiFi.begin(entry.ssid, entry.pass);

    _attemptInProgress = true;
    _attemptStartMs    = millis();
}

void WiFiManager::_checkPendingConnection() {
    wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED) {
        _attemptInProgress = false;
        return;
    }

    unsigned long elapsed = millis() - _attemptStartMs;
    if (elapsed >= _connectTimeoutMs) {
        diag.warn("WIFI", "Connection timed out after %lu ms.", elapsed);
        WiFi.disconnect(true);
        _attemptInProgress = false;
    }
}

int WiFiManager::_findBestNetwork(int scannedCount) const {
    int storedCount = _storage.getWifiCount();
    for (int s = 0; s < storedCount; s++) {
        WiFiEntry entry = _storage.getWifiEntry(s);
        if (strlen(entry.ssid) == 0) continue;
        for (int n = 0; n < scannedCount; n++) {
            if (WiFi.SSID(n) == String(entry.ssid)) {
                diag.info("WIFI", "Found known network: %s (RSSI %d)",
                    entry.ssid, WiFi.RSSI(n));
                return s;
            }
        }
    }
    return -1;
}

void WiFiManager::printStatus() const {
    if (isConnected()) {
        diag.info("WIFI", "Active: %s | IP: %s | RSSI: %d dBm",
            WiFi.SSID().c_str(),
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI());
    } else {
        diag.info("WIFI", "Not connected.");
    }
}

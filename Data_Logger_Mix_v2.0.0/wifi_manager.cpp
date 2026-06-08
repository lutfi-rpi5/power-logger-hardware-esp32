#include "wifi_manager.h"
#include "diagnostics.h"

/**
 * @file wifi_manager.cpp
 * @brief Asynchronous WiFi connection manager with network scanning and auto-retry.
 *
 * Connection flow:
 * ```
 * tick() ──(WiFi connected)──→ Update RSSI, return
 * tick() ──(WiFi disconnected)──→
 *   ├── Attempt in progress? → Check timeout
 *   └── Cooldown elapsed? → Start new scan+connect
 * ```
 *
 * @note WiFi.setAutoReconnect(false) because we use a custom retry
 *       strategy with milli()-based timing.
 */

WiFiManager::WiFiManager(StorageManager& storage, unsigned long connectTimeoutMs)
    : _storage(storage), _connectTimeoutMs(connectTimeoutMs)
{}

/**
 * @brief Initialise WiFi in STA mode and start the first scan+connect.
 *
 * Non-blocking — the scan runs synchronously (~3 s) but connect()
 * is asynchronous. The tick() method monitors progress.
 */
void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // We manage reconnection manually
    diag.info("WIFI", "Manager initialized. Starting first scan...");
    _startBestConnection();
}

/**
 * @brief Periodic WiFi handler — call this from the main loop.
 *
 * Tasks:
 * 1. Update gState with current WiFi status and RSSI.
 * 2. Detect fresh connection events (LED signal in _checkConnectionSignals).
 * 3. If disconnected, manage retry timing and connection attempts.
 */
void WiFiManager::tick() {
    bool connected = (WiFi.status() == WL_CONNECTED);

    // Log connection state transitions (reconnection only)
    if (!_prevConnected && connected) {
        diag.info("WIFI", "Connected: %s | IP: %s | RSSI: %d dBm",
            WiFi.SSID().c_str(),
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI());
    }
    _prevConnected = connected;

    // Update shared state with current connection info
    updateState([&](SystemState& s) {
        s.wifiConnected = connected;
        s.wifiRSSI      = connected ? WiFi.RSSI() : 0;
    });

    // ── Connected: clear attempt flag, nothing more to do ────────────
    if (connected) {
        _attemptInProgress = false;
        return;
    }

    // ── Attempt in progress: check timeout ───────────────────────────
    if (_attemptInProgress) {
        _checkPendingConnection();
        return;
    }

    // ── Cooldown elapsed: start new connection cycle ─────────────────
    unsigned long now = millis();
    if (now - _lastAttemptMs >= WIFI_RECONNECT_INTERVAL_MS) {
        _startBestConnection();
    }
}

/**
 * @brief Check WiFi connection status.
 * @return true if WiFi.status() returns WL_CONNECTED.
 */
bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief Scan for WiFi networks and connect to the best known match.
 *
 * Calls WiFi.scanNetworks() which is a **blocking** call that takes
 * approximately 3 seconds. This is acceptable because:
 * - Scanning only happens every WIFI_RECONNECT_INTERVAL_MS (30 s).
 * - The WDT timeout is set to 30 s, well above the scan duration.
 *
 * After scanning, compares result SSIDs against the NVS-stored network
 * list and connects to the first match found.
 */
void WiFiManager::_startBestConnection() {
    _lastAttemptMs = millis();

    diag.info("WIFI", "Scanning networks...");

    // Blocking scan: ~3 s for full channel scan
    int n = WiFi.scanNetworks(false, false);

    if (n <= 0) {
        diag.info("WIFI", "No networks found.");
        return;
    }

    // Find the first known network in the scan results
    int bestIdx = _findBestNetwork(n);
    if (bestIdx < 0) {
        diag.info("WIFI", "No known network in range.");
        WiFi.scanDelete();
        return;
    }

    // Connect asynchronously to the selected network
    WiFiEntry entry = _storage.getWifiEntry(bestIdx);
    diag.info("WIFI", "Connecting to: %s (list[%d])", entry.ssid, bestIdx);
    WiFi.scanDelete();  // Free scan result memory
    WiFi.begin(entry.ssid, entry.pass);

    _attemptInProgress = true;
    _attemptStartMs    = millis();
}

/**
 * @brief Check the status of a pending connection attempt.
 *
 * If WL_CONNECTED, the attempt succeeded.
 * If the timeout has elapsed, disconnect and clear the flag
 * so tick() will retry after the cooldown interval.
 */
void WiFiManager::_checkPendingConnection() {
    wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED) {
        _attemptInProgress = false;
        return;
    }

    unsigned long elapsed = millis() - _attemptStartMs;
    if (elapsed >= _connectTimeoutMs) {
        diag.warn("WIFI", "Connection timed out after %lu ms.", elapsed);
        WiFi.disconnect(true);  // Force disconnect to clean up
        _attemptInProgress = false;
    }
}

/**
 * @brief Find the first stored WiFi network that appears in scan results.
 *
 * Iterates over the NVS-stored network list (priority order) and checks
 * each against all scanned networks. Returns the first match.
 *
 * @param scannedCount Number of networks found by the scan.
 * @return NVS list index of the matched network, or -1 if none found.
 */
int WiFiManager::_findBestNetwork(int scannedCount) const {
    int storedCount = _storage.getWifiCount();
    for (int s = 0; s < storedCount; s++) {
        WiFiEntry entry = _storage.getWifiEntry(s);
        if (strlen(entry.ssid) == 0) continue;  // Skip empty entries
        for (int n = 0; n < scannedCount; n++) {
            if (WiFi.SSID(n) == String(entry.ssid)) {
                diag.info("WIFI", "Found known network: %s (RSSI %d)",
                    entry.ssid, WiFi.RSSI(n));
                return s;
            }
        }
    }
    return -1;  // No known network in range
}

/**
 * @brief Print current WiFi connection status to the serial log.
 */
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

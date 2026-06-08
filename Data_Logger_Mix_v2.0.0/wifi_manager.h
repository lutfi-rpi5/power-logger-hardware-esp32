#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "types.h"
#include "storage_manager.h"

/**
 * @file wifi_manager.h
 * @brief Asynchronous WiFi connection manager with automatic network selection.
 *
 * The WiFiManager does NOT block the boot sequence. It scans for known
 * networks (stored in NVS via StorageManager) and connects to the first
 * one found in range. Reconnection is attempted every 30 seconds on failure.
 *
 * Behaviour:
 * - begin(): Starts a scan and connects asynchronously (non-blocking).
 * - tick(): Called from loop(); detects disconnections and triggers retries.
 * - Network selection: Multi-pass comparison over scanned results vs NVS list.
 *
 * @note WiFi.setAutoReconnect(false) because we manage reconnection manually
 *       to avoid blocking on auto-reconnect attempts.
 */

/**
 * @class WiFiManager
 * @brief Async WiFi connection handler with scanning and auto-retry.
 *
 * State machine (implicit, via tick() logic):
 * ```
 * IDLE ──(tick, WiFi disconnected, cooldown elapsed)──→ SCAN+CONNECT
 * SCAN+CONNECT ──(scan result: known network)──→ CONNECTING
 * CONNECTING ──(timeout)──→ IDLE (waits for cooldown)
 * CONNECTING ──(success)──→ CONNECTED
 * CONNECTED ──(WiFi disconnect)──→ IDLE
 * ```
 */
class WiFiManager {
public:
    /**
     * @brief Construct the WiFi manager.
     * @param storage            Reference to StorageManager for NVS Wi-Fi entries.
     * @param connectTimeoutMs   Max duration to wait for a connection attempt (default 10 s).
     */
    explicit WiFiManager(StorageManager& storage, unsigned long connectTimeoutMs = 10000);

    /**
     * @brief Initialise WiFi in STA mode and trigger the first scan+connect.
     * Non-blocking — returns immediately.
     */
    void begin();

    /**
     * @brief Periodic handler. Call this from the main loop (Core 1).
     *
     * Tasks:
     * - Update gState with current connection status and RSSI.
     * - Detect disconnections and trigger reconnection scans.
     * - Monitor pending connection attempts for timeout.
     */
    void tick();

    /**
     * @brief Check WiFi connection status.
     * @return true if WiFi.status() == WL_CONNECTED.
     */
    bool isConnected() const;

    /**
     * @brief Print current connection status to serial log.
     */
    void printStatus() const;

private:
    StorageManager& _storage;           ///< NVS storage for WiFi credentials
    unsigned long   _connectTimeoutMs;  ///< Connection attempt timeout (ms)
    unsigned long   _lastAttemptMs;     ///< millis() of the last connection attempt
    bool            _attemptInProgress; ///< true during connect(), before timeout or success
    unsigned long   _attemptStartMs;    ///< millis() when the current attempt started
    bool            _prevConnected;     ///< WiFi status from the previous tick() call

    /**
     * @brief Scan for networks and connect to the best known match.
     * Calls WiFi.scanNetworks() synchronously (~3 s blocking).
     */
    void _startBestConnection();

    /**
     * @brief Check if a pending connection attempt has completed or timed out.
     */
    void _checkPendingConnection();

    /**
     * @brief Find the first stored network that appears in the scan results.
     * @param scannedCount Number of networks found by the scan.
     * @return Index into the NVS WiFi list, or -1 if no known network found.
     */
    int  _findBestNetwork(int scannedCount) const;
};

#pragma once

/******************************************************
 * File      : WiFiManager.h
 * Description:
 *   Smart WiFi connection manager.
 *   - Scan dan konek ke jaringan KNOWN dengan prioritas tertinggi
 *   - Reconnect non-blocking berbasis timer
 *   - Tidak memblok boot — sistem jalan meski tanpa WiFi
 *   - Update gState.wifiConnected / wifiRSSI
 ******************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "PhaseData.h"
#include "StorageManager.h"

class WiFiManager {
public:
    explicit WiFiManager(StorageManager& storage, unsigned long connectTimeoutMs = 10000);

    /** Begin: mulai percobaan koneksi pertama (non-blocking). */
    void begin();

    /** Panggil tiap loop(). Handle reconnect timer & update gState. */
    void tick();

    /** True jika sedang terhubung ke AP. */
    bool isConnected() const;

    /** Print status WiFi ke Serial. */
    void printStatus() const;

private:
    StorageManager& _storage;
    unsigned long   _connectTimeoutMs;
    unsigned long   _lastAttemptMs    = 0;
    bool            _attemptInProgress = false;
    unsigned long   _attemptStartMs   = 0;
    bool            _prevConnected    = false;

    void _startBestConnection();
    void _checkPendingConnection();
    int  _findBestNetwork(int scannedCount) const;
};

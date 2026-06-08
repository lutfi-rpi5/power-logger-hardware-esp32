#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "types.h"
#include "storage_manager.h"

class WiFiManager {
public:
    explicit WiFiManager(StorageManager& storage, unsigned long connectTimeoutMs = 10000);
    void begin();
    void tick();
    bool isConnected() const;
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

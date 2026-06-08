#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "storage_manager.h"

// ============================================================
// ESP32 AP mode + HTTP web server for configuration
// ============================================================
// Pages:
//   /              → Index (Manage WiFi, MQTT, Calibration links)
//   /wifi          → Known network table + add/delete
//   /mqtt          → MQTT broker config form
//   /calibration   → PZEM calibration offsets + thresholds
//   /factoryreset  → POST: erase all NVS settings
// Protected by basic login (ADMIN / 18273645 from config.h)
// ============================================================

class WebServerManager {
public:
    WebServerManager(StorageManager* storage);

    void begin();
    void update();  // call in loop

private:
    AsyncWebServer _server;
    StorageManager* _storage;

    // HTML generators
    String _htmlHead(const char* title);
    String _htmlFoot();
    bool _authenticate(AsyncWebServerRequest* request);

    // Page handlers
    void _handleLogin(AsyncWebServerRequest* request);
    void _handleIndex(AsyncWebServerRequest* request);
    void _handleWiFi(AsyncWebServerRequest* request);
    void _handleWiFiAdd(AsyncWebServerRequest* request);
    void _handleWiFiDelete(AsyncWebServerRequest* request);
    void _handleMQTT(AsyncWebServerRequest* request);
    void _handleMQTTSave(AsyncWebServerRequest* request);
    void _handleCalibration(AsyncWebServerRequest* request);
    void _handleCalibrationSave(AsyncWebServerRequest* request);
    void _handleFactoryReset(AsyncWebServerRequest* request);

    // Page content builders
    String _pageIndex();
    String _pageWiFi();
    String _pageMQTT();
    String _pageCalibration();
    String _pageLogin();
};

#endif

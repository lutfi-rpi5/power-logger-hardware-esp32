#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <functional>
#include "config.h"
#include "storage_manager.h"
#include "types.h"

class WebServerManager {
public:
    explicit WebServerManager(StorageManager& storage);
    void begin();
    void activate();
    void deactivate();
    bool isActive() const { return _active; }
    String getIP() const;

    std::function<void()> onCalibrationSaved;
    std::function<void()> onMQTTSaved;
    std::function<void()> onThresholdSaved;

private:
    StorageManager&  _storage;
    AsyncWebServer   _server;
    bool             _active = false;

    void _setupRoutes();
    bool _authenticate(AsyncWebServerRequest* request);
    void _handleLogin(AsyncWebServerRequest* request);

    static String _pageIndex(StorageManager& s);
    static String _pageWifi(StorageManager& s);
    static String _pageMQTT(StorageManager& s);
    static String _pageCalibration(StorageManager& s);
    static String _pageLogin();
    static String _htmlHead(const char* title);
    static String _htmlFoot();
};

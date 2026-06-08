#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <functional>
#include "config.h"
#include "storage_manager.h"
#include "types.h"

/**
 * @file webserver_manager.h
 * @brief Web configuration server with Soft-AP mode, HTTP Basic Auth,
 *        and responsive HTML configuration pages.
 *
 * When activated via the OLED menu, the ESP32 starts a Soft-AP (default:
 * SSID "DataLogger", IP 192.168.4.1) and a web server on port 80.
 * Users connect to the AP and access the configuration pages via browser.
 *
 * All pages are protected by HTTP Basic Authentication (default:
 * ADMIN / 18273645). The credential define-only in config.h (recompile
 * required to change).
 *
 * Pages:
 * - `/` — Home with links to all config sections + Danger Zone (factory reset).
 * - `/wifi` — Manage known WiFi networks (table + add/delete).
 * - `/mqtt` — MQTT broker settings (server, ports, auth, SSL cert, topics).
 * - `/calibration` — PZEM V/I offsets + line status thresholds.
 * - `/factoryreset` — POST-only, erases all NVS data and reboots.
 *
 * Captive portal: Unknown requests are redirected to 192.168.4.1/.
 */

/**
 * @class WebServerManager
 * @brief Manages the ESP32 Soft-AP and HTTP configuration server.
 *
 * The server routes are registered in begin() but the server is NOT
 * started until activate() is called from the menu. deactivate() stops
 * both the HTTP server and the Soft-AP.
 *
 * Callbacks (set by the main .ino):
 * - onCalibrationSaved: triggers DAQ reload of calibration offsets.
 * - onMQTTSaved: triggers MQTT reconnect with new config.
 * - onThresholdSaved: triggers DAQ reload of line status thresholds.
 */
class WebServerManager {
public:
    /**
     * @brief Construct the web server manager.
     * @param storage Reference to StorageManager for config read/write.
     */
    explicit WebServerManager(StorageManager& storage);

    /**
     * @brief Register HTTP routes. Does NOT start the server or AP.
     * Call activate() to start serving.
     */
    void begin();

    /**
     * @brief Start the Soft-AP and HTTP server.
     * Reads AP config (SSID/pass) from NVS. Enables WIFI_AP_STA mode
     * (client + AP simultaneously). Non-blocking.
     */
    void activate();

    /**
     * @brief Stop the HTTP server and disconnect the Soft-AP.
     */
    void deactivate();

    /**
     * @brief Check if the web server is currently active.
     * @return true if the server is running.
     */
    bool isActive() const { return _active; }

    /**
     * @brief Get the Soft-AP IP address string.
     * @return IP string (e.g. "192.168.4.1") or empty string if not active.
     */
    String getIP() const;

    /// @name Configuration Change Callbacks
    /// Set by Data_Logger_Mix_v2.0.0.ino during setup.
    /// @{
    std::function<void()> onCalibrationSaved;  ///< Called after calibration form submit
    std::function<void()> onMQTTSaved;         ///< Called after MQTT form submit
    std::function<void()> onThresholdSaved;    ///< Called after threshold form submit
    /// @}

private:
    StorageManager&  _storage;  ///< NVS storage for config
    AsyncWebServer   _server;   ///< ESPAsyncWebServer instance on port 80
    bool             _active = false;  ///< Server + AP active flag

    /**
     * @brief Register all HTTP route handlers (/, /wifi, /mqtt, /calibration, etc.).
     */
    void _setupRoutes();

    /**
     * @brief Verify HTTP Basic Auth credentials against config.h values.
     * @param request Incoming request.
     * @return true if authenticated.
     */
    bool _authenticate(AsyncWebServerRequest* request);

    /**
     * @brief Send a 401 Unauthorised response with the login page.
     * @param request Incoming request.
     */
    void _handleLogin(AsyncWebServerRequest* request);

    /// @name HTML Page Generators
    /// Each returns a complete HTML document string.
    /// @{
    static String _pageIndex(StorageManager& s);       ///< Home page with navigation cards
    static String _pageWifi(StorageManager& s);        ///< WiFi network management
    static String _pageMQTT(StorageManager& s);        ///< MQTT configuration form
    static String _pageCalibration(StorageManager& s); ///< Calibration + thresholds form
    static String _pageLogin();                         ///< Simple login-required page
    /// @}

    /**
     * @brief Generate the HTML head section with embedded CSS and navigation bar.
     * @param title Page title.
     * @return HTML string including <head> and opening <body> tags.
     */
    static String _htmlHead(const char* title);

    /**
     * @brief Generate the HTML footer with firmware version and author credit.
     * @return HTML string with closing tags.
     */
    static String _htmlFoot();
};

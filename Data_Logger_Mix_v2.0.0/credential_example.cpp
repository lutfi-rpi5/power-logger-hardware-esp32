#include "credentials.h"

/**
 * @file credential_example.cpp
 * @brief TEMPLATE for WiFi and MQTT credentials.
 *
 * IMPORTANT:
 * 1. Copy this file to credentials.cpp:
 *    @code
 *    cp credential_example.cpp credentials.cpp
 *    @endcode
 * 2. Fill in your actual WiFi SSID/password and MQTT broker details.
 * 3. credentials.cpp is gitignored — your secrets will NOT be committed.
 *
 * NOTE (v2.x): The WiFi and MQTT credentials defined here are the STATIC
 * v1.x fallback. In v2.x, credentials are managed via:
 *   - The web configuration portal (connect to AP "DataLogger")
 *   - NVS persistent storage (StorageManager class)
 *   - OLED menu → Config to activate the web server
 *
 * If NVS is empty (first boot or after factory reset), the static values
 * below are ignored — use the web UI to configure your network.
 */

/// List of known WiFi networks. The device scans and connects to any
/// matching network found in range.
WiFiCredential wifiList[] = {
    { "YourWiFiSSID", "YourWiFiPassword", 1 },
};

const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);

/// MQTT broker connection parameters (fallback defaults).
const char* MQTT_SERVER = "broker.example.com";
const int   MQTT_PORT   = 1883;   ///< Default MQTT TCP port
const int   MQTTS_PORT  = 8883;   ///< Default MQTT SSL port

const char* MQTT_USER = "";   ///< Leave empty for anonymous
const char* MQTT_PASS = "";

const char* CA_CERT = "";     ///< Leave empty for insecure SSL (fallback)

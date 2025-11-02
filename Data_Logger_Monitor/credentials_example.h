/******************************************************
 * Project   : ESP32 MQTTs Publisher
 * File      : credentials.h
 * Author    : Muhammad Lutfi Nur Anendi
 * Created   : 5 August 2025
 * Last Edit : 23 October 2025
 *
 * Description:
 *   Configuration file for WiFi, MQTT, and SSL credentials.
 *   Rename this file from `credentials_example.h` → `credentials.h`
 *   before compiling the project.
 *
 * IMPORTANT:
 *   - DO NOT commit this file to any public repository.
 *   - Keep your credentials private.
 *   - Modify only the fields below as required.
 *
 * MQTT Notes:
 *   - MQTT_PORT  = 1883 for non-SSL
 *   - MQTTS_PORT = 8883 for SSL (secure)
 *   - MQTT_TOPIC example: "user_topic/#"
 *
 * SSL Notes:
 *   - CA_CERT should contain your Root Certificate between
 *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
 *
 * Developer Info:
 *   GitHub   : https://github.com/lutfi-rpi5
 *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
 *   Instagram: https://instagram.com/lutfiianendi
 *
 * Revision History:
 *   v1.0  Initial release with MQTT & SSL credentials
 *   v1.1  Added developer info and usage guide
 ******************************************************/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// ---------- WiFi Configuration ----------
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASS = "WIFI_PASS";

// ---------- MQTT Broker Configuration ----------
const char* MQTT_SERVER = "192.168.0.0";      // Broker address
const int   MQTT_PORT   = 1883;               // Non-SSL port
const int   MQTTS_PORT  = 8883;               // SSL port
const char* MQTT_USER   = "username_example"; // MQTT Username
const char* MQTT_PASS   = "pass_example";     // MQTT Password
const char* MQTT_TOPIC  = "user_topic/#";     // Topic prefix (optional)

// ---------- Optional API Key ----------
const char* API_KEY = "";  // For API Integration (REST or HTTP API)

// ---------- Root CA Certificate ----------
const char* CA_CERT = \
"-----BEGIN CERTIFICATE-----\n"
// Drop your CA certificate content here
"-----END CERTIFICATE-----\n";

#endif  // CREDENTIALS_H

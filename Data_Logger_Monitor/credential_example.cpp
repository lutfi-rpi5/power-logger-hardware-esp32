// /******************************************************
//  * Project   : ESP32 MQTTs Publisher
//  * File      : credentials.h
//  * Author    : Muhammad Lutfi Nur Anendi
//  * Created   : 5 August 2025
//  * Last Edit : 23 October 2025
//  *
//  * Description:
//  *   Configuration file for WiFi, MQTT, and SSL credentials.
//  *   Rename this file from `credentials_example.cpp` → `credentials.cpp`
//  *   before compiling the project.
//  *
//  * IMPORTANT:
//  *   - DO NOT commit this file to any public repository.
//  *   - Keep your credentials private.
//  *   - Modify only the fields below as required.
//  *
//  * MQTT Notes:
//  *   - MQTT_PORT  = 1883 for non-SSL
//  *   - MQTTS_PORT = 8883 for SSL (secure)
//  *   - MQTT_TOPIC example: "user_topic/#"
//  *
//  * SSL Notes:
//  *   - CA_CERT should contain your Root Certificate between
//  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
//  *
//  * Developer Info:
//  *   GitHub   : https://github.com/lutfi-rpi5
//  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
//  *   Instagram: https://instagram.com/lutfiianendi
//  *
//  * Revision History:
//  *   v1.0  Initial release with MQTT & SSL credentials
//  *   v1.1  Added developer info and usage guide
//  ******************************************************/

// #include "credentials.h"

// // ---------- WiFi Configuration ----------
// const char* WIFI_SSID1 = "";
// const char* WIFI_PASS1 = "";

// const char* WIFI_SSID2 = "";
// const char* WIFI_PASS2 = "";

// const char* WIFI_SSID3 = "";
// const char* WIFI_PASS3 = "";

// const char* WIFI_SSID4 = "";
// const char* WIFI_PASS4 = "";

// // ---------- WiFi Networks List ----------
// WiFiCredential wifiList[] = {
//   {WIFI_SSID1, WIFI_PASS1, 0},   // Highest priority
//   {WIFI_SSID4, WIFI_PASS4, 1},
//   {WIFI_SSID2, WIFI_PASS2, 2},
//   {WIFI_SSID3, WIFI_PASS3, 3}
// };
// const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);

// // ---------- MQTT Broker Configuration ----------
// const char* MQTT_SERVER = "";                 // Broker address
// const int   MQTT_PORT   = 1883;               // Non-SSL port
// const int   MQTTS_PORT  = 8883;               // SSL port
// const char* MQTT_USER   = "";                 // MQTT Username
// const char* MQTT_PASS   = "";                 // MQTT Password
// const char* MQTT_TOPIC  = "";                 // Topic prefix (optional)

// // ---------- Optional API Key ----------
// const char* API_KEY = "";  // For future REST API integration

// // ---------- Root CA Certificate ----------
// const char* CA_CERT = 
// "-----BEGIN CERTIFICATE-----\n"
// // Drop your CA certificate content here
// "-----END CERTIFICATE-----\n";



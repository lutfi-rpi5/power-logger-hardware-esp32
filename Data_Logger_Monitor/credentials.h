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

/******************************************************
 * Project   : ESP32 MQTTs Publisher
 * File      : credentials.h
 * Author    : Muhammad Lutfi Nur Anendi
 * Created   : 5 August 2025
 * Last Edit : 23 October 2025
 *
 * Description:
 *   Configuration header for WiFi, MQTT, and SSL credentials.
 ******************************************************/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// ---------- WiFi Credential Struct ----------
struct WiFiCredential {
  const char* ssid;
  const char* password;
  int priority;
};

// ---------- WiFi Configuration ----------
extern const char* WIFI_SSID1;
extern const char* WIFI_PASS1;
extern const char* WIFI_SSID2;
extern const char* WIFI_PASS2;
extern const char* WIFI_SSID3;
extern const char* WIFI_PASS3;
extern const char* WIFI_SSID4;
extern const char* WIFI_PASS4;

extern WiFiCredential wifiList[];
extern const int WIFI_COUNT;

// ---------- MQTT Broker Configuration ----------
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const int   MQTTS_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASS;
extern const char* MQTT_TOPIC;

// ---------- Optional API Key ----------
extern const char* API_KEY;

// ---------- Root CA Certificate ----------
extern const char* CA_CERT;

#endif  // CREDENTIALS_H



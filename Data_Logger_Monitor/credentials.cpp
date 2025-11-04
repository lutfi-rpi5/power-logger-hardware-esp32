/******************************************************
 * Project   : ESP32 MQTTs Publisher
 * File      : credentials.h
 * Author    : Muhammad Lutfi Nur Anendi
 * Created   : 5 August 2025
 * Last Edit : 23 October 2025
 *
 * Description:
 *   Configuration file for WiFi, MQTT, and SSL credentials.
 *   Rename this file from `credentials_example.cpp` → `credentials.cpp`
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

#include "credentials.h"

// ---------- WiFi Configuration ----------
const char* WIFI_SSID1 = "FLA 2.4G";
const char* WIFI_PASS1 = "20092009";

const char* WIFI_SSID2 = "PTBA-LOGGER";
const char* WIFI_PASS2 = "katekpasswordnyo";

const char* WIFI_SSID3 = "PTBA-KERTAPATI";
const char* WIFI_PASS3 = "Spirit7jutaton";

const char* WIFI_SSID4 = "rpi-5";
const char* WIFI_PASS4 = "getalifebro";

// ---------- WiFi Networks List ----------
WiFiCredential wifiList[] = {
  {WIFI_SSID1, WIFI_PASS1, 0},   // Highest priority
  {WIFI_SSID4, WIFI_PASS4, 1},
  {WIFI_SSID2, WIFI_PASS2, 2},
  {WIFI_SSID3, WIFI_PASS3, 3}
};
const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);

// ---------- MQTT Broker Configuration ----------
const char* MQTT_SERVER = "broker.avisha.id";   // Broker address (Avisha Broker) 103.127.97.247
const int   MQTT_PORT   = 1883;               // Non-SSL port
const int   MQTTS_PORT  = 8883;               // SSL port
const int   WS_PORT     = 8083;               // Non-SSL port
const int   WSS_PORT    = 8084;               // SSL port
const char* MQTT_USER   = "lutpiii";          // MQTT Username
const char* MQTT_PASS   = "lutpiiiMQTT";      // MQTT Password
const char* MQTT_TOPIC  = "lutpiii/";         // Topic prefix (optional)

// ---------- Optional API Key ----------
const char* API_KEY = "";  // For future REST API integration

// ---------- Root CA Certificate ----------
// const char* CA_CERT = 
// "-----BEGIN CERTIFICATE-----\n"
// // Drop your CA certificate content here
// "-----END CERTIFICATE-----\n";

const char *CA_CERT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDtTCCAp2gAwIBAgIUOKMAqMXFCssUwLZtEx+8/dj+d/AwDQYJKoZIhvcNAQEL
BQAwajELMAkGA1UEBhMCSUQxEDAOBgNVBAgMB0pha2FydGExEDAOBgNVBAcMB0ph
a2FydGExDTALBgNVBAoMBEVNUVgxDTALBgNVBAsMBE1RVFQxGTAXBgNVBAMMEGJy
b2tlci5hdmlzaGEuaWQwHhcNMjUxMTAyMTEzODM2WhcNMjYxMTAyMTEzODM2WjBq
MQswCQYDVQQGEwJJRDEQMA4GA1UECAwHSmFrYXJ0YTEQMA4GA1UEBwwHSmFrYXJ0
YTENMAsGA1UECgwERU1RWDENMAsGA1UECwwETVFUVDEZMBcGA1UEAwwQYnJva2Vy
LmF2aXNoYS5pZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMAvKNx4
/EBrDB/vhjL6L+hnoRuM623QFqkmje+I7YjsL0ehNTBnTCTbI0giflQlx0lE1OUC
r3HbahOFovFJGbMbLF3JgKvJ7rtmfEhaURyuSNCAVJRxmnEFt+OgB6SntSQaJ0zP
q5DT2EapWdiwbFkYsDTO6kyRtkBReZVc4rhTCaHFJnxEab3DbzVzteC2g8GSv0NX
clO3+6yi9bRh0uH0u3rfux4I9r4GMYF9vdC7NDDaVFeHIsQRXKW1dh4KajdMnJvl
K25whh4HAEryZ5JJsyCviWcmjZhvPW/WEha1ORPjpcw3dbCSa40zSQVoFKgJAPvJ
mcsCKk6TUT8ENiUCAwEAAaNTMFEwHQYDVR0OBBYEFHDNv7W0hpVlgFt82uqmGU0O
AnPuMB8GA1UdIwQYMBaAFHDNv7W0hpVlgFt82uqmGU0OAnPuMA8GA1UdEwEB/wQF
MAMBAf8wDQYJKoZIhvcNAQELBQADggEBAIOK9N+TF7t/2bsbX3RlFzKG6Nn+qkA4
KNXEbfHhgKN/xRrZWw5ay8v+mhwTKh/bgv2qb8sEVN4+79cgio+azzGGVTOsYQ0R
bWblQy40xPZSzylEHPLtxDEvy/iQLeenV3MFIGRNgzDOzum5zEH6Vna81LaGyqOf
W2fzRa0QuyV8i+NzUIDwKd2S6UBMqAqCkmoL3c8cAFXMabVuN3Lz+co0yebDX2kW
SihneS6XYxxRAgoien4o814k4rP1OYarFUNSK7JFOOj7uyAr/3VkgX3e0n1M0IfD
vzvlEDsvCUU3FP8IN5bhgF+sth/w6K4qUsmsWzdds4HJoMjFBpCQwyA=
-----END CERTIFICATE-----
)EOF";



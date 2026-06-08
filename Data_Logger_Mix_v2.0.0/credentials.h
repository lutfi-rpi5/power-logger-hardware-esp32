#pragma once

/**
 * @file credentials.h
 * @brief Credential declarations for WiFi and MQTT.
 *
 * NOTE: These declarations are from the v1.x architecture. In v2.x,
 * all credentials are stored in NVS and managed through StorageManager
 * and the web configuration interface. The static arrays declared here
 * are NO LONGER USED at runtime — they exist only as a reference
 * template and for backward compatibility with credential_example.cpp.
 *
 * @warning Do NOT commit credentials.cpp with real passwords.
 *          The .gitignore already excludes credentials.cpp.
 *          Copy credential_example.cpp → credentials.cpp and fill in values.
 */

/**
 * @struct WiFiCredential
 * @brief A single WiFi network credential for the static list (v1.x compatibility).
 */
struct WiFiCredential {
    const char* ssid;       ///< Network SSID
    const char* password;   ///< Network password
    int priority;           ///< Connection priority (higher = preferred)
};

/// Static WiFi credential list (defined in credentials.cpp).
extern WiFiCredential wifiList[];
extern const int WIFI_COUNT;  ///< Number of entries in wifiList[]

/// Static MQTT broker parameters (defined in credentials.cpp).
extern const char* MQTT_SERVER;  ///< Broker hostname
extern const int   MQTT_PORT;    ///< MQTT TCP port
extern const int   MQTTS_PORT;   ///< MQTT SSL port

extern const char* MQTT_USER;    ///< MQTT username
extern const char* MQTT_PASS;    ///< MQTT password

extern const char* CA_CERT;      ///< CA certificate for SSL verification (PEM)

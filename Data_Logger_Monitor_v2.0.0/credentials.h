#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// ============================================================
// Credential declarations — DO NOT commit real values to git
// ============================================================
// Copy `credential_example.cpp` → `credentials.cpp` and fill in
// your actual WiFi and MQTT credentials before compiling.
// `credentials.cpp` is gitignored.
// ============================================================

struct WiFiCredential {
    const char* ssid;
    const char* password;
    int priority;
};

// WiFi network list (defined in credentials.cpp)
extern WiFiCredential wifiList[];
extern const int WIFI_COUNT;

// MQTT broker (hardcoded fallback, overridden by NVS config)
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const int   MQTTS_PORT;

extern const char* MQTT_USER;
extern const char* MQTT_PASS;

// SSL CA Certificate
extern const char* CA_CERT;

#endif

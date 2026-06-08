#pragma once

struct WiFiCredential {
    const char* ssid;
    const char* password;
    int priority;
};

extern WiFiCredential wifiList[];
extern const int WIFI_COUNT;

extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const int   MQTTS_PORT;

extern const char* MQTT_USER;
extern const char* MQTT_PASS;

extern const char* CA_CERT;

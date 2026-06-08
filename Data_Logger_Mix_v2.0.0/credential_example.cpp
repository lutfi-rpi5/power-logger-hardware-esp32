#include "credentials.h"

WiFiCredential wifiList[] = {
    { "YourWiFiSSID", "YourWiFiPassword", 1 },
};

const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);

const char* MQTT_SERVER = "broker.example.com";
const int   MQTT_PORT   = 1883;
const int   MQTTS_PORT  = 8883;

const char* MQTT_USER = "";
const char* MQTT_PASS = "";

const char* CA_CERT = "";

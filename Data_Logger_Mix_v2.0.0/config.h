#pragma once

// ===================== HARDWARE PINS =====================
#define PIN_PZEM_RX1        4
#define PIN_PZEM_TX1        15
#define PIN_PZEM_RX2        17
#define PIN_PZEM_TX2        16

#define PZEM_ADDR_R         0x10
#define PZEM_ADDR_S         0x11
#define PZEM_ADDR_T         0x12
#define NUM_PHASES          3

#define PIN_SDA             21
#define PIN_SCL             22
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_I2C_ADDR       0x3C

#define PIN_BUTTON          5
#define PIN_LED             2

// ===================== TIMING ============================
#define BTN_LONG_PRESS_MS   600
#define BTN_DEBOUNCE_MS     50
#define PZEM_READ_INTERVAL_MS       1000
#define MQTT_PUBLISH_INTERVAL_MS    1000
#define WIFI_RECONNECT_INTERVAL_MS  30000
#define MQTT_RECONNECT_INTERVAL_MS  5000
#define OLED_REFRESH_INTERVAL_MS    500
#define OLED_BLINK_INTERVAL_MS      500
#define MENU_AUTO_RETURN_MS         2000
#define BOOT_DURATION_MS            3000
#define REBOOT_COUNTDOWN_SEC        3

// ===================== WATCHDOG ==========================
#define WDT_TIMEOUT_SEC             30
#define HEAP_CRITICAL_MIN_BYTES     8192

// ===================== FIRMWARE ==========================
#define FW_VERSION          "v2.1.0"
#define DEVICE_ID           "3ph-logger-001"

// ===================== FreeRTOS TASKS ====================
#define TASK_STACK_PZEM     4096
#define TASK_PRIO_PZEM      3
#define CORE_SENSOR         0
#define CORE_COMM           1

// ===================== STORAGE ===========================
#define PREF_NAMESPACE      "dlcfg"
#define MAX_WIFI_NETWORKS   5

// ===================== DEFAULT VALUES ====================
#define DEFAULT_AP_SSID     "DataLogger"
#define DEFAULT_AP_PASS     "12345678"

#define DEFAULT_MQTT_SERVER     "broker.avisha.id"
#define DEFAULT_MQTT_PORT       1883
#define DEFAULT_MQTT_SSL_PORT   8884
#define DEFAULT_MQTT_WS_PORT    8083
#define DEFAULT_MQTT_WSS_PORT   8084
#define DEFAULT_MQTT_USER       ""
#define DEFAULT_MQTT_PASS       ""
#define DEFAULT_MQTT_PREFIX     "lutpiii"
#define DEFAULT_MQTT_TOPIC      "telemetry"
#define DEFAULT_MQTT_USE_SSL    false
#define DEFAULT_MQTT_USE_WS     false

#define DEFAULT_CAL_VOLTAGE     0.0f
#define DEFAULT_CAL_CURRENT     0.0f

#define VOLTAGE_OK_MIN          180.0f
#define VOLTAGE_LOST_MAX         80.0f
#define VOLTAGE_OVER_MAX        240.0f
#define VOLTAGE_UNBALANCE_MAX     3.0f

// ===================== WEB AUTH ==========================
#define WEB_USERNAME    "ADMIN"
#define WEB_PASSWORD    "18273645"

// ===================== NTP ===============================
#define NTP_SERVER1     "pool.ntp.org"
#define NTP_SERVER2     "time.google.com"
#define NTP_TIMEOUT_MS  10000

// ===================== FAKE DATA ==========================
#define FAKE_DATA_ENABLED   false

// ===================== AP MODE ===========================
#define AP_CHANNEL      1
#define AP_MAX_CLIENTS  4

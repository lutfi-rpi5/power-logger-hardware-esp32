#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// 3-Phase Data Logger v2.0.0 — Hardcoded Configuration
// ============================================================
// This file contains programmer-only settings.
// User-configurable settings are stored in NVS (Preferences)
// and editable via the web interface.
// ============================================================

// ---------- Firmware Identity ----------
#define DEVICE_ID       "3ph-logger-001"
#define FW_VERSION      "v2.0.0"

// ---------- Hardware Pinout ----------
#define PZEM_RX1_PIN    4
#define PZEM_TX1_PIN    15
#define PZEM_RX2_PIN    17
#define PZEM_TX2_PIN    16

#define OLED_SDA_PIN    21
#define OLED_SCL_PIN    22
#define OLED_ADDR       0x3C
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

#define BUTTON_PIN      5
#define LED_BUILTIN_PIN 2

// ---------- PZEM Configuration ----------
#define NUM_PZEMS       3
#define PZEM_BAUD       9600
#define PZEM_ADDR_R     0x10
#define PZEM_ADDR_S     0x11
#define PZEM_ADDR_T     0x12

// ---------- PZEM Serial Ports ----------
// Phase R + S on UART1, Phase T on UART2
#define PZEM_SERIAL_1  1   // HardwareSerial(1)
#define PZEM_SERIAL_2  2   // HardwareSerial(2)

// ---------- Default Thresholds (used on factory reset) ----------
#define DEFAULT_VOLTAGE_LOST_MAX    80.0f
#define DEFAULT_VOLTAGE_UNDER_MIN   180.0f
#define DEFAULT_VOLTAGE_OVER_MIN    240.0f
#define DEFAULT_UNBALANCE_MAX       3.0f

// ---------- Timing (milliseconds) ----------
#define MQTT_PUBLISH_INTERVAL_MS    2000
#define OLED_REFRESH_INTERVAL_MS    500
#define WIFI_RECONNECT_INTERVAL_MS  30000
#define MQTT_RECONNECT_INTERVAL_MS  10000
#define BUTTON_LONG_PRESS_MS        600
#define BUTTON_DEBOUNCE_MS          50

// ---------- WiFi AP Mode ----------
#define AP_SSID         "3PH-LOGGER"
#define AP_PASS         "12345678"
#define AP_CHANNEL      1
#define AP_MAX_CLIENTS  4

// ---------- Web Server Auth ----------
#define WEB_USERNAME    "ADMIN"
#define WEB_PASSWORD    "18273645"

// ---------- NTP ----------
#define NTP_SERVER1     "pool.ntp.org"
#define NTP_SERVER2     "time.google.com"
#define NTP_TIMEOUT_MS  10000

// ---------- Watchdog ----------
#define WDT_TIMEOUT_MS  10000

// ---------- MQTT Topic (default fallback) ----------
#define MQTT_DEFAULT_PREFIX   "lutpiii"
#define MQTT_DEFAULT_TOPIC    "telemetry"

#endif

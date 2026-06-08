#pragma once

/******************************************************
 * Project   : 3-Phase Data Logger ESP32
 * File      : config.h
 * Author    : Muhammad Lutfi Nur Anendi
 * Version   : v2.1.0
 *
 * Description:
 *   Central configuration file. All compile-time constants
 *   live here. Runtime-configurable settings (WiFi, MQTT,
 *   calibration, thresholds) are loaded from EEPROM via StorageManager.
 *
 *   SECTION A – HARDCODED (programmer only, requires reflash)
 *   SECTION B – RUNTIME DEFAULTS (overridden by EEPROM values)
 ******************************************************/

// ============================================================
// SECTION A  :  HARDWARE PINS  (HARDCODED)
// ============================================================

// --- PZEM-004T Serial ---
#define PIN_PZEM_RX1        4
#define PIN_PZEM_TX1        15
#define PIN_PZEM_RX2        17
#define PIN_PZEM_TX2        16

// PZEM Modbus addresses
#define PZEM_ADDR_R         0x10    // Line R (Phase 1)
#define PZEM_ADDR_S         0x11    // Line S (Phase 2)
#define PZEM_ADDR_T         0x12    // Line T (Phase 3)
#define NUM_PHASES          3

// --- OLED SSD1306 (I2C) ---
#define PIN_SDA             21
#define PIN_SCL             22
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_I2C_ADDR       0x3C

// --- Push Button & LED ---
#define PIN_BUTTON          5       // Normally Open, active LOW (INPUT_PULLUP)
#define PIN_LED             2       // Built-in LED (active HIGH)

// ============================================================
// SECTION A  :  TIMING CONSTANTS  (HARDCODED)
// ============================================================

// Button
#define BTN_LONG_PRESS_MS   600     // Long press threshold (ms)
#define BTN_DEBOUNCE_MS     50      // Debounce time (ms)

// PZEM reading task
#define PZEM_READ_INTERVAL_MS       1000    // Read every 1 second

// MQTT publishing interval
#define MQTT_PUBLISH_INTERVAL_MS    1000    // Publish JSON every 1 second

// WiFi reconnect attempt interval
#define WIFI_RECONNECT_INTERVAL_MS  30000

// MQTT reconnect attempt interval
#define MQTT_RECONNECT_INTERVAL_MS  5000

// OLED refresh interval (monitoring pages)
#define OLED_REFRESH_INTERVAL_MS    500 // 800 default

// OLED warning blink interval for LOST/UNDER/OVER status and [!] icon
#define OLED_BLINK_INTERVAL_MS      500     // ms – adjustable here

// Auto-return delay for result screens (Reset kWh result)
#define MENU_AUTO_RETURN_MS         2000

// Reboot countdown duration (seconds)
#define REBOOT_COUNTDOWN_SEC        3

// ============================================================
// SECTION A  :  SELF-HEALING WATCHDOG  (HARDCODED)
// ============================================================

#define WDT_TIMEOUT_SEC             30      // Task watchdog timeout (seconds)
#define HEAP_CRITICAL_MIN_BYTES     8192    // Reboot if free heap below this

// ============================================================
// SECTION A  :  FIRMWARE METADATA  (HARDCODED)
// ============================================================

#define FW_VERSION          "v2.1.0"
#define DEVICE_ID           "3ph-logger-001"

// ============================================================
// SECTION A  :  FreeRTOS TASK CONFIG  (HARDCODED)
// ============================================================

#define TASK_STACK_PZEM     4096
#define TASK_STACK_WIFI     4096
#define TASK_STACK_MQTT     6144
#define TASK_PRIO_PZEM      3       // Highest – real-time sensor read
#define TASK_PRIO_WIFI      2
#define TASK_PRIO_MQTT      1
#define CORE_SENSOR         0       // Core 0: sensor acquisition
#define CORE_COMM           1       // Core 1: comms & UI (main loop)

// ============================================================
// SECTION A  :  STORAGE KEYS  (HARDCODED)
// ============================================================

#define PREF_NAMESPACE      "dlcfg"
#define MAX_WIFI_NETWORKS   5

// ============================================================
// SECTION B  :  RUNTIME DEFAULTS  (can be overridden by EEPROM)
// ============================================================

// Access Point (ESP32 config mode)
#define DEFAULT_AP_SSID     "DataLogger"
#define DEFAULT_AP_PASS     "12345678"

// MQTT defaults
#define DEFAULT_MQTT_SERVER     "broker.avisha.id"
#define DEFAULT_MQTT_PORT       1883
#define DEFAULT_MQTT_SSL_PORT   8884
#define DEFAULT_MQTT_WS_PORT    8083
#define DEFAULT_MQTT_WSS_PORT   8084
#define DEFAULT_MQTT_USER       ""
#define DEFAULT_MQTT_PASS       ""
#define DEFAULT_MQTT_TOPIC      ""
#define DEFAULT_MQTT_USE_SSL    false
#define DEFAULT_MQTT_USE_WS     false

// Calibration offsets (additive, applied after raw reading)
#define DEFAULT_CAL_VOLTAGE     0.0f
#define DEFAULT_CAL_CURRENT     0.0f

// ─── Line Status Voltage Thresholds (default, overrideable via EEPROM) ────────
// These serve as compile-time defaults. Actual runtime values are stored
// in EEPROM and are configurable via the PZEM Calibration web page.
#define VOLTAGE_OK_MIN          180.0f  // Below this → UNDER
#define VOLTAGE_LOST_MAX         80.0f  // Below this → LOST
#define VOLTAGE_OVER_MAX        240.0f  // Above this → OVER
#define VOLTAGE_UNBALANCE_MAX     3.0f  // Above this (%) → OVER UNBALANCE

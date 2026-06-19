#pragma once

/**
 * @file config.h
 * @brief Hardware pin definitions, timing constants, thresholds, and firmware metadata.
 *
 * This is the single configuration header for the entire firmware. All
 * hardware-dependent and system-level constants are defined here. Changes
 * to pins or thresholds should only be made in this file.
 *
 * @note PZEM-004T v3.0 uses Modbus RTU over TTL serial. Two phases (R, S)
 *       share UART1 with individual Modbus addresses. Phase T uses UART2.
 * @note OLED operates at 100 kHz I2C (default Wire speed).
 * @note Button uses INPUT_PULLUP — external pull-down resistor is NOT required.
 */

// ======================================================================
// HARDWARE PIN ASSIGNMENTS (ESP32 DevKit V1 30-pin)
// ======================================================================
// PZEM-004T v3.0 uses 3.3V TTL serial at 115200 baud.
// UART1 is shared by phases R and S; each has a unique Modbus address.
// UART2 is dedicated to phase T.
#define PIN_PZEM_RX1        4   ///< UART1 RX pin (GPIO4)  — shared by PZEM R & S
#define PIN_PZEM_TX1        15  ///< UART1 TX pin (GPIO15) — shared by PZEM R & S
#define PIN_PZEM_RX2        17  ///< UART2 RX pin (GPIO17) — PZEM T only
#define PIN_PZEM_TX2        16  ///< UART2 TX pin (GPIO16) — PZEM T only

/// @name PZEM-004T Modbus Addresses
/// Addresses are set via the standalone setAddressPZEM sketch.
/// @{
#define PZEM_ADDR_R         0x10  ///< Phase R Modbus address (16 decimal)
#define PZEM_ADDR_S         0x11  ///< Phase S Modbus address (17 decimal)
#define PZEM_ADDR_T         0x12  ///< Phase T Modbus address (18 decimal)
#define NUM_PHASES          3     ///< Number of monitored phases (R, S, T)
/// @}

/// @name OLED Display (SSD1306 128x64)
/// I2C interface at address 0x3C, standard 100 kHz bus.
/// @{
#define PIN_SDA             21  ///< I2C data line (GPIO21)
#define PIN_SCL             22  ///< I2C clock line (GPIO22)
#define OLED_WIDTH          128 ///< Display width in pixels
#define OLED_HEIGHT         64  ///< Display height in pixels
#define OLED_I2C_ADDR       0x3C ///< SSD1306 I2C address (default)
/// @}

/// @name User Input & Indicator
/// @{
#define PIN_BUTTON          5  ///< Push button GPIO (NO, INPUT_PULLUP). Short=<600ms, Long=≥600ms
#define PIN_LED             18  ///< Built-in LED GPIO (active HIGH). Used for status patterns
/// @}

// ======================================================================
// TIMING CONSTANTS (all in milliseconds unless stated)
// ======================================================================
#define BTN_LONG_PRESS_MS           600   ///< Minimum hold duration to register a long press (ms)
#define BTN_DEBOUNCE_MS             50    ///< Button debounce window — ignores noise shorter than this (ms)
#define PZEM_READ_INTERVAL_MS       1000  ///< Interval between PZEM sensor read cycles (ms, equals 1 Hz)
#define MQTT_PUBLISH_INTERVAL_MS    1000  ///< Minimum interval between MQTT telemetry publishes (ms)
#define WIFI_RECONNECT_INTERVAL_MS  30000 ///< Retry interval for WiFi reconnection attempts (ms, 30 s)
#define MQTT_RECONNECT_INTERVAL_MS  5000  ///< Cooldown between MQTT connection retries (ms, 5 s)
#define OLED_REFRESH_INTERVAL_MS    500   ///< OLED monitoring page refresh rate (ms)
#define OLED_BLINK_INTERVAL_MS      500   ///< Blink period for warning status indicators (ms)
#define MENU_AUTO_RETURN_MS         2000  ///< Auto-return delay from result screens back to menu (ms)
#define BOOT_DURATION_MS            3000  ///< Boot animation/progress bar duration before entering monitoring (ms)
#define REBOOT_COUNTDOWN_SEC        3     ///< Countdown duration before forced reboot (seconds)

// ======================================================================
// WATCHDOG & HEAP PROTECTION
// ======================================================================
#define WDT_TIMEOUT_SEC             30    ///< Task Watchdog Timer timeout (seconds). Set high to accommodate
                                          ///< potential MQTT TCP blocking up to ~10 s.
#define HEAP_CRITICAL_MIN_BYTES     8192  ///< Minimum free heap before forced reboot (bytes).
                                          ///< ESP32 typical headroom: ~40-80 KB idle.

// ======================================================================
// FIRMWARE IDENTITY
// ======================================================================
#define FW_VERSION          "v2.1.0"  ///< Human-readable firmware version string
#define DEVICE_ID           "3ph-logger-002" ///< Unique device identifier used in MQTT and logging 3ph-logger-xxx

// ======================================================================
// FreeRTOS TASK CONFIGURATION
// ======================================================================
#define TASK_STACK_PZEM     4096  ///< Stack depth (words) for the PZEM reader task on Core 0
#define TASK_PRIO_PZEM      3     ///< Priority for the PZEM reader task (higher = more CPU)
#define CORE_SENSOR         0     ///< CPU core dedicated to sensor acquisition (Pro CPU)
#define CORE_COMM           1     ///< CPU core dedicated to communication & display (App CPU)

// ======================================================================
// NON-VOLATILE STORAGE (NVS / Preferences)
// ======================================================================
#define PREF_NAMESPACE      "dlcfg"  ///< NVS namespace key for all persistent config data
#define MAX_WIFI_NETWORKS   5        ///< Maximum number of stored WiFi network entries

// ======================================================================
// DEFAULT VALUES (used when NVS is empty or after factory reset)
// ======================================================================
#define DEFAULT_AP_SSID     "DataLogger"  ///< Default AP SSID for the web config portal
#define DEFAULT_AP_PASS     "12345678"    ///< Default AP password (minimum 8 chars for ESP32)

#define DEFAULT_MQTT_SERVER     "broker.avisha.id" ///< Default MQTT broker hostname
#define DEFAULT_MQTT_PORT       1883   ///< Default MQTT TCP port (non-SSL)
#define DEFAULT_MQTT_SSL_PORT   8884   ///< Default MQTT SSL port
#define DEFAULT_MQTT_WS_PORT    8083   ///< Default MQTT WebSocket port (reference only, not used by PubSubClient)
#define DEFAULT_MQTT_WSS_PORT   8084   ///< Default MQTT WebSocket Secure port (reference only)
#define DEFAULT_MQTT_USER       ""     ///< Default MQTT username (empty = anonymous)
#define DEFAULT_MQTT_PASS       ""     ///< Default MQTT password (empty = no auth)
#define DEFAULT_MQTT_PREFIX     "lutpiii" ///< Default MQTT topic prefix (used as: prefix/topic/device_id)
#define DEFAULT_MQTT_TOPIC      "telemetry"  ///< Default MQTT topic name
#define DEFAULT_MQTT_USE_SSL    false  ///< Default: plain TCP (not SSL)
#define DEFAULT_MQTT_USE_WS     false  ///< Default: raw TCP (not WebSocket)

#define DEFAULT_CAL_VOLTAGE     0.0f   ///< Default voltage offset (Volts) — no adjustment
#define DEFAULT_CAL_CURRENT     0.0f   ///< Default current offset (Amperes) — no adjustment

// ======================================================================
// LINE STATUS THRESHOLDS (configurable via web UI)
// ======================================================================
#define VOLTAGE_OK_MIN          180.0f ///< Voltage below this → UNDER status (V)
#define VOLTAGE_LOST_MAX         80.0f ///< Voltage below this → LOST status (V)
#define VOLTAGE_OVER_MAX        240.0f ///< Voltage above this → OVER status (V)
#define VOLTAGE_UNBALANCE_MAX     3.0f ///< Unbalance above this → OLED warning [!] (%)

// ======================================================================
// WEB SERVER AUTHENTICATION
// ======================================================================
#define WEB_USERNAME    "ADMIN"     ///< HTTP Basic Auth username for web config portal
#define WEB_PASSWORD    "18273645"  ///< HTTP Basic Auth password for web config portal

// ======================================================================
// NTP (NETWORK TIME PROTOCOL) SETTINGS
// ======================================================================
#define NTP_SERVER1     "pool.ntp.org"      ///< Primary NTP server
#define NTP_SERVER2     "time.google.com"   ///< Fallback NTP server
#define NTP_TIMEOUT_MS  10000               ///< Maximum time to wait for NTP response (ms)

// ======================================================================
// DEVELOPMENT & DEBUGGING
// ======================================================================
#define FAKE_DATA_ENABLED   false  ///< Enable synthetic PZEM data for testing without hardware.
                                   ///< Set to false for production deployment with real PZEM sensors.

// ======================================================================
// SOFT-AP (ACCESS POINT) SETTINGS
// ======================================================================
#define AP_CHANNEL      1     ///< WiFi channel for the config AP (1–11)
#define AP_MAX_CLIENTS  4     ///< Maximum simultaneous clients on the config AP

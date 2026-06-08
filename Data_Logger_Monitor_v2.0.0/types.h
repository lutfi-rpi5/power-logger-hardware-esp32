#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

// ============================================================
// Shared structs and enums used across all modules
// ============================================================

// ---------- Line Status Enum ----------
enum class LineStatus : uint8_t {
    LOST  = 0,  // Voltage below LOST threshold (sensor missing / probe error)
    UNDER = 1,  // Voltage below OK minimum but above LOST
    OK    = 2,  // Voltage within normal range
    OVER  = 3   // Voltage above OVER threshold
};

// ---------- App State Enum ----------
enum class AppState : uint8_t {
    BOOTING      = 0,
    MONITORING   = 1,  // Normal monitoring mode
    MENU         = 2,  // OLED menu navigation
    CONFIG_AP    = 3,  // AP + web server active
    REBOOTING    = 4   // Countdown to system reboot
};

// ---------- OLED Display Mode ----------
enum class OLEDMode : uint8_t {
    MONITOR_PAGE_1 = 0,
    MONITOR_PAGE_2 = 1,
    MONITOR_PAGE_3 = 2,
    MENU_MAIN      = 10,
    MENU_CONFIG    = 11,
    MENU_RESET_KWH = 12,
    MENU_REBOOT    = 13,
    MENU_FACTORY   = 14
};

// ---------- Per-Phase Data ----------
struct PhaseData {
    bool    valid;
    float   voltage;     // V
    float   current;     // A
    float   power;       // W   (active)
    float   apparent;    // VA
    float   reactive;    // VAr
    float   pf;          // power factor
    float   frequency;   // Hz
    float   energy;      // Wh  (cumulative)
    LineStatus status;
};

// ---------- Calibration Offsets (per phase) ----------
struct CalibrationData {
    float voltageOffset[3];  // additive offset per phase (R, S, T)
    float currentOffset[3];  // additive offset per phase
};

// ---------- Line Status Thresholds ----------
struct ThresholdData {
    float voltageLost;    // Below this -> LOST
    float voltageUnder;   // Below this -> UNDER (but above LOST range)
    float voltageOver;    // Above this -> OVER
    float unbalanceMax;   // Percentage, above this -> warning [!]
};

// ---------- MQTT Configuration (stored in NVS) ----------
struct MQTTConfig {
    char server[64];
    uint16_t port;
    uint16_t sslPort;
    char user[32];
    char pass[32];
    char prefix[32];
    char topic[32];
    bool useSSL;
    bool useWS;
    char caCert[2048];
};

// ---------- WiFi Network Entry (stored in NVS) ----------
struct WiFiEntry {
    char ssid[32];
    char password[64];
};

#endif

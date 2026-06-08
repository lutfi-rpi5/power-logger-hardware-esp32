/******************************************************
 * Project   : 3-Phase Data Logger ESP32
 * File      : Data_Logger_Mix_v2.0.0.ino
 * Author    : Muhammad Lutfi Nur Anendi
 * Version   : v2.1.0
 * Board     : ESP32 Devkit V1 (30-pin)
 * Package   : esp32 by Espressif System: ESP32 Dev Module v3.3.0
 *
 * Description:
 *   Main orchestrator — mix of alternate + opencode features:
 *   - Data acquisition: opencode PZEM read + Calibration class
 *   - JSON: opencode JSONBuilder with seq/valid fields
 *   - Diagnostics: opencode tagged logging (replaces Serial.print)
 *   - NTP sync via TaskManager
 *   - Web auth/login from opencode
 *   - Menu/OLED/Button/LED/WiFi/MQTT from alternate with diagnostics
 *
 * Hardware:
 *   - ESP32 Devkit V1 30-pin
 *   - 3x PZEM-004T v3.0 (100A), Modbus addresses 0x10/0x11/0x12
 *   - OLED SSD1306 0.96" I2C (128x64)
 *   - Push button (NO) on GPIO 5
 *   - Built-in LED GPIO 2
 ******************************************************/

// ════════════════════════════════════════════════════
// INCLUDES
// ════════════════════════════════════════════════════

#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "esp_task_wdt.h"

#include "config.h"
#include "types.h"
#include "diagnostics.h"
#include "storage_manager.h"
#include "button_manager.h"
#include "led_signal.h"
#include "data_acquisition.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "display_oled.h"
#include "menu_manager.h"
#include "webserver_manager.h"
#include "task_manager.h"
#include "credentials.h"

// ════════════════════════════════════════════════════
// GLOBAL SHARED STATE
// ════════════════════════════════════════════════════

SystemState       gState;
SemaphoreHandle_t gStateMutex;

// ════════════════════════════════════════════════════
// HARDWARE SERIAL
// ════════════════════════════════════════════════════

HardwareSerial PZEMSerial1(1);   // UART1: Line R & S
HardwareSerial PZEMSerial2(2);   // UART2: Line T

// ════════════════════════════════════════════════════
// PZEM ARRAY
// ════════════════════════════════════════════════════

PZEM004Tv30 pzems[NUM_PHASES] = {
    PZEM004Tv30(PZEMSerial1, PIN_PZEM_RX1, PIN_PZEM_TX1, PZEM_ADDR_R),
    PZEM004Tv30(PZEMSerial1, PIN_PZEM_RX1, PIN_PZEM_TX1, PZEM_ADDR_S),
    PZEM004Tv30(PZEMSerial2, PIN_PZEM_RX2, PIN_PZEM_TX2, PZEM_ADDR_T)
};

// ════════════════════════════════════════════════════
// MODULE INSTANCES
// ════════════════════════════════════════════════════

StorageManager   storage;
DataAcquisition  daq(pzems, storage);
WiFiManager      wifiMgr(storage);
MQTTManager      mqttMgr(storage);
DisplayOLED      display;
Button           button(PIN_BUTTON, BTN_LONG_PRESS_MS, BTN_DEBOUNCE_MS);
MenuSystem       menu(button, display);
LEDSignal        led(PIN_LED);
WebServerManager webServer(storage);
TaskManager      taskMgr;

// ════════════════════════════════════════════════════
// FORWARD DECLARATIONS
// ════════════════════════════════════════════════════

static void _wireCallbacks();
static void _printBanner();
static void _selfHealingCheck();
static void _checkConnectionSignals();
static void _initWatchdog();
static void _handleBoot();

static unsigned long _bootStartMs = 0;
static bool          _bootComplete = false;

// ════════════════════════════════════════════════════
// SETUP
// ════════════════════════════════════════════════════

void setup() {
    // ── Diagnostics (includes Serial.begin) ──────────
    diag.begin(LOG_INFO);
    _printBanner();

    // ── Mutex ────────────────────────────────────────
    gStateMutex = xSemaphoreCreateMutex();
    configASSERT(gStateMutex != nullptr);

    // ── LED ─────────────────────────────────────────
    led.begin();

    // ── OLED ─────────────────────────────────────────
    display.begin();

    // ── Storage (NVS) ────────────────────────────────
    storage.begin();
    storage.dump();

    // ── Propagate thresholds to gState ───────────────
    {
        ThresholdConfig thr = storage.getThresholds();
        updateState([&](SystemState& s) {
            s.thresholdUnbalanceMax = thr.unbalanceMax;
        });
    }

    // ── Task Manager (NTP, scheduling) ───────────────
    taskMgr.begin();

    // ── Wire callbacks ───────────────────────────────
    _wireCallbacks();

    // ── WiFi (non-blocking) ─────────────────────────
    wifiMgr.begin();

    // ── MQTT ─────────────────────────────────────────
    mqttMgr.begin();

    // ── Web Server (routes only, not active) ─────────
    webServer.begin();

    // ── Menu ─────────────────────────────────────────
    menu.begin();

    // Populate AP info
    APConfig ap = storage.getAPConfig();
    menu.setAPInfo(ap.ssid, ap.pass, "0.0.0.0");

    // ── Watchdog ────────────────────────────────────
    _initWatchdog();

    // ── Sensor Task on Core 0 ───────────────────────
    daq.begin();

    // ── Boot complete ────────────────────────────────
    led.play(LEDSignal::Pattern::IDLE);
    _bootStartMs = millis();
    _bootComplete = false;

    diag.info("MAIN", "Setup complete. Entering boot screen on Core 1.");
}

// ════════════════════════════════════════════════════
// LOOP (Core 1)
// ════════════════════════════════════════════════════

void loop() {
    esp_task_wdt_reset();

    // ── Boot screen animation ─────────────────────────
    if (!_bootComplete) {
        _handleBoot();
        vTaskDelay(pdMS_TO_TICKS(5));
        return;
    }

    // ── NTP sync (periodic, non-blocking) ────────────
    static unsigned long lastNtpCheck = 0;
    if (taskMgr.isTime(lastNtpCheck, 60000UL)) {
        if (WiFi.isConnected()) {
            taskMgr.syncNTP();
        }
    }

    // ── WiFi ─────────────────────────────────────────
    wifiMgr.tick();

    // ── MQTT ─────────────────────────────────────────
    mqttMgr.tick();

    // ── Menu + Display ───────────────────────────────
    {
        SystemState snap = getStateCopy();
        if (snap.webServerActive) {
            menu.setAPInfo(snap.apSSID, snap.apPass, snap.apIP);
        }
        menu.tick(snap);
    }

    // ── LED ──────────────────────────────────────────
    led.tick();
    _checkConnectionSignals();

    // ── Self-healing ─────────────────────────────────
    _selfHealingCheck();

    vTaskDelay(pdMS_TO_TICKS(5));
}

// ════════════════════════════════════════════════════
// BOOT HANDLER
// ════════════════════════════════════════════════════

static void _handleBoot() {
    unsigned long elapsed = millis() - _bootStartMs;
    int progress = (elapsed * 100) / BOOT_DURATION_MS;
    if (progress > 100) progress = 100;

    display.drawBoot(progress);

    if (elapsed >= BOOT_DURATION_MS) {
        _bootComplete = true;
        display.clear();
        diag.info("MAIN", "Boot complete, entering monitoring mode.");
    }
}

// ════════════════════════════════════════════════════
// WATCHDOG INIT
// ════════════════════════════════════════════════════

static void _initWatchdog() {
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms     = (uint32_t)(WDT_TIMEOUT_SEC * 1000),
        .idle_core_mask = 0,
        .trigger_panic  = true
    };
    esp_err_t recfgRet = esp_task_wdt_reconfigure(&wdt_config);
    if (recfgRet == ESP_OK) {
        diag.info("WDT", "Timeout reconfigured to %ds.", WDT_TIMEOUT_SEC);
    } else if (recfgRet == ESP_ERR_INVALID_STATE) {
        diag.info("WDT", "WDT not running, skipping reconfigure.");
    } else {
        diag.warn("WDT", "reconfigure warning: %s", esp_err_to_name(recfgRet));
    }

    esp_err_t addRet = esp_task_wdt_add(xTaskGetCurrentTaskHandle());
    if (addRet == ESP_OK) {
        diag.info("WDT", "Loop task subscribed. Effective timeout: %ds", WDT_TIMEOUT_SEC);
    } else if (addRet == ESP_ERR_INVALID_ARG) {
        diag.info("WDT", "Loop task already subscribed (OK).");
    } else {
        diag.warn("WDT", "esp_task_wdt_add: %s", esp_err_to_name(addRet));
    }
}

// ════════════════════════════════════════════════════
// SELF-HEALING
// ════════════════════════════════════════════════════

static void _selfHealingCheck() {
    static unsigned long _lastHeapWarnMs = 0;
    uint32_t freeHeap = ESP.getFreeHeap();

    if (freeHeap < HEAP_CRITICAL_MIN_BYTES) {
        diag.error("WDT", "CRITICAL: free heap %u bytes < threshold %u — rebooting!",
                    freeHeap, (uint32_t)HEAP_CRITICAL_MIN_BYTES);
        delay(200);
        ESP.restart();
    }

    unsigned long now = millis();
    if (freeHeap < (HEAP_CRITICAL_MIN_BYTES * 2) && (now - _lastHeapWarnMs > 60000UL)) {
        _lastHeapWarnMs = now;
        diag.warn("WDT", "WARNING: free heap low: %u bytes", freeHeap);
    }
}

// ════════════════════════════════════════════════════
// CALLBACK WIRING
// ════════════════════════════════════════════════════

static void _wireCallbacks() {

    menu.onResetKwh([&]() -> bool {
        diag.info("MAIN", "Reset kWh triggered from menu.");
        led.play(LEDSignal::Pattern::RESET_KWH);
        bool ok = daq.resetAllEnergy();
        diag.info("MAIN", "Reset result: %s", ok ? "OK" : "FAIL");
        return ok;
    });

    menu.onWebToggle([&](bool activate) {
        if (activate) {
            webServer.activate();
            APConfig ap = storage.getAPConfig();
            String ip = webServer.getIP();
            menu.setAPInfo(ap.ssid, ap.pass, ip.c_str());
            diag.info("MAIN", "WebServer ON  IP: %s", ip.c_str());
            led.play(LEDSignal::Pattern::WIFI_OK);
        } else {
            webServer.deactivate();
            menu.setAPInfo("", "", "0.0.0.0");
            diag.info("MAIN", "WebServer OFF");
        }
        menu.requestRedraw();
    });

    webServer.onCalibrationSaved = [&]() {
        diag.info("MAIN", "Calibration updated → reloading DAQ offsets.");
        daq.reloadCalibration();
    };

    webServer.onMQTTSaved = [&]() {
        diag.info("MAIN", "MQTT config updated → reconnecting.");
        mqttMgr.reloadConfig();
    };

    webServer.onThresholdSaved = [&]() {
        diag.info("MAIN", "Thresholds updated → reloading DAQ thresholds.");
        daq.reloadThresholds();
    };
}

// ════════════════════════════════════════════════════
// CONNECTION SIGNALS
// ════════════════════════════════════════════════════

static void _checkConnectionSignals() {
    static bool prevWifi = false;
    static bool prevMQTT = false;

    bool nowWifi = wifiMgr.isConnected();
    bool nowMQTT = mqttMgr.isConnected();

    if (!prevWifi && nowWifi) {
        led.play(LEDSignal::Pattern::WIFI_OK);
        diag.info("MAIN", "WiFi connected → LED signal");
        wifiMgr.printStatus();
    }
    if (!prevMQTT && nowMQTT) {
        led.play(LEDSignal::Pattern::MQTT_OK);
        diag.info("MAIN", "MQTT connected → LED signal");
    }

    prevWifi = nowWifi;
    prevMQTT = nowMQTT;
}

// ════════════════════════════════════════════════════
// BANNER
// ════════════════════════════════════════════════════

static void _printBanner() {
    diag.info("MAIN", "╔══════════════════════════════════════════╗");
    diag.info("MAIN", "║     3-Phase Data Logger  " FW_VERSION "          ║");
    diag.info("MAIN", "║     ESP32  |  3× PZEM-004T  |  MQTT      ║");
    diag.info("MAIN", "║     Muhammad Lutfi Nur Anendi            ║");
    diag.info("MAIN", "╚══════════════════════════════════════════╝");
    diag.info("MAIN", "Device ID : %s", DEVICE_ID);
    diag.info("MAIN", "Core freq : %d MHz", getCpuFrequencyMhz());
    diag.info("MAIN", "Free heap : %d bytes", ESP.getFreeHeap());
}

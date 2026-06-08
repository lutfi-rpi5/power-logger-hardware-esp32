/**
 * @file Data_Logger_Mix_v2.0.0.ino
 * @brief Main firmware entry point for the 3-Phase Power Data Logger ESP32.
 *
 * This is the orchestrator for the entire firmware. It:
 * 1. Initialises all hardware and software modules in setup().
 * 2. Runs the main control loop on Core 1 (communication + display).
 * 3. Starts the PZEM sensor reading task on Core 0 (data acquisition).
 *
 * Dual-core architecture:
 * ```
 * Core 0 (CORE_SENSOR): DataAcquisition task — 1 Hz PZEM read → calibrate → gState
 * Core 1 (CORE_COMM):   loop() — WiFi → MQTT → OLED → Menu → LED → Self-heal
 * ```
 *
 * Shared state model:
 * - SystemState gState is the single source of truth for all live data.
 * - All reads go through getStateCopy() (50 ms mutex timeout).
 * - All writes go through updateState() lambda (100 ms mutex timeout).
 *
 * Hardware:
 *   Board: ESP32 DevKit V1 (30-pin)
 *   Package: ESP32 by Espressif v3.3.0 (Arduino-ESP32)
 *   3× PZEM-004T v3.0 (Modbus addresses 0x10/0x11/0x12)
 *   SSD1306 OLED 128×64 I2C (0x3C)
 *   Push button GPIO5 (INPUT_PULLUP)
 *   Built-in LED GPIO2 (active HIGH)
 *
 * @author    Muhammad Lutfi Nur Anendi
 * @version   v2.1.0
 * @date      2025-2026
 */

// ═══════════════════════════════════════════════════════════════════
// INCLUDES
// ═══════════════════════════════════════════════════════════════════

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

// ═══════════════════════════════════════════════════════════════════
// GLOBAL SHARED STATE
// ═══════════════════════════════════════════════════════════════════

/// Global device state — shared between Core 0 (sensor) and Core 1 (loop).
/// All access must go through getStateCopy() / updateState().
SystemState       gState;

/// FreeRTOS binary semaphore guarding gState.
/// Created in setup(), used by getStateCopy() and updateState().
SemaphoreHandle_t gStateMutex;

// ═══════════════════════════════════════════════════════════════════
// HARDWARE SERIAL PORTS
// ═══════════════════════════════════════════════════════════════════

/// UART1 (pins 4/15): shared by PZEM phase R (0x10) and phase S (0x11).
/// Both use the same UART but distinct Modbus addresses.
HardwareSerial PZEMSerial1(1);

/// UART2 (pins 17/16): dedicated to PZEM phase T (0x12).
HardwareSerial PZEMSerial2(2);

// ═══════════════════════════════════════════════════════════════════
// PZEM-004T ARRAY
// ═══════════════════════════════════════════════════════════════════

/// Three PZEM-004T v3.0 instances, one per phase.
/// The first two share PZEMSerial1 (UART1); the third uses PZEMSerial2 (UART2).
PZEM004Tv30 pzems[NUM_PHASES] = {
    PZEM004Tv30(PZEMSerial1, PIN_PZEM_RX1, PIN_PZEM_TX1, PZEM_ADDR_R),
    PZEM004Tv30(PZEMSerial1, PIN_PZEM_RX1, PIN_PZEM_TX1, PZEM_ADDR_S),
    PZEM004Tv30(PZEMSerial2, PIN_PZEM_RX2, PIN_PZEM_TX2, PZEM_ADDR_T)
};

// ═══════════════════════════════════════════════════════════════════
// MODULE INSTANCES (constructed in dependency-safe order)
// ═══════════════════════════════════════════════════════════════════

StorageManager   storage;     ///< NVS persistent config (must be first)
DataAcquisition  daq(pzems, storage);   ///< Core 0 PZEM reader task
WiFiManager      wifiMgr(storage);       ///< Async WiFi connection manager
MQTTManager      mqttMgr(storage);       ///< Non-blocking MQTT client
DisplayOLED      display;               ///< SSD1306 OLED driver
Button           button(PIN_BUTTON, BTN_LONG_PRESS_MS, BTN_DEBOUNCE_MS);  ///< Single button
MenuSystem       menu(button, display);  ///< OLED menu state machine
LEDSignal        led(PIN_LED);          ///< Built-in LED patterns
WebServerManager webServer(storage);    ///< Web config server
TaskManager      taskMgr;               ///< NTP sync + scheduler utility

// ═══════════════════════════════════════════════════════════════════
// FORWARD DECLARATIONS (static helper functions)
// ═══════════════════════════════════════════════════════════════════

static void _wireCallbacks();             ///< Connect menu/web callbacks to modules
static void _printBanner();               ///< Print firmware banner to serial
static void _selfHealingCheck();          ///< Heap-critical watchdog
static void _checkConnectionSignals();    ///< LED signals on WiFi/MQTT connect
static void _initWatchdog();              ///< Configure Task Watchdog Timer
static void _handleBoot();                ///< Boot screen animation

static unsigned long _bootStartMs = 0;    ///< millis() when setup finished
static bool          _bootComplete = false;  ///< Boot screen has finished

// ═══════════════════════════════════════════════════════════════════
// SETUP (runs once on Core 1 at boot)
// ═══════════════════════════════════════════════════════════════════

void setup() {
    // ── Step 1: Diagnostics (initialises Serial at 115200 baud) ─────
    diag.begin(LOG_INFO);
    _printBanner();

    // ── Step 2: Create mutex for shared state protection ────────────
    gStateMutex = xSemaphoreCreateMutex();
    configASSERT(gStateMutex != nullptr);

    // ── Step 3: Hardware initialisation ──────────────────────────────
    led.begin();         // Built-in LED (initially off)
    display.begin();     // SSD1306 OLED (shows boot screen at 0%)

    // ── Step 4: Storage (NVS Preferences) ───────────────────────────
    storage.begin();     // Opens "dlcfg" namespace
    storage.dump();      // Log current config at INFO level

    // ── Step 5: Propagate initial thresholds to gState ──────────────
    {
        ThresholdConfig thr = storage.getThresholds();
        updateState([&](SystemState& s) {
            s.thresholdUnbalanceMax = thr.unbalanceMax;
        });
    }

    // ── Step 6: Task Manager (NTP + scheduling) ─────────────────────
    taskMgr.begin();

    // ── Step 7: Wire inter-module callbacks ─────────────────────────
    _wireCallbacks();

    // ── Step 8: Network services (non-blocking, async) ──────────────
    wifiMgr.begin();     // STA mode, starts scan+connect
    mqttMgr.begin();     // Load config, idle until tick()
    webServer.begin();   // Register routes, NOT active yet

    // ── Step 9: OLED menu ───────────────────────────────────────────
    menu.begin();        // Starts in MONITORING mode
    // Populate initial AP info (default "DataLogger" before activation)
    APConfig ap = storage.getAPConfig();
    menu.setAPInfo(ap.ssid, ap.pass, "0.0.0.0");

    // ── Step 10: Watchdog ───────────────────────────────────────────
    _initWatchdog();

    // ── Step 11: Start Core 0 sensor task ───────────────────────────
    daq.begin();         // Creates FreeRTOS task "PZEMReader"

    // ── Step 12: Boot complete ──────────────────────────────────────
    led.play(LEDSignal::Pattern::IDLE);
    _bootStartMs = millis();
    _bootComplete = false;

    diag.info("MAIN", "Setup complete. Entering boot screen on Core 1.");
}

// ═══════════════════════════════════════════════════════════════════
// LOOP (runs continuously on Core 1)
// ═══════════════════════════════════════════════════════════════════

void loop() {
    esp_task_wdt_reset();  // Feed the Task Watchdog Timer

    // ── Boot screen animation (progress bar for BOOT_DURATION_MS) ────
    if (!_bootComplete) {
        _handleBoot();
        vTaskDelay(pdMS_TO_TICKS(5));  // Yield to idle task
        return;
    }

    // ── Periodic NTP sync (every 60 s, only if WiFi connected) ─────
    static unsigned long lastNtpCheck = 0;
    if (taskMgr.isTime(lastNtpCheck, 60000UL)) {
        if (WiFi.isConnected()) {
            taskMgr.syncNTP();
        }
    }

    // ── Network service handlers (non-blocking tick functions) ──────
    wifiMgr.tick();      // Scan + reconnect management
    mqttMgr.tick();      // State machine: DNS → TCP → MQTT + publish

    // ── OLED menu + display ─────────────────────────────────────────
    {
        SystemState snap = getStateCopy();
        if (snap.webServerActive) {
            menu.setAPInfo(snap.apSSID, snap.apPass, snap.apIP);
        }
        menu.tick(snap);  // Button handling + screen rendering
    }

    // ── LED indicator ───────────────────────────────────────────────
    led.tick();
    _checkConnectionSignals();  // Play LED patterns on new connections

    // ── Self-healing: heap guard + WDT ──────────────────────────────
    _selfHealingCheck();

    vTaskDelay(pdMS_TO_TICKS(5));  // Allow idle task to run
}

// ═══════════════════════════════════════════════════════════════════
// BOOT SCREEN HANDLER
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Animate the boot progress bar during BOOT_DURATION_MS.
 *
 * Runs as part of loop() until _bootComplete is set.
 * Calculates progress from elapsed time; caps at 100%.
 * Transitions to MONITORING mode when complete.
 */
static void _handleBoot() {
    unsigned long elapsed = millis() - _bootStartMs;
    int progress = (elapsed * 100) / BOOT_DURATION_MS;
    if (progress > 100) progress = 100;  // Clamp to [0, 100]

    display.drawBoot(progress);

    if (elapsed >= BOOT_DURATION_MS) {
        _bootComplete = true;
        display.clear();
        diag.info("MAIN", "Boot complete, entering monitoring mode.");
    }
}

// ═══════════════════════════════════════════════════════════════════
// TASK WATCHDOG TIMER (WDT) INITIALISATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Configure the ESP32 Task Watchdog Timer.
 *
 * Sets timeout to WDT_TIMEOUT_SEC (30 s) with panic enabled (reboot
 * on timeout). Subscribes the current task (loop on Core 1) to the WDT.
 *
 * The DAQ task on Core 0 also subscribes itself in data_acquisition.cpp.
 */
static void _initWatchdog() {
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms     = (uint32_t)(WDT_TIMEOUT_SEC * 1000),
        .idle_core_mask = 0,         // Monitor all cores
        .trigger_panic  = true       // Reboot on WDT timeout
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
        // Already subscribed (harmless — can happen after reboot).
        diag.info("WDT", "Loop task already subscribed (OK).");
    } else {
        diag.warn("WDT", "esp_task_wdt_add: %s", esp_err_to_name(addRet));
    }
}

// ═══════════════════════════════════════════════════════════════════
// SELF-HEALING: HEAP PROTECTION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Monitor free heap and reboot if critically low.
 *
 * Two-stage protection:
 * 1. If free heap < HEAP_CRITICAL_MIN_BYTES (8 KB): immediate reboot.
 * 2. If free heap < 2 × threshold: log a warning (rate-limited to 60 s).
 *
 * ESP32 typically has 200–300 KB free heap at idle. 8 KB is dangerously
 * low — fragmentation or a leak is likely.
 */
static void _selfHealingCheck() {
    static unsigned long _lastHeapWarnMs = 0;
    uint32_t freeHeap = ESP.getFreeHeap();

    // ── Critical: immediate reboot ───────────────────────────────────
    if (freeHeap < HEAP_CRITICAL_MIN_BYTES) {
        diag.error("WDT", "CRITICAL: free heap %u bytes < threshold %u — rebooting!",
                    freeHeap, (uint32_t)HEAP_CRITICAL_MIN_BYTES);
        delay(200);  // Allow serial buffer to flush
        ESP.restart();
    }

    // ── Warning: log once per minute ─────────────────────────────────
    unsigned long now = millis();
    if (freeHeap < (HEAP_CRITICAL_MIN_BYTES * 2) && (now - _lastHeapWarnMs > 60000UL)) {
        _lastHeapWarnMs = now;
        diag.warn("WDT", "WARNING: free heap low: %u bytes", freeHeap);
    }
}

// ═══════════════════════════════════════════════════════════════════
// INTER-MODULE CALLBACK WIRING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Register callbacks connecting the menu system and webserver
 *        to the underlying hardware/network modules.
 *
 * Callback wiring:
 * - Menu "Reset kWh" → DataAcquisition::resetAllEnergy()
 * - Menu "Config" toggle → WebServerManager::activate() / deactivate()
 * - Web config save (calibration) → DataAcquisition::reloadCalibration()
 * - Web config save (MQTT) → MQTTManager::reloadConfig()
 * - Web config save (thresholds) → DataAcquisition::reloadThresholds()
 */
static void _wireCallbacks() {

    // ── Menu: Reset kWh ──────────────────────────────────────────────
    menu.onResetKwh([&]() -> bool {
        diag.info("MAIN", "Reset kWh triggered from menu.");
        led.play(LEDSignal::Pattern::RESET_KWH);
        bool ok = daq.resetAllEnergy();
        diag.info("MAIN", "Reset result: %s", ok ? "OK" : "FAIL");
        return ok;
    });

    // ── Menu: Web server toggle ──────────────────────────────────────
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

    // ── Web: Calibration saved → reload DAQ ─────────────────────────
    webServer.onCalibrationSaved = [&]() {
        diag.info("MAIN", "Calibration updated → reloading DAQ offsets.");
        daq.reloadCalibration();
    };

    // ── Web: MQTT config saved → reconnect ──────────────────────────
    webServer.onMQTTSaved = [&]() {
        diag.info("MAIN", "MQTT config updated → reconnecting.");
        mqttMgr.reloadConfig();
    };

    // ── Web: Thresholds saved → reload DAQ ──────────────────────────
    webServer.onThresholdSaved = [&]() {
        diag.info("MAIN", "Thresholds updated → reloading DAQ thresholds.");
        daq.reloadThresholds();
    };
}

// ═══════════════════════════════════════════════════════════════════
// CONNECTION STATUS SIGNALS (LED patterns)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Play LED blink patterns when WiFi or MQTT connects.
 *
 * Edge-triggered: compares current state against previous state.
 * WiFi OK → double-blink. MQTT OK → triple-blink.
 */
static void _checkConnectionSignals() {
    static bool prevWifi = false;
    static bool prevMQTT = false;

    bool nowWifi = wifiMgr.isConnected();
    bool nowMQTT = mqttMgr.isConnected();

    if (!prevWifi && nowWifi) {
        led.play(LEDSignal::Pattern::WIFI_OK);  // Double-blink
        diag.info("MAIN", "WiFi connected → LED signal");
        wifiMgr.printStatus();
    }
    if (!prevMQTT && nowMQTT) {
        led.play(LEDSignal::Pattern::MQTT_OK);  // Triple-blink
        diag.info("MAIN", "MQTT connected → LED signal");
    }

    prevWifi = nowWifi;
    prevMQTT = nowMQTT;
}

// ═══════════════════════════════════════════════════════════════════
// FIRMWARE BANNER
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Print the firmware startup banner to the serial console.
 *
 * Includes project name, version, author, device ID, CPU frequency,
 * and free heap at boot.
 */
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

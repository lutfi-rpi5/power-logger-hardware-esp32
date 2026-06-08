/******************************************************
 * Project   : 3-Phase Data Logger ESP32
 * File      : Data_logger_GD1.ino
 * Author    : Muhammad Lutfi Nur Anendi
 * Version   : v2.1.0
 * Board     : ESP32 Devkit V1 (30-pin)
 * IDE       : Arduino IDE v2.x
 * Package   : esp32 by Espressif System: ESP32 Dev Module v3.3.0
 * Board Package Reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *
 * Description:
 *   Main orchestrator.  All feature modules are in
 *   separate .h/.cpp files. This file only:
 *     1. Defines the global shared state & mutex
 *     2. Instantiates every module
 *     3. Wires up callbacks between modules
 *     4. Runs setup() / loop()
 *
 * Hardware:
 *   - ESP32 Devkit V1 30-pin
 *   - 3× PZEM-004T v3.0 (100A), Modbus addresses 0x10/0x11/0x12
 *   - OLED SSD1306 0.96" I2C (128×64)
 *   - Push button (NO) on GPIO 5
 *   - Built-in LED GPIO 2
 *
 * Libraries required (install via Library Manager):
 *   - PZEM004Tv30          by Olexa Prokopenko
 *   - Adafruit SSD1306     by Adafruit
 *   - Adafruit GFX         by Adafruit
 *   - PubSubClient         by Nick O'Leary
 *   - ArduinoJson          by Benoit Blanchon  (v6)
 *   - ESP AsyncWebServer   by ESP32Async
 *   - Async TCP            by ESP32Async
 *
 * Architecture overview:
 *   ┌─ Core 0 (CORE_SENSOR) ─────────────────────┐
 *   │  PZEMReader task  (1 Hz, highest priority) │
 *   │   └─ reads 3 PZEMs → writes gState.phases  │
 *   │   └─ computes voltage unbalance            │
 *   │   └─ feeds Task Watchdog (WDT)             │
 *   └────────────────────────────────────────────┘
 *   ┌─ Core 1 (CORE_COMM / Arduino loop) ────────┐
 *   │  WiFiManager::tick()   – reconnect mgmt    │
 *   │  MQTTManager::tick()   – keepalive + pub   │
 *   │  MenuSystem::tick()    – button + display  │
 *   │  LEDSignal::tick()     – non-blocking LED  │
 *   │  WebServerManager      – AsyncWebServer    │
 *   │  _selfHealingCheck()   – heap + WDT guard  │
 *   └────────────────────────────────────────────┘
 *
 * Revision History:
 *   v1.0  Basic MQTT publisher, blocking WiFi boot
 *   v2.0  Full async/non-blocking, FreeRTOS dual-core,
 *         JSON telemetry, OLED menu, ESPAsyncWebServer,
 *         EEPROM config, calibration, smart WiFi mgr
 *   v2.1  AP internet passthrough (NAT/NAPT), OVER voltage
 *         status, voltage unbalance 3-phase (NEMA),
 *         thresholds configurable via web (EEPROM),
 *         OLED unbalance row + blink on warn,
 *         JSON includes OVER status & unbalance,
 *         Task Watchdog + heap guard self-healing
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
#include "PhaseData.h"
#include "StorageManager.h"
#include "Button.h"
#include "LEDSignal.h"
#include "DataAcquisition.h"
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DisplayOLED.h"
#include "MenuSystem.h"
#include "WebServerManager.h"

// ════════════════════════════════════════════════════
// GLOBAL SHARED STATE  (defined here, extern in PhaseData.h)
// ════════════════════════════════════════════════════

SystemState       gState;
SemaphoreHandle_t gStateMutex;

// ════════════════════════════════════════════════════
// HARDWARE SERIAL  (PZEM-004T)
// ════════════════════════════════════════════════════

HardwareSerial PZEMSerial1(1);   // UART1: Line R & S
HardwareSerial PZEMSerial2(2);   // UART2: Line T

// ════════════════════════════════════════════════════
// PZEM ARRAY
// ════════════════════════════════════════════════════

PZEM004Tv30 pzems[NUM_PHASES] = {
    PZEM004Tv30(PZEMSerial1, PIN_PZEM_RX1, PIN_PZEM_TX1, PZEM_ADDR_R),  // Line R
    PZEM004Tv30(PZEMSerial1, PIN_PZEM_RX1, PIN_PZEM_TX1, PZEM_ADDR_S),  // Line S
    PZEM004Tv30(PZEMSerial2, PIN_PZEM_RX2, PIN_PZEM_TX2, PZEM_ADDR_T)   // Line T
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

// ════════════════════════════════════════════════════
// FORWARD DECLARATIONS
// ════════════════════════════════════════════════════

static void _wireCallbacks();
static void _printBanner();
static void _selfHealingCheck();
static void _initWatchdog();

// ════════════════════════════════════════════════════
// SETUP
// ════════════════════════════════════════════════════

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 2000) {}   // wait up to 2s for USB-serial
    _printBanner();

    // ── Mutex ────────────────────────────────────────
    gStateMutex = xSemaphoreCreateMutex();
    configASSERT(gStateMutex != nullptr);

    // ── LED (first so we can signal boot immediately) ─
    led.begin();

    // ── OLED (boot screen while rest initialises) ─────
    display.begin();
    display.drawBoot("Initializing...");

    // ── Storage (NVS Preferences) ─────────────────────
    storage.begin();
    storage.dump();   // print all stored settings to Serial

    // ── Propagate stored thresholds to gState ─────────
    {
        ThresholdConfig thr = storage.getThresholds();
        updateState([&](SystemState& s) {
            s.thresholdUnbalanceMax = thr.unbalanceMax;
        });
    }

    // ── Wire cross-module callbacks ───────────────────
    _wireCallbacks();

    // ── WiFi (non-blocking – won't stall boot) ────────
    wifiMgr.begin();

    // ── MQTT ──────────────────────────────────────────
    mqttMgr.begin();

    // ── Web Server (routes registered; not active yet) ─
    webServer.begin();

    // ── Menu & Display ────────────────────────────────
    menu.begin();

    // Populate AP info in menu from storage
    APConfig ap = storage.getAPConfig();
    menu.setAPInfo(ap.ssid, ap.pass, "0.0.0.0");

    // ── Task Watchdog ─────────────────────────────────
    // Core 1 (loop) is registered here; Core 0 (DAQ task) registers itself in begin()
    _initWatchdog();

    // ── Sensor Task on Core 0 ────────────────────────
    // Must be last so all shared objects are ready
    display.drawBoot("Starting sensors...");
    daq.begin();

    // ── Boot complete ─────────────────────────────────
    led.play(LEDSignal::Pattern::IDLE);
    display.drawBoot("Ready!");
    delay(600);

    Serial.println("[Main] Setup complete. Entering main loop on Core 1.");
}

// ════════════════════════════════════════════════════
// LOOP  (runs on Core 1)
// ════════════════════════════════════════════════════

void loop() {
    // ── Task Watchdog feed (Core 1 / loop task) ───────
    esp_task_wdt_reset();

    // ── WiFi reconnect management ─────────────────────
    wifiMgr.tick();

    // ── MQTT keepalive & publish ──────────────────────
    mqttMgr.tick();

    // ── Button + OLED menu ────────────────────────────
    {
        SystemState snap = getStateCopy();

        // Keep AP info in menu updated when webserver is active
        if (snap.webServerActive) {
            menu.setAPInfo(snap.apSSID, snap.apPass, snap.apIP);
        }

        menu.tick(snap);
    }

    // ── LED animations ────────────────────────────────
    led.tick();

    // ── Connection-change LED signals ─────────────────
    _checkConnectionSignals();

    // ── Self-healing check (heap guard) ───────────────
    _selfHealingCheck();

    // Short yield so watchdog & lower-prio tasks get CPU
    vTaskDelay(pdMS_TO_TICKS(5));
}

// ════════════════════════════════════════════════════
// WATCHDOG INIT
// ════════════════════════════════════════════════════

/**
 * Initialise the ESP32 Task Watchdog Timer.
 * - Timeout: WDT_TIMEOUT_SEC seconds
 * - Panic on timeout: triggers a reboot (self-healing)
 * - The loop() task is subscribed here; the DAQ task
 *   subscribes itself inside DataAcquisition::_taskFn().
 */
// static void _initWatchdog() {
//     // Initialize watchdog with timeout and panic-on-timeout enabled
//     esp_task_wdt_init(WDT_TIMEOUT_SEC, true);

//     // Subscribe the loop (Core 1) task
//     esp_task_wdt_add(NULL);

//     Serial.printf("[WDT] Task watchdog initialized: timeout=%ds, panic=enabled\n",
//                   WDT_TIMEOUT_SEC);
// }

static void _initWatchdog() {
    // ── IDF5 WDT init strategy ────────────────────────────────────────────────
    // IDF5 auto-inits WDT pada boot (timeout default = 5 detik dari menuconfig).
    // esp_task_wdt_init() ulang → "TWDT already initialized" error.
    //
    // Yang harus dilakukan:
    //   1. RECONFIGURE timeout ke WDT_TIMEOUT_SEC (30s) via esp_task_wdt_reconfigure()
    //      → ini mengubah timeout WDT yang sudah berjalan tanpa re-init
    //   2. SUBSCRIBE loop task ke WDT
    //
    // Mengapa reconfigure WAJIB dilakukan:
    //   Default IDF5 WDT timeout = 5 detik.
    //   WiFiClientSecure::connect(hostname, port) melakukan DNS resolution
    //   secara blocking di loop task. Jika tidak ada internet, DNS timeout
    //   bisa 5–10 detik → WDT 5s fire duluan → reboot.
    //   Dengan timeout 30s, DNS resolution selalu selesai (timeout atau berhasil)
    //   sebelum WDT trigger, sehingga state machine bisa menangani hasilnya.

    // Step 1: Reconfigure timeout ke nilai yang kita inginkan
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms     = (uint32_t)(WDT_TIMEOUT_SEC * 1000),
        .idle_core_mask = 0,      // JANGAN pantau IDLE task (bisa false-trigger)
        .trigger_panic  = true    // panic → reboot otomatis (self-healing)
    };
    esp_err_t recfgRet = esp_task_wdt_reconfigure(&wdt_config);
    if (recfgRet == ESP_OK) {
        Serial.printf("[WDT] Timeout reconfigured to %ds.\n", WDT_TIMEOUT_SEC);
    } else if (recfgRet == ESP_ERR_INVALID_STATE) {
        // WDT belum running sama sekali — jarang terjadi di IDF5 standard build
        Serial.println("[WDT] WDT not running, skipping reconfigure.");
    } else {
        Serial.printf("[WDT] reconfigure warning: %s\n", esp_err_to_name(recfgRet));
    }

    // Step 2: Subscribe loop task
    esp_err_t addRet = esp_task_wdt_add(xTaskGetCurrentTaskHandle());
    if (addRet == ESP_OK) {
        Serial.printf("[WDT] Loop task subscribed. Effective timeout: %ds\n",
                      WDT_TIMEOUT_SEC);
    } else if (addRet == ESP_ERR_INVALID_ARG) {
        // Task sudah terdaftar sebelumnya — tidak masalah
        Serial.println("[WDT] Loop task already subscribed (OK).");
    } else {
        Serial.printf("[WDT] esp_task_wdt_add: %s\n", esp_err_to_name(addRet));
    }
}

// ════════════════════════════════════════════════════
// SELF-HEALING CHECK
// ════════════════════════════════════════════════════

/**
 * Heap guard: if free heap drops below HEAP_CRITICAL_MIN_BYTES,
 * the system is likely heading toward a crash (fragmentation,
 * memory leak, etc.). Log a warning and force a clean reboot
 * before a hard panic or undefined behaviour occurs.
 *
 * The Task Watchdog (configured in _initWatchdog) covers the
 * case where a task is stuck / hung:
 *   - If loop() stops calling esp_task_wdt_reset() → panic → reboot
 *   - If the DAQ task stops → same
 */
static void _selfHealingCheck() {
    static unsigned long _lastHeapWarnMs = 0;
    uint32_t freeHeap = ESP.getFreeHeap();

    if (freeHeap < HEAP_CRITICAL_MIN_BYTES) {
        Serial.printf("[WDT] CRITICAL: free heap %u bytes < threshold %u — rebooting!\n",
                      freeHeap, (uint32_t)HEAP_CRITICAL_MIN_BYTES);
        delay(200);          // brief pause to flush serial
        ESP.restart();
    }

    // Periodic heap warning (every 60 s) when heap is getting low but not critical
    unsigned long now = millis();
    if (freeHeap < (HEAP_CRITICAL_MIN_BYTES * 2) && (now - _lastHeapWarnMs > 60000UL)) {
        _lastHeapWarnMs = now;
        Serial.printf("[WDT] WARNING: free heap low: %u bytes\n", freeHeap);
    }
}

// ════════════════════════════════════════════════════
// CALLBACK WIRING
// ════════════════════════════════════════════════════

/**
 * Wire all inter-module callbacks here so main.ino
 * stays as the single source of truth for the
 * dependency graph between modules.
 */
static void _wireCallbacks() {

    // ── Menu: Reset kWh ──────────────────────────────
    menu.onResetKwh([&]() -> bool {
        Serial.println("[Main] Reset kWh triggered from menu.");
        led.play(LEDSignal::Pattern::RESET_KWH);
        bool ok = daq.resetAllEnergy();
        Serial.printf("[Main] Reset result: %s\n", ok ? "OK" : "FAIL");
        return ok;
    });

    // ── Menu: Web server toggle ───────────────────────
    menu.onWebToggle([&](bool activate) {
        if (activate) {
            webServer.activate();
            APConfig ap = storage.getAPConfig();
            String ip = webServer.getIP();
            menu.setAPInfo(ap.ssid, ap.pass, ip.c_str());
            Serial.printf("[Main] WebServer ON  IP: %s\n", ip.c_str());
            led.play(LEDSignal::Pattern::WIFI_OK);
        } else {
            webServer.deactivate();
            menu.setAPInfo("", "", "0.0.0.0");
            Serial.println("[Main] WebServer OFF");
        }
        menu.requestRedraw();
    });

    // ── WebServer: Calibration saved ─────────────────
    webServer.onCalibrationSaved = [&]() {
        Serial.println("[Main] Calibration updated → reloading DAQ offsets.");
        daq.reloadCalibration();
    };

    // ── WebServer: MQTT config saved ──────────────────
    webServer.onMQTTSaved = [&]() {
        Serial.println("[Main] MQTT config updated → reconnecting.");
        mqttMgr.reloadConfig();
    };

    // ── WebServer: Threshold config saved ────────────
    webServer.onThresholdSaved = [&]() {
        Serial.println("[Main] Thresholds updated → reloading DAQ thresholds.");
        daq.reloadThresholds();
    };
}

// ════════════════════════════════════════════════════
// CONNECTION-CHANGE LED SIGNALS
// ════════════════════════════════════════════════════

/**
 * Track previous WiFi/MQTT state to emit a one-shot
 * LED blink whenever a connection is established.
 */
static void _checkConnectionSignals() {
    static bool prevWifi = false;
    static bool prevMQTT = false;

    bool nowWifi = wifiMgr.isConnected();
    bool nowMQTT = mqttMgr.isConnected();

    if (!prevWifi && nowWifi) {
        led.play(LEDSignal::Pattern::WIFI_OK);
        Serial.println("[Main] WiFi connected → LED signal");
        wifiMgr.printStatus();
    }
    if (!prevMQTT && nowMQTT) {
        led.play(LEDSignal::Pattern::MQTT_OK);
        Serial.println("[Main] MQTT connected → LED signal");
    }

    prevWifi = nowWifi;
    prevMQTT = nowMQTT;
}

// ════════════════════════════════════════════════════
// SERIAL BANNER
// ════════════════════════════════════════════════════

static void _printBanner() {
    Serial.println();
    Serial.println("╔══════════════════════════════════════════╗");
    Serial.println("║     3-Phase Data Logger  " FW_VERSION "          ║");
    Serial.println("║     ESP32  |  3× PZEM-004T  |  MQTT      ║");
    Serial.println("║     Muhammad Lutfi Nur Anendi            ║");
    Serial.println("╚══════════════════════════════════════════╝");
    Serial.printf ("  Device ID : %s\n", DEVICE_ID);
    Serial.printf ("  Core freq : %d MHz\n", getCpuFrequencyMhz());
    Serial.printf ("  Free heap : %d bytes\n", ESP.getFreeHeap());
    Serial.println();
}

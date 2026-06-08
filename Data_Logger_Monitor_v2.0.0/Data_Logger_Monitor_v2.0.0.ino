// ============================================================
// 3-Phase Data Logger v2.0.0 — Main Entry Point
// ============================================================
// ESP32 + 3x PZEM-004T + OLED SSD1306 + MQTT
//
// Non-blocking, async-friendly, state-machine-driven firmware.
// Device runs immediately without waiting for WiFi/MQTT.
// ============================================================

#include "config.h"
#include "types.h"
#include "system_state.h"
#include "credentials.h"
#include "diagnostics.h"
#include "storage_manager.h"
#include "button_manager.h"
#include "led_manager.h"
#include "calibration.h"
#include "data_acquisition.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "json_builder.h"
#include "display_manager.h"
#include "menu_manager.h"
#include "webserver_manager.h"
#include "task_manager.h"

// ─── Global Instances ──────────────────────────────────────

SystemState g_state;
StorageManager g_storage;
ButtonManager g_button(BUTTON_PIN, BUTTON_LONG_PRESS_MS);
LEDManager g_led(LED_BUILTIN_PIN);
Calibration g_cal;
DataAcquisition g_daq;
WiFiManager g_wifi;
MQTTManager g_mqtt;
JSONBuilder g_json;
DisplayManager g_display;
MenuManager g_menu;
WebServerManager g_web(&g_storage);
TaskManager g_task;

// ─── Timing Trackers ───────────────────────────────────────

static unsigned long lastAcqMs = 0;
static unsigned long lastPubMs = 0;
static unsigned long lastNtpSyncMs = 0;
static unsigned long bootProgressStartMs = 0;
static unsigned long rebootCountdownStartMs = 0;
static unsigned long statusMsgStartMs = 0;

static bool bootComplete = false;
static bool wifiConnectInitiated = false;
static bool mqttConnectInitiated = false;
static bool ntpSyncInProgress = false;
static unsigned long ntpSyncStartMs = 0;

// Reboot / factory / reset flags handled in loop
static bool pendingReboot = false;
static unsigned long rebootStartMs = 0;
static int rebootCountdown = 3;

// ─── Forward Declarations ──────────────────────────────────

static void handleBoot();
static void handleDataAcquisition();
static void handleMQTTPublish();
static void handleButtonEvents();
static void handleMenuActions();
static void handleNTPSync();
static void updateSystemState();
static void applyCalibrationToState();

// ============================================================
// SETUP
// ============================================================

void setup() {
    // Diagnostics (includes Serial.begin)
    diag.begin(LOG_INFO);
    diag.info("MAIN", "3-Phase Data Logger %s starting...", FW_VERSION);

    // Hardware
    g_led.begin();
    g_button.begin();

    // Storage (NVS)
    g_storage.begin();

    // Display
    g_display.begin();
    g_menu.begin(&g_display);

    // Calibration (load offsets from NVS)
    g_cal.setOffsets(g_storage.getCalibration());

    // Data acquisition (PZEM serial)
    g_daq.begin();

    // Networking
    g_wifi.begin();

    // MQTT
    g_mqtt.begin();
    g_mqtt.setConfig(g_storage.getMQTTConfig());

    // Web server (start immediately; reachable via AP or STA)
    g_web.begin();

    // Task manager + watchdog
    g_task.begin();

    // Init state
    g_state.bootTime = millis();
    g_state.appState = AppState::BOOTING;
    g_state.fakeDataMode = true;

    // Boot progress
    bootProgressStartMs = millis();

    diag.info("MAIN", "Setup complete. Entering loop.");
}

// ============================================================
// LOOP
// ============================================================

void loop() {
    // ── Boot sequence ──
    if (!bootComplete) {
        handleBoot();
        return;
    }

    // ── Always-run subsystems ──
    g_button.update();
    g_led.update();
    g_wifi.update();
    g_mqtt.update();
    g_task.feedWatchdog();

    // ── Data acquisition (every MQTT_PUBLISH_INTERVAL_MS) ──
    if (g_task.isTime(lastAcqMs, MQTT_PUBLISH_INTERVAL_MS)) {
        handleDataAcquisition();
    }

    // ── Button events ──
    handleButtonEvents();

    // ── Menu actions ──
    handleMenuActions();

    // ── MQTT publish ──
    if (g_state.appState == AppState::MONITORING ||
        g_state.appState == AppState::MENU) {
        if (g_task.isTime(lastPubMs, MQTT_PUBLISH_INTERVAL_MS)) {
            updateSystemState();
            applyCalibrationToState();
            handleMQTTPublish();
        }
    }

    // ── NTP sync (every 1 hour after first sync) ──
    handleNTPSync();

    // ── Reboot countdown ──
    if (pendingReboot) {
        handleRebootCountdown();
    }

    // ── OLED refresh ──
    static unsigned long lastOledMs = 0;
    if (g_task.isTime(lastOledMs, OLED_REFRESH_INTERVAL_MS)) {
        updateSystemState();
        applyCalibrationToState();

        g_menu.update(
            g_state.wifiConnected, g_state.mqttConnected,
            g_state.phases, g_state.unbalancePercent,
            g_storage.getThresholds(),
            g_state.apActive,
            WiFi.softAPIP().toString().c_str(),
            pendingReboot, rebootCountdown
        );
    }
}

// ============================================================
// BOOT HANDLER
// ============================================================

void handleBoot() {
    unsigned long elapsed = millis() - bootProgressStartMs;
    int progress = (int)((elapsed * 100) / 3000);  // 3-second boot screen
    if (progress > 100) progress = 100;

    g_display.drawBootScreen(progress);

    // Start WiFi connect after brief delay (non-blocking)
    if (!wifiConnectInitiated && elapsed > 500) {
        wifiConnectInitiated = true;
        g_wifi.beginConnect();
        diag.info("MAIN", "Initiated async WiFi connect");
    }

    // Boot complete after 3 seconds or when WiFi connects (whichever first)
    if (elapsed >= 3000) {
        bootComplete = true;
        g_state.appState = AppState::MONITORING;
        g_state.oledMode = OLEDMode::MONITOR_PAGE_1;
        g_display.clear();
        diag.info("MAIN", "Boot complete, entering monitoring mode");
    }
}

// ============================================================
// DATA ACQUISITION
// ============================================================

void handleDataAcquisition() {
    if (g_state.fakeDataMode) {
        g_daq.generateFakeData();
    } else {
        g_daq.readAll();
    }
    diag.debug("MAIN", "Data acquisition cycle complete");
}

// ============================================================
// STATE UPDATE + CALIBRATION
// ============================================================

void updateSystemState() {
    g_state.wifiConnected = g_wifi.isConnected();
    g_state.mqttConnected = g_mqtt.isConnected();
    g_state.rssi = g_wifi.getRSSI();
    g_state.apActive = g_wifi.isAPActive();
}

void applyCalibrationToState() {
    ThresholdData thr = g_storage.getThresholds();

    for (int i = 0; i < NUM_PZEMS; i++) {
        float rawV = g_daq.getRawVoltage(i);
        float rawI = g_daq.getRawCurrent(i);
        float rawP = g_daq.getRawPower(i);
        float rawE = g_daq.getRawEnergy(i);
        float rawF = g_daq.getRawFrequency(i);
        float rawPF = g_daq.getRawPF(i);

        g_cal.apply(rawV, rawI, rawP, rawE, rawF, rawPF, i, g_state.phases[i]);
        g_state.phases[i].status = g_daq.determineLineStatus(g_state.phases[i].voltage, thr);
    }

    g_state.unbalancePercent = Calibration::computeUnbalance(g_state.phases);
}

// ============================================================
// MQTT PUBLISH
// ============================================================

void handleMQTTPublish() {
    if (!g_state.mqttConnected) {
        diag.debug("MAIN", "MQTT not connected, skipping publish");
        return;
    }

    uint32_t seq = g_storage.getSequence();
    g_storage.incrementSequence();

    // Timestamp: Unix epoch from NTP, or 0 if not synced
    unsigned long ts = 0;  // TODO: replace with NTP time when synced

    String payload = g_json.build(g_state, seq, ts);
    if (payload.length() > 0) {
        g_mqtt.publish(payload.c_str());
    }
}

// ============================================================
// BUTTON EVENTS
// ============================================================

void handleButtonEvents() {
    if (g_button.isShortPressed()) {
        g_menu.onShortPress();
    }

    if (g_button.isLongPressed()) {
        g_menu.onLongPress();
    }
}

// ============================================================
// MENU ACTIONS
// ============================================================

void handleMenuActions() {
    // Toggle AP mode
    if (g_menu.shouldToggleAP()) {
        g_menu.clearToggleAPFlag();
        if (g_state.apActive) {
            g_wifi.stopAP();
            g_state.apActive = false;
            diag.info("MAIN", "AP mode deactivated");
        } else {
            g_wifi.startAP(AP_SSID, AP_PASS);
            g_state.apActive = true;
            diag.info("MAIN", "AP mode activated: SSID=%s IP=%s",
                      AP_SSID, WiFi.softAPIP().toString().c_str());
        }
    }

    // Reset energy
    if (g_menu.shouldResetEnergy()) {
        g_menu.clearResetEnergyFlag();
        g_daq.resetEnergy();
        g_led.blink(5);
        statusMsgStartMs = millis();
    }

    // Reboot device
    if (g_menu.shouldReboot() && !pendingReboot) {
        pendingReboot = true;
        rebootCountdown = 3;
        rebootStartMs = millis();
    }

    // Factory reset
    if (g_menu.shouldFactoryReset()) {
        g_menu.clearFactoryResetFlag();
        g_storage.factoryReset();
        g_led.blink(3);
        delay(500);
        ESP.restart();
    }
}

// ============================================================
// REBOOT COUNTDOWN
// ============================================================

void handleRebootCountdown() {
    unsigned long elapsed = millis() - rebootStartMs;
    int newCountdown = 3 - (elapsed / 1000);
    if (newCountdown < 0) newCountdown = 0;

    if (newCountdown != rebootCountdown) {
        rebootCountdown = newCountdown;
        diag.info("MAIN", "Rebooting in %d...", rebootCountdown);
    }

    if (elapsed >= 3000) {
        diag.info("MAIN", "Rebooting now!");
        g_led.blink(2, 100, 100);
        delay(500);
        ESP.restart();
    }
}

// ============================================================
// NTP SYNC
// ============================================================

void handleNTPSync() {
    if (!g_state.wifiConnected) return;

    static bool ntpSynced = false;
    unsigned long now = millis();

    // First sync: when WiFi connects
    if (!ntpSynced && g_state.wifiConnected && !ntpSyncInProgress) {
        ntpSyncInProgress = true;
        ntpSyncStartMs = now;
        configTime(7 * 3600, 0, NTP_SERVER1, NTP_SERVER2);  // WIB (UTC+7)
        diag.info("MAIN", "NTP sync started");
    }

    // Check if sync succeeded
    if (ntpSyncInProgress) {
        if (now - ntpSyncStartMs > NTP_TIMEOUT_MS) {
            ntpSyncInProgress = false;
            diag.warn("MAIN", "NTP sync timed out");
        } else {
            time_t now_t;
            time(&now_t);
            if (now_t > 100000) {  // Valid timestamp
                ntpSyncInProgress = false;
                ntpSynced = true;
                lastNtpSyncMs = now;
                struct tm timeinfo;
                localtime_r(&now_t, &timeinfo);
                diag.info("MAIN", "NTP synced: %04d-%02d-%02d %02d:%02d:%02d",
                          timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            }
        }
    }

    // Re-sync every hour
    if (ntpSynced && now - lastNtpSyncMs > 3600000) {
        ntpSynced = false;
    }
}

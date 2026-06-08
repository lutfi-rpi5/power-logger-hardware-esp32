#include "storage_manager.h"
#include "diagnostics.h"

/**
 * @file storage_manager.cpp
 * @brief NVS-backed persistent configuration storage via the Preferences library.
 *
 * All configuration data is stored in the "dlcfg" namespace in ESP32
 * NVS (Non-Volatile Storage). Each section uses typed Preferences keys.
 *
 * Key naming convention:
 * - WiFi SSID:  "ws0", "ws1", ... "ws4"
 * - WiFi pass:  "wp0", "wp1", ... "wp4"
 * - MQTT:       "mqtt_srv", "mqtt_port", "mqtt_user", etc.
 * - Calibration: "cal_v0", "cal_v1", "cal_v2", "cal_i0", etc.
 * - Thresholds:  "th_vlost", "th_vunder", "th_vover", "th_unbal"
 * - AP:          "ap_ssid", "ap_pass"
 *
 * Each public method opens and closes the namespace to minimise NVS
 * handle contention — ESP32 Preferences is not thread-safe.
 */

void StorageManager::_open(bool readOnly) const { _prefs.begin(PREF_NAMESPACE, readOnly); }
void StorageManager::_close() const { _prefs.end(); }

/**
 * @brief Generate the Preferences key for a WiFi SSID at a given index.
 * @param i  Index (0–MAX_WIFI_NETWORKS-1).
 * @param buf Output buffer (min 16 chars). Receives e.g. "ws0".
 */
void StorageManager::_wifiSSIDKey(int i, char* buf) { snprintf(buf, 16, "ws%d", i); }

/**
 * @brief Generate the Preferences key for a WiFi password at a given index.
 * @param i  Index (0–MAX_WIFI_NETWORKS-1).
 * @param buf Output buffer (min 16 chars). Receives e.g. "wp0".
 */
void StorageManager::_wifiPassKey(int i, char* buf) { snprintf(buf, 16, "wp%d", i); }

/**
 * @brief Initialise the NVS subsystem.
 * Opens and closes the namespace as a sanity check.
 */
void StorageManager::begin() {
    diag.info("STORAGE", "Initializing NVS...");
    _open();
    _close();
    diag.info("STORAGE", "Ready. WiFi networks stored: %d", getWifiCount());
}

// ── WiFi Network List ────────────────────────────────────────────────

/**
 * @brief Get the count of stored WiFi networks.
 * @return The stored count (key "wifi_count"), or 0 if not set.
 */
int StorageManager::getWifiCount() const {
    _open(true);
    int count = _prefs.getInt("wifi_count", 0);
    _close();
    return count;
}

/**
 * @brief Get a WiFi network entry by index.
 * @param index 0-based index.
 * @return WiFiEntry struct. Empty strings if index is out of range.
 */
WiFiEntry StorageManager::getWifiEntry(int index) const {
    WiFiEntry entry = {};
    if (index < 0 || index >= MAX_WIFI_NETWORKS) return entry;
    char sk[16], pk[16];
    _wifiSSIDKey(index, sk);
    _wifiPassKey(index, pk);
    _open(true);
    _prefs.getString(sk, entry.ssid, sizeof(entry.ssid));
    _prefs.getString(pk, entry.pass, sizeof(entry.pass));
    _close();
    return entry;
}

/**
 * @brief Add a network to the end of the WiFi list.
 * @param ssid Network SSID.
 * @param pass Network password.
 * @return true on success, false if the list is full.
 */
bool StorageManager::addWifiEntry(const char* ssid, const char* pass) {
    int count = getWifiCount();
    if (count >= MAX_WIFI_NETWORKS) {
        diag.warn("STORAGE", "WiFi list full (%d max)", MAX_WIFI_NETWORKS);
        return false;
    }
    char sk[16], pk[16];
    _wifiSSIDKey(count, sk);
    _wifiPassKey(count, pk);
    _open();
    _prefs.putString(sk, ssid);
    _prefs.putString(pk, pass);
    _prefs.putInt("wifi_count", count + 1);
    _close();
    diag.info("STORAGE", "Added WiFi[%d]: %s", count, ssid);
    return true;
}

/**
 * @brief Delete a WiFi entry by index.
 *
 * Shifts all subsequent entries up to fill the gap (cascade delete),
 * then decrements the count and removes the last duplicated entry.
 *
 * @param index 0-based index to delete.
 * @return true on success, false if index is out of range.
 */
bool StorageManager::deleteWifiEntry(int index) {
    int count = getWifiCount();
    if (index < 0 || index >= count) return false;

    _open();
    // Shift entries left to fill the deletion gap
    for (int i = index; i < count - 1; i++) {
        char sk_cur[16], pk_cur[16], sk_next[16], pk_next[16];
        _wifiSSIDKey(i,   sk_cur);  _wifiPassKey(i,   pk_cur);
        _wifiSSIDKey(i+1, sk_next); _wifiPassKey(i+1, pk_next);
        char ssid[64] = {}, pass[64] = {};
        _prefs.getString(sk_next, ssid, sizeof(ssid));
        _prefs.getString(pk_next, pass, sizeof(pass));
        _prefs.putString(sk_cur, ssid);
        _prefs.putString(pk_cur, pass);
    }
    // Remove the last (now-duplicated) entry
    char sk_last[16], pk_last[16];
    _wifiSSIDKey(count - 1, sk_last);
    _wifiPassKey(count - 1, pk_last);
    _prefs.remove(sk_last);
    _prefs.remove(pk_last);
    _prefs.putInt("wifi_count", count - 1);
    _close();
    diag.info("STORAGE", "Deleted WiFi[%d], count now %d", index, count - 1);
    return true;
}

/**
 * @brief Remove all stored WiFi networks.
 */
void StorageManager::clearAllWifi() {
    int count = getWifiCount();
    _open();
    for (int i = 0; i < count; i++) {
        char sk[16], pk[16];
        _wifiSSIDKey(i, sk);
        _wifiPassKey(i, pk);
        _prefs.remove(sk);
        _prefs.remove(pk);
    }
    _prefs.putInt("wifi_count", 0);
    _close();
}

// ── MQTT Configuration ──────────────────────────────────────────────

/**
 * @brief Load the MQTT configuration from NVS.
 *
 * If any field is empty in NVS, the corresponding default from
 * config.h is substituted. This ensures the device always has
 * valid configuration even on first boot or after factory reset.
 *
 * @return MQTTConfig struct with current or default values.
 */
MQTTConfig StorageManager::getMQTTConfig() const {
    MQTTConfig cfg = {};
    _open(true);
    _prefs.getString("mqtt_srv",   cfg.server,  sizeof(cfg.server));
    cfg.port    = _prefs.getInt("mqtt_port",   DEFAULT_MQTT_PORT);
    cfg.sslPort = _prefs.getInt("mqtt_sport",  DEFAULT_MQTT_SSL_PORT);
    cfg.wsPort  = _prefs.getInt("mqtt_wsp",    DEFAULT_MQTT_WS_PORT);
    cfg.wssPort = _prefs.getInt("mqtt_wssp",   DEFAULT_MQTT_WSS_PORT);
    _prefs.getString("mqtt_user",  cfg.user,    sizeof(cfg.user));
    _prefs.getString("mqtt_pass",  cfg.pass,    sizeof(cfg.pass));
    _prefs.getString("mqtt_pref",  cfg.prefix,  sizeof(cfg.prefix));
    _prefs.getString("mqtt_topic", cfg.topic,   sizeof(cfg.topic));
    cfg.useSSL  = _prefs.getBool("mqtt_ssl",   DEFAULT_MQTT_USE_SSL);
    cfg.useWS   = _prefs.getBool("mqtt_ws",    DEFAULT_MQTT_USE_WS);
    _prefs.getString("mqtt_cert",  cfg.caCert,  sizeof(cfg.caCert));
    _close();

    // Apply defaults for empty fields
    if (strlen(cfg.server) == 0) strncpy(cfg.server, DEFAULT_MQTT_SERVER, sizeof(cfg.server));
    if (strlen(cfg.user)   == 0) strncpy(cfg.user,   DEFAULT_MQTT_USER,   sizeof(cfg.user));
    if (strlen(cfg.pass)   == 0) strncpy(cfg.pass,   DEFAULT_MQTT_PASS,   sizeof(cfg.pass));
    if (strlen(cfg.prefix) == 0) strncpy(cfg.prefix, DEFAULT_MQTT_PREFIX, sizeof(cfg.prefix));
    if (strlen(cfg.topic)  == 0) strncpy(cfg.topic,  DEFAULT_MQTT_TOPIC,  sizeof(cfg.topic));
    return cfg;
}

/**
 * @brief Save the MQTT configuration to NVS.
 * @param cfg MQTTConfig to persist.
 */
void StorageManager::saveMQTTConfig(const MQTTConfig& cfg) {
    _open();
    _prefs.putString("mqtt_srv",   cfg.server);
    _prefs.putInt   ("mqtt_port",  cfg.port);
    _prefs.putInt   ("mqtt_sport", cfg.sslPort);
    _prefs.putInt   ("mqtt_wsp",   cfg.wsPort);
    _prefs.putInt   ("mqtt_wssp",  cfg.wssPort);
    _prefs.putString("mqtt_user",  cfg.user);
    _prefs.putString("mqtt_pass",  cfg.pass);
    _prefs.putString("mqtt_pref",  cfg.prefix);
    _prefs.putString("mqtt_topic", cfg.topic);
    _prefs.putBool  ("mqtt_ssl",   cfg.useSSL);
    _prefs.putBool  ("mqtt_ws",    cfg.useWS);
    _prefs.putString("mqtt_cert",  cfg.caCert);
    _close();
    diag.info("STORAGE", "MQTT config saved: %s:%d", cfg.server, cfg.port);
}

// ── Calibration Offsets ─────────────────────────────────────────────

/**
 * @brief Load calibration offsets from NVS.
 * @return CalibrationConfig with per-phase V/I offsets.
 */
CalibrationConfig StorageManager::getCalibration() const {
    CalibrationConfig cfg = {};
    _open(true);
    for (int i = 0; i < 3; i++) {
        char kv[12], ki[12];
        snprintf(kv, sizeof(kv), "cal_v%d", i);
        snprintf(ki, sizeof(ki), "cal_i%d", i);
        cfg.voltageOffset[i] = _prefs.getFloat(kv, DEFAULT_CAL_VOLTAGE);
        cfg.currentOffset[i] = _prefs.getFloat(ki, DEFAULT_CAL_CURRENT);
    }
    _close();
    return cfg;
}

void StorageManager::saveCalibration(const CalibrationConfig& cfg) {
    _open();
    for (int i = 0; i < 3; i++) {
        char kv[12], ki[12];
        snprintf(kv, sizeof(kv), "cal_v%d", i);
        snprintf(ki, sizeof(ki), "cal_i%d", i);
        _prefs.putFloat(kv, cfg.voltageOffset[i]);
        _prefs.putFloat(ki, cfg.currentOffset[i]);
    }
    _close();
    diag.info("STORAGE", "Calibration saved.");
}

// ── AP Configuration ───────────────────────────────────────────────

APConfig StorageManager::getAPConfig() const {
    APConfig cfg = {};
    _open(true);
    _prefs.getString("ap_ssid", cfg.ssid, sizeof(cfg.ssid));
    _prefs.getString("ap_pass", cfg.pass, sizeof(cfg.pass));
    _close();
    if (strlen(cfg.ssid) == 0) strncpy(cfg.ssid, DEFAULT_AP_SSID, sizeof(cfg.ssid));
    if (strlen(cfg.pass) == 0) strncpy(cfg.pass, DEFAULT_AP_PASS, sizeof(cfg.pass));
    return cfg;
}

void StorageManager::saveAPConfig(const APConfig& cfg) {
    _open();
    _prefs.putString("ap_ssid", cfg.ssid);
    _prefs.putString("ap_pass", cfg.pass);
    _close();
}

// ── Line Status Thresholds ─────────────────────────────────────────

ThresholdConfig StorageManager::getThresholds() const {
    ThresholdConfig cfg = {};
    _open(true);
    cfg.voltageLost   = _prefs.getFloat("th_vlost",  VOLTAGE_LOST_MAX);
    cfg.voltageUnder  = _prefs.getFloat("th_vunder", VOLTAGE_OK_MIN);
    cfg.voltageOver   = _prefs.getFloat("th_vover",  VOLTAGE_OVER_MAX);
    cfg.unbalanceMax  = _prefs.getFloat("th_unbal",  VOLTAGE_UNBALANCE_MAX);
    _close();
    return cfg;
}

void StorageManager::saveThresholds(const ThresholdConfig& cfg) {
    _open();
    _prefs.putFloat("th_vlost",  cfg.voltageLost);
    _prefs.putFloat("th_vunder", cfg.voltageUnder);
    _prefs.putFloat("th_vover",  cfg.voltageOver);
    _prefs.putFloat("th_unbal",  cfg.unbalanceMax);
    _close();
    diag.info("STORAGE", "Thresholds saved: LOST=%.1f UNDER=%.1f OVER=%.1f UNBAL=%.2f%%",
        cfg.voltageLost, cfg.voltageUnder, cfg.voltageOver, cfg.unbalanceMax);
}

// ── Factory Reset ──────────────────────────────────────────────────

void StorageManager::factoryReset() {
    diag.warn("STORAGE", "*** FACTORY RESET – clearing all NVS data ***");
    _open();
    _prefs.clear();  // Removes ALL keys in the "dlcfg" namespace
    _close();
    diag.info("STORAGE", "Factory reset complete. Please reboot.");
}

// ── Diagnostics Dump ───────────────────────────────────────────────

/**
 * @brief Log the entire configuration state at INFO level.
 *
 * Useful for debugging: shows WiFi list, MQTT config (without password),
 * calibration offsets per phase, thresholds, and AP config.
 *
 * Password fields are omitted from the log for security.
 */
void StorageManager::dump() const {
    diag.info("STORAGE", "=== StorageManager Dump ===");
    int n = getWifiCount();
    diag.info("STORAGE", "WiFi count: %d", n);
    for (int i = 0; i < n; i++) {
        WiFiEntry e = getWifiEntry(i);
        diag.info("STORAGE", "  [%d] SSID: %s", i, e.ssid);
    }
    MQTTConfig m = getMQTTConfig();
    diag.info("STORAGE", "MQTT: %s  port:%d  ssl:%d  user:%s  prefix:%s  topic:%s",
        m.server, m.port, m.useSSL, m.user, m.prefix, m.topic);
    CalibrationConfig c = getCalibration();
    for (int i = 0; i < 3; i++) {
        diag.info("STORAGE", "Cal[%d]: V_offset=%.3f  I_offset=%.3f",
            i, c.voltageOffset[i], c.currentOffset[i]);
    }
    ThresholdConfig t = getThresholds();
    diag.info("STORAGE", "Thresholds: LOST=%.1f  UNDER=%.1f  OVER=%.1f  UNBAL=%.2f%%",
        t.voltageLost, t.voltageUnder, t.voltageOver, t.unbalanceMax);
    APConfig ap = getAPConfig();
    diag.info("STORAGE", "AP: SSID=%s", ap.ssid);
    diag.info("STORAGE", "===========================");
}

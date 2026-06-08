#include "storage_manager.h"
#include "diagnostics.h"

void StorageManager::_open(bool readOnly) const { _prefs.begin(PREF_NAMESPACE, readOnly); }
void StorageManager::_close() const { _prefs.end(); }
void StorageManager::_wifiSSIDKey(int i, char* buf) { snprintf(buf, 16, "ws%d", i); }
void StorageManager::_wifiPassKey(int i, char* buf) { snprintf(buf, 16, "wp%d", i); }

void StorageManager::begin() {
    diag.info("STORAGE", "Initializing NVS...");
    _open();
    _close();
    diag.info("STORAGE", "Ready. WiFi networks stored: %d", getWifiCount());
}

int StorageManager::getWifiCount() const {
    _open(true);
    int count = _prefs.getInt("wifi_count", 0);
    _close();
    return count;
}

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

bool StorageManager::addWifiEntry(const char* ssid, const char* pass) {
    int count = getWifiCount();
    if (count >= MAX_WIFI_NETWORKS) {
        diag.warn("STORAGE", "WiFi list full");
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

bool StorageManager::deleteWifiEntry(int index) {
    int count = getWifiCount();
    if (index < 0 || index >= count) return false;
    _open();
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
    if (strlen(cfg.server) == 0) strncpy(cfg.server, DEFAULT_MQTT_SERVER, sizeof(cfg.server));
    if (strlen(cfg.user)   == 0) strncpy(cfg.user,   DEFAULT_MQTT_USER,   sizeof(cfg.user));
    if (strlen(cfg.pass)   == 0) strncpy(cfg.pass,   DEFAULT_MQTT_PASS,   sizeof(cfg.pass));
    if (strlen(cfg.prefix) == 0) strncpy(cfg.prefix, DEFAULT_MQTT_PREFIX, sizeof(cfg.prefix));
    if (strlen(cfg.topic)  == 0) strncpy(cfg.topic,  DEFAULT_MQTT_TOPIC,  sizeof(cfg.topic));
    return cfg;
}

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

void StorageManager::factoryReset() {
    diag.warn("STORAGE", "*** FACTORY RESET – clearing all NVS data ***");
    _open();
    _prefs.clear();
    _close();
    diag.info("STORAGE", "Factory reset complete. Please reboot.");
}

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

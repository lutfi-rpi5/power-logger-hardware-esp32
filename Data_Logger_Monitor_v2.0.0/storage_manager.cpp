#include "storage_manager.h"
#include "config.h"
#include <Preferences.h>

static Preferences prefs;

void StorageManager::begin() {
    prefs.begin(PREF_NAMESPACE, false);  // read-write
    _seq = prefs.getUInt("seq", 0);
}

// ─── WiFi Network List ──────────────────────────────────────

int StorageManager::getWifiCount() {
    return prefs.getInt("wifiCnt", 0);
}

WiFiEntry StorageManager::getWifiEntry(int index) {
    WiFiEntry e = { "", "" };
    char key[16];
    snprintf(key, sizeof(key), "w_ssid_%d", index);
    String ssid = prefs.getString(key, "");
    snprintf(key, sizeof(key), "w_pass_%d", index);
    String pass = prefs.getString(key, "");
    ssid.toCharArray(e.ssid, sizeof(e.ssid));
    pass.toCharArray(e.password, sizeof(e.password));
    return e;
}

bool StorageManager::addWifiEntry(const char* ssid, const char* pass) {
    int cnt = getWifiCount();
    if (cnt >= MAX_WIFI_ENTRIES) return false;

    char key[16];
    snprintf(key, sizeof(key), "w_ssid_%d", cnt);
    prefs.putString(key, ssid);
    snprintf(key, sizeof(key), "w_pass_%d", cnt);
    prefs.putString(key, pass);
    prefs.putInt("wifiCnt", cnt + 1);
    return true;
}

bool StorageManager::deleteWifiEntry(int index) {
    int cnt = getWifiCount();
    if (index < 0 || index >= cnt) return false;

    // Shift remaining entries up
    for (int i = index; i < cnt - 1; i++) {
        WiFiEntry next = getWifiEntry(i + 1);
        char key[16];
        snprintf(key, sizeof(key), "w_ssid_%d", i);
        prefs.putString(key, next.ssid);
        snprintf(key, sizeof(key), "w_pass_%d", i);
        prefs.putString(key, next.password);
    }

    // Remove last entry
    char key[16];
    snprintf(key, sizeof(key), "w_ssid_%d", cnt - 1);
    prefs.remove(key);
    snprintf(key, sizeof(key), "w_pass_%d", cnt - 1);
    prefs.remove(key);

    prefs.putInt("wifiCnt", cnt - 1);
    return true;
}

// ─── MQTT Config ────────────────────────────────────────────

MQTTConfig StorageManager::getMQTTConfig() {
    MQTTConfig cfg;
    memset(&cfg, 0, sizeof(cfg));

    String s = prefs.getString("mqtt_server", "");
    strlcpy(cfg.server, s.c_str(), sizeof(cfg.server));
    cfg.port    = prefs.getUShort("mqtt_port", 1883);
    cfg.sslPort = prefs.getUShort("mqtt_sslport", 8883);

    s = prefs.getString("mqtt_user", "");
    strlcpy(cfg.user, s.c_str(), sizeof(cfg.user));
    s = prefs.getString("mqtt_pass", "");
    strlcpy(cfg.pass, s.c_str(), sizeof(cfg.pass));
    s = prefs.getString("mqtt_prefix", MQTT_DEFAULT_PREFIX);
    strlcpy(cfg.prefix, s.c_str(), sizeof(cfg.prefix));
    s = prefs.getString("mqtt_topic", MQTT_DEFAULT_TOPIC);
    strlcpy(cfg.topic, s.c_str(), sizeof(cfg.topic));

    cfg.useSSL = prefs.getBool("mqtt_ssl", false);
    cfg.useWS  = prefs.getBool("mqtt_ws", false);

    s = prefs.getString("mqtt_cacert", "");
    strlcpy(cfg.caCert, s.c_str(), sizeof(cfg.caCert));

    return cfg;
}

bool StorageManager::saveMQTTConfig(const MQTTConfig& cfg) {
    prefs.putString("mqtt_server", cfg.server);
    prefs.putUShort("mqtt_port", cfg.port);
    prefs.putUShort("mqtt_sslport", cfg.sslPort);
    prefs.putString("mqtt_user", cfg.user);
    prefs.putString("mqtt_pass", cfg.pass);
    prefs.putString("mqtt_prefix", cfg.prefix);
    prefs.putString("mqtt_topic", cfg.topic);
    prefs.putBool("mqtt_ssl", cfg.useSSL);
    prefs.putBool("mqtt_ws", cfg.useWS);
    prefs.putString("mqtt_cacert", cfg.caCert);
    return true;
}

// ─── Calibration ────────────────────────────────────────────

CalibrationData StorageManager::getCalibration() {
    CalibrationData cal;
    for (int i = 0; i < 3; i++) {
        char key[16];
        snprintf(key, sizeof(key), "cal_v_%d", i);
        cal.voltageOffset[i] = prefs.getFloat(key, 0.0f);
        snprintf(key, sizeof(key), "cal_i_%d", i);
        cal.currentOffset[i] = prefs.getFloat(key, 0.0f);
    }
    return cal;
}

bool StorageManager::saveCalibration(const CalibrationData& cal) {
    for (int i = 0; i < 3; i++) {
        char key[16];
        snprintf(key, sizeof(key), "cal_v_%d", i);
        prefs.putFloat(key, cal.voltageOffset[i]);
        snprintf(key, sizeof(key), "cal_i_%d", i);
        prefs.putFloat(key, cal.currentOffset[i]);
    }
    return true;
}

// ─── Thresholds ─────────────────────────────────────────────

ThresholdData StorageManager::getThresholds() {
    ThresholdData thr;
    thr.voltageLost  = prefs.getFloat("th_vlost",  DEFAULT_VOLTAGE_LOST_MAX);
    thr.voltageUnder = prefs.getFloat("th_vunder", DEFAULT_VOLTAGE_UNDER_MIN);
    thr.voltageOver  = prefs.getFloat("th_vover",  DEFAULT_VOLTAGE_OVER_MIN);
    thr.unbalanceMax = prefs.getFloat("th_unbal",  DEFAULT_UNBALANCE_MAX);
    return thr;
}

bool StorageManager::saveThresholds(const ThresholdData& thr) {
    prefs.putFloat("th_vlost",  thr.voltageLost);
    prefs.putFloat("th_vunder", thr.voltageUnder);
    prefs.putFloat("th_vover",  thr.voltageOver);
    prefs.putFloat("th_unbal",  thr.unbalanceMax);
    return true;
}

// ─── Factory Reset ──────────────────────────────────────────

void StorageManager::factoryReset() {
    prefs.clear();
    _seq = 0;
}

// ─── Sequence ───────────────────────────────────────────────

uint32_t StorageManager::getSequence() {
    return _seq;
}

void StorageManager::incrementSequence() {
    _seq++;
    prefs.putUInt("seq", _seq);
}

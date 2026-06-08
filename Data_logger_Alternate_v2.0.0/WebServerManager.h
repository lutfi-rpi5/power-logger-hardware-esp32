#pragma once

/******************************************************
 * File      : WebServerManager.h
 * Description:
 *   HTTP web server untuk konfigurasi device.
 *   Aktif saat config mode (dari menu OLED).
 *
 *   Saat aktif:
 *     - ESP32 membuat SoftAP (SSID/pass dari EEPROM)
 *     - Melayani halaman config di http://192.168.4.1
 *     - Mode WIFI_AP_STA: STA tetap jalan untuk MQTT
 *
 *   AP ini TIDAK meneruskan internet ke client.
 *   Client hanya bisa akses WebConfig di 192.168.4.1.
 *   Semua request lain di-redirect ke 192.168.4.1.
 *
 *   Halaman config:
 *     GET  /               → Navigasi utama
 *     GET  /wifi           → Kelola daftar WiFi
 *     POST /wifi/add       → Tambah WiFi
 *     POST /wifi/delete    → Hapus WiFi
 *     GET  /mqtt           → Config MQTT broker
 *     POST /mqtt           → Simpan config MQTT
 *     GET  /calibration    → Kalibrasi PZEM + threshold
 *     POST /calibration    → Simpan kalibrasi & threshold
 *     POST /factoryreset   → Hapus semua EEPROM
 *
 *   Library: ESPAsyncWebServer by ESP32Async v3.11.0
 *            AsyncTCP by ESP32Async v3.4.10
 ******************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "StorageManager.h"
#include "PhaseData.h"

class WebServerManager {
public:
    explicit WebServerManager(StorageManager& storage);

    /** Register semua routes. Panggil sekali di setup(). */
    void begin();

    /** Aktifkan SoftAP + HTTP server. */
    void activate();

    /** Matikan SoftAP + HTTP server. */
    void deactivate();

    bool isActive() const { return _active; }

    /** IP address AP (sebagai string). */
    String getIP() const;

    /** Callback: kalibrasi disimpan → DAQ reload offsets. */
    std::function<void()> onCalibrationSaved;

    /** Callback: config MQTT disimpan → MQTT reconnect. */
    std::function<void()> onMQTTSaved;

    /** Callback: threshold disimpan → DAQ reload thresholds. */
    std::function<void()> onThresholdSaved;

private:
    StorageManager&  _storage;
    AsyncWebServer   _server;
    bool             _active = false;

    void _setupRoutes();

    static String _pageIndex(StorageManager& s);
    static String _pageWifi(StorageManager& s);
    static String _pageMQTT(StorageManager& s);
    static String _pageCalibration(StorageManager& s);
    static String _htmlHead(const char* title);
    static String _htmlFoot();
};

# 3-Phase Data Logger v2.1.0 — Mix

ESP32-based firmware for a 3-phase power data logger using 3× PZEM-004T sensors, SSD1306 OLED display, and MQTT telemetry.

**Mix Version** — Menggabungkan arsitektur FreeRTOS dual-core dari kode alternat dengan fitur kalibrasi, JSON builder, diagnostics, dan auth web dari opencode.

---

## Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [Software Requirements](#software-requirements)
- [Setup & Installation](#setup--installation)
- [Firmware Architecture](#firmware-architecture)
- [File Structure](#file-structure)
- [MQTT JSON Schema](#mqtt-json-schema)
- [OLED Display](#oled-display)
- [Menu Navigation](#menu-navigation)
- [Web Interface](#web-interface)
- [Calibration System](#calibration-system)
- [Fake Data Mode](#fake-data-mode)
- [Diagnostics System](#diagnostics-system)
- [Voltage Unbalance Calculation](#voltage-unbalance-calculation)
- [Non-Blocking Design](#non-blocking-design)
- [MQTT Non-Blocking Connect](#mqtt-non-blocking-connect)
- [Self-Healing](#self-healing)
- [File Reference](#file-reference)
- [Troubleshooting](#troubleshooting)

---

## Overview

This device **acquires** 3-phase electrical data from PZEM-004T sensors, **processes** it with calibration offsets (V/I additive model), **displays** real-time readings on an OLED screen, and **publishes** structured JSON telemetry to an MQTT broker.

| Feature | Deskripsi |
|---------|-----------|
| Architecture | FreeRTOS dual-core: DAQ di Core 0, loop di Core 1 |
| Multi-threading | Mutex-protected `SystemState` dengan `getStateCopy()` / `updateState()` |
| Data acquisition | Task terpisah Core 0, 1 Hz, priority tertinggi |
| MQTT | PubSubClient + state machine non-blocking 5-state + DNS di background task |
| JSON | ArduinoJson, format `{seq, device, phases, unbalance, ts}` |
| Web config | AP mode + ESPAsyncWebServer + Basic Auth |
| OLED | 3 monitoring pages + menu (Config / Reset kWh / Reboot) |
| Kalibrasi | Additive V/I offset, recompute S, Q, PF |
| Diagnostics | Leveled logging (NONE/ERROR/WARN/INFO/DEBUG), gantikan `Serial.print` |
| Fake data | Compile-time flag untuk testing tanpa hardware |

---

## Hardware Requirements

| Component | Specification | Quantity |
|-----------|--------------|----------|
| MCU | ESP32 DevKit V1 (30-pin) | 1 |
| Sensor | PZEM-004T v3.0 (100A) | 3 |
| Display | OLED SSD1306 128×64 I2C | 1 |
| Button | Push button (NO, not latching) | 1 |
| LED | Built-in ESP32 (GPIO2) | 1 |

### 3-Phase WYE Connection

```
L1 (R) ─────── PZEM #1 ───────┐
L2 (S) ─────── PZEM #2 ───────┤
L3 (T) ─────── PZEM #3 ───────┤
                               │
N (Neutral) ───────────────────┘
```

Setiap PZEM mengukur phase-to-neutral.

---

## Pin Configuration

| Peripheral | Pin | Notes |
|------------|-----|-------|
| PZEM R + S (UART1) | RX=4, TX=15 | Dua phase berbagi satu serial |
| PZEM T (UART2) | RX=17, TX=16 | Phase T di serial terpisah |
| OLED I2C | SDA=21, SCL=22 | Address 0x3C |
| Button | GPIO5 | INPUT_PULLUP |
| Built-in LED | GPIO2 | Active HIGH |

### PZEM Addresses

| Phase | Address | Serial Port |
|-------|---------|-------------|
| R | 0x10 | UART1 |
| S | 0x11 | UART1 |
| T | 0x12 | UART2 |

---

## Software Requirements

### Arduino IDE

- **Arduino IDE** v2.0 or later
- **ESP32 Board Package** v3.3.0
  - Board URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-packages/package_esp32_index.json`
  - Board: `ESP32 Dev Module`

### Required Libraries

| Library | Version | Author | Purpose |
|---------|---------|--------|---------|
| PZEM004Tv30 | ≥1.2.1 | Jakub Mandula | PZEM sensor communication |
| Adafruit SSD1306 | ≥2.5.16 | Adafruit | OLED display driver |
| Adafruit GFX | ≥1.12.5 | Adafruit | Graphics primitives |
| PubSubClient | ≥2.8 | Nick O'Leary | MQTT client |
| ArduinoJson | ≥6.21.6 | Benoit Blanchon | JSON serialization |
| ESP Async WebServer | ≥3.11.0 | ESP32Async | HTTP web server |
| Async TCP | ≥3.4.10 | ESP32Async | TCP async support |

Install via Arduino Library Manager (Sketch → Include Library → Manage Libraries).

---

## Setup & Installation

### 1. Clone / download folder

Buka folder `Data_Logger_Mix_v2.0.0/` di Arduino IDE.

### 2. Configure credentials

```bash
cp credential_example.cpp credentials.cpp
```

Edit `credentials.cpp` dan isi WiFi & MQTT credentials:

```cpp
WiFiCredential wifiList[] = {
    { "YourWiFiSSID", "YourWiFiPassword", 1 },
};
const char* MQTT_SERVER = "broker.example.com";
```

> **Security:** `credentials.cpp` sudah di `.gitignore`. Jangan commit credentials asli.

### 3. Open in Arduino IDE

- Buka `Data_Logger_Mix_v2.0.0.ino`
- Pilih board: **ESP32 Dev Module**
- Pilih port
- Klik **Upload**

### 4. First boot

1. Device tampilkan boot screen + pesan inisialisasi
2. WiFi koneksi async di background (tidak block boot)
3. OLED langsung menampilkan data real-time
4. MQTT koneksi state machine berjalan otomatis

---

## Firmware Architecture

FreeRTOS dual-core design dengan mutex-protected shared state:

```
┌─ Core 0 (CORE_SENSOR) ─────────────────────────┐
│  PZEMReader task  (1 Hz, highest priority)     │
│   ├─ readOnce() → baca 3× PZEM                 │
│   ├─ Calibration::apply() → offset V/I         │
│   ├─ computeUnbalance() → NEMA method          │
│   ├─ write ke gState (mutex-protected)          │
│   └─ feed Task Watchdog Timer                  │
└────────────────────────────────────────────────┘
┌─ Core 1 (CORE_COMM / Arduino loop) ────────────┐
│  WiFiManager::tick()   — reconnect management  │
│  MQTTManager::tick()   — keepalive + JSON pub  │
│  MenuSystem::tick()    — button + OLED render  │
│  LEDSignal::tick()     — non-blocking LED      │
│  _checkConnectionSignals() — LED on connect    │
│  _selfHealingCheck()   — heap guard            │
│  taskMgr.syncNTP()     — periodic NTP sync     │
└────────────────────────────────────────────────┘
```

### Shared State

Semua data hidup di `gState` (struct `SystemState`) yang dilindungi `gStateMutex` (FreeRTOS Semaphore):

```cpp
// Thread-safe read
SystemState snap = getStateCopy();

// Thread-safe write
updateState([&](SystemState& s) {
    s.wifiConnected = true;
    s.wifiRSSI      = WiFi.RSSI();
});
```

---

## File Structure

| File | Purpose |
|------|---------|
| `Data_Logger_Mix_v2.0.0.ino` | Entry point, setup/loop, callback wiring, watchdog |
| `config.h` | Hardcoded settings (pins, thresholds, versions, timing) |
| `types.h` | Shared structs (`SystemState`, `PhaseReading`, `CalibrationConfig`) + mutex helpers |
| `credentials.h` | Credential declarations |
| `credential_example.cpp` | Credential template (copy → `credentials.cpp`) |
| `diagnostics.h/.cpp` | Leveled logging system |
| `calibration.h/.cpp` | Additive V/I offset + recompute derived params |
| `json_builder.h/.cpp` | Build JSON MQTT payload |
| `task_manager.h/.cpp` | NTP sync, `isTime()` scheduler |
| `button_manager.h/.cpp` | Single button: debounce, short/long press |
| `led_signal.h/.cpp` | Built-in LED blink patterns |
| `display_oled.h/.cpp` | OLED rendering (3 monitor pages + menus) |
| `menu_manager.h/.cpp` | Menu state machine (4 items + submenus) |
| `data_acquisition.h/.cpp` | PZEM reads, FreeRTOS task, calibration, unbalance |
| `wifi_manager.h/.cpp` | Async WiFi connect from known networks |
| `mqtt_manager.h/.cpp` | Non-blocking MQTT connect + publish (5-state machine) |
| `storage_manager.h/.cpp` | NVS read/write for WiFi, MQTT, calibration, thresholds, AP |
| `webserver_manager.h/.cpp` | AP + HTTP server with HTML config pages + Basic Auth |
| `storage_manager.h/.cpp` | NVS persistent config storage |

---

## MQTT JSON Schema

Topic: `{prefix}telemetry` (dari MQTT config web, misal `lutpiii/telemetry`)

### Payload

```json
{
  "seq": 1247,
  "device": {
    "id": "3ph-logger-001",
    "fw": "v2.1.0",
    "uptime": 3600,
    "heap": 148432,
    "rssi": -65
  },
  "phases": {
    "R": {
      "valid": true,
      "v": 220.1,
      "i": 12.34,
      "p": 2650.0,
      "s": 2714.6,
      "q": 582.1,
      "pf": 0.97,
      "f": 50.0,
      "e": 15230.0,
      "status": "OK"
    },
    "S": {
      "valid": true,
      "v": 200.6,
      "i": 12.34,
      "p": 2650.0,
      "s": 2714.6,
      "q": 582.1,
      "pf": 0.97,
      "f": 50.0,
      "e": 15230.0,
      "status": "UNDER"
    },
    "T": {
      "valid": false,
      "v": 0.0,
      "i": 0.0,
      "p": 0.0,
      "s": 0.0,
      "q": 0.0,
      "pf": 0.0,
      "f": 0.0,
      "e": 0.0,
      "status": "LOST"
    },
    "unbalance": 4.25
  },
  "ts": 1717001234
}
```

### Field Descriptions

| Field | Type | Description |
|-------|------|-------------|
| `seq` | uint32 | Incrementing sequence number (detect data gaps) |
| `device.id` | string | Device identifier |
| `device.fw` | string | Firmware version |
| `device.uptime` | uint32 | Seconds since boot |
| `device.heap` | uint32 | Free heap memory (bytes) |
| `device.rssi` | int32 | WiFi RSSI (dBm) |
| `phases.*.valid` | bool | Phase data validity |
| `phases.*.v` | float | Voltage (V, 1 decimal) |
| `phases.*.i` | float | Current (A, 2 decimals) |
| `phases.*.p` | float | Active power (W, 1 decimal) |
| `phases.*.s` | float | Apparent power (VA, 1 decimal) |
| `phases.*.q` | float | Reactive power (VAr, 1 decimal) |
| `phases.*.pf` | float | Power factor (2 decimals) |
| `phases.*.f` | float | Frequency (Hz, 1 decimal) |
| `phases.*.e` | float | Cumulative energy (Wh, 1 decimal) |
| `phases.*.status` | string | `OK` / `UNDER` / `OVER` / `LOST` |
| `phases.unbalance` | float | Voltage unbalance % (NEMA method) |
| `ts` | uint32 | Timestamp (`millis()` untuk sementara) |

### Key Differences from v1.x

- `seq` field added for gap detection
- `valid` boolean per phase (tidak perlu parsing status untuk skip data)
- Semua angka adalah **raw number** (bukan string)
- `ts` menggunakan `millis()` jika NTP belum sync (nanti akan pakai Unix epoch)
- `valid: false` tetap mengirim semua field dengan nilai 0

---

## OLED Display

128×64 pixel SSD1306 I2C display.

### Title Bar

```
3-Phase Data Logger  QW
────────────────────
```

- **Q** = MQTT connected
- **W** = WiFi connected
- Ikon independen — masing-masing muncul jika terhubung

### Monitoring Mode

**Page 1 — Voltage / Current / Status / Unbalance**

```
3-Phase Data Logger  QW
  R         S         T
 220V      220V      220V
 10.0A     10.0A     10.0A
  OK        OK        OK
 Unbalance=2.50%
```

- Status blink saat LOST / UNDER / OVER
- `[!]` icon blink saat unbalance melebihi threshold

**Page 2 — Frequency / Apparent Power / Reactive Power**

```
3-Phase Data Logger  QW
  R         S         T
 50.0Hz    50.0Hz    50.0Hz
 2200VA   2300VA    2100VA
 1200VAr  1300VAr   1100VAr
```

- Auto-scale: `<1000` base unit, `≥1000` kilo prefix (kVA, kVAr)

**Page 3 — Power Factor / Active Power / Energy**

```
3-Phase Data Logger  QW
  R         S         T
 0.97PF    0.95PF    0.96PF
 2100W     2200W     2000W
 15230Wh   14800Wh   16000Wh
```

- Auto-scale: W↔kW, Wh↔kWh

---

## Menu Navigation

| Action | Duration | Effect |
|--------|----------|--------|
| Short press | <600ms | Next monitoring page / move cursor |
| Long press | ≥600ms | Enter menu / confirm / back |

### Menu Tree

```
Monitoring (Page 1/2/3)
    │ long press
    ▼
Main Menu
    ├── Config ────────▶ [Active / Inactive] (toggle AP+Web)
    ├── Reset kWh ─────▶ [Reset / Back] → Result screen (2s)
    ├── Reboot Device ─▶ [Reboot / Back] → Countdown 3..2..1..0
    └── Back to Monitoring
```

### Screen Layouts

**Main Menu:**
```
3-Phase Data Logger  QW
> Config
  Reset kWh
  Reboot Device
  Back to Monitoring
```

**Config (AP toggle):**
```
    CONFIGURATION     QW
 SSID: DataLogger
 PW  : 12345678
 192.168.4.1
 > Active         Back
```

**Reset kWh confirm:**
```
    RESET kWh         QW
 Apakah anda yakin
 untuk Reset kWh?

 > Reset          Back
```

**Reboot countdown:**
```
    REBOOT            QW

   Device akan Reboot
     dalam 3 detik
```

---

## Web Interface

Aktifkan via OLED Menu → Config → pilih **Active** (long press).

### Login

Default credentials:
- Username: `ADMIN`
- Password: `18273645`

(ubah hanya di `config.h` — perlu recompile)

Halaman web dilindungi **HTTP Basic Auth**. Browser akan menampilkan popup login.

### Pages

**Home (`/`)** — Card navigation:

- Manage Known Networks
- MQTT Connection Config
- PZEM-004T Calibration & Thresholds
- Danger Zone: Hard Reset EEPROM

**Manage Known Networks (`/wifi`)** — Table of saved WiFi + Add form

| No. | SSID | Action |
|-----|------|--------|
| 1 | NetworkA | [Delete] |
| 2 | NetworkB | [Delete] |

**MQTT Connection Config (`/mqtt`)**

- Server, Port (non-SSL), Port (SSL)
- Username, Password
- Topic Prefix
- Use SSL / Use WebSocket (checkbox)
- CA Certificate (textarea)

**PZEM Calibration & Thresholds (`/calibration`)**

Per-phase:
- Voltage offset (V) — additive, 0.001 precision
- Current offset (A) — additive, 0.001 precision

Thresholds:
- LOST threshold (V)
- UNDER threshold (V)
- OVER threshold (V)
- Max Unbalance (%)

**Hard Reset (`/factoryreset`)** — POST-only, konfirmasi via JavaScript `confirm()`, reboot otomatis setelah reset.

### Captive Portal

Semua request yang tidak dikenal di-redirect ke `http://192.168.4.1/` — menangani captive portal detection dari Android/iOS/Windows.

---

## Calibration System

### Offset Model

Additive offset:

```
V_calibrated = V_raw + voltageOffset[phase]
I_calibrated = I_raw + currentOffset[phase]
```

Offset dapat positif atau negatif. Dikonfigurasi via web, disimpan di NVS.

### Derived Parameter Propagation

| Parameter | Formula | Efek Kalibrasi? |
|-----------|---------|-----------------|
| Voltage (V) | V_raw + V_off | **Ya** |
| Current (I) | I_raw + I_off | **Ya** |
| Apparent power (S) | V_cal × I_cal | **Ya** |
| Active power (P) | Dari PZEM (raw) | Tidak (masih raw) |
| Reactive power (Q) | √(S² − P²) | **Ya** (via S) |
| Power factor (PF) | P / S | **Ya** (via S) |
| Energy (Wh) | Dari PZEM | Tidak |
| Frequency (Hz) | Dari PZEM | **Tidak** |

---

## Fake Data Mode

Untuk testing tanpa hardware PZEM:

Di `config.h`:

```cpp
#define FAKE_DATA_ENABLED   true   // default: false
```

Saat diaktifkan, `readOnce()` akan generate data random:

| Parameter | Range |
|-----------|-------|
| V | 200.0 – 203.99 V |
| I | 10.00 – 14.99 A |
| P | V × I × 0.85 |
| S | V × I |
| E | 10000 – 10999 Wh |
| F | 49.90 – 50.09 Hz |
| PF | 0.80 – 0.99 |

Fake data tetap melewati `Calibration::apply()` dan `_computeStatus()`.

---

## Diagnostics System

Menggantikan semua `Serial.print/printf` dengan logging level:

```cpp
diag.begin(LOG_INFO);       // default di setup()
diag.setLevel(LOG_DEBUG);   // maksimal verbosity

diag.error("TAG", "message");
diag.warn("TAG", "message %d", value);
diag.info("TAG", "message %s", str);
diag.debug("TAG", "message %.2f", val);
```

### Log Levels

| Level | Value | Output |
|-------|-------|--------|
| `LOG_NONE` | 0 | Tidak ada output |
| `LOG_ERROR` | 1 | Error saja |
| `LOG_WARN` | 2 | Error + Warning |
| `LOG_INFO` | 3 | Error + Warning + Info (default) |
| `LOG_DEBUG` | 4 | Semua termasuk debug |

### Format Output

```
[I][MAIN] Setup complete. Entering main loop on Core 1.
[W][MQTT] DNS gagal: broker.example.com tidak dapat di-resolve.
[E][WDT] CRITICAL: free heap 4096 bytes < threshold 8192 — rebooting!
```

---

## Voltage Unbalance Calculation

Menggunakan **NEMA method** (standar Indonesia / SPLN):

1. Rata-rata tegangan:
   `V_avg = (V_R + V_S + V_T) / 3`

2. Deviasi maksimum dari rata-rata:
   `max_dev = max(|V_R − V_avg|, |V_S − V_avg|, |V_T − V_avg|)`

3. Persentase unbalance:
   `unbalance = (max_dev / V_avg) × 100`

### Threshold Defaults (configurable via web)

| Threshold | Default | Deskripsi |
|-----------|---------|-----------|
| `VOLTAGE_LOST_MAX` | 80.0 V | Di bawah ini → LOST |
| `VOLTAGE_UNDER_MIN` | 180.0 V | Di bawah ini (tapi > LOST) → UNDER |
| `VOLTAGE_OVER_MAX` | 240.0 V | Di atas ini → OVER |
| `UNBALANCE_MAX` | 3.0% | Di atas ini → `[!]` warning di OLED |

---

## Non-Blocking Design

**Tidak ada `delay()`** kecuali:
- `delay(600)` di akhir setup (boot screen)
- `delay(100)` di diagnostics `begin()` (stabilisasi serial)
- `delay(200)` antara PZEM reset commands

| Operation | Method |
|-----------|--------|
| WiFi connect | `WiFi.begin()` async, non-blocking |
| WiFi reconnection | `millis()`-based retry setiap 30s |
| MQTT connect | 5-state machine + DNS di background task |
| OLED refresh | `millis()`-based, interval 500ms |
| Data acquisition | FreeRTOS task Core 0, `vTaskDelayUntil()` |
| MQTT publish | `millis()`-based, interval 1000ms |
| Button debounce | State machine + `millis()` tracking |
| Reboot countdown | Non-blocking `millis()` decrement |
| NTP sync | Periodic, hanya jalan jika WiFi connected |

**Device berjalan segera** tanpa menunggu WiFi/MQTT. OLED dan sensor bekerja dari detik pertama.

---

## MQTT Non-Blocking Connect

MQTT menggunakan state machine 5-state untuk memastikan loop task tidak pernah block:

```
IDLE → DNS_RESOLVING → TCP_CONNECTING → MQTT_CONNECTING → CONNECTED
  │          │               │                 │              │
  └──────────┴───────────────┴─────────────────┴── COOLDOWN ──┘
```

### Detail State

| State | Deskripsi |
|-------|-----------|
| `IDLE` | Menunggu cooldown selesai, lalu mulai siklus baru |
| `DNS_RESOLVING` | DNS resolution di FreeRTOS task terpisah (Core 0, prio 0) — loop **tidak nge-block** |
| `TCP_CONNECTING` | TCP connect ke IP langsung (bukan hostname) — synchronous tapi WDT 30s cukup |
| `MQTT_CONNECTING` | Kirim MQTT CONNECT, tunggu CONNACK |
| `CONNECTED` | Fully connected, publish telemetry |
| `COOLDOWN` | Gagal, tunggu `MQTT_RECONNECT_INTERVAL_MS` (5s) sebelum retry |

Root cause reboot (IDF5): `client.connect(hostname, port)` blocking untuk DNS resolution (~5-8 detik tanpa internet) → WDT fire. Fix:
1. **WDT timeout 30s** (bukan default 5s)
2. **DNS resolution di background task** — loop tidak pernah block untuk DNS
3. **TCP connect ke IPAddress langsung** — tidak ada DNS call saat connect

---

## Self-Healing

Dua mekanisme independen melindungi dari system malfunction:

### 1. Task Watchdog Timer (WDT)

- DAQ task (Core 0) dan loop task (Core 1) terdaftar
- Timeout: 30 detik (`WDT_TIMEOUT_SEC`)
- Panic enabled → reboot otomatis jika task stall

### 2. Heap Guard

```cpp
static void _selfHealingCheck() {
    uint32_t freeHeap = ESP.getFreeHeap();
    if (freeHeap < HEAP_CRITICAL_MIN_BYTES) {
        ESP.restart();  // reboot sebelum crash
    }
}
```

- Threshold: 8192 bytes (`HEAP_CRITICAL_MIN_BYTES`)
- Warning log saat heap < 2× threshold
- Reboot paksa jika di bawah threshold

---

## File Reference

### `config.h`

Semua programmer-only settings:

```cpp
#define DEVICE_ID       "3ph-logger-001"
#define FW_VERSION      "v2.1.0"
#define FAKE_DATA_ENABLED   false   // ganti true untuk fake data
#define WEB_USERNAME    "ADMIN"
#define WEB_PASSWORD    "18273645"
```

### `types.h`

Struct dan helper untuk thread-safe shared state:

```cpp
struct PhaseReading {
    float voltage, current, activePower, apparentPower, reactivePower;
    float powerFactor, frequency, energyWh;
    LineStatus status;
    bool valid;
    unsigned long timestampMs;
};

struct SystemState {
    PhaseReading phases[3];
    float unbalance, thresholdUnbalanceMax;
    bool wifiConnected, mqttConnected;
    int32_t wifiRSSI;
    uint32_t freeHeapBytes, uptimeSeconds;
    bool webServerActive;
    char apSSID[32], apPass[32], apIP[16];
};
```

### `diagnostics.h/.cpp`

```cpp
diag.begin(LOG_INFO);               // inisialisasi
diag.info("TAG", "format %s", val); // info log
diag.setLevel(LOG_DEBUG);           // ubah level runtime
```

### `calibration.h/.cpp`

```cpp
Calibration cal;
cal.setOffsets(storage.getCalibration());
cal.apply(rawV, rawI, rawP, rawE, rawF, rawPF, phaseIndex, outputReading);
```

### `json_builder.h/.cpp`

```cpp
JSONBuilder builder;
String json = builder.build(state, seq, millis());
// → {"seq":1,"device":{...},"phases":{...},"unbalance":0,"ts":1234}
```

### `data_acquisition.h/.cpp`

FreeRTOS task di Core 0. Method utama:

- `readOnce()` — baca 3 PZEM atau generate fake data
- `resetAllEnergy()` — reset counter semua PZEM
- `reloadCalibration()` — reload offset dari NVS
- `reloadThresholds()` — reload threshold dari NVS

### `mqtt_manager.h/.cpp`

Non-blocking 5-state machine. Method:

- `tick()` — panggil tiap loop: handle reconnect + publish
- `reloadConfig()` — reload konfigurasi dari NVS

### `webserver_manager.h/.cpp`

- `activate()` — start AP (192.168.4.1) + HTTP server
- `deactivate()` — stop AP + HTTP server

---

## Troubleshooting

### OLED tidak muncul

1. Cek koneksi I2C (SDA=21, SCL=22)
2. Verifikasi alamat OLED: 0x3C
3. Jalankan I2C scanner sketch untuk verifikasi

### PZEM readings NaN semua

1. Cek wiring serial (RX/TX mungkin perlu ditukar)
2. Verifikasi alamat PZEM: 0x10, 0x11, 0x12
3. Coba `FAKE_DATA_ENABLED true` untuk test firmware tanpa hardware
4. Jalankan `setAddressPZEM.ino` untuk konfigurasi ulang alamat

### MQTT tidak connect

1. Cek status WiFi di OLED (ikon W muncul?)
2. Verifikasi server/credentials di web config
3. Cek firewall mengizinkan outbound MQTT (1883/8883)
4. Untuk SSL: pastikan CA certificate benar
5. WDT sudah 30s — DNS resolve di background task tidak akan block loop

### Web server tidak bisa diakses

1. Aktifkan dari OLED Menu → Config → pilih Active
2. Connect ke SSID `DataLogger` (password `12345678`)
3. Buka `http://192.168.4.1`
4. Login: `ADMIN` / `18273645`
5. Jika muncul popup login, masukkan credentials tersebut

### Fake data tidak aktif

Di `config.h`, pastikan:
```cpp
#define FAKE_DATA_ENABLED   true
```
Lalu recompile dan upload ulang.

---

## License

MIT License

## Author

**Muhammad Lutfi Nur Anendi**
- GitHub: https://github.com/lutfi-rpi5

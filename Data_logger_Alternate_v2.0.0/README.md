# 3-Phase Data Logger — ESP32

**Author:** Muhammad Lutfi Nur Anendi  
**Version:** v2.1.0  
**Board:** ESP32 Devkit V1 (30-pin)  
**IDE:** Arduino IDE v2.x  
**Board Package:** esp32 by Espressif System — `ESP32 Dev Module v3.3.0`  
**Board Package URL:** `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

---

## Overview

Real-time 3-phase electrical data logger using three PZEM-004T v3.0 (100 A) sensors, an SSD1306 OLED display, and MQTT telemetry over Wi-Fi. All configuration (Wi-Fi credentials, MQTT broker, calibration offsets, and voltage thresholds) is stored in NVS flash and editable through a built-in web interface — no reflash needed.

---

## Hardware

| Component | Detail |
|---|---|
| MCU | ESP32 Devkit V1 (30-pin) |
| Sensors | 3× PZEM-004T v3.0, Modbus 0x10 / 0x11 / 0x12 |
| Display | SSD1306 OLED 0.96″ I2C (128×64) |
| Button | Normally-open push button on GPIO 5 |
| LED | Built-in LED GPIO 2 |

### Pin Map

| Signal | GPIO |
|---|---|
| PZEM UART1 RX (R & S) | 4 |
| PZEM UART1 TX (R & S) | 15 |
| PZEM UART2 RX (T) | 17 |
| PZEM UART2 TX (T) | 16 |
| OLED SDA | 21 |
| OLED SCL | 22 |
| Push Button | 5 |
| Status LED | 2 |

---

## Libraries Required

Install all via Arduino Library Manager:

| Library | Author |
|---|---|
| PZEM004Tv30 | Olexa Prokopenko |
| Adafruit SSD1306 | Adafruit |
| Adafruit GFX | Adafruit |
| PubSubClient | Nick O'Leary |
| ArduinoJson (v6) | Benoit Blanchon |
| ESP Async WebServer | ESP32Async |
| Async TCP | ESP32Async |

---

## Architecture

```
┌─ Core 0 (CORE_SENSOR) ─────────────────────────┐
│  PZEMReader task  (1 Hz, highest priority)     │
│   ├─ reads 3× PZEM → writes gState.phases      │
│   ├─ computes voltage unbalance (NEMA method)  │
│   └─ feeds Task Watchdog Timer (WDT)           │
└────────────────────────────────────────────────┘
┌─ Core 1 (CORE_COMM / Arduino loop) ────────────┐
│  WiFiManager::tick()   – smart reconnect       │
│  MQTTManager::tick()   – keepalive + publish   │
│  MenuSystem::tick()    – button + OLED render  │
│  LEDSignal::tick()     – non-blocking LED      │
│  WebServerManager      – ESPAsyncWebServer     │
│  _selfHealingCheck()   – heap guard + WDT      │
└────────────────────────────────────────────────┘
```

All shared data lives in `gState` (type `SystemState`) protected by a FreeRTOS mutex. Use `getStateCopy()` for thread-safe reads and `updateState(lambda)` for thread-safe writes.

---

## Features

### AP + Internet Passthrough (NAT)
ESP32 runs in `WIFI_AP_STA` mode when the web configuration server is active. Clients connected to the ESP32 AP (`DataLogger` / `12345678`) can **access the internet** through the ESP32's STA connection via lwIP NAPT. DNS for AP clients is automatically set to `8.8.8.8`.

### Line Status
Each phase reports one of four statuses based on configurable EEPROM thresholds:

| Status | Condition |
|---|---|
| `LOST` | Voltage below LOST threshold (default 80 V) — no signal |
| `UNDER` | Voltage below UNDER threshold (default 180 V) |
| `OK` | Voltage within normal range |
| `OVER` | Voltage above OVER threshold (default 240 V) |

### 3-Phase Voltage Unbalance
Computed every reading cycle using the NEMA method:

```
VAVG     = (VR + VS + VT) / 3
MAX_DEV  = max(|VR − VAVG|, |VS − VAVG|, |VT − VAVG|)
UNBALANCE(%) = (MAX_DEV / VAVG) × 100
```

Shown on OLED Page 1. Blinks with `[!]` icon when above the configurable threshold.

### OLED Display

**Page 1 — Voltage / Current / Status / Unbalance**
```
┌────────────────────────────┐
│  3-Phase Data Logger    QW │  ← Q=MQTT  W=WiFi (independent, shown if connected)
│   R         S         T   │
│  220V      220V      220V  │
│  10.00A    10.00A    10.00A│
│   OK        OK        OK   │  ← blinks when LOST / UNDER / OVER
│  Unbalance=2.50%           │  ← shows [!] and blinks when > max threshold
└────────────────────────────┘
```

**Page 2 — Frequency / Apparent Power / Reactive Power**  
**Page 3 — Power Factor / Active Power / Energy (kWh)**

### OLED Warning Blink
Status text (LOST / UNDER / OVER) and the `[!]` unbalance icon blink at `OLED_BLINK_INTERVAL_MS` (default 500 ms, adjustable in `config.h`).

### Self-Healing
Two independent mechanisms protect against system malfunction:

1. **Task Watchdog Timer (WDT):** Both the DAQ task (Core 0) and the loop task (Core 1) are registered. Timeout is `WDT_TIMEOUT_SEC` (default 30 s). If either task stalls, the ESP32 panics and reboots automatically.

2. **Heap Guard:** `_selfHealingCheck()` runs every loop iteration. If `ESP.getFreeHeap()` drops below `HEAP_CRITICAL_MIN_BYTES` (default 8 192 bytes), the device reboots immediately before a crash can occur. A soft warning is logged when heap reaches 2× the critical threshold.

### JSON Telemetry (MQTT)

Published to `{topic}telemetry` every `MQTT_PUBLISH_INTERVAL_MS`:

```json
{
  "device": { "id": "3ph-logger-001", "fw": "v2.1.0", "uptime": 3600, "heap": 120000, "rssi": -65 },
  "phases": {
    "R": { "v": "220.1", "i": "10.20", "p": "2100.0", "s": "2244.2", "q": "785.6",
           "pf": "0.94", "f": "50.0", "e": "1200.5", "status": "OK" },
    "S": { ... },
    "T": { "v": "245.0", ..., "status": "OVER" }
  },
  "unbalance": "4.25",
  "ts": 3600000
}
```

`status` field values: `OK` / `UNDER` / `OVER` / `LOST`

---

## Web Configuration

Access at `http://192.168.4.1` after enabling config mode from the OLED menu.

| Page | Path | Description |
|---|---|---|
| Home | `/` | Navigation hub + factory reset |
| WiFi | `/wifi` | Add / delete known networks (up to 5) |
| MQTT | `/mqtt` | Broker address, port, SSL, credentials, topic |
| Calibration & Thresholds | `/calibration` | Per-phase V/I offsets + LOST/UNDER/OVER/Unbalance thresholds |

### Threshold Defaults (configurable via web)

| Parameter | Default | NVS Key |
|---|---|---|
| LOST threshold (V) | 80 V | `th_vlost` |
| UNDER threshold (V) | 180 V | `th_vunder` |
| OVER threshold (V) | 240 V | `th_vover` |
| Max Unbalance (%) | 3.0 % | `th_unbal` |

---

## OLED Menu Navigation

| Button Action | Effect |
|---|---|
| Short press (< 600 ms) | Next item / cycle monitoring page |
| Long press (≥ 600 ms) | Confirm / enter / back |

Menu tree: **Monitoring → Main Menu → Config / Reset kWh / Reboot**

---

## Hardcoded Constants (`config.h` — requires reflash to change)

| Constant | Default | Description |
|---|---|---|
| `OLED_BLINK_INTERVAL_MS` | 500 | Warning blink interval (ms) |
| `WDT_TIMEOUT_SEC` | 30 | Task watchdog timeout (s) |
| `HEAP_CRITICAL_MIN_BYTES` | 8192 | Minimum free heap before forced reboot |
| `PZEM_READ_INTERVAL_MS` | 1000 | Sensor read period (ms) |
| `MQTT_PUBLISH_INTERVAL_MS` | 1000 | MQTT publish period (ms) |
| `WIFI_RECONNECT_INTERVAL_MS` | 30000 | WiFi retry interval (ms) |
| `OLED_REFRESH_INTERVAL_MS` | 500 | Display refresh interval (ms) |

---

## Revision History

| Version | Changes |
|---|---|
| v1.0 | Basic MQTT publisher, blocking WiFi boot |
| v2.0 | Full async/non-blocking, FreeRTOS dual-core, JSON telemetry, OLED menu, ESPAsyncWebServer, EEPROM config, calibration, smart WiFi manager |
| v2.1 | AP internet passthrough via lwIP NAPT; `OVER` voltage status; 3-phase voltage unbalance (NEMA method); voltage thresholds (LOST/UNDER/OVER/Unbalance) configurable from web and stored in EEPROM; OLED Page 1 shows unbalance with blinking `[!]` warning; OLED status text blinks on LOST/UNDER/OVER; JSON includes `OVER` status and `unbalance` field; Task Watchdog Timer + heap guard self-healing |

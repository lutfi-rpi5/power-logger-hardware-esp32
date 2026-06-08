# 3-Phase Data Logger v2.0.0

ESP32-based firmware for a 3-phase power data logger using 3× PZEM-004T sensors, OLED display, and MQTT telemetry.

---

## Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [Software Requirements](#software-requirements)
- [Setup & Installation](#setup--installation)
- [Firmware Architecture](#firmware-architecture)
- [State Machine](#state-machine)
- [MQTT JSON Schema](#mqtt-json-schema)
- [OLED Display](#oled-display)
  - [Monitoring Mode](#monitoring-mode)
  - [Menu Mode](#menu-mode)
- [Web Interface](#web-interface)
- [Calibration System](#calibration-system)
- [Voltage Unbalance Calculation](#voltage-unbalance-calculation)
- [Non-Blocking Design](#non-blocking-design)
- [File Reference](#file-reference)
- [Troubleshooting](#troubleshooting)

---

## Overview

This device **acquires** 3-phase electrical data from PZEM-004T sensors, **processes** it with calibration offsets, **displays** real-time readings on an OLED screen, and **publishes** structured JSON telemetry to an MQTT broker.

**Important:** This device does NOT log data locally. Data logging is handled by a backend server subscribed to the MQTT broker topics.

| Feature | v1.0.0 | v2.0.0 |
|---------|--------|--------|
| MQTT publish | Per-topic (desynced) | Single JSON payload |
| WiFi connect | Blocking (boot stuck) | Async (runs without network) |
| Calibration | None | Additive V/I offset model |
| OLED display | Raw text | Professional 3-page + menu system |
| Web config | None | AP mode + HTTP forms |
| State machine | None | Full state machine |
| Self-healing | None | Watchdog + auto-reconnect |
| Line status | OK/UNDER/LOST | OK/UNDER/OVER/LOST |

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

Each PZEM-004T measures one phase line-to-neutral in a 3-phase WYE (star) 220V system:

- PZEM 1 → Phase R to Neutral
- PZEM 2 → Phase S to Neutral
- PZEM 3 → Phase T to Neutral

---

## Pin Configuration

| Peripheral | Pin | Notes |
|------------|-----|-------|
| PZEM R + S (UART1) | RX=4, TX=15 | Both phases share one serial port |
| PZEM T (UART2) | RX=17, TX=16 | Phase T on separate serial port |
| OLED I2C | SDA=21, SCL=22 | Address 0x3C |
| Button | GPIO5 | INPUT_PULLUP |
| Built-in LED | GPIO2 | Active HIGH |

### PZEM Addresses

| Phase | Address | Serial Port |
|-------|---------|-------------|
| R | 0x10 | UART1 |
| S | 0x11 | UART1 |
| T | 0x12 | UART2 |

> **Note:** To change PZEM addresses, use the sketches in the `setAddressPZEM/` folder (v1.x).

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
| AsyncMqttClient | latest | marvinroger | Async MQTT client |
| ArduinoJson | ≥6.21.6 | Benoit Blanchon | JSON serialization |
| ESP Async WebServer | ≥3.11.0 | ESP32Async | HTTP web server |
| Async TCP | ≥3.4.10 | ESP32Async | TCP async support |

Install via Arduino Library Manager (Sketch → Include Library → Manage Libraries).

---

## Setup & Installation

### 1. Clone the repository

```bash
git clone <repo-url>
cd power-logger-hardware-esp32/Data_Logger_Monitor_v2.0.0
```

### 2. Configure credentials

```bash
cp credential_example.cpp credentials.cpp
```

Edit `credentials.cpp` and fill in your WiFi and MQTT credentials:

```cpp
const char* WIFI_SSID1 = "YourNetwork";
const char* WIFI_PASS1 = "YourPassword";

const char* MQTT_SERVER = "broker.example.com";
const char* MQTT_USER   = "username";
const char* MQTT_PASS   = "password";
```

> **Security:** `credentials.cpp` is listed in `.gitignore`. Never commit real credentials.

### 3. Open in Arduino IDE

- Open `Data_Logger_Monitor_v2.0.0.ino`
- Select board: **ESP32 Dev Module**
- Select port
- Click **Upload**

### 4. First boot

1. Device shows boot screen with progress bar (3 seconds)
2. Automatically enters Monitoring Mode
3. WiFi connects asynchronously in background
4. OLED displays real-time data even without network

---

## Firmware Architecture

The firmware uses OOP with ROS2-inspired modularity. Each subsystem has a dedicated `.h`/`.cpp` pair.

```
┌─────────────────────────────────────────────────────────────┐
│                      Data_Logger_Monitor_v2.0.0.ino         │
│  Main loop — state machine, timing, wiring                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ DataAcq      │  │ Calibration  │  │ JSONBuilder      │  │
│  │ PZEM reads   │  │ V/I offset   │  │ Payload build    │  │
│  │ 3 phases     │  │ propagate    │  │ {seq,device,..}  │  │
│  └──────┬───────┘  └──────┬───────┘  └────────┬─────────┘  │
│         │                 │                    │            │
│  ┌──────▼─────────────────▼────────────────────▼─────────┐  │
│  │                    MQTTManager                        │  │
│  │           Async publish to broker                     │  │
│  └────────────────────────┬──────────────────────────────┘  │
│                           │                                 │
│  ┌────────────────────────▼──────────────────────────────┐  │
│  │                    WiFiManager                         │  │
│  │          Async connect from known networks             │  │
│  └────────────────────────┬──────────────────────────────┘  │
│                           │                                 │
│  ┌────────────────────────▼──────────────────────────────┐  │
│  │                   SystemState                          │  │
│  │         Runtime state + phase data buffer              │  │
│  └─────┬────────────┬──────────────────┬──────────────────┘  │
│        │            │                  │                     │
│  ┌─────▼────┐  ┌────▼──────┐  ┌───────▼──────────┐          │
│  │ Button   │  │ OLED     │  │ WebServer        │          │
│  │ Manager  │  │ Display  │  │ AP + HTTP config │          │
│  │          │  │ + Menu   │  │ WiFi/MQTT/Cal    │          │
│  └──────────┘  └──────────┘  └──────────────────┘          │
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ Storage     │  │ Diagnostics  │  │ LEDManager       │  │
│  │ NVS/EEPROM  │  │ Log levels   │  │ Signal patterns  │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### File Structure

| File | Purpose |
|------|---------|
| `Data_Logger_Monitor_v2.0.0.ino` | Entry point, main loop, state machine wiring |
| `config.h` | Hardcoded programmer settings (pins, thresholds, versions) |
| `types.h` | Shared structs and enums |
| `system_state.h` | Global runtime state struct |
| `credentials.h` | Credential declarations |
| `credential_example.cpp` | Credential template (copy → `credentials.cpp`) |
| `storage_manager.h/.cpp` | NVS read/write for WiFi, MQTT, calibration, thresholds |
| `button_manager.h/.cpp` | Single button: debounce, short/long press |
| `led_manager.h/.cpp` | Built-in LED blink patterns |
| `diagnostics.h/.cpp` | Serial log levels, fake data mode, heap diag |
| `calibration.h/.cpp` | Additive V/I offset, propagate to derived params |
| `data_acquisition.h/.cpp` | PZEM reads, line status, unbalance calculation |
| `wifi_manager.h/.cpp` | Async WiFi connect from known networks |
| `mqtt_manager.h/.cpp` | Async MQTT connect/publish |
| `json_builder.h/.cpp` | Build JSON MQTT payload |
| `display_manager.h/.cpp` | OLED rendering (title bar, monitoring pages) |
| `menu_manager.h/.cpp` | Menu navigation state machine |
| `webserver_manager.h/.cpp` | AP + HTTP server with HTML config pages |
| `task_manager.h/.cpp` | Watchdog, scheduling, task creation |

---

## State Machine

```
                  ┌──────────┐
                  │  BOOTING │ (3s progress bar)
                  └────┬─────┘
                       │
                  ┌────▼──────┐  short press  ┌──────────────────┐
        ┌────────▶│ MONITORING│──────────────▶│ MONITORING       │
        │         │  Page 1   │               │  Page 2 / 3      │
        │         └────┬──────┘               └──────────────────┘
        │              │ long press
        │         ┌────▼──────┐
        │         │    MENU   │
        │         │   Main    │
        │         └────┬──────┘
        │              │ long press on item
        │    ┌─────────┼──────────┬──────────────┐
        │    │         │          │              │
        │   ┌─▼──┐  ┌──▼───┐  ┌──▼───┐   ┌─────▼───┐
        │   │Con-│  │Reset │  │Reboot│   │Factory  │
        │   │fig │  │kWh   │  │Device│   │Reset    │
        │   └────┘  └──────┘  └──┬───┘   └────┬────┘
        │                         │             │
        │                    ┌────▼────┐  ┌────▼────┐
        │                    │COUNTDOWN│  │REBOOT   │
        │                    │3..2..1..0│  │(self)   │
        │                    └─────────┘  └─────────┘
        │
        └──── "Back to Monitoring" or "Back" long press ────┘
```

### Transitions

| Event | Current State | Next State |
|-------|--------------|------------|
| Boot complete | BOOTING | MONITORING Page 1 |
| Short press | MONITORING Page N | MONITORING Page N+1 (cycles) |
| Long press | MONITORING (any page) | MENU Main |
| Short press | MENU Main | Cursor moves down (loops) |
| Long press on menu item | MENU Main | MENU Sub-page |
| Long press "Back" | MENU Sub-page | MENU Main |
| Long press "Back to Monitoring" | MENU Main | MONITORING Page 1 |
| Reboot confirm + countdown | MENU Reboot | REBOOTING → restart |

---

## MQTT JSON Schema

Topic: `{prefix}/{topic}/{device_id}` (e.g., `lutpiii/telemetry/3ph-logger-001`)

Backend subscribes to `+/telemetry/+` (wildcard) to receive data from all devices.

### Payload

```json
{
  "seq": 1247,
  "device": {
    "id": "3ph-logger-001",
    "fw": "v2.0.0",
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
    }
  },
  "unbalance": 4.25,
  "ts": 1717001234
}
```

### Field Descriptions

| Field | Type | Description |
|-------|------|-------------|
| `seq` | uint32 | Incrementing sequence number (detect data gaps) |
| `device.id` | string | Device identifier (from `config.h`) |
| `device.fw` | string | Firmware version |
| `device.uptime` | uint32 | Seconds since boot |
| `device.heap` | uint32 | Free heap memory (bytes) |
| `device.rssi` | int8 | WiFi RSSI (dBm) |
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
| `unbalance` | float | Voltage unbalance % (NEMA method) |
| `ts` | uint32 | Unix epoch seconds (0 until NTP synced) |

### Backend Notes

1. Subscribe to `lutpiii/telemetry/+` (wildcard) to receive data from all devices.
2. Index by `device.id` + `ts` for per-device time-series queries.
3. Use `seq` to detect missed messages (gaps in sequence per device).
4. When `valid: false`, the phase data can be ignored without conditional parsing.
5. If `ts == 0`, NTP is not yet synced; fall back to MQTT broker timestamp.

---

## OLED Display

128×64 pixel SSD1306 I2C display.

### Title Bar

Every page has a title bar with independent status icons:

```
3-Phase Data Logger  QW
────────────────────
```

- **Q** = MQTT connected (only shown when connected)
- **W** = WiFi connected (only shown when connected)
- Both shown when both connected: `QW`
- Separator line below title

### Monitoring Mode

**Page 1 — Voltage, Current, Status, Unbalance**

```
3-Phase Data Logger  QW
  R         S         T
 220V      220V      220V
 100A      100A      100A
  OK        OK        OK
 Unbal=2.5%
```

Warning states:
- Line status `LOST` / `UNDER` / `OVER` displayed in place of `OK`
- `[!]` icon when unbalance exceeds configured threshold

**Page 2 — Frequency, Apparent Power, Reactive Power**

```
3-Phase Data Logger  QW
  R         S         T
 60Hz      60Hz      60Hz
 100VA    1.2kVA    10kVA
 100VAr   2.0kVAr  13kVAr
```

Auto-scale: `<1000` uses base unit, `≥1000` uses kilo prefix.

**Page 3 — Power Factor, Active Power, Energy**

```
3-Phase Data Logger  QW
  R         S         T
 0.8PF     1.0PF     0.7PF
 100W      12kW     2.2kW
 300Wh    1.4kWh    30kWh
```

### Menu Mode

**Main Menu (5 items)**

```
3-Phase Data Logger  QW
  > Config
  > Reset kWh
  > Reboot Device
  > Reset to Factory
  > Back to Monitoring
```

**Config Page** — Enable/disable AP + web server

```
    CONFIGURATION     QW
 SSID: LOGGER
 PW  : 12345678
 192.168.1.5
  > Active         Back
```

**Reset kWh** — Confirm + result display (2s)

```
    CONFIGURATION     QW
 Are you sure to
      Reset kWh?

  > Reset          Back
```

**Reboot Device** — Confirm + countdown (3..2..1..0)

```
    CONFIGURATION     QW

   Device Rebooting
     in 3 seconds
```

**Reset to Factory** — Confirm + reboot on success

### Button Controls

| Action | Duration | Monitoring Mode | Menu Mode |
|--------|----------|----------------|-----------|
| Short press | <2s | Next page | Move cursor down (loops) |
| Long press | ≥2s | Enter Menu Mode | Enter/confirm/back |

- Cursor wraps from bottom back to top.
- Horizontal selection (left/right) also wraps.
- Every submenu has a "Back" option.
- "Back to Monitoring" returns directly to Monitoring Page 1.

---

## Web Interface

Activate via OLED Menu → Config → "Active" (long press).

### Login

Default credentials:
- Username: `ADMIN`
- Password: `18273645`

(Changeable only in `config.h` — recompile required.)

### Pages

**Home** — Card navigation:

- Manage Known Networks
- MQTT Connection Config
- PZEM-004T Calibration & Thresholds
- Danger Zone: Hard Reset EEPROM

**Manage Known Networks** — Table of saved WiFi credentials:

| No. | SSID | Action |
|-----|------|--------|
| 1 | NetworkA | [Delete] |
| 2 | NetworkB | [Delete] |

Add Network form: SSID + Password.

**MQTT Connection Config** — Editable fields:

- Server, Port (non-SSL), Port (SSL)
- Username, Password
- Topic Prefix, Topic
- Use SSL (checkbox)
- Use WebSocket (checkbox)
- CA Certificate (textarea)

Connection mode logic:

| Use WS | Use SSL | Protocol |
|--------|---------|----------|
| 0 | 0 | MQTT (1883) |
| 0 | 1 | MQTTS (8883) |
| 1 | 0 | WS (8083) |
| 1 | 1 | WSS (8084) |

**PZEM Calibration & Thresholds** — Per-phase:

- Voltage offset (V) — additive, 0.001 precision
- Current offset (A) — additive, 0.001 precision
- LOST threshold (V)
- UNDER threshold (V)
- OVER threshold (V)
- Max Unbalance (%)

---

## Calibration System

### Offset Model

Calibration uses an additive offset model:

```
V_calibrated = V_raw + voltageOffset
I_calibrated = I_raw + currentOffset
```

Offsets can be positive or negative. Configured via web interface, stored in NVS.

### Derived Parameter Propagation

When V or I is calibrated, all derived parameters are recalculated:

| Parameter | Formula | Affected by Calibration? |
|-----------|---------|--------------------------|
| Voltage (V) | V_raw + V_off | **Yes** |
| Current (I) | I_raw + I_off | **Yes** |
| Apparent power (S) | V_cal × I_cal | **Yes** |
| Active power (P) | From PZEM (unchanged) | No (uses PZEM value) |
| Reactive power (Q) | √(S² − P²) | **Yes** (via S) |
| Power factor (PF) | P / S | **Yes** (via S) |
| Energy (Wh) | From PZEM (unchanged) | No |
| Frequency (Hz) | From PZEM | **No** |

---

## Voltage Unbalance Calculation

Uses the **NEMA method** (standard in Indonesia / SPLN):

1. Calculate average voltage:  
   `V_avg = (V_R + V_S + V_T) / 3`

2. Find maximum deviation from average:  
   `max_dev = max(|V_R − V_avg|, |V_S − V_avg|, |V_T − V_avg|)`

3. Calculate unbalance percentage:  
   `unbalance = (max_dev / V_avg) × 100`

### Thresholds (default, configurable)

| Threshold | Default | Description |
|-----------|---------|-------------|
| `VOLTAGE_LOST_MAX` | 80.0 V | Below this → LOST |
| `VOLTAGE_UNDER_MIN` | 180.0 V | Below this (but above LOST) → UNDER |
| `VOLTAGE_OVER_MIN` | 240.0 V | Above this → OVER |
| `UNBALANCE_MAX` | 3.0% | Above this → `[!]` warning on OLED |

---

## Non-Blocking Design

**No `delay()` used** except:
- Boot screen (3-second progress bar, unavoidable)
- `delay(100)` in diagnostics `begin()` for serial stability
- `delay(200)` between PZEM reset commands

**All operations are asynchronous:**

| Operation | Method |
|-----------|--------|
| WiFi connect | Async event-driven, non-blocking |
| WiFi reconnection | `millis()`-based retry every 30s |
| MQTT connect | Async client, auto-reconnect |
| OLED refresh | `millis()`-based, 500ms interval |
| Data acquisition | `millis()`-based, 2s interval |
| MQTT publish | `millis()`-based, 2s interval |
| Button debounce | State machine + `millis()` tracking |
| Reboot countdown | Non-blocking `millis()` decrement |

**Device runs immediately** without WiFi/MQTT. OLED and sensors work from second 1.

---

## File Reference

### `config.h`

All programmer-only settings. Change these before flashing:

```cpp
#define DEVICE_ID       "3ph-logger-001"
#define FW_VERSION      "v2.0.0"
#define WDT_TIMEOUT_MS  10000
```

### `types.h`

Shared enums and structs used by all modules:

```cpp
enum class LineStatus { LOST, UNDER, OK, OVER };
enum class AppState { BOOTING, MONITORING, MENU, CONFIG_AP, REBOOTING };
struct PhaseData { bool valid; float voltage; float current; ... };
```

### `credentials.h` + `credential_example.cpp`

Credentials are separated into `credentials.cpp` (gitignored). Copy the example and fill in.

### `storage_manager.h/.cpp`

Uses ESP32 Preferences (NVS key-value store) for persistent configuration:
- WiFi known networks (up to 10)
- MQTT broker config (server, ports, auth, SSL, topic)
- Calibration offsets (per-phase)
- Line status thresholds
- Publish sequence counter

### `diagnostics.h/.cpp`

Runtime-configurable log levels:

```cpp
diag.begin(LOG_INFO);     // Default
diag.setLevel(LOG_DEBUG); // Max verbosity
diag.info("TAG", "message %d", value);
```

Levels: `LOG_NONE` (0), `LOG_ERROR` (1), `LOG_WARN` (2), `LOG_INFO` (3), `LOG_DEBUG` (4).

### `data_acquisition.h/.cpp`

Handles communication with 3 PZEM-004T sensors on two UART ports. Methods:

- `readAll()` — Read all 3 phases
- `getRawVoltage(i)` — Get raw reading for phase i
- `determineLineStatus(v, thr)` — Classify voltage against thresholds
- `generateFakeData()` — Inject random data for testing
- `resetEnergy()` — Reset all PZEM energy counters

### `calibration.h/.cpp`

Applies calibration offsets and recomputes derived parameters. Key method:

```cpp
cal.apply(rawV, rawI, rawP, rawE, rawF, rawPF, phaseIndex, outputPhaseData);
```

### `wifi_manager.h/.cpp`

- Async connect: `beginConnect()` starts connection, returns immediately
- Scans visible networks and matches against known networks (NVS + hardcoded fallback)
- AP mode: `startAP()` / `stopAP()`
- Auto-reconnect every 30s on failure

### `mqtt_manager.h/.cpp`

- Async MQTT using `AsyncMqttClient`
- `setConfig()` from NVS data
- `publish(json)` — publishes to `{prefix}/{topic}/{device_id}`
- Auto-reconnect every 10s when WiFi is available

### `json_builder.h/.cpp`

Builds the structured JSON payload using ArduinoJson. Returns `String` ready for MQTT publish.

### `display_manager.h/.cpp`

OLED rendering with:
- Title bar with Q/W status icons
- 3 monitoring pages with formatted data
- Menu layout helpers
- Boot screen with progress bar
- Auto-scaling units (VA↔kVA, W↔kW, Wh↔kWh)

### `menu_manager.h/.cpp`

Menu state machine handling:
- 3 monitoring pages (cycled by short press)
- 5 menu pages with cursor navigation
- Sub-menus: Config, Reset kWh, Reboot Device, Reset to Factory
- Confirmation prompts (left/right cursor)
- Reboot countdown

### `webserver_manager.h/.cpp`

ESP32 AP mode HTTP server with:
- Login page (Basic Auth, default ADMIN/18273645)
- WiFi network management (table + add/delete)
- MQTT config form
- Calibration + threshold config form
- Factory reset button
- Mobile-friendly CSS (from spec template)

### `task_manager.h/.cpp`

- Watchdog timer management
- `isTime()` helper for millis()-based scheduling
- Task creation stubs (extensible for FreeRTOS tasks)

---

## Troubleshooting

### OLED shows nothing

1. Check I2C connections (SDA=21, SCL=22)
2. Verify OLED address: 0x3C
3. Run I2C scanner sketch

### PZEM readings are NaN

1. Check serial wiring (RX/TX may need swapping)
2. Verify PZEM addresses match config (0x10, 0x11, 0x12)
3. Check that serial ports are properly initialized
4. Try `setAddressPZEM.ino` from `setAddressPZEM/` folder to reconfigure addresses

### MQTT not connecting

1. Check WiFi connection status on OLED (W icon visible?)
2. Verify MQTT server/credentials in web config
3. Check firewall allows outbound MQTT (1883/8883)
4. For SSL: ensure CA certificate is correct in web config

### Device won't boot past progress bar

1. Check serial monitor for error messages
2. If PZEM serial init fails, the device still boots
3. Try flashing with fake data mode enabled to isolate hardware issues

### How to enable fake data mode

In `diagnostics.h`:
```cpp
g_state.fakeDataMode = true;
```
Or set via a future menu option.

---

## License

MIT License — see repository root.

## Author

Muhammad Lutfi Nur Anendi
- GitHub: https://github.com/lutfi-rpi5
- LinkedIn: https://linkedin.com/in/muhammad-lutfi-nur-anendi

# AGENTS.md — Power Logger Hardware ESP32

## Project overview

Arduino-based ESP32 firmware for a 3-phase power data logger (3× PZEM-004T). v1.0.0 is production-deployed; v2.0.0 is an in-progress ground-up refactor. The device acquires data, publishes JSON to MQTT, and displays on OLED — it does **not** log data locally (that's handled by a backend subscribed to the broker).

## Build & toolchain

- **IDE:** Arduino IDE v2.0 (or PlatformIO as alternative)
- **Board package:** ESP32 v2.0.14 (v1.x), upgrade to **v3.3.0** for v2.0.0
- **Board URL:** `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-packages/package_esp32_index.json`
- **No** package.json, CMakeLists, tests, CI, or linter — zero standard project infra
- Verify by compiling in Arduino IDE; no automated tests exist

## Key directories

| Path | Purpose |
|---|---|
| `Data_Logger_Monitor/` | v1.0.0 firmware entrypoint (`Data_Logger_Monitor.ino`) and modules |
| `Data_Logger_Monitor_v2.0.0/` | **v2.0.0 refactor** — modular OOP, async, JSON, web config, OLED menu |
| `Data_Logger_Monitor/.agents/skills/log-skill/` | Session log skill (append-only progress tracking) |
| `setAddressPZEM/` | Standalone sketches for configuring PZEM-004T addresses |
| `brief-cl.md`, `brief.md`, `revision to 2.0.0.md` | AI session prompts and full v2 spec — read before working on v2 features |

## Current firmware (v1.0.0) architecture

**Modular classes:** `Button`, `WiFiSelector`, `DisplayOLED`, `ResetEnergy` — each has `.h`/`.cpp`. The main `.ino` is monolithic (~290 lines).

**Critical bugs to fix (documented in brief.md):**
- MQTT publishes per-topic (desynced timestamps) — must switch to single JSON payload
- Wi-Fi connect blocks boot — must become async/non-blocking
- No calibration, no webserver, no state machine, no self-healing
- OLED display is raw text
- Debug requires commenting/uncommenting code

## Hardware pinout (non-negotiable)

```
PZEM:  UART1 RX=4  TX=15  (phases R, S)   |   UART2 RX=17 TX=16  (phase T)
OLED:  SDA=21 SCL=22  I2C addr 0x3C
BUTTON: GPIO5  (INPUT_PULLUP)
LED:    GPIO2  (built-in)
```

3× PZEM-004T all share address space: `0x10` (R), `0x11` (S), `0x12` (T).

## Libraries

**Installed (v1.x):** PZEM004Tv30 v1.2.1, Adafruit SSD1306 v2.5.16, Adafruit GFX v1.12.5, PubSubClient v2.8

**Add for v2.0.0:** ArduinoJson v6.21.6, ESP Async WebServer v3.11.0, Async TCP v3.4.10, AsyncMqttClient (replaces PubSubClient)

## Credentials

`credentials.cpp` is **gitignored**. Copy `Data_Logger_Monitor/credential_example.cpp` → `credentials.cpp` and fill in real values. Never commit credentials.

## Target JSON schema (v2.0.0 MQTT payload)

Topic: `{prefix}/{topic}/{device_id}` (hierarchical). Backend subscribes to `+/telemetry/+`.

```json
{
  "seq": 1247,
  "device": { "id": "3ph-logger-001", "fw": "v2.1.0", "uptime": 3600, "heap": 148432, "rssi": -65 },
  "phases": {
    "R": { "valid": true,  "v": 220.1, "i": 12.34, "p": 2650.0, "s": 2714.6, "q": 582.1, "pf": 0.97, "f": 50.0, "e": 15230.0, "status": "OK" },
    "S": { "valid": true,  "v": 200.6, "i": 12.34, "p": 2650.0, "s": 2714.6, "q": 582.1, "pf": 0.97, "f": 50.0, "e": 15230.0, "status": "UNDER" },
    "T": { "valid": false, "v": 0.0,   "i": 0.0,   "p": 0.0,    "s": 0.0,    "q": 0.0,   "pf": 0.0, "f": 0.0, "e": 0.0,    "status": "LOST" }
  },
  "unbalance": 4.25,
  "ts": 1717001234
}
```

- Numeric values as raw numbers (not strings), `ts` = Unix epoch (NTP), `valid: false` phases still send full schema with zeros.
- Calibration offsets on V/I propagate to all derived params (VA, VAr, W, Wh, PF — but not Hz).

## OLED UI requirements

Single button: **short press** = navigate/page/cursor; **long press (2s)** = enter/confirm/back.

Two modes with exact ASCII layouts in `brief.md` (reproduce faithfully):

### Monitoring Mode (3 pages)
1. Voltage/Current/Line Status + Unbalance %
2. Frequency/Apparent Power/Reactive Power (auto-scale VA↔kVA, VAr↔kVAr)
3. Power Factor/Active Power/Energy (auto-scale W↔kW, Wh↔kWh)

Title bar: `"3-Phase Data Logger"` with independent `W` (Wi‑Fi) and `Q` (MQTT) icons. Warning blinking for LOST/UNDER/OVER and unbalance threshold `[!]`.

### Menu Mode (5 pages)
Config → Reset kWh → Reboot Device → Reset to Factory → Back to Monitoring. Each has confirmation/countdown screens.

## Web server (v2.0.0)

ESP32 AP mode + HTTP server with responsive HTML/CSS. Pages: Manage Known Networks (table + add/delete), MQTT Config (server/ports/SSL/WS/topic), PZEM Calibration (V/I offsets per phase + thresholds). Login page default user/pass in `config.h`. All config stored in EEPROM/NVS.

## Line status thresholds

Configurable from webpage (defaults in `config.h`):
```
VOLTAGE_OK_MIN    = 180.0f  // Below → UNDER
VOLTAGE_LOST_MAX  = 80.0f   // Below → LOST
VOLTAGE_OVER_MAX  = 240.0f  // Above → OVER
UNBALANCE_MAX     = 3.0f    // Above → OLED warning [!]
```

## Non-blocking rules

- No `delay()` except on boot screens or where truly unavoidable
- Wi-Fi/MQTT connect must be async — device runs immediately without network
- OLED must remain responsive during network ops
- Use `millis()` scheduling, FreeRTOS tasks, and state machines
- Watchdog-safe task design required

## Development workflow notes

- All code, comments, docs in **English**
- Target audience: beginners — keep code readable with inline docs
- `brief-cl.md` and `brief.md` are AI briefs for the v2 refactor; read them before implementing any v2 feature
- Session logs go in `Data_Logger_Monitor/.agents/skills/log-skill/` via `progress.txt` (append only)
- After any code change, update `progress.txt` following the log-skill format

!!!IMPORTANT: IGNORE THIS FILE!!!

You are a senior embedded systems architect and firmware engineer.

Your task is to redesign and refactor an existing production ESP32 firmware project into a highly professional, scalable, asynchronous, non-blocking, maintainable, and beginner-friendly firmware architecture.

Primary language for all code, comments, documentation, variable naming, commit-style notes, and explanations MUST be English.

Target users are beginner embedded engineers, students, and hobbyists.
Therefore:
- The code MUST be extremely readable.
- The code MUST contain extensive inline documentation.
- The code MUST be easy to debug and troubleshoot.
- The code MUST avoid hidden logic and magic numbers.
- The code MUST prioritize stability and field reliability.
- The code MUST minimize system blocking.
- The code MUST contain proper modularization and architecture.
- The code MUST be optimized for ESP32 resource limitations.
- The code MUST be token-efficient but still maintain professional documentation quality.

==================================================
PROJECT OVERVIEW
==================================================

Project Name:
3-Phase Data Logger

Current Device Purpose:
This device ONLY performs:
1. Data acquisition from 3x PZEM-004T
2. Data processing
3. MQTT publishing
4. OLED display handling
5. Button handling
6. Wi-Fi management

IMPORTANT:
This device DOES NOT perform database logging internally.
Actual data logging is handled by a backend website/server subscribed to MQTT broker topics.

==================================================
CURRENT HARDWARE
==================================================

MCU:
- ESP32 DevKit V1 30-pin
- Arduino IDE v2.0
- ESP32 Board Package v2.0.14

Sensors:
- 3x PZEM-004T 100A
  Each PZEM handles:
  Line-to-Neutral measurement for 3-phase WYE 220V system

Display:
- OLED SSD1306 128x64 I2C

Input:
- 1x Push Button (Normally Open)

Indicators:
- Built-in ESP32 LED

==================================================
PIN CONFIGURATION
==================================================

//================ PZEM =================//

#define PZEM_RX1_PIN 4
#define PZEM_TX1_PIN 15
#define PZEM_RX2_PIN 17
#define PZEM_TX2_PIN 16

HardwareSerial PZEMSerial1(1);
HardwareSerial PZEMSerial2(2);

#define NUM_PZEMS 3

PZEM004Tv30 pzems[NUM_PZEMS] = {
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10),
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11),
  PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)
};

//================ OLED =================//

SDA = GPIO21
SCL = GPIO22

//================ BUTTON & LED =================//

BUTTON = GPIO5
LED    = GPIO2

==================================================
CURRENT SYSTEM PROBLEMS
==================================================

The current firmware is already deployed in production but still has architectural weaknesses and bugs:

1. MQTT data is published per-topic individually.
   Result:
   - asynchronous timestamp mismatch
   - data arriving at backend not synchronized
   - some values are delayed by milliseconds

2. Wi-Fi connection during boot is blocking.
   Result:
   - device stuck on connecting state
   - OLED UI inaccessible until Wi-Fi connected

3. No calibration system.

4. No webserver or webpage configuration system.

5. Several blocking delays still exist.

6. No state machine architecture.

7. No self-healing/recovery mechanism.

8. OLED UI is primitive and messy.

9. Wi-Fi manager is not intelligent.
   Still uses hardcoded priority logic.

10. Debug system is poor.
    Requires commenting/uncommenting code manually.

==================================================
MAIN DEVELOPMENT TARGETS
==================================================

The redesigned firmware MUST:

1. Be FULLY asynchronous and non-blocking.
2. Use FreeRTOS tasks properly if needed.
3. Utilize ESP32 dual-core architecture appropriately.
4. Avoid delay() completely.
5. Use millis()-based scheduling or task scheduling.
6. Implement robust state machines.
7. Implement watchdog-safe design.
8. Support self-recovery mechanisms.
9. Be scalable for future sensors/modules.
10. Be beginner-friendly and educational.

==================================================
CONFIGURATION ARCHITECTURE
==================================================

Create centralized configuration architecture.

Split configuration into TWO categories:

1. HARD-CODE CONFIGURATION
   - programmer-only settings
   - stored in config.h

2. USER CONFIGURATION
   - editable from webpage
   - stored in EEPROM or Preferences/NVS
   - editable without recompiling firmware

==================================================
MQTT REQUIREMENTS
==================================================

Replace old MQTT publish-per-topic architecture.

NEW REQUIREMENTS:
- Use structured JSON payload.
- Publish synchronized data snapshots.
- Backend parser target = Node.js server.

Design a PROFESSIONAL JSON schema including:
- device metadata
- timestamp
- Wi-Fi status
- MQTT status
- all 3-phase electrical parameters
- calibration info
- system health
- uptime
- RSSI
- error flags

Explain WHY the JSON structure is designed that way.

==================================================
OLED DISPLAY REQUIREMENTS
==================================================

Display:
SSD1306 128x64

UI MUST:
- look professional
- readable
- organized
- modular
- scalable

Button Logic:
ONLY ONE BUTTON is used.

Button Actions:
1. Short press
   - move page
   - move cursor

2. Long press (default 2 seconds, configurable from webpage)
   - ENTER action
   - open menu
   - confirm action
   - back action

==================================================
DETAILED OLED UI REQUIREMENTS
==================================================

IMPORTANT:
The OLED UI design below MUST be implemented EXACTLY as specified.

Display Type:
- SSD1306
- 128x64
- I2C

The OLED system MUST:
- use clean rendering architecture
- support modular page rendering
- support page state machine
- support cursor state machine
- avoid flickering
- support partial redraw optimization if possible
- separate UI logic from business logic
- avoid blocking redraw
- support future menu scalability

==================================================
TITLE BAR REQUIREMENTS
==================================================

Top title bar rules:
- Every page MUST have a title bar.
- Add separator line below title.
- MQTT icon uses letter "Q"
- Wi-Fi icon uses letter "W"
- Q and W indicators are INDEPENDENT.
- Do NOT show W if Wi-Fi disconnected.
- Do NOT show Q if MQTT disconnected.
- If both connected:
  "QW"
- If only Wi-Fi connected:
  "W"
- If only MQTT connected:
  "Q"

Example:
"3-Phase Data Logger  QW"

==================================================
BUTTON CONTROL RULES
==================================================

ONLY ONE BUTTON exists.

Button behavior:
1. SHORT PRESS
   - move monitoring page
   - move menu cursor
   - cycle selection
   - horizontal selection

2. LONG PRESS
   - default = 2 seconds
   - configurable from webpage
   - acts as ENTER
   - acts as CONFIRM
   - acts as BACK depending on state

==================================================
MONITORING MODE RULES
==================================================

Monitoring mode behavior:
- short press:
  move to next monitoring page

- long press:
  enter Menu Mode

Monitoring pages MUST auto refresh asynchronously.

==================================================
MONITORING PAGE 1
==================================================

Layout:

┌────────────────────────────┐
│    3-Phase Data Logger  QW │
│   R         S         T    │
│  220V      220V      220V  │
│  100A      100A      100A  │
│   OK        OK        OK   │
└────────────────────────────┘

Page content:
- Phase labels: R S T
- Voltage line-to-neutral
- Current per phase
- Line status:
  - LOST
  - UNDER
  - OK

==================================================
MONITORING PAGE 2
==================================================

Layout:

┌────────────────────────────┐
│    3-Phase Data Logger  QW │
│   R         S         T    │
│  60Hz      60Hz      60Hz  │
│ 100VA     1.2kVA    10kVA  │
│ 100VAr    2.0kVAr   13kVAr │
└────────────────────────────┘

Page content:
- Frequency
- Apparent Power
- Reactive Power

Unit conversion rules:
- <1000:
  VA / VAR
- >=1000:
  kVA / kVAr

==================================================
MONITORING PAGE 3
==================================================

Layout:

┌────────────────────────────┐
│    3-Phase Data Logger  QW │
│   R         S         T    │
│ 0.8PF     1.0PF     0.7PF  │
│ 100W       12kW     2.2kW  │
│ 300Wh     1.4kWh     30kWh │
└────────────────────────────┘

Page content:
- Power Factor
- Active Power
- Total Active Energy

Unit conversion rules:
- <1000:
  W / Wh
- >=1000:
  kW / kWh

==================================================
MENU MODE RULES
==================================================

Menu mode behavior:
- short press:
  move cursor

- long press:
  ENTER selected menu

Cursor rules:
- cursor loops from bottom back to top
- horizontal selection also loops
- use visual indentation or ">" symbol

Every submenu MUST contain:
- Back option

EXCEPTION:
- "Back to Monitoring"

If selected using long press:
- return directly to Monitoring Page 1

==================================================
MENU MODE PAGE 1
==================================================

Layout:

┌────────────────────────────┐
│    3-Phase Data Logger  QW │
│   > Config                 │
│  > Reset kWh               │
│  > Reboot Device           │
│  > Back to Monitoring      │
└────────────────────────────┘

Menu items:
- Config
- Reset kWh
- Reboot Device
- Back to Monitoring

==================================================
MENU MODE > CONFIG PAGE
==================================================

Purpose:
- ONLY used for enabling/disabling AP mode and webserver
- actual configuration handled from webpage

ACTIVE STATE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│  SSID: LOGGER              │
│  PW  : 12345678            │
│  192.168.1.5               │
│  > Active         Back     │
└────────────────────────────┘

INACTIVE STATE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│  SSID: LOGGER              │
│  PW  : 12345678            │
│  192.168.1.5               │
│  Inactive       > Back     │
└────────────────────────────┘

Rules:
- long press on Active:
  enable AP + webserver

- long press again:
  disable AP + webserver

- Back returns to Menu Mode

==================================================
MENU MODE > RESET KWH PAGE
==================================================

CONFIRM PAGE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│  Are you sure to           │
│      Reset kWh?            │
│                            │
│  > Reset          Back     │
└────────────────────────────┘

SUCCESS PAGE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│                            │
│     Reset Success          │
│                            │
│                            │
└────────────────────────────┘

FAIL PAGE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│                            │
│      Reset Failed          │
│                            │
│                            │
└────────────────────────────┘

Rules:
- reset ALL PZEM energy counters
- show result page for 2 seconds
- auto return to Menu Mode

==================================================
MENU MODE > REBOOT DEVICE PAGE
==================================================

CONFIRM PAGE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│  Are you sure to           │
│    Reboot Device?          │
│                            │
│  > Reboot         Back     │
└────────────────────────────┘

COUNTDOWN PAGE:

┌────────────────────────────┐
│       CONFIGURATION     QW │
│                            │
│     Device Rebooting       │
│       in 3 seconds         │
│                            │
└────────────────────────────┘

Rules:
- perform software reboot
- countdown:
  3
  2
  1
  0

- reboot automatically after countdown
- use non-blocking countdown implementation

==================================================
OLED ENGINEERING REQUIREMENTS
==================================================

Implement:
- UI renderer abstraction
- display page manager
- display state machine
- menu navigation manager
- button event manager
- long-press detection
- debounce protection
- redraw throttling
- asynchronous display refresh

IMPORTANT:
The OLED system MUST remain responsive even when:
- Wi-Fi reconnecting
- MQTT reconnecting
- sensor timeout occurs
- AP/webserver active
- MQTT publish active

The UI MUST NEVER freeze because of networking operations.

==================================================
OLED UI STRUCTURE
==================================================

Implement:

1. Monitoring Mode
2. Menu Mode

Implement all pages exactly as described below.

[IMPORTANT]
You MUST implement:
- cursor system
- page state machine
- menu navigation logic
- back navigation
- confirmation pages
- reboot countdown
- reset success/fail messages
- monitoring page auto refresh
- dynamic icons
- proper formatting

Use clean UI rendering abstraction.

Avoid flickering.

Use partial redraw if possible.

==================================================
MONITORING PAGE REQUIREMENTS
==================================================

Monitoring Page 1:
- Voltage
- Current
- Line Status

Monitoring Page 2:
- Frequency
- Apparent Power
- Reactive Power

Monitoring Page 3:
- Power Factor
- Active Power
- Energy

Formatting Rules:
- automatic unit conversion
- VA → kVA
- VAR → kVAR
- W → kW
- Wh → kWh

==================================================
MENU MODE REQUIREMENTS
==================================================

Menu Tree:
- Config
- Reset kWh
- Reboot Device
- Back to Monitoring

Config Page:
- Enable/disable AP + webserver

Reset Page:
- confirmation screen
- success/fail status page

Reboot Page:
- reboot countdown page
- system reboot via software

==================================================
WEB SERVER REQUIREMENTS
==================================================

Implement:
- ESP32 Access Point
- HTTP Webserver
- Responsive HTML/CSS UI
- Mobile-friendly layout

IMPORTANT:
If ESP32 is connected to internet Wi-Fi,
and AP mode is active,
attempt internet passthrough behavior if possible.

==================================================
WEBPAGE STRUCTURE
==================================================

Main Page:
- Manage Known Network
- MQTT Config
- PZEM Calibration
- HARDRESET EEPROM

Known Network Page:
- Table of saved Wi-Fi credentials
- Add new network
- Delete network

MQTT Config Page:
Editable:
- MQTT server
- ports
- username/password
- SSL enable
- websocket enable
- topic prefix

Calibration Page:
Per-phase calibration:
- voltage offset
- current offset

Use (+/-) offset model.

==================================================
SYSTEM ARCHITECTURE REQUIREMENTS
==================================================

Use OOP architecture.

Project structure should resemble ROS2-style modularity.

Example structure:

- main.ino
- config.h
- types.h
- system_state.h
- task_manager.h/.cpp
- wifi_manager.h/.cpp
- mqtt_manager.h/.cpp
- data_acquisition.h/.cpp
- calibration.h/.cpp
- json_builder.h/.cpp
- display_manager.h/.cpp
- menu_manager.h/.cpp
- button_manager.h/.cpp
- webserver_manager.h/.cpp
- storage_manager.h/.cpp
- led_manager.h/.cpp
- diagnostics.h/.cpp

You may improve the structure if needed.

==================================================
DEBUG SYSTEM REQUIREMENTS
==================================================

Create PROFESSIONAL debug architecture.

Requirements:
- runtime debug enable/disable
- serial logging levels
- fake/random sensor injection mode
- communication diagnostics
- task timing diagnostics
- MQTT publish diagnostics
- Wi-Fi diagnostics
- memory usage diagnostics

DO NOT require commenting/uncommenting code manually.

==================================================
SELF-HEALING REQUIREMENTS
==================================================

Implement:
- automatic Wi-Fi reconnect
- automatic MQTT reconnect
- task timeout recovery
- sensor timeout handling
- invalid data filtering
- brownout-safe logic
- watchdog-safe task design

==================================================
IMPORTANT ENGINEERING REQUIREMENTS
==================================================

1. NO delay() unless absolutely justified.
2. NO spaghetti code.
3. NO giant monolithic file.
4. NO blocking Wi-Fi loops.
5. NO unsafe String abuse causing heap fragmentation.
6. Use const/progmem where reasonable.
7. Explain RAM optimization strategy.
8. Explain task allocation strategy.
9. Explain why certain modules belong to certain CPU cores.
10. Explain design decisions professionally.

==================================================
OUTPUT FORMAT
==================================================

Generate:
1. Full firmware architecture explanation
2. File-by-file explanation
3. Recommended libraries
4. Dependency explanation
5. State machine design
6. JSON schema design
7. Task scheduling design
8. Folder structure
9. Example code implementation
10. OLED rendering architecture
11. Webserver architecture
12. Calibration architecture
13. EEPROM/NVS storage design
14. Error handling strategy
15. Debug strategy
16. Scaling strategy for future expansion

VERY IMPORTANT:
Do NOT generate incomplete toy examples.

Generate production-grade architecture and implementation examples suitable for real industrial-style deployment on ESP32.

Focus heavily on:
- reliability
- maintainability
- readability
- scalability
- asynchronous design
- robustness

Act like a real senior embedded architect reviewing firmware for commercial deployment.
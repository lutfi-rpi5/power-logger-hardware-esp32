# Create or update AGENTS.md

**Session ID:** ses_1596509dfffefFf4mvy6R81CqT
**Created:** 6/8/2026, 4:40:33 PM
**Updated:** 6/8/2026, 4:52:44 PM

---

## User

Create or update `AGENTS.md` for this repository.

The goal is a compact instruction file that helps future OpenCode sessions avoid mistakes and ramp up quickly. Every line should answer: "Would an agent likely miss this without help?" If not, leave it out.

User-provided focus or constraints (honor these):


## How to investigate

Read the highest-value sources first:
- `README*`, root manifests, workspace config, lockfiles
- build, test, lint, formatter, typecheck, and codegen config
- CI workflows and pre-commit / task runner config
- existing instruction files (`AGENTS.md`, `CLAUDE.md`, `.cursor/rules/`, `.cursorrules`, `.github/copilot-instructions.md`)
- repo-local OpenCode config such as `opencode.json`

If architecture is still unclear after reading config and docs, inspect a small number of representative code files to find the real entrypoints, package boundaries, and execution flow. Prefer reading the files that explain how the system is wired together over random leaf files.

Prefer executable sources of truth over prose. If docs conflict with config or scripts, trust the executable source and only keep what you can verify.

## What to extract

Look for the highest-signal facts for an agent working in this repo:
- exact developer commands, especially non-obvious ones
- how to run a single test, a single package, or a focused verification step
- required command order when it matters, such as `lint -> typecheck -> test`
- monorepo or multi-package boundaries, ownership of major directories, and the real app/library entrypoints
- framework or toolchain quirks: generated code, migrations, codegen, build artifacts, special env loading, dev servers, infra deploy flow
- repo-specific style or workflow conventions that differ from defaults
- testing quirks: fixtures, integration test prerequisites, snapshot workflows, required services, flaky or expensive suites
- important constraints from existing instruction files worth preserving

Good `AGENTS.md` content is usually hard-earned context that took reading multiple files to infer.

## Questions

Only ask the user questions if the repo cannot answer something important. Use the `question` tool for one short batch at most.

Good questions:
- undocumented team conventions
- branch / PR / release expectations
- missing setup or test prerequisites that are known but not written down

Do not ask about anything the repo already makes clear.

## Writing rules

Include only high-signal, repo-specific guidance such as:
- exact commands and shortcuts the agent would otherwise guess wrong
- architecture notes that are not obvious from filenames
- conventions that differ from language or framework defaults
- setup requirements, environment quirks, and operational gotchas
- references to existing instruction sources that matter

Exclude:
- generic software advice
- long tutorials or exhaustive file trees
- obvious language conventions
- speculative claims or anything you could not verify
- content better stored in another file referenced via `opencode.json` `instructions`

When in doubt, omit.

Prefer short sections and bullets. If the repo is simple, keep the file simple. If the repo is large, summarize the few structural facts that actually change how an agent should work.

If `AGENTS.md` already exists at `/mnt/d/project web/power-logger-hardware-esp32`, improve it in place rather than rewriting blindly. Preserve verified useful guidance, delete fluff or stale claims, and reconcile it with the current codebase.

---

## Assistant (Build · DeepSeek V4 Flash Free · 10.0s)

_Thinking:_

The user wants me to create or update an `AGENTS.md` file for this repository. Let me start by investigating the repository structure to understand what this project is about, then create a concise, high-signal instruction file.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32</path>
<type>directory</type>
<entries>
.git/
.gitignore
brief-cl.txt
Data_Logger_Monitor.zip
Data_Logger_Monitor/
setAddressPZEM/

(6 entries)
</entries>
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "README*"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "AGENTS.md"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "CLAUDE.md"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "opencode.json*"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": ".cursorrules"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": ".github/copilot-instructions.md"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": ".cursor/rules/**"
}
```

**Output:**
```
No files found
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 4.8s)

_Thinking:_

No README, no AGENTS.md, no CLAUDE.md, no opencode.json. Let me dig deeper into the repo structure.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor</path>
<type>directory</type>
<entries>
.agents/
brief-cl.md
brief.md
button.cpp
button.h
credential_example.cpp
credentials.cpp
credentials.h
Data_Logger_Monitor.ino
DisplayOLED.cpp
DisplayOLED.h
ResetEnergy.cpp
ResetEnergy.h
revision to 2.0.0.md
session-ses_15a8.md
WifiSelector.cpp
WifiSelector.h

(17 entries)
</entries>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/setAddressPZEM"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/setAddressPZEM</path>
<type>directory</type>
<entries>
queryTesting.ino
setAddressPZEM.ino

(2 entries)
</entries>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/.gitignore"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/.gitignore</path>
<type>file</type>
<content>
1: # ESP32
2: credentials.cpp
3: 
4: # Logs
5: logs
6: *.log
7: npm-debug.log*
8: yarn-debug.log*
9: yarn-error.log*
10: lerna-debug.log*
11: 
12: # Diagnostic reports (https://nodejs.org/api/report.html)
13: report.[0-9]*.[0-9]*.[0-9]*.[0-9]*.json
14: 
15: # Runtime data
16: pids
17: *.pid
18: *.seed
19: *.pid.lock
20: 
21: # Directory for instrumented libs generated by jscoverage/JSCover
22: lib-cov
23: 
24: # Coverage directory used by tools like istanbul
25: coverage
26: *.lcov
27: 
28: # nyc test coverage
29: .nyc_output
30: 
31: # Grunt intermediate storage (https://gruntjs.com/creating-plugins#storing-task-files)
32: .grunt
33: 
34: # Bower dependency directory (https://bower.io/)
35: bower_components
36: 
37: # node-waf configuration
38: .lock-wscript
39: 
40: # Compiled binary addons (https://nodejs.org/api/addons.html)
41: build/Release
42: 
43: # Dependency directories
44: node_modules/
45: jspm_packages/
46: 
47: # Snowpack dependency directory (https://snowpack.dev/)
48: web_modules/
49: 
50: # TypeScript cache
51: *.tsbuildinfo
52: 
53: # Optional npm cache directory
54: .npm
55: 
56: # Optional eslint cache
57: .eslintcache
58: 
59: # Optional stylelint cache
60: .stylelintcache
61: 
62: # Optional REPL history
63: .node_repl_history
64: 
65: # Output of 'npm pack'
66: *.tgz
67: 
68: # Yarn Integrity file
69: .yarn-integrity
70: 
71: # dotenv environment variable files
72: .env
73: .env.*
74: !.env.example
75: 
76: # parcel-bundler cache (https://parceljs.org/)
77: .cache
78: .parcel-cache
79: 
80: # Prisma
81: src/generated/prisma
82: 
83: # Next.js build output
84: .next
85: out
86: 
87: # Nuxt.js build / generate output
88: .nuxt
89: dist
90: 
91: # Gatsby files
92: .cache/
93: # Comment in the public line in if your project uses Gatsby and not Next.js
94: # https://nextjs.org/blog/next-9-1#public-directory-support
95: # public
96: 
97: # vuepress build output
98: .vuepress/dist
99: 
100: # vuepress v2.x temp and cache directory
101: .temp
102: .cache
103: 
104: # Sveltekit cache directory
105: .svelte-kit/
106: 
107: # vitepress build output
108: **/.vitepress/dist
109: 
110: # vitepress cache directory
111: **/.vitepress/cache
112: 
113: # Docusaurus cache and generated files
114: .docusaurus
115: 
116: # Serverless directories
117: .serverless/
118: 
119: # FuseBox cache
120: .fusebox/
121: 
122: # DynamoDB Local files
123: .dynamodb/
124: 
125: # Firebase cache directory
126: .firebase/
127: 
128: # TernJS port file
129: .tern-port
130: 
131: # Stores VSCode versions used for testing VSCode extensions
132: .vscode-test
133: 
134: # yarn v3
135: .pnp.*
136: .yarn/*
137: !.yarn/patches
138: !.yarn/plugins
139: !.yarn/releases
140: !.yarn/sdks
141: !.yarn/versions
142: 
143: # Vite logs files
144: vite.config.js.timestamp-*
145: vite.config.ts.timestamp-*
146: 
147: /src/generated/prisma

(End of file - total 147 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/brief-cl.txt"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/brief-cl.txt</path>
<type>file</type>
<content>
1: You are a senior embedded systems architect and firmware engineer.
2: 
3: Your task is to redesign and refactor an existing production ESP32 firmware project into a highly professional, scalable, asynchronous, non-blocking, maintainable, and beginner-friendly firmware architecture.
4: 
5: Primary language for all code, comments, documentation, variable naming, commit-style notes, and explanations MUST be English.
6: 
7: Target users are beginner embedded engineers, students, and hobbyists.
8: Therefore:
9: - The code MUST be extremely readable.
10: - The code MUST contain extensive inline documentation.
11: - The code MUST be easy to debug and troubleshoot.
12: - The code MUST avoid hidden logic and magic numbers.
13: - The code MUST prioritize stability and field reliability.
14: - The code MUST minimize system blocking.
15: - The code MUST contain proper modularization and architecture.
16: - The code MUST be optimized for ESP32 resource limitations.
17: - The code MUST be token-efficient but still maintain professional documentation quality.
18: 
19: ==================================================
20: PROJECT OVERVIEW
21: ==================================================
22: 
23: Project Name:
24: 3-Phase Data Logger
25: 
26: Current Device Purpose:
27: This device ONLY performs:
28: 1. Data acquisition from 3x PZEM-004T
29: 2. Data processing
30: 3. MQTT publishing
31: 4. OLED display handling
32: 5. Button handling
33: 6. Wi-Fi management
34: 
35: IMPORTANT:
36: This device DOES NOT perform database logging internally.
37: Actual data logging is handled by a backend website/server subscribed to MQTT broker topics.
38: 
39: ==================================================
40: CURRENT HARDWARE
41: ==================================================
42: 
43: MCU:
44: - ESP32 DevKit V1 30-pin
45: - Arduino IDE v2.0
46: - ESP32 Board Package v2.0.14
47: 
48: Sensors:
49: - 3x PZEM-004T 100A
50:   Each PZEM handles:
51:   Line-to-Neutral measurement for 3-phase WYE 220V system
52: 
53: Display:
54: - OLED SSD1306 128x64 I2C
55: 
56: Input:
57: - 1x Push Button (Normally Open)
58: 
59: Indicators:
60: - Built-in ESP32 LED
61: 
62: ==================================================
63: PIN CONFIGURATION
64: ==================================================
65: 
66: //================ PZEM =================//
67: 
68: #define PZEM_RX1_PIN 4
69: #define PZEM_TX1_PIN 15
70: #define PZEM_RX2_PIN 17
71: #define PZEM_TX2_PIN 16
72: 
73: HardwareSerial PZEMSerial1(1);
74: HardwareSerial PZEMSerial2(2);
75: 
76: #define NUM_PZEMS 3
77: 
78: PZEM004Tv30 pzems[NUM_PZEMS] = {
79:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10),
80:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11),
81:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)
82: };
83: 
84: //================ OLED =================//
85: 
86: SDA = GPIO21
87: SCL = GPIO22
88: 
89: //================ BUTTON & LED =================//
90: 
91: BUTTON = GPIO5
92: LED    = GPIO2
93: 
94: ==================================================
95: CURRENT SYSTEM PROBLEMS
96: ==================================================
97: 
98: The current firmware is already deployed in production but still has architectural weaknesses and bugs:
99: 
100: 1. MQTT data is published per-topic individually.
101:    Result:
102:    - asynchronous timestamp mismatch
103:    - data arriving at backend not synchronized
104:    - some values are delayed by milliseconds
105: 
106: 2. Wi-Fi connection during boot is blocking.
107:    Result:
108:    - device stuck on connecting state
109:    - OLED UI inaccessible until Wi-Fi connected
110: 
111: 3. No calibration system.
112: 
113: 4. No webserver or webpage configuration system.
114: 
115: 5. Several blocking delays still exist.
116: 
117: 6. No state machine architecture.
118: 
119: 7. No self-healing/recovery mechanism.
120: 
121: 8. OLED UI is primitive and messy.
122: 
123: 9. Wi-Fi manager is not intelligent.
124:    Still uses hardcoded priority logic.
125: 
126: 10. Debug system is poor.
127:     Requires commenting/uncommenting code manually.
128: 
129: ==================================================
130: MAIN DEVELOPMENT TARGETS
131: ==================================================
132: 
133: The redesigned firmware MUST:
134: 
135: 1. Be FULLY asynchronous and non-blocking.
136: 2. Use FreeRTOS tasks properly if needed.
137: 3. Utilize ESP32 dual-core architecture appropriately.
138: 4. Avoid delay() completely.
139: 5. Use millis()-based scheduling or task scheduling.
140: 6. Implement robust state machines.
141: 7. Implement watchdog-safe design.
142: 8. Support self-recovery mechanisms.
143: 9. Be scalable for future sensors/modules.
144: 10. Be beginner-friendly and educational.
145: 
146: ==================================================
147: CONFIGURATION ARCHITECTURE
148: ==================================================
149: 
150: Create centralized configuration architecture.
151: 
152: Split configuration into TWO categories:
153: 
154: 1. HARD-CODE CONFIGURATION
155:    - programmer-only settings
156:    - stored in config.h
157: 
158: 2. USER CONFIGURATION
159:    - editable from webpage
160:    - stored in EEPROM or Preferences/NVS
161:    - editable without recompiling firmware
162: 
163: ==================================================
164: MQTT REQUIREMENTS
165: ==================================================
166: 
167: Replace old MQTT publish-per-topic architecture.
168: 
169: NEW REQUIREMENTS:
170: - Use structured JSON payload.
171: - Publish synchronized data snapshots.
172: - Backend parser target = Node.js server.
173: 
174: Design a PROFESSIONAL JSON schema including:
175: - device metadata
176: - timestamp
177: - Wi-Fi status
178: - MQTT status
179: - all 3-phase electrical parameters
180: - calibration info
181: - system health
182: - uptime
183: - RSSI
184: - error flags
185: 
186: Explain WHY the JSON structure is designed that way.
187: 
188: ==================================================
189: OLED DISPLAY REQUIREMENTS
190: ==================================================
191: 
192: Display:
193: SSD1306 128x64
194: 
195: UI MUST:
196: - look professional
197: - readable
198: - organized
199: - modular
200: - scalable
201: 
202: Button Logic:
203: ONLY ONE BUTTON is used.
204: 
205: Button Actions:
206: 1. Short press
207:    - move page
208:    - move cursor
209: 
210: 2. Long press (default 2 seconds, configurable from webpage)
211:    - ENTER action
212:    - open menu
213:    - confirm action
214:    - back action
215: 
216: ==================================================
217: DETAILED OLED UI REQUIREMENTS
218: ==================================================
219: 
220: IMPORTANT:
221: The OLED UI design below MUST be implemented EXACTLY as specified.
222: 
223: Display Type:
224: - SSD1306
225: - 128x64
226: - I2C
227: 
228: The OLED system MUST:
229: - use clean rendering architecture
230: - support modular page rendering
231: - support page state machine
232: - support cursor state machine
233: - avoid flickering
234: - support partial redraw optimization if possible
235: - separate UI logic from business logic
236: - avoid blocking redraw
237: - support future menu scalability
238: 
239: ==================================================
240: TITLE BAR REQUIREMENTS
241: ==================================================
242: 
243: Top title bar rules:
244: - Every page MUST have a title bar.
245: - Add separator line below title.
246: - MQTT icon uses letter "Q"
247: - Wi-Fi icon uses letter "W"
248: - Q and W indicators are INDEPENDENT.
249: - Do NOT show W if Wi-Fi disconnected.
250: - Do NOT show Q if MQTT disconnected.
251: - If both connected:
252:   "QW"
253: - If only Wi-Fi connected:
254:   "W"
255: - If only MQTT connected:
256:   "Q"
257: 
258: Example:
259: "3-Phase Data Logger  QW"
260: 
261: ==================================================
262: BUTTON CONTROL RULES
263: ==================================================
264: 
265: ONLY ONE BUTTON exists.
266: 
267: Button behavior:
268: 1. SHORT PRESS
269:    - move monitoring page
270:    - move menu cursor
271:    - cycle selection
272:    - horizontal selection
273: 
274: 2. LONG PRESS
275:    - default = 2 seconds
276:    - configurable from webpage
277:    - acts as ENTER
278:    - acts as CONFIRM
279:    - acts as BACK depending on state
280: 
281: ==================================================
282: MONITORING MODE RULES
283: ==================================================
284: 
285: Monitoring mode behavior:
286: - short press:
287:   move to next monitoring page
288: 
289: - long press:
290:   enter Menu Mode
291: 
292: Monitoring pages MUST auto refresh asynchronously.
293: 
294: ==================================================
295: MONITORING PAGE 1
296: ==================================================
297: 
298: Layout:
299: 
300: ┌────────────────────────────┐
301: │    3-Phase Data Logger  QW │
302: │   R         S         T    │
303: │  220V      220V      220V  │
304: │  100A      100A      100A  │
305: │   OK        OK        OK   │
306: └────────────────────────────┘
307: 
308: Page content:
309: - Phase labels: R S T
310: - Voltage line-to-neutral
311: - Current per phase
312: - Line status:
313:   - LOST
314:   - UNDER
315:   - OK
316: 
317: ==================================================
318: MONITORING PAGE 2
319: ==================================================
320: 
321: Layout:
322: 
323: ┌────────────────────────────┐
324: │    3-Phase Data Logger  QW │
325: │   R         S         T    │
326: │  60Hz      60Hz      60Hz  │
327: │ 100VA     1.2kVA    10kVA  │
328: │ 100VAr    2.0kVAr   13kVAr │
329: └────────────────────────────┘
330: 
331: Page content:
332: - Frequency
333: - Apparent Power
334: - Reactive Power
335: 
336: Unit conversion rules:
337: - <1000:
338:   VA / VAR
339: - >=1000:
340:   kVA / kVAr
341: 
342: ==================================================
343: MONITORING PAGE 3
344: ==================================================
345: 
346: Layout:
347: 
348: ┌────────────────────────────┐
349: │    3-Phase Data Logger  QW │
350: │   R         S         T    │
351: │ 0.8PF     1.0PF     0.7PF  │
352: │ 100W       12kW     2.2kW  │
353: │ 300Wh     1.4kWh     30kWh │
354: └────────────────────────────┘
355: 
356: Page content:
357: - Power Factor
358: - Active Power
359: - Total Active Energy
360: 
361: Unit conversion rules:
362: - <1000:
363:   W / Wh
364: - >=1000:
365:   kW / kWh
366: 
367: ==================================================
368: MENU MODE RULES
369: ==================================================
370: 
371: Menu mode behavior:
372: - short press:
373:   move cursor
374: 
375: - long press:
376:   ENTER selected menu
377: 
378: Cursor rules:
379: - cursor loops from bottom back to top
380: - horizontal selection also loops
381: - use visual indentation or ">" symbol
382: 
383: Every submenu MUST contain:
384: - Back option
385: 
386: EXCEPTION:
387: - "Back to Monitoring"
388: 
389: If selected using long press:
390: - return directly to Monitoring Page 1
391: 
392: ==================================================
393: MENU MODE PAGE 1
394: ==================================================
395: 
396: Layout:
397: 
398: ┌────────────────────────────┐
399: │    3-Phase Data Logger  QW │
400: │   > Config                 │
401: │  > Reset kWh               │
402: │  > Reboot Device           │
403: │  > Back to Monitoring      │
404: └────────────────────────────┘
405: 
406: Menu items:
407: - Config
408: - Reset kWh
409: - Reboot Device
410: - Back to Monitoring
411: 
412: ==================================================
413: MENU MODE > CONFIG PAGE
414: ==================================================
415: 
416: Purpose:
417: - ONLY used for enabling/disabling AP mode and webserver
418: - actual configuration handled from webpage
419: 
420: ACTIVE STATE:
421: 
422: ┌────────────────────────────┐
423: │       CONFIGURATION     QW │
424: │  SSID: LOGGER              │
425: │  PW  : 12345678            │
426: │  192.168.1.5               │
427: │  > Active         Back     │
428: └────────────────────────────┘
429: 
430: INACTIVE STATE:
431: 
432: ┌────────────────────────────┐
433: │       CONFIGURATION     QW │
434: │  SSID: LOGGER              │
435: │  PW  : 12345678            │
436: │  192.168.1.5               │
437: │  Inactive       > Back     │
438: └────────────────────────────┘
439: 
440: Rules:
441: - long press on Active:
442:   enable AP + webserver
443: 
444: - long press again:
445:   disable AP + webserver
446: 
447: - Back returns to Menu Mode
448: 
449: ==================================================
450: MENU MODE > RESET KWH PAGE
451: ==================================================
452: 
453: CONFIRM PAGE:
454: 
455: ┌────────────────────────────┐
456: │       CONFIGURATION     QW │
457: │  Are you sure to           │
458: │      Reset kWh?            │
459: │                            │
460: │  > Reset          Back     │
461: └────────────────────────────┘
462: 
463: SUCCESS PAGE:
464: 
465: ┌────────────────────────────┐
466: │       CONFIGURATION     QW │
467: │                            │
468: │     Reset Success          │
469: │                            │
470: │                            │
471: └────────────────────────────┘
472: 
473: FAIL PAGE:
474: 
475: ┌────────────────────────────┐
476: │       CONFIGURATION     QW │
477: │                            │
478: │      Reset Failed          │
479: │                            │
480: │                            │
481: └────────────────────────────┘
482: 
483: Rules:
484: - reset ALL PZEM energy counters
485: - show result page for 2 seconds
486: - auto return to Menu Mode
487: 
488: ==================================================
489: MENU MODE > REBOOT DEVICE PAGE
490: ==================================================
491: 
492: CONFIRM PAGE:
493: 
494: ┌────────────────────────────┐
495: │       CONFIGURATION     QW │
496: │  Are you sure to           │
497: │    Reboot Device?          │
498: │                            │
499: │  > Reboot         Back     │
500: └────────────────────────────┘
501: 
502: COUNTDOWN PAGE:
503: 
504: ┌────────────────────────────┐
505: │       CONFIGURATION     QW │
506: │                            │
507: │     Device Rebooting       │
508: │       in 3 seconds         │
509: │                            │
510: └────────────────────────────┘
511: 
512: Rules:
513: - perform software reboot
514: - countdown:
515:   3
516:   2
517:   1
518:   0
519: 
520: - reboot automatically after countdown
521: - use non-blocking countdown implementation
522: 
523: ==================================================
524: OLED ENGINEERING REQUIREMENTS
525: ==================================================
526: 
527: Implement:
528: - UI renderer abstraction
529: - display page manager
530: - display state machine
531: - menu navigation manager
532: - button event manager
533: - long-press detection
534: - debounce protection
535: - redraw throttling
536: - asynchronous display refresh
537: 
538: IMPORTANT:
539: The OLED system MUST remain responsive even when:
540: - Wi-Fi reconnecting
541: - MQTT reconnecting
542: - sensor timeout occurs
543: - AP/webserver active
544: - MQTT publish active
545: 
546: The UI MUST NEVER freeze because of networking operations.
547: 
548: ==================================================
549: OLED UI STRUCTURE
550: ==================================================
551: 
552: Implement:
553: 
554: 1. Monitoring Mode
555: 2. Menu Mode
556: 
557: Implement all pages exactly as described below.
558: 
559: [IMPORTANT]
560: You MUST implement:
561: - cursor system
562: - page state machine
563: - menu navigation logic
564: - back navigation
565: - confirmation pages
566: - reboot countdown
567: - reset success/fail messages
568: - monitoring page auto refresh
569: - dynamic icons
570: - proper formatting
571: 
572: Use clean UI rendering abstraction.
573: 
574: Avoid flickering.
575: 
576: Use partial redraw if possible.
577: 
578: ==================================================
579: MONITORING PAGE REQUIREMENTS
580: ==================================================
581: 
582: Monitoring Page 1:
583: - Voltage
584: - Current
585: - Line Status
586: 
587: Monitoring Page 2:
588: - Frequency
589: - Apparent Power
590: - Reactive Power
591: 
592: Monitoring Page 3:
593: - Power Factor
594: - Active Power
595: - Energy
596: 
597: Formatting Rules:
598: - automatic unit conversion
599: - VA → kVA
600: - VAR → kVAR
601: - W → kW
602: - Wh → kWh
603: 
604: ==================================================
605: MENU MODE REQUIREMENTS
606: ==================================================
607: 
608: Menu Tree:
609: - Config
610: - Reset kWh
611: - Reboot Device
612: - Back to Monitoring
613: 
614: Config Page:
615: - Enable/disable AP + webserver
616: 
617: Reset Page:
618: - confirmation screen
619: - success/fail status page
620: 
621: Reboot Page:
622: - reboot countdown page
623: - system reboot via software
624: 
625: ==================================================
626: WEB SERVER REQUIREMENTS
627: ==================================================
628: 
629: Implement:
630: - ESP32 Access Point
631: - HTTP Webserver
632: - Responsive HTML/CSS UI
633: - Mobile-friendly layout
634: 
635: IMPORTANT:
636: If ESP32 is connected to internet Wi-Fi,
637: and AP mode is active,
638: attempt internet passthrough behavior if possible.
639: 
640: ==================================================
641: WEBPAGE STRUCTURE
642: ==================================================
643: 
644: Main Page:
645: - Manage Known Network
646: - MQTT Config
647: - PZEM Calibration
648: - HARDRESET EEPROM
649: 
650: Known Network Page:
651: - Table of saved Wi-Fi credentials
652: - Add new network
653: - Delete network
654: 
655: MQTT Config Page:
656: Editable:
657: - MQTT server
658: - ports
659: - username/password
660: - SSL enable
661: - websocket enable
662: - topic prefix
663: 
664: Calibration Page:
665: Per-phase calibration:
666: - voltage offset
667: - current offset
668: 
669: Use (+/-) offset model.
670: 
671: ==================================================
672: SYSTEM ARCHITECTURE REQUIREMENTS
673: ==================================================
674: 
675: Use OOP architecture.
676: 
677: Project structure should resemble ROS2-style modularity.
678: 
679: Example structure:
680: 
681: - main.ino
682: - config.h
683: - types.h
684: - system_state.h
685: - task_manager.h/.cpp
686: - wifi_manager.h/.cpp
687: - mqtt_manager.h/.cpp
688: - data_acquisition.h/.cpp
689: - calibration.h/.cpp
690: - json_builder.h/.cpp
691: - display_manager.h/.cpp
692: - menu_manager.h/.cpp
693: - button_manager.h/.cpp
694: - webserver_manager.h/.cpp
695: - storage_manager.h/.cpp
696: - led_manager.h/.cpp
697: - diagnostics.h/.cpp
698: 
699: You may improve the structure if needed.
700: 
701: ==================================================
702: DEBUG SYSTEM REQUIREMENTS
703: ==================================================
704: 
705: Create PROFESSIONAL debug architecture.
706: 
707: Requirements:
708: - runtime debug enable/disable
709: - serial logging levels
710: - fake/random sensor injection mode
711: - communication diagnostics
712: - task timing diagnostics
713: - MQTT publish diagnostics
714: - Wi-Fi diagnostics
715: - memory usage diagnostics
716: 
717: DO NOT require commenting/uncommenting code manually.
718: 
719: ==================================================
720: SELF-HEALING REQUIREMENTS
721: ==================================================
722: 
723: Implement:
724: - automatic Wi-Fi reconnect
725: - automatic MQTT reconnect
726: - task timeout recovery
727: - sensor timeout handling
728: - invalid data filtering
729: - brownout-safe logic
730: - watchdog-safe task design
731: 
732: ==================================================
733: IMPORTANT ENGINEERING REQUIREMENTS
734: ==================================================
735: 
736: 1. NO delay() unless absolutely justified.
737: 2. NO spaghetti code.
738: 3. NO giant monolithic file.
739: 4. NO blocking Wi-Fi loops.
740: 5. NO unsafe String abuse causing heap fragmentation.
741: 6. Use const/progmem where reasonable.
742: 7. Explain RAM optimization strategy.
743: 8. Explain task allocation strategy.
744: 9. Explain why certain modules belong to certain CPU cores.
745: 10. Explain design decisions professionally.
746: 
747: ==================================================
748: OUTPUT FORMAT
749: ==================================================
750: 
751: Generate:
752: 1. Full firmware architecture explanation
753: 2. File-by-file explanation
754: 3. Recommended libraries
755: 4. Dependency explanation
756: 5. State machine design
757: 6. JSON schema design
758: 7. Task scheduling design
759: 8. Folder structure
760: 9. Example code implementation
761: 10. OLED rendering architecture
762: 11. Webserver architecture
763: 12. Calibration architecture
764: 13. EEPROM/NVS storage design
765: 14. Error handling strategy
766: 15. Debug strategy
767: 16. Scaling strategy for future expansion
768: 
769: VERY IMPORTANT:
770: Do NOT generate incomplete toy examples.
771: 
772: Generate production-grade architecture and implementation examples suitable for real industrial-style deployment on ESP32.
773: 
774: Focus heavily on:
775: - reliability
776: - maintainability
777: - readability
778: - scalability
779: - asynchronous design
780: - robustness
781: 
782: Act like a real senior embedded architect reviewing firmware for commercial deployment.

(End of file - total 782 lines)
</content>
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 7.5s)

_Thinking:_

Now let me look at the `.agents/` directory and the existing `.ino` files to understand the current state of the code.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents</path>
<type>directory</type>
<entries>
skills/

(1 entries)
</entries>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/Data_Logger_Monitor.ino"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/Data_Logger_Monitor.ino</path>
<type>file</type>
<content>
1: #include <WiFi.h>
2: #include <WiFiClientSecure.h>
3: #include <ArduinoMqttClient.h>
4: #include <PubSubClient.h>
5: #include <PZEM004Tv30.h>
6: #include "credentials.h"
7: #include "ResetEnergy.h"
8: #include "WifiSelector.h"
9: #include "DisplayOLED.h"
10: #include "Button.h"
11: // #include "credentials_example.h"
12: 
13: // ===================== WiFi ======================
14: WiFiClientSecure espClient;
15: 
16: // ============ Instance WiFiSelector ==============
17: WiFiSelector wifiManager(10000);  // timeout 10 detik
18: 
19: // ===================== MQTT ======================
20: PubSubClient client(espClient);
21: 
22: // ==================== Millis =====================
23: unsigned long lastMsg = 0;
24: 
25: // ================== PZEM CONFIG ==================
26: #define PZEM_RX1_PIN 4
27: #define PZEM_TX1_PIN 15
28: #define PZEM_RX2_PIN 17
29: #define PZEM_TX2_PIN 16
30: // #define PZEM_SERIAL Serial2
31: HardwareSerial PZEMSerial1(1);
32: HardwareSerial PZEMSerial2(2);
33: #define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM
34: DisplayOLED oled;
35: 
36: #define BUTTON_PIN  5
37: Button btn(BUTTON_PIN, 5000);
38: 
39: // Inisialisasi PZEM dengan alamat unik
40: PZEM004Tv30 pzems[NUM_PZEMS] = {
41:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
42:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
43:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
44: };
45: 
46: // Reset Energy Initial
47: #define RESET_ENERGY_BUTTON_PIN  5
48: #define RESET_ENERGY_LED_PIN     2   // LED internal ESP32 (GPIO 2)/18
49: ResetEnergy resetEnergy(RESET_ENERGY_BUTTON_PIN, RESET_ENERGY_LED_PIN);
50: 
51: // ================== MQTT TOPICS ==================
52: // Voltage (V)
53: const char* workshop_v[]        = {"lutpiii/workshop/voltage/l1",        "lutpiii/workshop/voltage/l2",        "lutpiii/workshop/voltage/l3"};
54: // Current (A)
55: const char* workshop_i[]        = {"lutpiii/workshop/current/l1",        "lutpiii/workshop/current/l2",        "lutpiii/workshop/current/l3"};
56: // Frequency (Hz)
57: const char* workshop_f[]        = {"lutpiii/workshop/frequency/l1",      "lutpiii/workshop/frequency/l2",      "lutpiii/workshop/frequency/l3"};
58: // Apparent Power (VA)
59: const char* workshop_va[]       = {"lutpiii/workshop/apparent_power/l1", "lutpiii/workshop/apparent_power/l2", "lutpiii/workshop/apparent_power/l3"};
60: // Reactive Power (VAr)
61: const char* workshop_var[]      = {"lutpiii/workshop/reactive_power/l1", "lutpiii/workshop/reactive_power/l2", "lutpiii/workshop/reactive_power/l3"};
62: // Active Power (W)
63: const char* workshop_p[]        = {"lutpiii/workshop/active_power/l1",   "lutpiii/workshop/active_power/l2",   "lutpiii/workshop/active_power/l3"};
64: // Power Factor
65: const char* workshop_pf[]       = {"lutpiii/workshop/power_factor/l1",   "lutpiii/workshop/power_factor/l2",   "lutpiii/workshop/power_factor/l3"};
66: // Energy (kWh)
67: const char* workshop_e[]        = {"lutpiii/workshop/active_energy/l1",  "lutpiii/workshop/active_energy/l2",  "lutpiii/workshop/active_energy/l3"};
68: // Status Main Power
69: const char* line_status[]       = {"lutpiii/workshop/line_status/l1",    "lutpiii/workshop/line_status/l2",    "lutpiii/workshop/line_status/l3"};
70: 
71: // ================== ENUM LINE STATUS ==================
72: enum LineStatus { LINE_GOOD, LINE_UNDERVOLTAGE, LINE_ISSUED };
73: LineStatus lineStatus[NUM_PZEMS];  // status tiap line 
74: 
75: // =============== Buffer untuk OLED ===============
76: float vol[3], cur[3], powe[3], pff[3], freqArr[3], ener[3];
77: int st[3];
78: 
79: // ================== SETUP WIFI ==================
80: void setup_wifi() {
81:   if (!wifiManager.connectBestNetwork()) {
82:     Serial.println("Tidak bisa terhubung ke jaringan mana pun!");
83:   } else {
84:     wifiManager.printStatus();
85:   }
86: }
87: 
88: // ================== MQTT CONNECT ==================
89: void reconnect() {
90:   while (!client.connected()) {
91:     Serial.print("Attempting MQTT connection...");
92:     String clientId = "ESP32Client-";
93:     clientId += String(random(0xffff), HEX);
94:     if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
95:       Serial.println("✅MQTT Server Connected!");
96:     } else {
97:       Serial.print("❌failed, rc=");
98:       Serial.print(client.state());
99:       Serial.println(" retry in 5s");
100:       delay(5000);
101:     }
102:   }
103: }
104: 
105: float randFloat(float minVal, float maxVal) {
106:   return minVal + ((float)random(0, 10000) / 10000.0) * (maxVal - minVal);
107: }
108: 
109: void generateDummyData(float &voltage, float &current, float &power, float &energy, float &freq, float &pf) {
110:   voltage = randFloat(200, 220);      // 180–240 V (normal + undervolt simulation)
111:   current = randFloat(48.0, 50.0);    // 0.1–50 A
112:   freq    = randFloat(50.0, 50.5);    // 50Hz +- tolerance
113:   pf      = randFloat(0.6, 0.8);      // 0.5–1.0 realistic PF
114: 
115:   power   = voltage * current * pf;   // P = V × I × pf
116:   energy += randFloat(2200, 4000);    // simulate energy increasing (Wh)
117: }
118: 
119: // ================== SETUP ==================
120: void setup() {
121:   Serial.begin(115200);
122:   setup_wifi();
123: 
124:   resetEnergy.begin();
125:   oled.begin();
126:   oled.enableAutoScroll(false);
127: 
128:   client.setServer(MQTT_SERVER, MQTTS_PORT);
129:   // espClient.setInsecure(); // WARNING: tidak aman
130:   espClient.setCACert(CA_CERT);
131: 
132:   // randomSeed(analogRead(0));  // seed PRNG
133:   // Init Serial2 untuk PZEM
134:   // PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
135: }
136: 
137: // ================== LOOP ==================
138: void loop() {
139:   if (!client.connected()) reconnect();
140:   client.loop();
141: 
142:   btn.update();
143: 
144:   // WiFi auto check tiap 30 detik
145:   static unsigned long lastWiFiCheck = 0;
146:   if (millis() - lastWiFiCheck > 30000) {
147:     lastWiFiCheck = millis();
148:     if (!wifiManager.isConnected()) {
149:       Serial.println("WiFi terputus, mencoba koneksi ulang...");
150:       wifiManager.connectBestNetwork();
151:     } else if(wifiManager.isConnected()) {
152:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
153:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
154:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
155:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
156:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
157:     }
158:   }
159: 
160:   // resetEnergy.handle(pzems, NUM_PZEMS);
161:   if (btn.isShortPressed()) {
162:     uint8_t next = (oled.getPage() + 1) % 3;
163:     oled.setPage(next);
164:   }
165: 
166:   if (btn.isLongPressed()) {
167:     // panggil reset untuk semua PZEM
168:     resetEnergy.resetAll(pzems, NUM_PZEMS);
169:   }
170: 
171:   unsigned long now = millis();
172:   if (now - lastMsg > 500) {  // interval 1 detik
173:     lastMsg = now;
174: 
175:     for (int i = 0; i < NUM_PZEMS; i++) {
176:       float voltage = pzems[i].voltage();
177:       float current = pzems[i].current();
178:       float power   = pzems[i].power();
179:       float energy  = pzems[i].energy()*1000;
180:       float freq    = pzems[i].frequency();
181:       float pf      = pzems[i].pf();
182: 
183:       // pzems[i].voltage();
184:       // pzems[i].current();
185:       // pzems[i].power();
186:       // pzems[i].energy();
187:       // pzems[i].frequency();
188:       // pzems[i].pf();
189: 
190:       // float voltage, current, power, energy = 0, freq, pf;
191:       // generateDummyData(voltage, current, power, energy, freq, pf);
192: 
193:       // Apparent Power & Reactive Power
194:       float va = voltage * current;
195:       float var = sqrt((va * va) - (power * power));
196: 
197:       // Simpan ke buffer
198:       vol[i]      = voltage;
199:       cur[i]      = current;
200:       powe[i]     = power;
201:       pff[i]      = pf;
202:       freqArr[i]  = freq;
203:       ener[i]     = energy;
204:       st[i]       = lineStatus[i];
205: 
206:       // if(!isnan(voltage)){
207:       //   lineStatus[i] = LINE_GOOD;
208:       //   // Debug Serial
209:       //   // Serial.print("PZEM "); Serial.print(i+1);
210:       //   // Serial.print(" | V: "); Serial.print(voltage);
211:       //   // Serial.print("V | I: "); Serial.print(current);
212:       //   // Serial.print("A | f: "); Serial.print(freq);
213:       //   // Serial.print("Hz | VA: "); Serial.print(va);
214:       //   // Serial.print("VA | VAR: "); Serial.print(var);
215:       //   // Serial.print("VAR | P: "); Serial.print(power);
216:       //   // Serial.print("W | PF: "); Serial.print(pf);
217:       //   // Serial.print(" | E: "); Serial.print(energy, 2);
218:       //   // Serial.println("Wh");
219:       // }else if(voltage < 199 && voltage > 1){
220:       //   lineStatus[i] = LINE_UNDERVOLTAGE;
221:       //   // Serial.println("⚡WARNING: 3-Phase is Running UNDERVOLTAGE!!!");
222:       // }else{
223:       //   lineStatus[i] = LINE_ISSUED;
224:       //   voltage = current = power = freq = va = var = pf = 0;
225:       //   // Serial.println("❌MAIN POWER ISSUED | PLEASE CHECK THE PROBE!!");
226:       // }
227: 
228:       if (isnan(voltage) || voltage <= 0) {
229:         lineStatus[i] = LINE_ISSUED; // Tidak ada data / probe error
230:         voltage = current = power = freq = va = var = pf = 0;
231:       }
232:       else if (voltage < 199) {
233:         lineStatus[i] = LINE_UNDERVOLTAGE; // Tegangan rendah
234:       }
235:       else {
236:         lineStatus[i] = LINE_GOOD; // Normal
237:       }
238: 
239:       // ============= SERIAL MONITOR =============
240:       switch (lineStatus[i]) {
241:         case LINE_GOOD:
242:           Serial.printf("✅ Line %d: GOOD (V: %.1fV", i + 1, voltage);
243:           // Serial.print("PZEM "); Serial.print(i+1);
244:           Serial.print(" | I: "); Serial.print(current);
245:           Serial.print("A | f: "); Serial.print(freq);
246:           Serial.print("Hz | VA: "); Serial.print(va);
247:           Serial.print("VA | VAR: "); Serial.print(var);
248:           Serial.print("VAR | P: "); Serial.print(power);
249:           Serial.print("W | PF: "); Serial.print(pf);
250:           Serial.print(" | E: "); Serial.print(energy, 2);
251:           Serial.println("Wh)");
252:           break;
253:         case LINE_UNDERVOLTAGE:
254:           Serial.printf("⚡ Line %d: UNDERVOLTAGE (V: %.1fV", i + 1, voltage);
255:           Serial.print(" | I: "); Serial.print(current);
256:           Serial.print("A | f: "); Serial.print(freq);
257:           Serial.print("Hz | VA: "); Serial.print(va);
258:           Serial.print("VA | VAR: "); Serial.print(var);
259:           Serial.print("VAR | P: "); Serial.print(power);
260:           Serial.print("W | PF: "); Serial.print(pf);
261:           Serial.print(" | E: "); Serial.print(energy, 2);
262:           Serial.println("Wh)");
263:           break;
264:         case LINE_ISSUED:
265:           Serial.printf("❌ Line %d: ISSUED / PROBE ERROR\n", i + 1);
266:           break;
267:       }
268: 
269: 
270:       // Publish ke MQTT
271:       client.publish(workshop_v[i], String(voltage).c_str());
272:       client.publish(workshop_i[i], String(current).c_str());
273:       client.publish(workshop_p[i], String(power).c_str());
274:       client.publish(workshop_e[i], String(energy).c_str());
275:       client.publish(workshop_f[i], String(freq).c_str());
276:       client.publish(workshop_pf[i], String(pf).c_str());
277:       client.publish(workshop_va[i], String(va).c_str());
278:       client.publish(workshop_var[i], String(var).c_str());
279: 
280:       // Publish status line
281:       const char* statusStr = (lineStatus[i] == LINE_GOOD) ? "good"
282:                             : (lineStatus[i] == LINE_UNDERVOLTAGE) ? "undervoltage"
283:                             : "issued";
284:       client.publish(line_status[i], statusStr);
285:     }
286: 
287:     Serial.println("=== Data Published ===");
288:     oled.update(vol, cur, powe, pff, freqArr, ener, st);
289:   }
290: }

(End of file - total 290 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.h"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.h</path>
<type>file</type>
<content>
1: /******************************************************
2:  * Project   : ESP32 MQTTs Publisher
3:  * File      : credentials.h
4:  * Author    : Muhammad Lutfi Nur Anendi
5:  * Created   : 5 August 2025
6:  * Last Edit : 23 October 2025
7:  *
8:  * Description:
9:  *   Configuration file for WiFi, MQTT, and SSL credentials.
10:  *   Rename this file from `credentials_example.h` → `credentials.h`
11:  *   before compiling the project.
12:  *
13:  * IMPORTANT:
14:  *   - DO NOT commit this file to any public repository.
15:  *   - Keep your credentials private.
16:  *   - Modify only the fields below as required.
17:  *
18:  * MQTT Notes:
19:  *   - MQTT_PORT  = 1883 for non-SSL
20:  *   - MQTTS_PORT = 8883 for SSL (secure)
21:  *   - MQTT_TOPIC example: "user_topic/#"
22:  *
23:  * SSL Notes:
24:  *   - CA_CERT should contain your Root Certificate between
25:  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
26:  *
27:  * Developer Info:
28:  *   GitHub   : https://github.com/lutfi-rpi5
29:  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
30:  *   Instagram: https://instagram.com/lutfiianendi
31:  *
32:  * Revision History:
33:  *   v1.0  Initial release with MQTT & SSL credentials
34:  *   v1.1  Added developer info and usage guide
35:  ******************************************************/
36: 
37: /******************************************************
38:  * Project   : ESP32 MQTTs Publisher
39:  * File      : credentials.h
40:  * Author    : Muhammad Lutfi Nur Anendi
41:  * Created   : 5 August 2025
42:  * Last Edit : 23 October 2025
43:  *
44:  * Description:
45:  *   Configuration header for WiFi, MQTT, and SSL credentials.
46:  ******************************************************/
47: 
48: #ifndef CREDENTIALS_H
49: #define CREDENTIALS_H
50: 
51: // ---------- WiFi Credential Struct ----------
52: struct WiFiCredential {
53:   const char* ssid;
54:   const char* password;
55:   int priority;
56: };
57: 
58: // ---------- WiFi Configuration ----------
59: extern const char* WIFI_SSID1;
60: extern const char* WIFI_PASS1;
61: extern const char* WIFI_SSID2;
62: extern const char* WIFI_PASS2;
63: extern const char* WIFI_SSID3;
64: extern const char* WIFI_PASS3;
65: extern const char* WIFI_SSID4;
66: extern const char* WIFI_PASS4;
67: 
68: extern WiFiCredential wifiList[];
69: extern const int WIFI_COUNT;
70: 
71: // ---------- MQTT Broker Configuration ----------
72: extern const char* MQTT_SERVER;
73: extern const int   MQTT_PORT;
74: extern const int   MQTTS_PORT;
75: extern const int   WS_PORT;
76: extern const int   WSS_PORT;
77: extern const char* MQTT_USER;
78: extern const char* MQTT_PASS;
79: extern const char* MQTT_TOPIC;
80: 
81: // ---------- Optional API Key ----------
82: extern const char* API_KEY;
83: 
84: // ---------- Root CA Certificate ----------
85: extern const char* CA_CERT;
86: 
87: #endif  // CREDENTIALS_H
88: 
89: 

(End of file - total 89 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/brief-cl.md"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/brief-cl.md</path>
<type>file</type>
<content>
1: !!!IMPORTANT: IGNORE THIS FILE!!!
2: 
3: You are a senior embedded systems architect and firmware engineer.
4: 
5: Your task is to redesign and refactor an existing production ESP32 firmware project into a highly professional, scalable, asynchronous, non-blocking, maintainable, and beginner-friendly firmware architecture.
6: 
7: Primary language for all code, comments, documentation, variable naming, commit-style notes, and explanations MUST be English.
8: 
9: Target users are beginner embedded engineers, students, and hobbyists.
10: Therefore:
11: - The code MUST be extremely readable.
12: - The code MUST contain extensive inline documentation.
13: - The code MUST be easy to debug and troubleshoot.
14: - The code MUST avoid hidden logic and magic numbers.
15: - The code MUST prioritize stability and field reliability.
16: - The code MUST minimize system blocking.
17: - The code MUST contain proper modularization and architecture.
18: - The code MUST be optimized for ESP32 resource limitations.
19: - The code MUST be token-efficient but still maintain professional documentation quality.
20: 
21: ==================================================
22: PROJECT OVERVIEW
23: ==================================================
24: 
25: Project Name:
26: 3-Phase Data Logger
27: 
28: Current Device Purpose:
29: This device ONLY performs:
30: 1. Data acquisition from 3x PZEM-004T
31: 2. Data processing
32: 3. MQTT publishing
33: 4. OLED display handling
34: 5. Button handling
35: 6. Wi-Fi management
36: 
37: IMPORTANT:
38: This device DOES NOT perform database logging internally.
39: Actual data logging is handled by a backend website/server subscribed to MQTT broker topics.
40: 
41: ==================================================
42: CURRENT HARDWARE
43: ==================================================
44: 
45: MCU:
46: - ESP32 DevKit V1 30-pin
47: - Arduino IDE v2.0
48: - ESP32 Board Package v2.0.14
49: 
50: Sensors:
51: - 3x PZEM-004T 100A
52:   Each PZEM handles:
53:   Line-to-Neutral measurement for 3-phase WYE 220V system
54: 
55: Display:
56: - OLED SSD1306 128x64 I2C
57: 
58: Input:
59: - 1x Push Button (Normally Open)
60: 
61: Indicators:
62: - Built-in ESP32 LED
63: 
64: ==================================================
65: PIN CONFIGURATION
66: ==================================================
67: 
68: //================ PZEM =================//
69: 
70: #define PZEM_RX1_PIN 4
71: #define PZEM_TX1_PIN 15
72: #define PZEM_RX2_PIN 17
73: #define PZEM_TX2_PIN 16
74: 
75: HardwareSerial PZEMSerial1(1);
76: HardwareSerial PZEMSerial2(2);
77: 
78: #define NUM_PZEMS 3
79: 
80: PZEM004Tv30 pzems[NUM_PZEMS] = {
81:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10),
82:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11),
83:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)
84: };
85: 
86: //================ OLED =================//
87: 
88: SDA = GPIO21
89: SCL = GPIO22
90: 
91: //================ BUTTON & LED =================//
92: 
93: BUTTON = GPIO5
94: LED    = GPIO2
95: 
96: ==================================================
97: CURRENT SYSTEM PROBLEMS
98: ==================================================
99: 
100: The current firmware is already deployed in production but still has architectural weaknesses and bugs:
101: 
102: 1. MQTT data is published per-topic individually.
103:    Result:
104:    - asynchronous timestamp mismatch
105:    - data arriving at backend not synchronized
106:    - some values are delayed by milliseconds
107: 
108: 2. Wi-Fi connection during boot is blocking.
109:    Result:
110:    - device stuck on connecting state
111:    - OLED UI inaccessible until Wi-Fi connected
112: 
113: 3. No calibration system.
114: 
115: 4. No webserver or webpage configuration system.
116: 
117: 5. Several blocking delays still exist.
118: 
119: 6. No state machine architecture.
120: 
121: 7. No self-healing/recovery mechanism.
122: 
123: 8. OLED UI is primitive and messy.
124: 
125: 9. Wi-Fi manager is not intelligent.
126:    Still uses hardcoded priority logic.
127: 
128: 10. Debug system is poor.
129:     Requires commenting/uncommenting code manually.
130: 
131: ==================================================
132: MAIN DEVELOPMENT TARGETS
133: ==================================================
134: 
135: The redesigned firmware MUST:
136: 
137: 1. Be FULLY asynchronous and non-blocking.
138: 2. Use FreeRTOS tasks properly if needed.
139: 3. Utilize ESP32 dual-core architecture appropriately.
140: 4. Avoid delay() completely.
141: 5. Use millis()-based scheduling or task scheduling.
142: 6. Implement robust state machines.
143: 7. Implement watchdog-safe design.
144: 8. Support self-recovery mechanisms.
145: 9. Be scalable for future sensors/modules.
146: 10. Be beginner-friendly and educational.
147: 
148: ==================================================
149: CONFIGURATION ARCHITECTURE
150: ==================================================
151: 
152: Create centralized configuration architecture.
153: 
154: Split configuration into TWO categories:
155: 
156: 1. HARD-CODE CONFIGURATION
157:    - programmer-only settings
158:    - stored in config.h
159: 
160: 2. USER CONFIGURATION
161:    - editable from webpage
162:    - stored in EEPROM or Preferences/NVS
163:    - editable without recompiling firmware
164: 
165: ==================================================
166: MQTT REQUIREMENTS
167: ==================================================
168: 
169: Replace old MQTT publish-per-topic architecture.
170: 
171: NEW REQUIREMENTS:
172: - Use structured JSON payload.
173: - Publish synchronized data snapshots.
174: - Backend parser target = Node.js server.
175: 
176: Design a PROFESSIONAL JSON schema including:
177: - device metadata
178: - timestamp
179: - Wi-Fi status
180: - MQTT status
181: - all 3-phase electrical parameters
182: - calibration info
183: - system health
184: - uptime
185: - RSSI
186: - error flags
187: 
188: Explain WHY the JSON structure is designed that way.
189: 
190: ==================================================
191: OLED DISPLAY REQUIREMENTS
192: ==================================================
193: 
194: Display:
195: SSD1306 128x64
196: 
197: UI MUST:
198: - look professional
199: - readable
200: - organized
201: - modular
202: - scalable
203: 
204: Button Logic:
205: ONLY ONE BUTTON is used.
206: 
207: Button Actions:
208: 1. Short press
209:    - move page
210:    - move cursor
211: 
212: 2. Long press (default 2 seconds, configurable from webpage)
213:    - ENTER action
214:    - open menu
215:    - confirm action
216:    - back action
217: 
218: ==================================================
219: DETAILED OLED UI REQUIREMENTS
220: ==================================================
221: 
222: IMPORTANT:
223: The OLED UI design below MUST be implemented EXACTLY as specified.
224: 
225: Display Type:
226: - SSD1306
227: - 128x64
228: - I2C
229: 
230: The OLED system MUST:
231: - use clean rendering architecture
232: - support modular page rendering
233: - support page state machine
234: - support cursor state machine
235: - avoid flickering
236: - support partial redraw optimization if possible
237: - separate UI logic from business logic
238: - avoid blocking redraw
239: - support future menu scalability
240: 
241: ==================================================
242: TITLE BAR REQUIREMENTS
243: ==================================================
244: 
245: Top title bar rules:
246: - Every page MUST have a title bar.
247: - Add separator line below title.
248: - MQTT icon uses letter "Q"
249: - Wi-Fi icon uses letter "W"
250: - Q and W indicators are INDEPENDENT.
251: - Do NOT show W if Wi-Fi disconnected.
252: - Do NOT show Q if MQTT disconnected.
253: - If both connected:
254:   "QW"
255: - If only Wi-Fi connected:
256:   "W"
257: - If only MQTT connected:
258:   "Q"
259: 
260: Example:
261: "3-Phase Data Logger  QW"
262: 
263: ==================================================
264: BUTTON CONTROL RULES
265: ==================================================
266: 
267: ONLY ONE BUTTON exists.
268: 
269: Button behavior:
270: 1. SHORT PRESS
271:    - move monitoring page
272:    - move menu cursor
273:    - cycle selection
274:    - horizontal selection
275: 
276: 2. LONG PRESS
277:    - default = 2 seconds
278:    - configurable from webpage
279:    - acts as ENTER
280:    - acts as CONFIRM
281:    - acts as BACK depending on state
282: 
283: ==================================================
284: MONITORING MODE RULES
285: ==================================================
286: 
287: Monitoring mode behavior:
288: - short press:
289:   move to next monitoring page
290: 
291: - long press:
292:   enter Menu Mode
293: 
294: Monitoring pages MUST auto refresh asynchronously.
295: 
296: ==================================================
297: MONITORING PAGE 1
298: ==================================================
299: 
300: Layout:
301: 
302: ┌────────────────────────────┐
303: │    3-Phase Data Logger  QW │
304: │   R         S         T    │
305: │  220V      220V      220V  │
306: │  100A      100A      100A  │
307: │   OK        OK        OK   │
308: └────────────────────────────┘
309: 
310: Page content:
311: - Phase labels: R S T
312: - Voltage line-to-neutral
313: - Current per phase
314: - Line status:
315:   - LOST
316:   - UNDER
317:   - OK
318: 
319: ==================================================
320: MONITORING PAGE 2
321: ==================================================
322: 
323: Layout:
324: 
325: ┌────────────────────────────┐
326: │    3-Phase Data Logger  QW │
327: │   R         S         T    │
328: │  60Hz      60Hz      60Hz  │
329: │ 100VA     1.2kVA    10kVA  │
330: │ 100VAr    2.0kVAr   13kVAr │
331: └────────────────────────────┘
332: 
333: Page content:
334: - Frequency
335: - Apparent Power
336: - Reactive Power
337: 
338: Unit conversion rules:
339: - <1000:
340:   VA / VAR
341: - >=1000:
342:   kVA / kVAr
343: 
344: ==================================================
345: MONITORING PAGE 3
346: ==================================================
347: 
348: Layout:
349: 
350: ┌────────────────────────────┐
351: │    3-Phase Data Logger  QW │
352: │   R         S         T    │
353: │ 0.8PF     1.0PF     0.7PF  │
354: │ 100W       12kW     2.2kW  │
355: │ 300Wh     1.4kWh     30kWh │
356: └────────────────────────────┘
357: 
358: Page content:
359: - Power Factor
360: - Active Power
361: - Total Active Energy
362: 
363: Unit conversion rules:
364: - <1000:
365:   W / Wh
366: - >=1000:
367:   kW / kWh
368: 
369: ==================================================
370: MENU MODE RULES
371: ==================================================
372: 
373: Menu mode behavior:
374: - short press:
375:   move cursor
376: 
377: - long press:
378:   ENTER selected menu
379: 
380: Cursor rules:
381: - cursor loops from bottom back to top
382: - horizontal selection also loops
383: - use visual indentation or ">" symbol
384: 
385: Every submenu MUST contain:
386: - Back option
387: 
388: EXCEPTION:
389: - "Back to Monitoring"
390: 
391: If selected using long press:
392: - return directly to Monitoring Page 1
393: 
394: ==================================================
395: MENU MODE PAGE 1
396: ==================================================
397: 
398: Layout:
399: 
400: ┌────────────────────────────┐
401: │    3-Phase Data Logger  QW │
402: │   > Config                 │
403: │  > Reset kWh               │
404: │  > Reboot Device           │
405: │  > Back to Monitoring      │
406: └────────────────────────────┘
407: 
408: Menu items:
409: - Config
410: - Reset kWh
411: - Reboot Device
412: - Back to Monitoring
413: 
414: ==================================================
415: MENU MODE > CONFIG PAGE
416: ==================================================
417: 
418: Purpose:
419: - ONLY used for enabling/disabling AP mode and webserver
420: - actual configuration handled from webpage
421: 
422: ACTIVE STATE:
423: 
424: ┌────────────────────────────┐
425: │       CONFIGURATION     QW │
426: │  SSID: LOGGER              │
427: │  PW  : 12345678            │
428: │  192.168.1.5               │
429: │  > Active         Back     │
430: └────────────────────────────┘
431: 
432: INACTIVE STATE:
433: 
434: ┌────────────────────────────┐
435: │       CONFIGURATION     QW │
436: │  SSID: LOGGER              │
437: │  PW  : 12345678            │
438: │  192.168.1.5               │
439: │  Inactive       > Back     │
440: └────────────────────────────┘
441: 
442: Rules:
443: - long press on Active:
444:   enable AP + webserver
445: 
446: - long press again:
447:   disable AP + webserver
448: 
449: - Back returns to Menu Mode
450: 
451: ==================================================
452: MENU MODE > RESET KWH PAGE
453: ==================================================
454: 
455: CONFIRM PAGE:
456: 
457: ┌────────────────────────────┐
458: │       CONFIGURATION     QW │
459: │  Are you sure to           │
460: │      Reset kWh?            │
461: │                            │
462: │  > Reset          Back     │
463: └────────────────────────────┘
464: 
465: SUCCESS PAGE:
466: 
467: ┌────────────────────────────┐
468: │       CONFIGURATION     QW │
469: │                            │
470: │     Reset Success          │
471: │                            │
472: │                            │
473: └────────────────────────────┘
474: 
475: FAIL PAGE:
476: 
477: ┌────────────────────────────┐
478: │       CONFIGURATION     QW │
479: │                            │
480: │      Reset Failed          │
481: │                            │
482: │                            │
483: └────────────────────────────┘
484: 
485: Rules:
486: - reset ALL PZEM energy counters
487: - show result page for 2 seconds
488: - auto return to Menu Mode
489: 
490: ==================================================
491: MENU MODE > REBOOT DEVICE PAGE
492: ==================================================
493: 
494: CONFIRM PAGE:
495: 
496: ┌────────────────────────────┐
497: │       CONFIGURATION     QW │
498: │  Are you sure to           │
499: │    Reboot Device?          │
500: │                            │
501: │  > Reboot         Back     │
502: └────────────────────────────┘
503: 
504: COUNTDOWN PAGE:
505: 
506: ┌────────────────────────────┐
507: │       CONFIGURATION     QW │
508: │                            │
509: │     Device Rebooting       │
510: │       in 3 seconds         │
511: │                            │
512: └────────────────────────────┘
513: 
514: Rules:
515: - perform software reboot
516: - countdown:
517:   3
518:   2
519:   1
520:   0
521: 
522: - reboot automatically after countdown
523: - use non-blocking countdown implementation
524: 
525: ==================================================
526: OLED ENGINEERING REQUIREMENTS
527: ==================================================
528: 
529: Implement:
530: - UI renderer abstraction
531: - display page manager
532: - display state machine
533: - menu navigation manager
534: - button event manager
535: - long-press detection
536: - debounce protection
537: - redraw throttling
538: - asynchronous display refresh
539: 
540: IMPORTANT:
541: The OLED system MUST remain responsive even when:
542: - Wi-Fi reconnecting
543: - MQTT reconnecting
544: - sensor timeout occurs
545: - AP/webserver active
546: - MQTT publish active
547: 
548: The UI MUST NEVER freeze because of networking operations.
549: 
550: ==================================================
551: OLED UI STRUCTURE
552: ==================================================
553: 
554: Implement:
555: 
556: 1. Monitoring Mode
557: 2. Menu Mode
558: 
559: Implement all pages exactly as described below.
560: 
561: [IMPORTANT]
562: You MUST implement:
563: - cursor system
564: - page state machine
565: - menu navigation logic
566: - back navigation
567: - confirmation pages
568: - reboot countdown
569: - reset success/fail messages
570: - monitoring page auto refresh
571: - dynamic icons
572: - proper formatting
573: 
574: Use clean UI rendering abstraction.
575: 
576: Avoid flickering.
577: 
578: Use partial redraw if possible.
579: 
580: ==================================================
581: MONITORING PAGE REQUIREMENTS
582: ==================================================
583: 
584: Monitoring Page 1:
585: - Voltage
586: - Current
587: - Line Status
588: 
589: Monitoring Page 2:
590: - Frequency
591: - Apparent Power
592: - Reactive Power
593: 
594: Monitoring Page 3:
595: - Power Factor
596: - Active Power
597: - Energy
598: 
599: Formatting Rules:
600: - automatic unit conversion
601: - VA → kVA
602: - VAR → kVAR
603: - W → kW
604: - Wh → kWh
605: 
606: ==================================================
607: MENU MODE REQUIREMENTS
608: ==================================================
609: 
610: Menu Tree:
611: - Config
612: - Reset kWh
613: - Reboot Device
614: - Back to Monitoring
615: 
616: Config Page:
617: - Enable/disable AP + webserver
618: 
619: Reset Page:
620: - confirmation screen
621: - success/fail status page
622: 
623: Reboot Page:
624: - reboot countdown page
625: - system reboot via software
626: 
627: ==================================================
628: WEB SERVER REQUIREMENTS
629: ==================================================
630: 
631: Implement:
632: - ESP32 Access Point
633: - HTTP Webserver
634: - Responsive HTML/CSS UI
635: - Mobile-friendly layout
636: 
637: IMPORTANT:
638: If ESP32 is connected to internet Wi-Fi,
639: and AP mode is active,
640: attempt internet passthrough behavior if possible.
641: 
642: ==================================================
643: WEBPAGE STRUCTURE
644: ==================================================
645: 
646: Main Page:
647: - Manage Known Network
648: - MQTT Config
649: - PZEM Calibration
650: - HARDRESET EEPROM
651: 
652: Known Network Page:
653: - Table of saved Wi-Fi credentials
654: - Add new network
655: - Delete network
656: 
657: MQTT Config Page:
658: Editable:
659: - MQTT server
660: - ports
661: - username/password
662: - SSL enable
663: - websocket enable
664: - topic prefix
665: 
666: Calibration Page:
667: Per-phase calibration:
668: - voltage offset
669: - current offset
670: 
671: Use (+/-) offset model.
672: 
673: ==================================================
674: SYSTEM ARCHITECTURE REQUIREMENTS
675: ==================================================
676: 
677: Use OOP architecture.
678: 
679: Project structure should resemble ROS2-style modularity.
680: 
681: Example structure:
682: 
683: - main.ino
684: - config.h
685: - types.h
686: - system_state.h
687: - task_manager.h/.cpp
688: - wifi_manager.h/.cpp
689: - mqtt_manager.h/.cpp
690: - data_acquisition.h/.cpp
691: - calibration.h/.cpp
692: - json_builder.h/.cpp
693: - display_manager.h/.cpp
694: - menu_manager.h/.cpp
695: - button_manager.h/.cpp
696: - webserver_manager.h/.cpp
697: - storage_manager.h/.cpp
698: - led_manager.h/.cpp
699: - diagnostics.h/.cpp
700: 
701: You may improve the structure if needed.
702: 
703: ==================================================
704: DEBUG SYSTEM REQUIREMENTS
705: ==================================================
706: 
707: Create PROFESSIONAL debug architecture.
708: 
709: Requirements:
710: - runtime debug enable/disable
711: - serial logging levels
712: - fake/random sensor injection mode
713: - communication diagnostics
714: - task timing diagnostics
715: - MQTT publish diagnostics
716: - Wi-Fi diagnostics
717: - memory usage diagnostics
718: 
719: DO NOT require commenting/uncommenting code manually.
720: 
721: ==================================================
722: SELF-HEALING REQUIREMENTS
723: ==================================================
724: 
725: Implement:
726: - automatic Wi-Fi reconnect
727: - automatic MQTT reconnect
728: - task timeout recovery
729: - sensor timeout handling
730: - invalid data filtering
731: - brownout-safe logic
732: - watchdog-safe task design
733: 
734: ==================================================
735: IMPORTANT ENGINEERING REQUIREMENTS
736: ==================================================
737: 
738: 1. NO delay() unless absolutely justified.
739: 2. NO spaghetti code.
740: 3. NO giant monolithic file.
741: 4. NO blocking Wi-Fi loops.
742: 5. NO unsafe String abuse causing heap fragmentation.
743: 6. Use const/progmem where reasonable.
744: 7. Explain RAM optimization strategy.
745: 8. Explain task allocation strategy.
746: 9. Explain why certain modules belong to certain CPU cores.
747: 10. Explain design decisions professionally.
748: 
749: ==================================================
750: OUTPUT FORMAT
751: ==================================================
752: 
753: Generate:
754: 1. Full firmware architecture explanation
755: 2. File-by-file explanation
756: 3. Recommended libraries
757: 4. Dependency explanation
758: 5. State machine design
759: 6. JSON schema design
760: 7. Task scheduling design
761: 8. Folder structure
762: 9. Example code implementation
763: 10. OLED rendering architecture
764: 11. Webserver architecture
765: 12. Calibration architecture
766: 13. EEPROM/NVS storage design
767: 14. Error handling strategy
768: 15. Debug strategy
769: 16. Scaling strategy for future expansion
770: 
771: VERY IMPORTANT:
772: Do NOT generate incomplete toy examples.
773: 
774: Generate production-grade architecture and implementation examples suitable for real industrial-style deployment on ESP32.
775: 
776: Focus heavily on:
777: - reliability
778: - maintainability
779: - readability
780: - scalability
781: - asynchronous design
782: - robustness
783: 
784: Act like a real senior embedded architect reviewing firmware for commercial deployment.

(End of file - total 784 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/brief.md"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/brief.md</path>
<type>file</type>
<content>
1: ini adalah project yang sudah jadi dan sudah rilis v1.0.0
2: ini penjelasan project yang sekarang dan yang akan di update firmware nya
3: saya lampirkan program yang akan di kembangkan menjadi v2.0.0
4: 
5: Nama Project  : Data Logger 3 Phase
6: 
7: Overview Alat (Hardware)      : konteksnya alat ini hanya berfungsi untuk akuisisi data dan mengirimkan ke mqtt broker (tidak melakukan data logger), data logger sdh ada unit website yang khusus menangani data logging yang datanya di ambil dari mqtt broker
8:                                 1. Akuisisi Data dari 3 buah PZEM-004T (Masing2 PZEM handle 1 Line RST to Netral (WYE/220V))
9:                                 2. Pengolahan Data
10:                                 3. Publish Data ke MQTT Broker (per topic)
11:                                 4. Handle Display OLED 0.96" (display tampilan sangat alakadarnya) dan push button untuk ganti slide page parameter 3 phase
12:                                 5. bisa management signal wi-fi dan prioritasnya (masih manual (hard-code) atur priotitas)
13:                                 6. ada fitur debug berupa kirim data random ke broker namun masih harus buka tutup komentar di baris program (dan masih cuma punya 1 jenis debug aja)
14:                                 7. masih ada beberapa kekurangan/bug sistem seperti:
15:                                   a. [asinkron data] data antar parameter yang sampai ke broker atau website tidak serentak/tidak bersamaan (ada yang terlalu cepat/terlalu lambat sepersekian detik) karena masih publish mentah per topic dan belum pakai json
16:                                   b. [blocking] saat booting esp32 terus melakukan connecting ke wifi dan tidak akan masuk display utama sampai esp32 terkoneksi ke wifi
17:                                   c. belum ada fitur kalibrasi
18:                                   d. belum ada webserver & webpage (untuk config dan kalibrasi)
19:                                   e. masih ada sistem blocking
20:                                   f. belum ada state machine & self-healing kalau error di lapangan
21:                                   g. display OLED masih polosan raw text dan susunan parameter masih berantakan
22:                                   h. Wifi Selector belum seperti pintar network management windows maupun smartphone yang mempunyai known network dan pintar mencari prioritas serta otomatis tersambung 
23: 
24: Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin (Arduino IDE v2.0 & Board Package ESP32 v2.0.14 upgrade ke v3.3.0 untuk firmware datalogger v2.0.0)
25:                                 2. PZEM-004T 100A (3 buah, masing2 handle 1 fasa dari RST ke Netral (WYE/220V)) 
26:                                 3. OLED 0.96" I2C
27:                                 4. Push Button (Normally Open, Not latching) untuk Menu OLED
28:                                 5. Built-in LED ESP32 untuk signal indikator seperti reset kWh
29: 
30: Konfigurasi Pinout Electrical : 
31: //************* PZEM ****************//
32: - PZEM Fasa R dan S dalam satu hardware serial 1
33: - PZEM Fasa T berada dalam hardware serial 2 
34: 
35: #define PZEM_RX1_PIN 4
36: #define PZEM_TX1_PIN 15
37: #define PZEM_RX2_PIN 17
38: #define PZEM_TX2_PIN 16
39: // #define PZEM_SERIAL Serial2
40: HardwareSerial PZEMSerial1(1);
41: HardwareSerial PZEMSerial2(2);
42: #define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM
43: 
44: PZEM004Tv30 pzems[NUM_PZEMS] = {
45:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
46:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
47:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
48: };
49: //************* PZEM ****************//
50: 
51: //************* OLED 0.96" I2C ****************//
52: SDA= GPIO 21
53: SCL= GPIO 22
54: //************* OLED 0.96" I2C ****************//
55: 
56: //*************** Button & LED ****************//
57: BUTTON  5
58: LED     2
59: //*************** Button & LED ****************//
60: 
61: * Board Package & Libraries yang sudah diinstall di firmware Data Logger v1.0.0 ini (install via Board & Library Manager):
62: *   Board Package   : Board Package ESP32 v2.0.14 (pada firmware data logger v2.0.0 gunakan ESP32 v3.3.0)
63: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
64: *   - PZEM004Tv30          by Jakub Mandula     v1.2.1
65: *   - Adafruit SSD1306     by Adafruit          v2.5.16
66: *   - Adafruit GFX         by Adafruit          v1.12.5
67: *   - PubSubClient         by Nick O'Leary      v2.8
68: *
69: * Board Package & Libraries yang perlu diinstall untuk pembaruan firmware Data Logger di versi v2.0.0 nanti (install via Board & Library Manager):
70: *   Board Package   : Board Package ESP32 v3.3.0
71: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
72: *   - ArduinoJson          by Benoit Blanchon   v6.21.6
73: *   - ESP Async WebServer  by ESP32Async        v3.11.0
74: *   - Async TCP            by ESP32Async        v3.4.10
75: 
76: 
77: Yang Harus diperbaiki/diupdate adalah:
78: 1. Semua bagian sistem dan fungsi harus FULL asynchronous task/non blocking system kalau perlu gunakan dua core esp32 atau gunakan FreeRTOS
79: 2. [hardcode & webpage config] sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded (hanya bisa diakses oleh programmer lewat program) dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
80: 3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker dengan topic {PREFIX_TOPIC/MQTT_TOPIC} (prefix dan topic diambil dari eeprom berdasarkan configurasi web config) (buatlah file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js)
81:   struktur json harus seperti ini
82:   ```json
83: {
84:   "device": {
85:     "id":     "3ph-logger-001",   → DEVICE_ID dari config.h
86:     "fw":     "v2.1.0",           → FW_VERSION dari config.h
87:     "uptime": 3600,               → detik sejak boot (uint32)
88:     "heap":   148432,             → free heap bytes
89:     "rssi":   -65                 → sinyal WiFi dBm
90:   },
91:   "phases": {
92:     "R": {   → valid = true
93:       "v":      "220.1",      → voltage (V), 1 desimal
94:       "i":      "12.34",      → current (A), 2 desimal
95:       "p":      "2650.0",     → active power (W)
96:       "s":      "2714.6",     → apparent power (VA)
97:       "q":      "582.1",      → reactive power (VAr) = √(S²-P²)
98:       "pf":     "0.97",       → power factor (0.0–1.0), 2 desimal
99:       "f":      "50.0",       → frequency (Hz)
100:       "e":      "15230.0",    → energy kumulatif (Wh)
101:       "status": "OK"          → "OK" / "UNDER" / "OVER" / "LOST"
102:     },
103:     "S": {   → valid = true
104:       "v":      "200.6",
105:       "i":      "12.34",
106:       "p":      "2650.0",
107:       "s":      "2714.6",
108:       "q":      "582.1",
109:       "pf":     "0.97",
110:       "f":      "50.0",
111:       "e":      "15230.0",
112:       "status": "UNDER"
113:     },
114:     "T": {   → valid = false → early return
115:       "status": "LOST" → jika salah satu atau lebih dari 1 fasa mengalami lost voltage maka esp32 hanya akan mengirimkan status "LOST" saja
116:     },
117:     "unbalance": {
118:       "value": "4.25"
119:     }
120:   },
121:   "ts": 3601234  → millis() saat publish, bukan Unix time
122: }
123: ```
124: 4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi atau mqtt dulu baru bisa masuk ke sistem (tetap buat opening screen OLED dengan Loading bar progress nya)
125:     seharusnya sistem terus berjalan meskipun:
126:     a. tidak terhubung ke wi-fi dan/atau mqtt, atau
127:     b. hanya terhubung ke wi-fi namun tidak terhubung ke mqtt server
128: 5. [Tambah] tambah status line dari OK/UNDER/LOST menjadi OK/UNDER/OVER/LOST. serta memindahkan line status threshold dari hardcoded jadi eeprom di config.h
129:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
130:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
131:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
132: 7. [Tambah] tambahkan parameter unbalanced antar phase berdasarkan standard di indonesia, masukkan fungsi di DataAcquisition.h/.cpp
133:         ALGORITMA HITUNG VOLTAGE UNBALANCE 3 PHASE
134:         Input:
135:             VR = Tegangan phase R ke netral
136:             VS = Tegangan phase S ke netral
137:             VT = Tegangan phase T ke netral
138:         Langkah 1:
139:             Hitung tegangan rata-rata
140:             VAVG = (VR + VS + VT) / 3
141:         Langkah 2:
142:             Hitung deviasi tiap phase terhadap rata-rata
143:             DEV_R = |VR - VAVG|
144:             DEV_S = |VS - VAVG|
145:             DEV_T = |VT - VAVG|
146:         Langkah 3:
147:             Tentukan deviasi terbesar
148:             MAX_DEV = nilai terbesar dari:
149:                       DEV_R, DEV_S, DEV_T
150:         Langkah 4:
151:             Hitung persentase unbalance
152:             UNBALANCE = (MAX_DEV / VAVG) × 100
153:         Langkah 5:
154:             Tampilkan hasil unbalance dalam persen (%)
155: 
156:         --------------------------------------------------
157: 
158:         CONTOH DATA:
159:             VR = 220V
160:             VS = 210V
161:             VT = 228V
162:         PROSES:
163:         1. Hitung rata-rata
164:             VAVG = (220 + 210 + 228) / 3
165:             VAVG = 219.33V
166:         2. Hitung deviasi
167:             DEV_R = |220 - 219.33|
168:             DEV_R = 0.67V
169:             DEV_S = |210 - 219.33|
170:             DEV_S = 9.33V
171:             DEV_T = |228 - 219.33|
172:             DEV_T = 8.67V
173:         3. Cari deviasi terbesar
174:             MAX_DEV = 9.33V
175:         4. Hitung unbalance
176:             UNBALANCE = (9.33 / 219.33) × 100
177:             UNBALANCE = 4.25%
178:         HASIL:
179:             Voltage Unbalance = 4.25%
180: 
181: 8. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
182:   5a. Rule set Display:
183:     - display OLED 0.96" SSD1306 sepenuhnya dikontrol dengan 1 push button
184:     - kuncinya cuma ada 2     : 1. jika push button di tekan dengan interval biasa maka itu untuk mengganti page/memindahkan kursor menu ">"
185:                                 2. jika push button di tekan dengan interval 2 detik (customable webpage config) maka itu untuk melakukan "Enter" pada menu yang di pilih atau khususnya pada Monitoring Mode adalah untuk berpindah ke Menu Mode
186:     - saat di Monitoring Mode : 1. tekan biasa push button untuk slide page ke page berikutnya
187:                                 2. tekan selama 2 detik untuk masuk ke "Menu Mode" (berlaku di monitoring page manapun untuk berpindah ke menu mode)
188:     - saat di Menu Mode       : 1. push button sebagai kursor untuk memilih menu ke bawah, dengan cara menekan biasa agar kursor berpindah ke menu dibawahnya
189:                                 2. jika kursor sudah pada menu paling bawah dan button di tekan lagi, maka kursor akan kembali ke menu paling atas lagi (berlaku juga untuk menu confirm "YES" or "NO" yang secara horizontal kanan-kiri)
190:                                 3. untuk masuk ke menu pilihan, maka button harus ditekan selama 2 detik sebagai tombol "Enter" nya
191:                                 4. setiap menu harus ada "Back" untuk keluar dari menu dengan cara menekan yang sama seperti masuk ke dalam menu (2 detik)
192:                                 5. setiap menu harus ada "Back" kecuali "Back to Monitoring", jika itu ditekan selama 2 detik maka akan kembali ke "Monitoring Mode"
193:  *    1. Monitoring Mode:
194:  *    Page 1 [Monitoring Mode]:
195:  *   ┌────────────────────────────┐
196:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
197:  *   │   R         S         T    │  ← Phase Name         
198:  *   │  220V      220V      220V  │  ← Voltage Line to Netral   
199:  *   │  100A      100A      100A  │  ← Current per phase   (0-100)
200:  *   │   OK        OK        OK   │  ← Line_Status   (LOST/UNDER/OK)
201:  *   │  Unbalance = 2.5%          │  ← Unbalance param
202:  *   └────────────────────────────┘
203:  *
204:  *   ┌────────────────────────────┐
205:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
206:  *   │   R         S         T    │  ← Phase Name         
207:  *   │  220V      220V        0V  │  ← Voltage Line to Netral   
208:  *   │  100A      100A      100A  │  ← Current per phase   (0-100)
209:  *   │   OK       UNDER     LOST  │  ← Line_Status   (LOST/UNDER/OK), muncul dengan efek beeping/kedip untuk warning ketika status LOST / UNDER / OVER
210:  *   │  Unbalance = 5.45%   [!]   │  ← Unbalance param ketika hit threshold max unbalance di web config, [!] icon ini juga akan muncul dengan efek beeping/kedip untuk warning
211:  *   └────────────────────────────┘
212:  *
213:  *    Page 2 [Monitoring Mode]:
214:  *   ┌────────────────────────────┐
215:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
216:  *   │   R         S         T    │  ← Phase Name         
217:  *   │  60Hz      60Hz      60Hz  │  ← Frequency     
218:  *   │ 100VA     1.2kVA    10kVA  │  ← Apparent Power (L to N) (jika dibawah 1000 maka satuannya "VA", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVA")  
219:  *   │ 100VAr    2.0kVAr   13kVAr │  ← Reactive Power (L to N) (jika dibawah 1000 maka satuannya "VAR", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVAr")         
220:  *   └────────────────────────────┘
221:  *
222:  *    Page 3 [Monitoring Mode]:
223:  *   ┌────────────────────────────┐
224:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
225:  *   │   R         S         T    │  ← Phase Name         
226:  *   │ 0.8PF     1.0PF     0.7PF  │  ← Power Factor (0.0 - 1.0)     
227:  *   │ 100W       12kW     2.2kW  │  ← Active Power (jika dibawah 1000 maka satuannya "W", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kW")   
228:  *   │ 300Wh     1.4kWh     30kWh │  ← Total Active Energy (jika dibawah 1000 maka satuannya "Wh", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kWh")          
229:  *   └────────────────────────────┘
230:  *
231:  *    2. Menu Mode:
232:  *    Page 1 [Menu Mode]: (jika menu highlighted maka ada indentasi maju 1 atau 2 karakter untuk menunjukkan posisi kursor pada OLED di menu mana)
233:  *   ┌────────────────────────────┐
234:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
235:  *   │  > Config                  │  ← Config Menu         
236:  *   │  > Reset kWh               │  ← Reset kWh semua fasa jadi 0kWh
237:  *   │  > Reboot Device           │  ← Reboot ESP32 lewat program setara dengan tekan tombol RST di board
238:  *   │  > Reset to Factory        │  ← Reset to Factory (reset configurasi yang ada di EEPROM, sama saja dengan reset EEPROM melalui web config, bedanya ini lewat menu OLED, yang satunya lewat web config)
239:  *   │  > Back to Monitoring      │  ← Kembali ke Page Monitoring Mode (jika kursor di "Back to Monitoring" dan tombol ditekan selama 2 detik maka akan kembali ke Page 1 Monitoring Mode )
240:  *   └────────────────────────────┘
241:  *
242:  *    Page 2 [Menu Mode > Config]:
243:  *    Note: Di menu Config ini hanya difungsikan untuk user mengaktifkan webserver, selebihnya config akan dilakukan melalui webpage dan akan di simpan ke dalam EEPROM ESP32
244:  *   ┌────────────────────────────┐
245:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
246:  *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
247:  *   │  PW  : 12345678            │  ← Password dari SSID ESP32
248:  *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
249:  *   │  > Active         Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
250:  *   └────────────────────────────┘
251:  *   ┌────────────────────────────┐
252:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
253:  *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
254:  *   │  PW  : 12345678            │  ← Password dari SSID ESP32
255:  *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
256:  *   │  Inactive       > Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
257:  *   └────────────────────────────┘
258:  *
259:  *    Page 3 [Menu Mode > Reset kWh]:
260:  *    Note: Di menu Reset kWh ini hanya difungsikan untuk user melakukan Reset Total kWh pada semua module PZEM-004T
261:  *   ┌────────────────────────────┐
262:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
263:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
264:  *   │        Reset kWh?          │  
265:  *   │                            │  
266:  *   │  > Reset          Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
267:  *   └────────────────────────────┘
268:  *   ┌────────────────────────────┐
269:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
270:  *   │                            │  ← Teks Konfirmasi         
271:  *   │     Reset kWh Berhasil     │  ← Teks Output Reset (Reset kWh Berhasil/Reset kWh Gagal) tampil selama 2 detik lalu otomatis kembali ke page Menu Mode
272:  *   │                            │  
273:  *   │                            │  
274:  *   └────────────────────────────┘
275:  *   ┌────────────────────────────┐
276:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
277:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
278:  *   │        Reset kWh?          │  
279:  *   │                            │  
280:  *   │   Reset         > Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
281:  *   └────────────────────────────┘
282:  *
283:  *    Page 4 [Menu Mode > Reboot Device]:
284:  *    Note: Di menu Config ini hanya difungsikan untuk user melakukan rebooting device ESP32
285:  *   ┌────────────────────────────┐
286:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
287:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
288:  *   │       Reboot Device?       │  
289:  *   │                            │  
290:  *   │  > Reboot         Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
291:  *   └────────────────────────────┘
292:  *   ┌────────────────────────────┐
293:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
294:  *   │                            │      
295:  *   │     Device akan Reboot     │  ← Teks Countdown 3 detik sebelum system otomatis reboot (dalam 3 detik, dalam 2 detik, dalam 1 detik, dalam 0 detik)
296:  *   │       dalam 3 detik        │  
297:  *   │                            │  
298:  *   └────────────────────────────┘
299:  *   ┌────────────────────────────┐
300:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
301:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
302:  *   │       Reboot Device?       │  
303:  *   │                            │  
304:  *   │   Reboot        > Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
305:  *   └────────────────────────────┘
306:  *
307:  *    Page 5 [Menu Mode > Reset to Factory]:
308:  *    Note: -
309:  *   ┌────────────────────────────┐
310:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
311:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
312:  *   │        Reset Device?       │  
313:  *   │                            │  
314:  *   │  > Reset          Back     │  ← (Confirm Choise) jika menu "Reset" ditekan selama 2 detik maka esp32 akan melakukan reset EEPROM dan menampilkan teks output jika berhasil selama 2 detik, kemudian otomatis akan melakukan reboot esp32 by system, namun jika "tidak" ditekan selama 2 detik maka akan kembali ke Menu Config
315:  *   └────────────────────────────┘
316:  *   ┌────────────────────────────┐
317:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
318:  *   │                            │      
319:  *   │      Reset to Factory      │  ← Teks Output Reset (Reset EEPROM/preferences Berhasil/Reset EEPROM Gagal) tampil selama 2 detik lalu otomatis melakukan reboot esp32 by system
320:  *   │          berhasil          │  
321:  *   │                            │  
322:  *   └────────────────────────────┘
323: 
324: 9. [HTTP Web-server & Access Point] 
325:   a. buat webpage config yang bisa diakses lewat smartphone atau laptop yang terhubung melalui IP Address yang ESP32 keluarkan saat Config pada Menu Mode "Active"
326:   b. jika saat access point aktif dan ESP32 sedang terhubung ke jaringan yang ada internetnya, maka teruskan internet itu ke access point ESP32 agar Laptop/smartphone yang sedang melakukan config/terhubung ke access point esp32 mendapatkan jaringan internet untuk tetap online (jika tidak, Laptop/smartphone tidak apa-apa dalam kondisi offline) 
327:   c. semua parameter config yang ada di webpage tersimpan di dalam EEPROM ESP32 (jadi device baru harus config terlebih dahulu untuk terkoneksi ke internet/MQTT)
328:   d. buatkan login page sebelum masuk ke main page config, dengan user dan pass yang hanya bisa diubah di hardcode config.h, dengan default USERNAME = ADMIN, PASS = 18273645
329:   d. page 1 [Config] HTML + CSS:
330:     - Manage Known Network   (Big/Medium Card with Link to "Manage Known Network")
331:     - MQTT Connection Config (Big/Medium Card with Link to "MQTT Connection Config")
332:     - PZEM-004T Calibration  (Big/Medium Card with Link to "PZEM-004T Calibration")
333: 
334:     {HARDRESET EEPROM}
335: 
336:   e. page 1 [Config > Manage Known Network] HTML + CSS:
337:       tampilkan semua jaringan yang diketahui (EEPROM) dalam tabel dengan kolom dan terdapat text box untuk menambahkan jaringan baru di bawah table dan tombol add network untuk menyimpan jaringan baru ke EEPROM
338:               Manage Known Network
339: 
340:       | No. | SSID | PASS     |   Aksi   |
341:       | 1.  | ABC  | 18273645 | {Delete} |
342:       | 2.  | esp  | 18273645 | {Delete} |
343: 
344:       Add Network
345:       [ SSID....       ]
346:       [ PASS....       ]
347: 
348:       {ADD NETWORK}
349: 
350:   e. page 2 [Config > MQTT Connection Config] HTML + CSS:
351:       tampilkan data koneksi MQTT dalam bentuk teks dari EEPROM
352:       MQTT Server = broker.avisha.id
353:       MQTT_PORT   = 1883               // Non-SSL port
354:       MQTTS_PORT  = 8884               // SSL port
355:       WS_PORT     = 8083               // Non-SSL port
356:       WSS_PORT    = 8084               // SSL port
357:       MQTT_USER   = "lutpiii"          // MQTT Username
358:       MQTT_PASS   = "lutpiiiMQTT"      // MQTT Password
359:       PREFIX_TOPIC= "lutpiii"          // Topic prefix (wajib)
360:       MQTT_TOPIC  = "telemetry"        // Topic (wajib)
361:       USE SSL     = TRUE
362:       USE WS      = FALSE              
363:       
364: 
365:       {EDIT}        {SAVE CONFIG}
366: 
367:       // jika WS=0 & SSL=0 gunakan MQTT
368:       // jika WS=0 & SSL=1 gunakan MQTTS
369:       // jika WS=1 & SSL=0 gunakan WS
370:       // jika WS=1 & SSL=1 gunakan WSS
371: 
372:   f. page 3 [Config > PZEM-004T Calibration] HTML + CSS:
373:       buat kalibrasi dengan model offset (-/+) untuk parameter Voltage & Current masing-masing Fasa dengan presisi 0.000 perubahan, threshold LOST, UNDER, OVER dan unbalance. dan tampilkan juga offset yang tersimpan
374:       dengan model text box
375: 
376:       {SAVE CALIBRATION}
377: 
378:   g. gunakan saja template yang sudah saya buat ini, namun bagusin lagi homa page bagian text link nya, jgn pakai text link lagi, gunakan card medium atau besar ke bawah saja susunannya, jumlah card = jumlah menu
379: String WebServerManager::_htmlHead(const char* title) {
380: return String(R"(<!DOCTYPE html><html><head>
381: <meta charset="UTF-8">
382: <meta name="viewport" content="width=device-width,initial-scale=1">
383: <meta name="color-scheme" content="light">
384: <title>)") + title + (R"(</title>
385: <style>
386: :root{
387:   --bg:#f7f8fa;
388:   --surface:#ffffff;
389:   --surface-2:#f3f7f4;
390:   --text:#1f2937;
391:   --muted:#6b7280;
392:   --line:#dbe3dc;
393:   --primary:#16a34a;
394:   --primary-hover:#15803d;
395:   --primary-soft:#dcfce7;
396:   --danger:#dc2626;
397:   --danger-hover:#b91c1c;
398:   --shadow:0 6px 18px rgba(15,23,42,.06);
399:   --radius:10px;
400: }
401: 
402: *{box-sizing:border-box}
403: html{-webkit-text-size-adjust:100%}
404: body{
405:   font-family:system-ui,-apple-system,"Segoe UI",Roboto,Arial,sans-serif;
406:   max-width:600px;
407:   margin:30px auto;
408:   padding:0 16px;
409:   background:var(--bg);
410:   color:var(--text);
411:   line-height:1.5;
412: }
413: 
414: h1{
415:   color:var(--text);
416:   font-size:1.4rem;
417:   margin:0 0 12px;
418:   letter-spacing:-0.02em;
419: }
420: 
421: h2{
422:   margin:0 0 10px;
423:   font-size:1rem;
424:   color:var(--primary);
425:   background:var(--primary-soft);
426:   padding:8px 12px;
427:   border-radius:8px;
428:   font-weight:700;
429: }
430: 
431: a{
432:   color:var(--primary);
433:   text-decoration:none;
434: }
435: a:hover{text-decoration:underline}
436: 
437: nav{
438:   margin:12px 0;
439:   display:flex;
440:   flex-wrap:wrap;
441:   gap:8px;
442: }
443: nav a{
444:   display:inline-flex;
445:   align-items:center;
446:   justify-content:center;
447:   padding:8px 12px;
448:   border:1px solid var(--line);
449:   border-radius:999px;
450:   background:var(--surface);
451:   color:var(--text);
452:   text-decoration:none;
453:   box-shadow:0 1px 2px rgba(15,23,42,.04);
454: }
455: nav a:hover{
456:   background:var(--surface-2);
457:   text-decoration:none;
458: }
459: 
460: hr{
461:   border:none;
462:   height:1px;
463:   background:var(--line);
464:   margin:14px 0;
465: }
466: 
467: .card{
468:   background:var(--surface);
469:   border:1px solid var(--line);
470:   border-left:4px solid var(--primary);
471:   border-radius:var(--radius);
472:   padding:16px;
473:   margin:12px 0;
474:   box-shadow:var(--shadow);
475: }
476: 
477: label{
478:   display:block;
479:   font-size:.85rem;
480:   color:var(--muted);
481:   margin:10px 0 6px;
482: }
483: 
484: .hint{
485:   font-size:.78rem;
486:   color:var(--muted);
487:   margin:-4px 0 8px;
488: }
489: 
490: input[type=text],
491: input[type=password],
492: input[type=number],
493: textarea{
494:   width:100%;
495:   box-sizing:border-box;
496:   padding:9px 10px;
497:   background:#fff;
498:   color:var(--text);
499:   border:1px solid var(--line);
500:   border-radius:8px;
501:   margin:4px 0 10px;
502:   font:inherit;
503:   outline:none;
504: }
505: 
506: input[type=text]:focus,
507: input[type=password]:focus,
508: input[type=number]:focus,
509: textarea:focus{
510:   border-color:rgba(22,163,74,.55);
511:   box-shadow:0 0 0 3px rgba(22,163,74,.12);
512: }
513: 
514: textarea{
515:   resize:vertical;
516:   min-height:120px;
517: }
518: 
519: button,
520: .btn,
521: .danger{
522:   background:var(--primary);
523:   color:#fff;
524:   border:none;
525:   padding:10px 16px;
526:   border-radius:8px;
527:   cursor:pointer;
528:   font-size:.92rem;
529:   font-weight:600;
530:   transition:background .15s ease, transform .05s ease;
531: }
532: 
533: button:hover,
534: .btn:hover{
535:   background:var(--primary-hover);
536: }
537: 
538: button:active,
539: .btn:active,
540: .danger:active{
541:   transform:translateY(1px);
542: }
543: 
544: .btn-del{
545:   background:#6b7280;
546: }
547: .btn-del:hover{
548:   background:#4b5563;
549: }
550: 
551: .danger{
552:   background:var(--danger);
553: }
554: .danger:hover{
555:   background:var(--danger-hover);
556: }
557: 
558: table{
559:   width:100%;
560:   border-collapse:collapse;
561:   margin:8px 0;
562: }
563: th,td{
564:   text-align:left;
565:   padding:8px;
566:   border-bottom:1px solid var(--line);
567:   vertical-align:top;
568: }
569: th{
570:   color:var(--primary);
571:   background:#f9fffb;
572: }
573: 
574: input[type=checkbox]{
575:   accent-color:var(--primary);
576:   transform:translateY(1px);
577: }
578: 
579: form{margin:0}
580: 
581: @media (max-width:640px){
582:   body{
583:     margin:18px auto;
584:     padding:0 12px;
585:   }
586:   nav a{
587:     width:calc(50% - 4px);
588:   }
589: }
590: </style></head><body>
591: <h1>&#9889; 3-Phase Data Logger</h1>
592: <nav>
593: <a href="/">Home</a>
594: <a href="/wifi">WiFi</a>
595: <a href="/mqtt">MQTT</a>
596: <a href="/calibration">Calibration</a>
597: </nav>
598: <hr>
599: )");
600: }
601: 
602: String WebServerManager::_htmlFoot() {
603:     return R"(<hr><p style="font-size:.75rem;color:#6b7280;text-align:center">Muhammad Lutfi Nur Anendi &bull; )" FW_VERSION R"(</p></body></html>)";
604: }
605: 
606: // ─── Page: Index ──────────────────────────────────────────
607: 
608: String WebServerManager::_pageIndex(StorageManager& s) {
609:     String h = _htmlHead("Config");
610:     h += R"HTML(<div class="card">
611: <h2>Configuration Pages</h2>
612: <p><a href="/wifi">&#128246; Manage Known Networks</a></p>
613: <p><a href="/mqtt">&#128272; MQTT Connection Config</a></p>
614: <p><a href="/calibration">&#9881; PZEM-004T Calibration &amp; Thresholds</a></p>
615: </div>
616: <div class="card">
617: <h2>&#9888; Danger Zone</h2>
618: <form method="POST" action="/factoryreset"
619:   onsubmit="return confirm('ERASE ALL settings? This cannot be undone.')">
620: <button class="danger" type="submit">Hard Reset EEPROM</button>
621: </form>
622: </div>
623: )HTML";
624:     h += _htmlFoot();
625:     return h;
626: }
627: 
628: // ─── Page: WiFi ───────────────────────────────────────────
629: 
630: String WebServerManager::_pageWifi(StorageManager& s) {
631:     String h = _htmlHead("Manage WiFi");
632:     h += "<h2>Known Networks</h2><div class='card'>";
633:     h += "<table><tr><th>No.</th><th>SSID</th><th>Action</th></tr>";
634: 
635:     int n = s.getWifiCount();
636:     for (int i = 0; i < n; i++) {
637:         WiFiEntry e = s.getWifiEntry(i);
638:         h += "<tr><td>" + String(i + 1) + "</td><td>" + String(e.ssid) + "</td><td>";
639:         h += "<form method='POST' action='/wifi/delete' style='display:inline'>";
640:         h += "<input type='hidden' name='idx' value='" + String(i) + "'>";
641:         h += "<button class='btn btn-del' type='submit'>Delete</button></form>";
642:         h += "</td></tr>";
643:     }
644:     if (n == 0) h += "<tr><td colspan='3'>No networks stored.</td></tr>";
645:     h += "</table></div>";
646: 
647:     h += R"(<div class="card"><h2>Add Network</h2>
648: <form method="POST" action="/wifi/add">
649: <label>SSID</label>
650: <input type="text" name="ssid" placeholder="Network name" required>
651: <label>Password</label>
652: <input type="password" name="pass" placeholder="Password">
653: <button type="submit">Add Network</button>
654: </form></div>)";
655: 
656:     h += _htmlFoot();
657:     return h;
658: }
659: 
660: // ─── Page: MQTT ───────────────────────────────────────────
661: 
662: String WebServerManager::_pageMQTT(StorageManager& s) {
663:     MQTTConfig cfg = s.getMQTTConfig();
664:     String h = _htmlHead("MQTT Config");
665:     h += "<h2>MQTT Broker</h2><div class='card'>";
666:     h += "<form method='POST' action='/mqtt'>";
667: 
668:     auto field = [&](const char* label, const char* name, const char* val, const char* type = "text") {
669:         h += String("<label>") + label + "</label>";
670:         h += String("<input type='") + type + "' name='" + name + "' value='" + val + "'>";
671:     };
672:     auto numField = [&](const char* label, const char* name, int val) {
673:         h += String("<label>") + label + "</label>";
674:         h += String("<input type='number' name='") + name + "' value='" + val + "'>";
675:     };
676:     auto checkbox = [&](const char* label, const char* name, bool checked) {
677:         h += String("<label><input type='checkbox' name='") + name + "' " +
678:              (checked ? "checked" : "") + "> " + label + "</label><br>";
679:     };
680: 
681:     field("MQTT Server",       "server",   cfg.server);
682:     numField("Port (non-SSL)", "port",     cfg.port);
683:     numField("Port (SSL)",     "sslport",  cfg.sslPort);
684:     field("Username",          "user",     cfg.user);
685:     field("Password",          "pass",     cfg.pass, "password");
686:     field("Topic Prefix",      "topic",    cfg.topic);
687:     checkbox("Use SSL",        "usessl",   cfg.useSSL);
688:     checkbox("Use WebSocket",  "usews",    cfg.useWS);
689: 
690:     h += "<label>CA Certificate (PEM)</label>";
691:     h += String("<textarea name='cacert' rows='5' placeholder='-----BEGIN CERTIFICATE-----...'>")
692:          + cfg.caCert + "</textarea>";
693:     h += "<button type='submit'>Save Config</button></form></div>";
694:     h += _htmlFoot();
695:     return h;
696: }
697: 
698: // ─── Page: Calibration + Thresholds ──────────────────────
699: 
700: String WebServerManager::_pageCalibration(StorageManager& s) {
701:     CalibrationConfig cal = s.getCalibration();
702:     ThresholdConfig   thr = s.getThresholds();
703: 
704:     String h = _htmlHead("Calibration & Thresholds");
705:     h += "<form method='POST' action='/calibration'>";
706: 
707:     h += "<h2>PZEM-004T Calibration</h2>";
708:     h += "<p class='hint'>Offset is additive: Reading = Raw + Offset. Use negative to subtract.</p>";
709:     h += "<div class='card'>";
710: 
711:     const char* phases[3] = {"R", "S", "T"};
712:     for (int i = 0; i < 3; i++) {
713:         h += String("<h2>Phase ") + phases[i] + "</h2>";
714:         h += String("<label>Voltage Offset (V) &nbsp; Current stored: ") +
715:              String(cal.voltageOffset[i], 3) + " V</label>";
716:         h += String("<input type='number' step='0.001' name='voff_") +
717:              phases[i] + "' value='" + String(cal.voltageOffset[i], 3) + "'>";
718:         h += String("<label>Current Offset (A) &nbsp; Current stored: ") +
719:              String(cal.currentOffset[i], 3) + " A</label>";
720:         h += String("<input type='number' step='0.001' name='ioff_") +
721:              phases[i] + "' value='" + String(cal.currentOffset[i], 3) + "'>";
722:     }
723:     h += "</div>";
724: 
725:     h += "<h2>Line Status Thresholds</h2>";
726:     h += "<p class='hint'>Thresholds used to determine line status: LOST / UNDER / OK / OVER.</p>";
727:     h += "<div class='card'>";
728: 
729:     h += "<label>LOST threshold (V) &ndash; voltage below this &rarr; LOST</label>";
730:     h += "<p class='hint'>Current: " + String(thr.voltageLost, 1) + " V</p>";
731:     h += "<input type='number' step='0.1' name='th_vlost' value='" + String(thr.voltageLost, 1) + "'>";
732: 
733:     h += "<label>UNDER threshold (V) &ndash; voltage below this &rarr; UNDER (above LOST)</label>";
734:     h += "<p class='hint'>Current: " + String(thr.voltageUnder, 1) + " V</p>";
735:     h += "<input type='number' step='0.1' name='th_vunder' value='" + String(thr.voltageUnder, 1) + "'>";
736: 
737:     h += "<label>OVER threshold (V) &ndash; voltage above this &rarr; OVER</label>";
738:     h += "<p class='hint'>Current: " + String(thr.voltageOver, 1) + " V</p>";
739:     h += "<input type='number' step='0.1' name='th_vover' value='" + String(thr.voltageOver, 1) + "'>";
740:     h += "</div>";
741: 
742:     h += "<h2>Voltage Unbalance Threshold</h2>";
743:     h += "<p class='hint'>Berdasarkan standar SNI/SPLN Indonesia. Jika unbalance &gt; threshold unbalance maka OLED menampilkan peringatan [!]</p>";
744:     h += "<br>";
745:     h += "<p class='hint'>3-phase voltage unbalance computed using NEMA method.</p>";
746:     h += "<div class='card'>";
747:     h += "<label>Max Unbalance (%) &ndash; above this &rarr; warning [!] on OLED &amp; MQTT</label>";
748:     h += "<p class='hint'>Current: " + String(thr.unbalanceMax, 2) + " %</p>";
749:     h += "<input type='number' step='0.01' name='th_unbal' value='" + String(thr.unbalanceMax, 2) + "'>";
750:     h += "</div>";
751: 
752:     h += "<button type='submit'>Save Calibration &amp; Thresholds</button>";
753:     h += "</form>";
754:     h += _htmlFoot();
755:     return h;
756: }
757:  
758: 
759: 6. [IMPORTANT!!!] sistem ini memiliki kalibrasi untuk TEGANGAN & ARUS PZEM, maka harus ada post processing untuk parameter lainnya seperti VA, VAr, W, Wh, dan pf (kecuali Hz) yang mana nilainya bergantung/yang nilainya berubah berdasarkan parameter tegangan atau arus,
760:     yang mana jika hanya melakukan kalibrasi pada V / A tetapi parameter lain tidak terpengaruh maka data akan salah total seperti VA, VAr, W, Wh, dan pf (kecuali Hz)
761:     jadi parameter lain harus terpengaruh/ikut terproses setelah tegangan atau arus dilakukan kalibrasi ataupun tidak, jadi semua parameter kecuali frequency harus berubah sesuai offset kalibarsi tegangan atau Arus
762: 
763: 6. [Tambah] di web config, tambahkan config untuk nilai LOST, UNDER, OVER voltage, serta persentase max unbalance phase di halaman PZEM Calibration
764:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
765:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
766:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
767:     #define VOLTAGE_UNBALANCE_MAX    3.0f  // Above this → OVER UNBALANCE (satuan persen)
768: 
769: 7. LED Signal 
770:   - sebagai indikator terhadap aksi-aksi dari fungsi/module program seperti Booting, Reset kWh, active/deactive webserver, dll.
771: 
772: 8. [Tambah] tambahkan fitur auto self-healing ketika esp32 mengalami freeze/hang/stuck/runningout memory atau sejenisnya atau sesuatu yang membuat esp32 malfungsi sistem, maka harus ditambahkan self healing dengan metode yang paling cocok untuk sistem ini
773: 
774: 8. Struktur File Project gunakan OOP dan seperti ROS2 (Robot Operating System 2) dengan dokumentasi-dokumentasi melimpahnya dan program mudah dipahami, mudah di konfigurasi, dan bisa scalable saat penambahan fitur/sensor baru
775:   (jumlah file menyesuaikan, ini hanya sebagai contoh):
776:   - main.ino
777:   - config.h
778:   - data_acquisition.h/.cpp
779:   - calibration.h/.cpp
780:   - display.h/cpp
781:   - menu.h/cpp
782:   - webserver.h/.cpp
783:   - etc
784: 

(End of file - total 784 lines)
</content>
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 9.4s)

_Thinking:_

Let me now look at the remaining source files to understand the current code structure better.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/DisplayOLED.h"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/DisplayOLED.h</path>
<type>file</type>
<content>
1: #pragma once
2: #include <Arduino.h>
3: #include <Wire.h>
4: #include <Adafruit_GFX.h>
5: #include <Adafruit_SSD1306.h>
6: 
7: #define OLED_WIDTH 128
8: #define OLED_HEIGHT 64
9: #define OLED_ADDR 0x3C
10: 
11: class DisplayOLED {
12: public:
13:     DisplayOLED(uint8_t address = OLED_ADDR);
14:     void begin();
15:     // update dipanggil tiap loop agar non-blocking
16:     void update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]);
17: 
18:     // Kontrol page dari luar (button)
19:     void setPage(uint8_t p);
20:     void enableAutoScroll(bool en); // kalau false, page hanya diganti oleh setPage
21:     uint8_t getPage() const { return page; }
22: 
23: private:
24:     Adafruit_SSD1306 display;
25:     uint8_t page;
26:     unsigned long lastUpdate;
27:     const uint16_t interval = 800; // refresh tiap 800ms
28: 
29:     bool autoScroll;
30: 
31:     void drawPage1(float v[3], float i[3]);
32:     void drawPage2(float p[3], float pf[3], float f[3]);
33:     void drawPage3(float e[3], int status[3]);
34:     const char* statusToText(int st);
35: };

(End of file - total 35 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/WifiSelector.h"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/WifiSelector.h</path>
<type>file</type>
<content>
1: #ifndef WIFI_SELECTOR_H
2: #define WIFI_SELECTOR_H
3: 
4: #include <WiFi.h>
5: #include "credentials.h"
6: 
7: class WiFiSelector {
8: public:
9:     WiFiSelector(unsigned long timeout = 10000);  // default 10 detik
10:     bool connectBestNetwork();                     // koneksi ke WiFi terbaik
11:     bool isConnected();                            // cek koneksi
12:     void printStatus();                            // tampilkan info koneksi
13: 
14: private:
15:     unsigned long _timeout;
16:     bool connectToNetwork(const WiFiCredential& cred);
17: };
18: 
19: #endif

(End of file - total 19 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/button.h"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/button.h</path>
<type>file</type>
<content>
1: #ifndef BUTTON_H
2: #define BUTTON_H
3: 
4: #include <Arduino.h>
5: 
6: class Button {
7: public:
8:     Button(uint8_t pin, unsigned long longPressTime = 5000, unsigned long debounceTime = 50);
9: 
10:     void update();
11:     bool isShortPressed();
12:     bool isLongPressed();
13: 
14: private:
15:     uint8_t _pin;
16:     unsigned long _longPressTime;
17:     unsigned long _debounceTime;
18: 
19:     bool _state;
20:     bool _lastState;
21:     unsigned long _pressStart;
22:     bool _longPressHandled;
23:     bool _shortPressDetected;
24:     bool _longPressDetected;
25: };
26: 
27: #endif

(End of file - total 27 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/ResetEnergy.h"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/ResetEnergy.h</path>
<type>file</type>
<content>
1: #pragma once
2: #include <Arduino.h>
3: #include <PZEM004Tv30.h>
4: 
5: class ResetEnergy {
6: public:
7:     ResetEnergy(uint8_t buttonPin, uint8_t ledPin);
8:     void begin();
9:     // dipanggil rutin di loop untuk handle long press flow (sudah kamu punya sebelumnya)
10:     void handle(PZEM004Tv30 pzems[], int count);
11: 
12:     // fungsi publik baru yang bisa dipanggil kapanpun untuk reset semua PZEM
13:     // fungsi ini blocking ringan: mengirim perintah satu-per-satu dengan delay kecil
14:     void resetAll(PZEM004Tv30 pzems[], int count);
15: 
16: private:
17:     uint8_t _buttonPin;
18:     uint8_t _ledPin;
19: 
20:     // internal helper
21:     void blinkLed(int times, unsigned int onMs = 150, unsigned int offMs = 150);
22: };

(End of file - total 22 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.cpp"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.cpp</path>
<type>file</type>
<content>
1: /******************************************************
2:  * Project   : ESP32 MQTTs Publisher
3:  * File      : credentials.h
4:  * Author    : Muhammad Lutfi Nur Anendi
5:  * Created   : 5 August 2025
6:  * Last Edit : 23 October 2025
7:  *
8:  * Description:
9:  *   Configuration file for WiFi, MQTT, and SSL credentials.
10:  *   Rename this file from `credentials_example.cpp` → `credentials.cpp`
11:  *   before compiling the project.
12:  *
13:  * IMPORTANT:
14:  *   - DO NOT commit this file to any public repository.
15:  *   - Keep your credentials private.
16:  *   - Modify only the fields below as required.
17:  *
18:  * MQTT Notes:
19:  *   - MQTT_PORT  = 1883 for non-SSL
20:  *   - MQTTS_PORT = 8883 for SSL (secure)
21:  *   - MQTT_TOPIC example: "user_topic/#"
22:  *
23:  * SSL Notes:
24:  *   - CA_CERT should contain your Root Certificate between
25:  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
26:  *
27:  * Developer Info:
28:  *   GitHub   : https://github.com/lutfi-rpi5
29:  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
30:  *   Instagram: https://instagram.com/lutfiianendi
31:  *
32:  * Revision History:
33:  *   v1.0  Initial release with MQTT & SSL credentials
34:  *   v1.1  Added developer info and usage guide
35:  ******************************************************/
36: 
37: #include "credentials.h"
38: 
39: // ---------- WiFi Configuration ----------
40: const char* WIFI_SSID1 = "PTBA-LOGGER";
41: const char* WIFI_PASS1 = "katekpasswordnyo";
42: 
43: const char* WIFI_SSID2 = "FLA 2.4G";
44: const char* WIFI_PASS2 = "20092009";
45: 
46: const char* WIFI_SSID3 = "PTBA-KERTAPATI";
47: const char* WIFI_PASS3 = "Spirit7jutaton";
48: 
49: const char* WIFI_SSID4 = "rpi-5";
50: const char* WIFI_PASS4 = "getalifebro";
51: 
52: // ---------- WiFi Networks List ----------
53: WiFiCredential wifiList[] = {
54:   {WIFI_SSID2, WIFI_PASS2, 0},   // Highest priority
55:   {WIFI_SSID4, WIFI_PASS4, 1},
56:   {WIFI_SSID1, WIFI_PASS1, 2},
57:   {WIFI_SSID3, WIFI_PASS3, 3}
58: };
59: const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);
60: 
61: // ---------- MQTT Broker Configuration ----------
62: const char* MQTT_SERVER = "broker.avisha.id"; // Broker address (Avisha Broker) 103.127.97.247
63: const int   MQTT_PORT   = 1883;               // Non-SSL port
64: const int   MQTTS_PORT  = 8884;               // SSL port
65: const int   WS_PORT     = 8083;               // Non-SSL port
66: const int   WSS_PORT    = 8084;               // SSL port
67: const char* MQTT_USER   = "lutpiii";          // MQTT Username
68: const char* MQTT_PASS   = "lutpiiiMQTT";      // MQTT Password
69: const char* MQTT_TOPIC  = "lutpiii/";         // Topic prefix (optional)
70: 
71: // ---------- Optional API Key ----------
72: const char* API_KEY = "";  // For future REST API integration
73: 
74: // ---------- Root CA Certificate ----------
75: // const char* CA_CERT = 
76: // "-----BEGIN CERTIFICATE-----\n"
77: // // Drop your CA certificate content here
78: // "-----END CERTIFICATE-----\n";
79: 
80: const char *CA_CERT = R"EOF(
81: -----BEGIN CERTIFICATE-----
82: MIIDtTCCAp2gAwIBAgIUOKMAqMXFCssUwLZtEx+8/dj+d/AwDQYJKoZIhvcNAQEL
83: BQAwajELMAkGA1UEBhMCSUQxEDAOBgNVBAgMB0pha2FydGExEDAOBgNVBAcMB0ph
84: a2FydGExDTALBgNVBAoMBEVNUVgxDTALBgNVBAsMBE1RVFQxGTAXBgNVBAMMEGJy
85: b2tlci5hdmlzaGEuaWQwHhcNMjUxMTAyMTEzODM2WhcNMjYxMTAyMTEzODM2WjBq
86: MQswCQYDVQQGEwJJRDEQMA4GA1UECAwHSmFrYXJ0YTEQMA4GA1UEBwwHSmFrYXJ0
87: YTENMAsGA1UECgwERU1RWDENMAsGA1UECwwETVFUVDEZMBcGA1UEAwwQYnJva2Vy
88: LmF2aXNoYS5pZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMAvKNx4
89: /EBrDB/vhjL6L+hnoRuM623QFqkmje+I7YjsL0ehNTBnTCTbI0giflQlx0lE1OUC
90: r3HbahOFovFJGbMbLF3JgKvJ7rtmfEhaURyuSNCAVJRxmnEFt+OgB6SntSQaJ0zP
91: q5DT2EapWdiwbFkYsDTO6kyRtkBReZVc4rhTCaHFJnxEab3DbzVzteC2g8GSv0NX
92: clO3+6yi9bRh0uH0u3rfux4I9r4GMYF9vdC7NDDaVFeHIsQRXKW1dh4KajdMnJvl
93: K25whh4HAEryZ5JJsyCviWcmjZhvPW/WEha1ORPjpcw3dbCSa40zSQVoFKgJAPvJ
94: mcsCKk6TUT8ENiUCAwEAAaNTMFEwHQYDVR0OBBYEFHDNv7W0hpVlgFt82uqmGU0O
95: AnPuMB8GA1UdIwQYMBaAFHDNv7W0hpVlgFt82uqmGU0OAnPuMA8GA1UdEwEB/wQF
96: MAMBAf8wDQYJKoZIhvcNAQELBQADggEBAIOK9N+TF7t/2bsbX3RlFzKG6Nn+qkA4
97: KNXEbfHhgKN/xRrZWw5ay8v+mhwTKh/bgv2qb8sEVN4+79cgio+azzGGVTOsYQ0R
98: bWblQy40xPZSzylEHPLtxDEvy/iQLeenV3MFIGRNgzDOzum5zEH6Vna81LaGyqOf
99: W2fzRa0QuyV8i+NzUIDwKd2S6UBMqAqCkmoL3c8cAFXMabVuN3Lz+co0yebDX2kW
100: SihneS6XYxxRAgoien4o814k4rP1OYarFUNSK7JFOOj7uyAr/3VkgX3e0n1M0IfD
101: vzvlEDsvCUU3FP8IN5bhgF+sth/w6K4qUsmsWzdds4HJoMjFBpCQwyA=
102: -----END CERTIFICATE-----
103: )EOF";
104: 
105: 

(End of file - total 105 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/revision to 2.0.0.md"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/revision to 2.0.0.md</path>
<type>file</type>
<content>
1: ini adalah project yang sudah jadi dan sudah rilis v1.0.0
2: ini penjelasan project yang sekarang dan yang akan di update firmware nya
3: saya lampirkan program yang akan di kembangkan menjadi v2.0.0
4: 
5: Nama Project  : Data Logger 3 Phase
6: 
7: Overview Alat (Hardware)      : konteksnya alat ini hanya berfungsi untuk akuisisi data dan mengirimkan ke mqtt broker (tidak melakukan data logger), data logger sdh ada unit website yang khusus menangani data logging yang datanya di ambil dari mqtt broker
8:                                 1. Akuisisi Data dari 3 buah PZEM-004T (Masing2 PZEM handle 1 Line RST to Netral (WYE/220V))
9:                                 2. Pengolahan Data
10:                                 3. Publish Data ke MQTT Broker (per topic)
11:                                 4. Handle Display OLED 0.96" (display tampilan sangat alakadarnya) dan push button untuk ganti slide page parameter 3 phase
12:                                 5. bisa management signal wi-fi dan prioritasnya (masih manual (hard-code) atur priotitas)
13:                                 6. ada fitur debug berupa kirim data random ke broker namun masih harus buka tutup komentar di baris program (dan masih cuma punya 1 jenis debug aja)
14:                                 7. masih ada beberapa kekurangan/bug sistem seperti:
15:                                   a. [asinkron data] data antar parameter yang sampai ke broker atau website tidak serentak/tidak bersamaan (ada yang terlalu cepat/terlalu lambat sepersekian detik) karena masih publish mentah per topic dan belum pakai json
16:                                   b. [blocking] saat booting esp32 terus melakukan connecting ke wifi dan tidak akan masuk display utama sampai esp32 terkoneksi ke wifi
17:                                   c. belum ada fitur kalibrasi
18:                                   d. belum ada webserver & webpage (untuk config dan kalibrasi)
19:                                   e. masih ada sistem blocking
20:                                   f. belum ada state machine & self-healing kalau error di lapangan
21:                                   g. display OLED masih polosan raw text dan susunan parameter masih berantakan
22:                                   h. Wifi Selector belum seperti pintar network management windows maupun smartphone yang mempunyai known network dan pintar mencari prioritas serta otomatis tersambung 
23: 
24: Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin (Arduino IDE v2.0 & Board Package ESP32 v2.0.14 upgrade ke v3.3.0 untuk firmware datalogger v2.0.0)
25:                                 2. PZEM-004T 100A (3 buah, masing2 handle 1 fasa dari RST ke Netral (WYE/220V)) 
26:                                 3. OLED 0.96" I2C
27:                                 4. Push Button (Normally Open, Not latching) untuk Menu OLED
28:                                 5. Built-in LED ESP32 untuk signal indikator seperti reset kWh
29: 
30: Konfigurasi Pinout Electrical : 
31: //************* PZEM ****************//
32: - PZEM Fasa R dan S dalam satu hardware serial 1
33: - PZEM Fasa T berada dalam hardware serial 2 
34: 
35: #define PZEM_RX1_PIN 4
36: #define PZEM_TX1_PIN 15
37: #define PZEM_RX2_PIN 17
38: #define PZEM_TX2_PIN 16
39: // #define PZEM_SERIAL Serial2
40: HardwareSerial PZEMSerial1(1);
41: HardwareSerial PZEMSerial2(2);
42: #define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM
43: 
44: PZEM004Tv30 pzems[NUM_PZEMS] = {
45:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
46:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
47:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
48: };
49: //************* PZEM ****************//
50: 
51: //************* OLED 0.96" I2C ****************//
52: SDA= GPIO 21
53: SCL= GPIO 22
54: //************* OLED 0.96" I2C ****************//
55: 
56: //*************** Button & LED ****************//
57: BUTTON  5
58: LED     2
59: //*************** Button & LED ****************//
60: 
61: * Board Package & Libraries yang sudah diinstall di firmware Data Logger v1.0.0 ini (install via Board & Library Manager):
62: *   Board Package   : Board Package ESP32 v2.0.14 (pada firmware data logger v2.0.0 gunakan ESP32 v3.3.0)
63: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
64: *   - PZEM004Tv30          by Jakub Mandula     v1.2.1
65: *   - Adafruit SSD1306     by Adafruit          v2.5.16
66: *   - Adafruit GFX         by Adafruit          v1.12.5
67: *   - PubSubClient         by Nick O'Leary      v2.8
68: *
69: * Board Package & Libraries yang perlu diinstall untuk pembaruan firmware Data Logger di versi v2.0.0 nanti (install via Board & Library Manager):
70: *   Board Package   : Board Package ESP32 v3.3.0
71: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
72: *   - ArduinoJson          by Benoit Blanchon   v6.21.6
73: *   - ESP Async WebServer  by ESP32Async        v3.11.0
74: *   - Async TCP            by ESP32Async        v3.4.10
75: 
76: 
77: Yang Harus diperbaiki/diupdate adalah:
78: 1. Semua bagian sistem dan fungsi harus FULL asynchronous task/non blocking system kalau perlu gunakan dua core esp32 atau gunakan FreeRTOS
79: 2. [hardcode & webpage config] sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded (hanya bisa diakses oleh programmer lewat program) dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
80: 3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker dengan topic hierarchical `{PREFIX_TOPIC}/{MQTT_TOPIC}/{DEVICE_ID}` (prefix dan topic diambil dari eeprom berdasarkan configurasi web config)
81:   buatlah file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js
82: 
83:   **Topik MQTT:** `lutpiii/telemetry/{device_id}` (hierarchical, bukan flat)
84:   Backend subscribe ke `lutpiii/telemetry/+` untuk menerima data dari semua device.
85: 
86:   **Final Schema JSON (v2.0.0):**
87:   ```json
88: {
89:   "seq": 1247,
90:   "device": {
91:     "id":     "3ph-logger-001",
92:     "fw":     "v2.1.0",
93:     "uptime": 3600,
94:     "heap":   148432,
95:     "rssi":   -65
96:   },
97:   "phases": {
98:     "R": {
99:       "valid":  true,
100:       "v":      220.1,
101:       "i":      12.34,
102:       "p":      2650.0,
103:       "s":      2714.6,
104:       "q":      582.1,
105:       "pf":     0.97,
106:       "f":      50.0,
107:       "e":      15230.0,
108:       "status": "OK"
109:     },
110:     "S": {
111:       "valid":  true,
112:       "v":      200.6,
113:       "i":      12.34,
114:       "p":      2650.0,
115:       "s":      2714.6,
116:       "q":      582.1,
117:       "pf":     0.97,
118:       "f":      50.0,
119:       "e":      15230.0,
120:       "status": "UNDER"
121:     },
122:     "T": {
123:       "valid":  false,
124:       "v":      0.0,
125:       "i":      0.0,
126:       "p":      0.0,
127:       "s":      0.0,
128:       "q":      0.0,
129:       "pf":     0.0,
130:       "f":      0.0,
131:       "e":      0.0,
132:       "status": "LOST"
133:     }
134:   },
135:   "unbalance": 4.25,
136:   "ts": 1717001234
137: }
138: ```
139: 
140:   **Perubahan dari JSON versi awal:**
141: 
142:   | Aspek | Sebelum | Sesudah | Alasan |
143:   |---|---|---|---|
144:   | Topic | `lutpiii/telemetry` (flat) | `lutpiii/telemetry/{device_id}` (hierarchical) | Backend filter by device via MQTT routing |
145:   | Numeric values | String `"220.1"` | Number `220.1` | Backend langsung pakai, payload lebih kecil |
146:   | Schema phase invalid | Parsial (hanya `status`) | Full schema + `valid: false` | Satu parser untuk semua kondisi |
147:   | `ts` | `millis()` lokal device | Unix epoch seconds (dari NTP) | Waktu absolut untuk sorting/graphing antar device |
148:   | `unbalance` | Di dalam object `phases` | Level terpisah di root | Milik agregat 3-fasa, bukan per-phase |
149:   | `seq` | Tidak ada | `uint32` increment setiap publish | Deteksi missed message / data gap |
150: 4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi atau mqtt dulu baru bisa masuk ke sistem (tetap buat opening screen OLED dengan Loading bar progress nya)
151:     seharusnya sistem terus berjalan meskipun:
152:     a. tidak terhubung ke wi-fi dan/atau mqtt, atau
153:     b. hanya terhubung ke wi-fi namun tidak terhubung ke mqtt server
154: 5. [Tambah] tambah status line dari OK/UNDER/LOST menjadi OK/UNDER/OVER/LOST. serta memindahkan line status threshold dari hardcoded jadi eeprom di config.h
155:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
156:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
157:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
158: 6. [Tambah] tambahkan parameter unbalanced antar phase berdasarkan standard di indonesia, masukkan fungsi di DataAcquisition.h/.cpp
159:         ALGORITMA HITUNG VOLTAGE UNBALANCE 3 PHASE
160:         Input:
161:             VR = Tegangan phase R ke netral
162:             VS = Tegangan phase S ke netral
163:             VT = Tegangan phase T ke netral
164:         Langkah 1:
165:             Hitung tegangan rata-rata
166:             VAVG = (VR + VS + VT) / 3
167:         Langkah 2:
168:             Hitung deviasi tiap phase terhadap rata-rata
169:             DEV_R = |VR - VAVG|
170:             DEV_S = |VS - VAVG|
171:             DEV_T = |VT - VAVG|
172:         Langkah 3:
173:             Tentukan deviasi terbesar
174:             MAX_DEV = nilai terbesar dari:
175:                       DEV_R, DEV_S, DEV_T
176:         Langkah 4:
177:             Hitung persentase unbalance
178:             UNBALANCE = (MAX_DEV / VAVG) × 100
179:         Langkah 5:
180:             Tampilkan hasil unbalance dalam persen (%)
181: 
182:         --------------------------------------------------
183: 
184:         CONTOH DATA:
185:             VR = 220V
186:             VS = 210V
187:             VT = 228V
188:         PROSES:
189:         1. Hitung rata-rata
190:             VAVG = (220 + 210 + 228) / 3
191:             VAVG = 219.33V
192:         2. Hitung deviasi
193:             DEV_R = |220 - 219.33|
194:             DEV_R = 0.67V
195:             DEV_S = |210 - 219.33|
196:             DEV_S = 9.33V
197:             DEV_T = |228 - 219.33|
198:             DEV_T = 8.67V
199:         3. Cari deviasi terbesar
200:             MAX_DEV = 9.33V
201:         4. Hitung unbalance
202:             UNBALANCE = (9.33 / 219.33) × 100
203:             UNBALANCE = 4.25%
204:         HASIL:
205:             Voltage Unbalance = 4.25%
206: 
207: 7. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
208:   Rule set Display:
209:     - display OLED 0.96" SSD1306 sepenuhnya dikontrol dengan 1 push button
210:     - kuncinya cuma ada 2     : 1. jika push button di tekan dengan interval biasa maka itu untuk mengganti page/memindahkan kursor menu ">"
211:                                 2. jika push button di tekan dengan interval 2 detik (customable webpage config) maka itu untuk melakukan "Enter" pada menu yang di pilih atau khususnya pada Monitoring Mode adalah untuk berpindah ke Menu Mode
212:     - saat di Monitoring Mode : 1. tekan biasa push button untuk slide page ke page berikutnya
213:                                 2. tekan selama 2 detik untuk masuk ke "Menu Mode" (berlaku di monitoring page manapun untuk berpindah ke menu mode)
214:     - saat di Menu Mode       : 1. push button sebagai kursor untuk memilih menu ke bawah, dengan cara menekan biasa agar kursor berpindah ke menu dibawahnya
215:                                 2. jika kursor sudah pada menu paling bawah dan button di tekan lagi, maka kursor akan kembali ke menu paling atas lagi (berlaku juga untuk menu confirm "YES" or "NO" yang secara horizontal kanan-kiri)
216:                                 3. untuk masuk ke menu pilihan, maka button harus ditekan selama 2 detik sebagai tombol "Enter" nya
217:                                 4. setiap menu harus ada "Back" untuk keluar dari menu dengan cara menekan yang sama seperti masuk ke dalam menu (2 detik)
218:                                 5. setiap menu harus ada "Back" kecuali "Back to Monitoring", jika itu ditekan selama 2 detik maka akan kembali ke "Monitoring Mode"
219:  *    1. Monitoring Mode:
220:  *    Page 1 [Monitoring Mode]:
221:  *   ┌────────────────────────────┐
222:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
223:  *   │   R         S         T    │  ← Phase Name         
224:  *   │  220V      220V      220V  │  ← Voltage Line to Netral   
225:  *   │  100A      100A      100A  │  ← Current per phase   (0-100)
226:  *   │   OK        OK        OK   │  ← Line_Status   (LOST/UNDER/OK)
227:  *   │  Unbalance = 2.5%          │  ← Unbalance param
228:  *   └────────────────────────────┘
229:  *
230:  *   ┌────────────────────────────┐
231:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
232:  *   │   R         S         T    │  ← Phase Name         
233:  *   │  220V      220V        0V  │  ← Voltage Line to Netral   
234:  *   │  100A      100A      100A  │  ← Current per phase   (0-100)
235:  *   │   OK       UNDER     LOST  │  ← Line_Status   (LOST/UNDER/OK), muncul dengan efek beeping/kedip untuk warning ketika status LOST / UNDER / OVER
236:  *   │  Unbalance = 5.45%   [!]   │  ← Unbalance param ketika hit threshold max unbalance di web config, [!] icon ini juga akan muncul dengan efek beeping/kedip untuk warning
237:  *   └────────────────────────────┘
238:  *
239:  *    Page 2 [Monitoring Mode]:
240:  *   ┌────────────────────────────┐
241:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
242:  *   │   R         S         T    │  ← Phase Name         
243:  *   │  60Hz      60Hz      60Hz  │  ← Frequency     
244:  *   │ 100VA     1.2kVA    10kVA  │  ← Apparent Power (L to N) (jika dibawah 1000 maka satuannya "VA", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVA")  
245:  *   │ 100VAr    2.0kVAr   13kVAr │  ← Reactive Power (L to N) (jika dibawah 1000 maka satuannya "VAR", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVAr")         
246:  *   └────────────────────────────┘
247:  *
248:  *    Page 3 [Monitoring Mode]:
249:  *   ┌────────────────────────────┐
250:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
251:  *   │   R         S         T    │  ← Phase Name         
252:  *   │ 0.8PF     1.0PF     0.7PF  │  ← Power Factor (0.0 - 1.0)     
253:  *   │ 100W       12kW     2.2kW  │  ← Active Power (jika dibawah 1000 maka satuannya "W", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kW")   
254:  *   │ 300Wh     1.4kWh     30kWh │  ← Total Active Energy (jika dibawah 1000 maka satuannya "Wh", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kWh")          
255:  *   └────────────────────────────┘
256:  *
257:  *    2. Menu Mode:
258:  *    Page 1 [Menu Mode]: (jika menu highlighted maka ada indentasi maju 1 atau 2 karakter untuk menunjukkan posisi kursor pada OLED di menu mana)
259:  *   ┌────────────────────────────┐
260:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
261:  *   │  > Config                  │  ← Config Menu         
262:  *   │  > Reset kWh               │  ← Reset kWh semua fasa jadi 0kWh
263:  *   │  > Reboot Device           │  ← Reboot ESP32 lewat program setara dengan tekan tombol RST di board
264:  *   │  > Reset to Factory        │  ← Reset to Factory (reset configurasi yang ada di EEPROM, sama saja dengan reset EEPROM melalui web config, bedanya ini lewat menu OLED, yang satunya lewat web config)
265:  *   │  > Back to Monitoring      │  ← Kembali ke Page Monitoring Mode (jika kursor di "Back to Monitoring" dan tombol ditekan selama 2 detik maka akan kembali ke Page 1 Monitoring Mode )
266:  *   └────────────────────────────┘
267:  *
268:  *    Page 2 [Menu Mode > Config]:
269:  *    Note: Di menu Config ini hanya difungsikan untuk user mengaktifkan webserver, selebihnya config akan dilakukan melalui webpage dan akan di simpan ke dalam EEPROM ESP32
270:  *   ┌────────────────────────────┐
271:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
272:  *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
273:  *   │  PW  : 12345678            │  ← Password dari SSID ESP32
274:  *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
275:  *   │  > Active         Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
276:  *   └────────────────────────────┘
277:  *   ┌────────────────────────────┐
278:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
279:  *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
280:  *   │  PW  : 12345678            │  ← Password dari SSID ESP32
281:  *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
282:  *   │  Inactive       > Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
283:  *   └────────────────────────────┘
284:  *
285:  *    Page 3 [Menu Mode > Reset kWh]:
286:  *    Note: Di menu Reset kWh ini hanya difungsikan untuk user melakukan Reset Total kWh pada semua module PZEM-004T
287:  *   ┌────────────────────────────┐
288:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
289:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
290:  *   │        Reset kWh?          │  
291:  *   │                            │  
292:  *   │  > Reset          Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
293:  *   └────────────────────────────┘
294:  *   ┌────────────────────────────┐
295:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
296:  *   │                            │  ← Teks Konfirmasi         
297:  *   │     Reset kWh Berhasil     │  ← Teks Output Reset (Reset kWh Berhasil/Reset kWh Gagal) tampil selama 2 detik lalu otomatis kembali ke page Menu Mode
298:  *   │                            │  
299:  *   │                            │  
300:  *   └────────────────────────────┘
301:  *   ┌────────────────────────────┐
302:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
303:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
304:  *   │        Reset kWh?          │  
305:  *   │                            │  
306:  *   │   Reset         > Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
307:  *   └────────────────────────────┘
308:  *
309:  *    Page 4 [Menu Mode > Reboot Device]:
310:  *    Note: Di menu Config ini hanya difungsikan untuk user melakukan rebooting device ESP32
311:  *   ┌────────────────────────────┐
312:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
313:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
314:  *   │       Reboot Device?       │  
315:  *   │                            │  
316:  *   │  > Reboot         Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
317:  *   └────────────────────────────┘
318:  *   ┌────────────────────────────┐
319:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
320:  *   │                            │      
321:  *   │     Device akan Reboot     │  ← Teks Countdown 3 detik sebelum system otomatis reboot (dalam 3 detik, dalam 2 detik, dalam 1 detik, dalam 0 detik)
322:  *   │       dalam 3 detik        │  
323:  *   │                            │  
324:  *   └────────────────────────────┘
325:  *   ┌────────────────────────────┐
326:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
327:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
328:  *   │       Reboot Device?       │  
329:  *   │                            │  
330:  *   │   Reboot        > Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
331:  *   └────────────────────────────┘
332:  *
333:  *    Page 5 [Menu Mode > Reset to Factory]:
334:  *    Note: -
335:  *   ┌────────────────────────────┐
336:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
337:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
338:  *   │        Reset Device?       │  
339:  *   │                            │  
340:  *   │  > Reset          Back     │  ← (Confirm Choise) jika menu "Reset" ditekan selama 2 detik maka esp32 akan melakukan reset EEPROM dan menampilkan teks output jika berhasil selama 2 detik, kemudian otomatis akan melakukan reboot esp32 by system, namun jika "tidak" ditekan selama 2 detik maka akan kembali ke Menu Config
341:  *   └────────────────────────────┘
342:  *   ┌────────────────────────────┐
343:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
344:  *   │                            │      
345:  *   │      Reset to Factory      │  ← Teks Output Reset (Reset EEPROM/preferences Berhasil/Reset EEPROM Gagal) tampil selama 2 detik lalu otomatis melakukan reboot esp32 by system
346:  *   │          berhasil          │  
347:  *   │                            │  
348:  *   └────────────────────────────┘
349: 
350: 8. [HTTP Web-server & Access Point] 
351:   a. buat webpage config yang bisa diakses lewat smartphone atau laptop yang terhubung melalui IP Address yang ESP32 keluarkan saat Config pada Menu Mode "Active"
352:   b. jika saat access point aktif dan ESP32 sedang terhubung ke jaringan yang ada internetnya, maka teruskan internet itu ke access point ESP32 agar Laptop/smartphone yang sedang melakukan config/terhubung ke access point esp32 mendapatkan jaringan internet untuk tetap online (jika tidak, Laptop/smartphone tidak apa-apa dalam kondisi offline) 
353:   c. semua parameter config yang ada di webpage tersimpan di dalam EEPROM ESP32 (jadi device baru harus config terlebih dahulu untuk terkoneksi ke internet/MQTT)
354:   d. buatkan login page sebelum masuk ke main page config, dengan user dan pass yang hanya bisa diubah di hardcode config.h, dengan default USERNAME = ADMIN, PASS = 18273645
355:   e. page 1 [Config] HTML + CSS:
356:     - Manage Known Network   (Big/Medium Card with Link to "Manage Known Network")
357:     - MQTT Connection Config (Big/Medium Card with Link to "MQTT Connection Config")
358:     - PZEM-004T Calibration  (Big/Medium Card with Link to "PZEM-004T Calibration")
359: 
360:     {HARDRESET EEPROM}
361: 
362:   f. page 1 [Config > Manage Known Network] HTML + CSS:
363:       tampilkan semua jaringan yang diketahui (EEPROM) dalam tabel dengan kolom dan terdapat text box untuk menambahkan jaringan baru di bawah table dan tombol add network untuk menyimpan jaringan baru ke EEPROM
364:               Manage Known Network
365: 
366:       | No. | SSID | PASS     |   Aksi   |
367:       | 1.  | ABC  | 18273645 | {Delete} |
368:       | 2.  | esp  | 18273645 | {Delete} |
369: 
370:       Add Network
371:       [ SSID....       ]
372:       [ PASS....       ]
373: 
374:       {ADD NETWORK}
375: 
376:   g. page 2 [Config > MQTT Connection Config] HTML + CSS:
377:       tampilkan data koneksi MQTT dalam bentuk teks dari EEPROM
378:       MQTT Server = broker.avisha.id
379:       MQTT_PORT   = 1883               // Non-SSL port
380:       MQTTS_PORT  = 8884               // SSL port
381:       WS_PORT     = 8083               // Non-SSL port
382:       WSS_PORT    = 8084               // SSL port
383:       MQTT_USER   = "lutpiii"          // MQTT Username
384:       MQTT_PASS   = "lutpiiiMQTT"      // MQTT Password
385:       PREFIX_TOPIC= "lutpiii"          // Topic prefix (wajib)
386:       MQTT_TOPIC  = "telemetry"        // Topic (wajib)
387:       USE SSL     = TRUE
388:       USE WS      = FALSE              
389:       
390: 
391:       {EDIT}        {SAVE CONFIG}
392: 
393:       // jika WS=0 & SSL=0 gunakan MQTT
394:       // jika WS=0 & SSL=1 gunakan MQTTS
395:       // jika WS=1 & SSL=0 gunakan WS
396:       // jika WS=1 & SSL=1 gunakan WSS
397: 
398:   h. page 3 [Config > PZEM-004T Calibration] HTML + CSS:
399:       buat kalibrasi dengan model offset (-/+) untuk parameter Voltage & Current masing-masing Fasa dengan presisi 0.000 perubahan, threshold LOST, UNDER, OVER dan unbalance. dan tampilkan juga offset yang tersimpan
400:       dengan model text box
401: 
402:       {SAVE CALIBRATION}
403: 
404:   i. gunakan saja template yang sudah saya buat ini, namun bagusin lagi homa page bagian text link nya, jgn pakai text link lagi, gunakan card medium atau besar ke bawah saja susunannya, jumlah card = jumlah menu
405: String WebServerManager::_htmlHead(const char* title) {
406: return String(R"(<!DOCTYPE html><html><head>
407: <meta charset="UTF-8">
408: <meta name="viewport" content="width=device-width,initial-scale=1">
409: <meta name="color-scheme" content="light">
410: <title>)") + title + (R"(</title>
411: <style>
412: :root{
413:   --bg:#f7f8fa;
414:   --surface:#ffffff;
415:   --surface-2:#f3f7f4;
416:   --text:#1f2937;
417:   --muted:#6b7280;
418:   --line:#dbe3dc;
419:   --primary:#16a34a;
420:   --primary-hover:#15803d;
421:   --primary-soft:#dcfce7;
422:   --danger:#dc2626;
423:   --danger-hover:#b91c1c;
424:   --shadow:0 6px 18px rgba(15,23,42,.06);
425:   --radius:10px;
426: }
427: 
428: *{box-sizing:border-box}
429: html{-webkit-text-size-adjust:100%}
430: body{
431:   font-family:system-ui,-apple-system,"Segoe UI",Roboto,Arial,sans-serif;
432:   max-width:600px;
433:   margin:30px auto;
434:   padding:0 16px;
435:   background:var(--bg);
436:   color:var(--text);
437:   line-height:1.5;
438: }
439: 
440: h1{
441:   color:var(--text);
442:   font-size:1.4rem;
443:   margin:0 0 12px;
444:   letter-spacing:-0.02em;
445: }
446: 
447: h2{
448:   margin:0 0 10px;
449:   font-size:1rem;
450:   color:var(--primary);
451:   background:var(--primary-soft);
452:   padding:8px 12px;
453:   border-radius:8px;
454:   font-weight:700;
455: }
456: 
457: a{
458:   color:var(--primary);
459:   text-decoration:none;
460: }
461: a:hover{text-decoration:underline}
462: 
463: nav{
464:   margin:12px 0;
465:   display:flex;
466:   flex-wrap:wrap;
467:   gap:8px;
468: }
469: nav a{
470:   display:inline-flex;
471:   align-items:center;
472:   justify-content:center;
473:   padding:8px 12px;
474:   border:1px solid var(--line);
475:   border-radius:999px;
476:   background:var(--surface);
477:   color:var(--text);
478:   text-decoration:none;
479:   box-shadow:0 1px 2px rgba(15,23,42,.04);
480: }
481: nav a:hover{
482:   background:var(--surface-2);
483:   text-decoration:none;
484: }
485: 
486: hr{
487:   border:none;
488:   height:1px;
489:   background:var(--line);
490:   margin:14px 0;
491: }
492: 
493: .card{
494:   background:var(--surface);
495:   border:1px solid var(--line);
496:   border-left:4px solid var(--primary);
497:   border-radius:var(--radius);
498:   padding:16px;
499:   margin:12px 0;
500:   box-shadow:var(--shadow);
501: }
502: 
503: label{
504:   display:block;
505:   font-size:.85rem;
506:   color:var(--muted);
507:   margin:10px 0 6px;
508: }
509: 
510: .hint{
511:   font-size:.78rem;
512:   color:var(--muted);
513:   margin:-4px 0 8px;
514: }
515: 
516: input[type=text],
517: input[type=password],
518: input[type=number],
519: textarea{
520:   width:100%;
521:   box-sizing:border-box;
522:   padding:9px 10px;
523:   background:#fff;
524:   color:var(--text);
525:   border:1px solid var(--line);
526:   border-radius:8px;
527:   margin:4px 0 10px;
528:   font:inherit;
529:   outline:none;
530: }
531: 
532: input[type=text]:focus,
533: input[type=password]:focus,
534: input[type=number]:focus,
535: textarea:focus{
536:   border-color:rgba(22,163,74,.55);
537:   box-shadow:0 0 0 3px rgba(22,163,74,.12);
538: }
539: 
540: textarea{
541:   resize:vertical;
542:   min-height:120px;
543: }
544: 
545: button,
546: .btn,
547: .danger{
548:   background:var(--primary);
549:   color:#fff;
550:   border:none;
551:   padding:10px 16px;
552:   border-radius:8px;
553:   cursor:pointer;
554:   font-size:.92rem;
555:   font-weight:600;
556:   transition:background .15s ease, transform .05s ease;
557: }
558: 
559: button:hover,
560: .btn:hover{
561:   background:var(--primary-hover);
562: }
563: 
564: button:active,
565: .btn:active,
566: .danger:active{
567:   transform:translateY(1px);
568: }
569: 
570: .btn-del{
571:   background:#6b7280;
572: }
573: .btn-del:hover{
574:   background:#4b5563;
575: }
576: 
577: .danger{
578:   background:var(--danger);
579: }
580: .danger:hover{
581:   background:var(--danger-hover);
582: }
583: 
584: table{
585:   width:100%;
586:   border-collapse:collapse;
587:   margin:8px 0;
588: }
589: th,td{
590:   text-align:left;
591:   padding:8px;
592:   border-bottom:1px solid var(--line);
593:   vertical-align:top;
594: }
595: th{
596:   color:var(--primary);
597:   background:#f9fffb;
598: }
599: 
600: input[type=checkbox]{
601:   accent-color:var(--primary);
602:   transform:translateY(1px);
603: }
604: 
605: form{margin:0}
606: 
607: @media (max-width:640px){
608:   body{
609:     margin:18px auto;
610:     padding:0 12px;
611:   }
612:   nav a{
613:     width:calc(50% - 4px);
614:   }
615: }
616: </style></head><body>
617: <h1>&#9889; 3-Phase Data Logger</h1>
618: <nav>
619: <a href="/">Home</a>
620: <a href="/wifi">WiFi</a>
621: <a href="/mqtt">MQTT</a>
622: <a href="/calibration">Calibration</a>
623: </nav>
624: <hr>
625: )");
626: }
627: 
628: String WebServerManager::_htmlFoot() {
629:     return R"(<hr><p style="font-size:.75rem;color:#6b7280;text-align:center">Muhammad Lutfi Nur Anendi &bull; )" FW_VERSION R"(</p></body></html>)";
630: }
631: 
632: // ─── Page: Index ──────────────────────────────────────────
633: 
634: String WebServerManager::_pageIndex(StorageManager& s) {
635:     String h = _htmlHead("Config");
636:     h += R"HTML(<div class="card">
637: <h2>Configuration Pages</h2>
638: <p><a href="/wifi">&#128246; Manage Known Networks</a></p>
639: <p><a href="/mqtt">&#128272; MQTT Connection Config</a></p>
640: <p><a href="/calibration">&#9881; PZEM-004T Calibration &amp; Thresholds</a></p>
641: </div>
642: <div class="card">
643: <h2>&#9888; Danger Zone</h2>
644: <form method="POST" action="/factoryreset"
645:   onsubmit="return confirm('ERASE ALL settings? This cannot be undone.')">
646: <button class="danger" type="submit">Hard Reset EEPROM</button>
647: </form>
648: </div>
649: )HTML";
650:     h += _htmlFoot();
651:     return h;
652: }
653: 
654: // ─── Page: WiFi ───────────────────────────────────────────
655: 
656: String WebServerManager::_pageWifi(StorageManager& s) {
657:     String h = _htmlHead("Manage WiFi");
658:     h += "<h2>Known Networks</h2><div class='card'>";
659:     h += "<table><tr><th>No.</th><th>SSID</th><th>Action</th></tr>";
660: 
661:     int n = s.getWifiCount();
662:     for (int i = 0; i < n; i++) {
663:         WiFiEntry e = s.getWifiEntry(i);
664:         h += "<tr><td>" + String(i + 1) + "</td><td>" + String(e.ssid) + "</td><td>";
665:         h += "<form method='POST' action='/wifi/delete' style='display:inline'>";
666:         h += "<input type='hidden' name='idx' value='" + String(i) + "'>";
667:         h += "<button class='btn btn-del' type='submit'>Delete</button></form>";
668:         h += "</td></tr>";
669:     }
670:     if (n == 0) h += "<tr><td colspan='3'>No networks stored.</td></tr>";
671:     h += "</table></div>";
672: 
673:     h += R"(<div class="card"><h2>Add Network</h2>
674: <form method="POST" action="/wifi/add">
675: <label>SSID</label>
676: <input type="text" name="ssid" placeholder="Network name" required>
677: <label>Password</label>
678: <input type="password" name="pass" placeholder="Password">
679: <button type="submit">Add Network</button>
680: </form></div>)";
681: 
682:     h += _htmlFoot();
683:     return h;
684: }
685: 
686: // ─── Page: MQTT ───────────────────────────────────────────
687: 
688: String WebServerManager::_pageMQTT(StorageManager& s) {
689:     MQTTConfig cfg = s.getMQTTConfig();
690:     String h = _htmlHead("MQTT Config");
691:     h += "<h2>MQTT Broker</h2><div class='card'>";
692:     h += "<form method='POST' action='/mqtt'>";
693: 
694:     auto field = [&](const char* label, const char* name, const char* val, const char* type = "text") {
695:         h += String("<label>") + label + "</label>";
696:         h += String("<input type='") + type + "' name='" + name + "' value='" + val + "'>";
697:     };
698:     auto numField = [&](const char* label, const char* name, int val) {
699:         h += String("<label>") + label + "</label>";
700:         h += String("<input type='number' name='") + name + "' value='" + val + "'>";
701:     };
702:     auto checkbox = [&](const char* label, const char* name, bool checked) {
703:         h += String("<label><input type='checkbox' name='") + name + "' " +
704:              (checked ? "checked" : "") + "> " + label + "</label><br>";
705:     };
706: 
707:     field("MQTT Server",       "server",   cfg.server);
708:     numField("Port (non-SSL)", "port",     cfg.port);
709:     numField("Port (SSL)",     "sslport",  cfg.sslPort);
710:     field("Username",          "user",     cfg.user);
711:     field("Password",          "pass",     cfg.pass, "password");
712:     field("Topic Prefix",      "topic",    cfg.topic);
713:     checkbox("Use SSL",        "usessl",   cfg.useSSL);
714:     checkbox("Use WebSocket",  "usews",    cfg.useWS);
715: 
716:     h += "<label>CA Certificate (PEM)</label>";
717:     h += String("<textarea name='cacert' rows='5' placeholder='-----BEGIN CERTIFICATE-----...'>")
718:          + cfg.caCert + "</textarea>";
719:     h += "<button type='submit'>Save Config</button></form></div>";
720:     h += _htmlFoot();
721:     return h;
722: }
723: 
724: // ─── Page: Calibration + Thresholds ──────────────────────
725: 
726: String WebServerManager::_pageCalibration(StorageManager& s) {
727:     CalibrationConfig cal = s.getCalibration();
728:     ThresholdConfig   thr = s.getThresholds();
729: 
730:     String h = _htmlHead("Calibration & Thresholds");
731:     h += "<form method='POST' action='/calibration'>";
732: 
733:     h += "<h2>PZEM-004T Calibration</h2>";
734:     h += "<p class='hint'>Offset is additive: Reading = Raw + Offset. Use negative to subtract.</p>";
735:     h += "<div class='card'>";
736: 
737:     const char* phases[3] = {"R", "S", "T"};
738:     for (int i = 0; i < 3; i++) {
739:         h += String("<h2>Phase ") + phases[i] + "</h2>";
740:         h += String("<label>Voltage Offset (V) &nbsp; Current stored: ") +
741:              String(cal.voltageOffset[i], 3) + " V</label>";
742:         h += String("<input type='number' step='0.001' name='voff_") +
743:              phases[i] + "' value='" + String(cal.voltageOffset[i], 3) + "'>";
744:         h += String("<label>Current Offset (A) &nbsp; Current stored: ") +
745:              String(cal.currentOffset[i], 3) + " A</label>";
746:         h += String("<input type='number' step='0.001' name='ioff_") +
747:              phases[i] + "' value='" + String(cal.currentOffset[i], 3) + "'>";
748:     }
749:     h += "</div>";
750: 
751:     h += "<h2>Line Status Thresholds</h2>";
752:     h += "<p class='hint'>Thresholds used to determine line status: LOST / UNDER / OK / OVER.</p>";
753:     h += "<div class='card'>";
754: 
755:     h += "<label>LOST threshold (V) &ndash; voltage below this &rarr; LOST</label>";
756:     h += "<p class='hint'>Current: " + String(thr.voltageLost, 1) + " V</p>";
757:     h += "<input type='number' step='0.1' name='th_vlost' value='" + String(thr.voltageLost, 1) + "'>";
758: 
759:     h += "<label>UNDER threshold (V) &ndash; voltage below this &rarr; UNDER (above LOST)</label>";
760:     h += "<p class='hint'>Current: " + String(thr.voltageUnder, 1) + " V</p>";
761:     h += "<input type='number' step='0.1' name='th_vunder' value='" + String(thr.voltageUnder, 1) + "'>";
762: 
763:     h += "<label>OVER threshold (V) &ndash; voltage above this &rarr; OVER</label>";
764:     h += "<p class='hint'>Current: " + String(thr.voltageOver, 1) + " V</p>";
765:     h += "<input type='number' step='0.1' name='th_vover' value='" + String(thr.voltageOver, 1) + "'>";
766:     h += "</div>";
767: 
768:     h += "<h2>Voltage Unbalance Threshold</h2>";
769:     h += "<p class='hint'>Berdasarkan standar SNI/SPLN Indonesia. Jika unbalance &gt; threshold unbalance maka OLED menampilkan peringatan [!]</p>";
770:     h += "<br>";
771:     h += "<p class='hint'>3-phase voltage unbalance computed using NEMA method.</p>";
772:     h += "<div class='card'>";
773:     h += "<label>Max Unbalance (%) &ndash; above this &rarr; warning [!] on OLED &amp; MQTT</label>";
774:     h += "<p class='hint'>Current: " + String(thr.unbalanceMax, 2) + " %</p>";
775:     h += "<input type='number' step='0.01' name='th_unbal' value='" + String(thr.unbalanceMax, 2) + "'>";
776:     h += "</div>";
777: 
778:     h += "<button type='submit'>Save Calibration &amp; Thresholds</button>";
779:     h += "</form>";
780:     h += _htmlFoot();
781:     return h;
782: }
783:  
784: 
785: 9. [IMPORTANT!!!] Kalibrasi Tegangan & Arus PZEM harus mempengaruhi semua parameter turunan (VA, VAr, W, Wh, PF kecuali Hz). Karena parameter lain bergantung pada V dan I, maka setelah kalibrasi offset V/I, semua parameter hitung ulang agar konsisten.
786: 
787: 10. [Tambah] di web config, tambahkan config untuk nilai LOST, UNDER, OVER voltage, serta persentase max unbalance phase di halaman PZEM Calibration
788:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
789:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
790:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
791:     #define VOLTAGE_UNBALANCE_MAX    3.0f  // Above this → OVER UNBALANCE (satuan persen)
792: 
793: 11. LED Signal 
794:   - sebagai indikator terhadap aksi-aksi dari fungsi/module program seperti Booting, Reset kWh, active/deactive webserver, dll.
795: 
796: 12. [Tambah] tambahkan fitur auto self-healing ketika esp32 mengalami freeze/hang/stuck/runningout memory atau sejenisnya atau sesuatu yang membuat esp32 malfungsi sistem, maka harus ditambahkan self healing dengan metode yang paling cocok untuk sistem ini
797: 
798: 13. Struktur File Project gunakan OOP dan seperti ROS2 (Robot Operating System 2) dengan dokumentasi-dokumentasi melimpahnya dan program mudah dipahami, mudah di konfigurasi, dan bisa scalable saat penambahan fitur/sensor baru
799:   (jumlah file menyesuaikan, ini hanya sebagai contoh):
800:   - main.ino
801:   - config.h
802:   - data_acquisition.h/.cpp
803:   - calibration.h/.cpp
804:   - display.h/.cpp
805:   - menu.h/.cpp
806:   - webserver.h/.cpp
807:   - types.h
808:   - system_state.h
809:   - task_manager.h/.cpp
810:   - wifi_manager.h/.cpp
811:   - mqtt_manager.h/.cpp
812:   - json_builder.h/.cpp
813:   - storage_manager.h/.cpp
814:   - led_manager.h/.cpp
815:   - diagnostics.h/.cpp
816:   - dll
817: 
818: ### Catatan untuk Backend Multi-Device (JSON Parser)
819: 1. Subscribe ke `lutpiii/telemetry/+` (wildcard) untuk menerima data dari semua device
820: 2. Indexing data berdasarkan `device.id` + `ts` (Unix timestamp) untuk query perangkat per periode waktu
821: 3. Gunakan field `seq` untuk mendeteksi apakah ada data terlewat (cek gap sequence tiap device)
822: 4. Field `valid` pada tiap phase memberi tahu bahwa data phase tersebut bisa diabaikan (tanpa perlu conditional parsing)
823: 5. Jika ESP32 belum sync NTP, kirim nilai `ts = 0` dan backend fallback ke MQTT broker timestamp
824: 

(End of file - total 824 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/session-ses_15a8.md"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/session-ses_15a8.md</path>
<type>file</type>
<content>
1: # Analisis firmware datalogger ESP32
2: 
3: **Session ID:** ses_15a88c5d1ffe84PfwEnUBpi27i
4: **Created:** 6/8/2026, 11:21:54 AM
5: **Updated:** 6/8/2026, 4:31:22 PM
6: 
7: ---
8: 
9: ## User
10: 
11: analisis firmware datalogger esp32 di folder path ini
12: 
13: ---
14: 
15: ## Assistant (Plan · DeepSeek V4 Flash Free · 5.7s)
16: 
17: _Thinking:_
18: 
19: The user wants me to analyze the firmware datalogger for ESP32 in the specified folder path. Let me first explore the directory structure to understand what we're working with.
20: 
21: **Tool: read**
22: 
23: **Input:**
24: ```json
25: {
26:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor"
27: }
28: ```
29: 
30: **Output:**
31: ```
32: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor</path>
33: <type>directory</type>
34: <entries>
35: .agents/
36: brief-cl.md
37: brief.md
38: button.cpp
39: button.h
40: credential_example.cpp
41: credentials.cpp
42: credentials.h
43: Data_Logger_Monitor.ino
44: DisplayOLED.cpp
45: DisplayOLED.h
46: ResetEnergy.cpp
47: ResetEnergy.h
48: WifiSelector.cpp
49: WifiSelector.h
50: 
51: (15 entries)
52: </entries>
53: ```
54: 
55: ---
56: 
57: ## Assistant (Plan · DeepSeek V4 Flash Free · 6.4s)
58: 
59: _Thinking:_
60: 
61: Let me explore all the source files to understand the firmware. Let me read all files concurrently.
62: 
63: **Tool: read**
64: 
65: **Input:**
66: ```json
67: {
68:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/Data_Logger_Monitor.ino"
69: }
70: ```
71: 
72: **Output:**
73: ```
74: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/Data_Logger_Monitor.ino</path>
75: <type>file</type>
76: <content>
77: 1: #include <WiFi.h>
78: 2: #include <WiFiClientSecure.h>
79: 3: #include <ArduinoMqttClient.h>
80: 4: #include <PubSubClient.h>
81: 5: #include <PZEM004Tv30.h>
82: 6: #include "credentials.h"
83: 7: #include "ResetEnergy.h"
84: 8: #include "WifiSelector.h"
85: 9: #include "DisplayOLED.h"
86: 10: #include "Button.h"
87: 11: // #include "credentials_example.h"
88: 12: 
89: 13: // ===================== WiFi ======================
90: 14: WiFiClientSecure espClient;
91: 15: 
92: 16: // ============ Instance WiFiSelector ==============
93: 17: WiFiSelector wifiManager(10000);  // timeout 10 detik
94: 18: 
95: 19: // ===================== MQTT ======================
96: 20: PubSubClient client(espClient);
97: 21: 
98: 22: // ==================== Millis =====================
99: 23: unsigned long lastMsg = 0;
100: 24: 
101: 25: // ================== PZEM CONFIG ==================
102: 26: #define PZEM_RX1_PIN 4
103: 27: #define PZEM_TX1_PIN 15
104: 28: #define PZEM_RX2_PIN 17
105: 29: #define PZEM_TX2_PIN 16
106: 30: // #define PZEM_SERIAL Serial2
107: 31: HardwareSerial PZEMSerial1(1);
108: 32: HardwareSerial PZEMSerial2(2);
109: 33: #define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM
110: 34: DisplayOLED oled;
111: 35: 
112: 36: #define BUTTON_PIN  5
113: 37: Button btn(BUTTON_PIN, 5000);
114: 38: 
115: 39: // Inisialisasi PZEM dengan alamat unik
116: 40: PZEM004Tv30 pzems[NUM_PZEMS] = {
117: 41:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
118: 42:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
119: 43:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
120: 44: };
121: 45: 
122: 46: // Reset Energy Initial
123: 47: #define RESET_ENERGY_BUTTON_PIN  5
124: 48: #define RESET_ENERGY_LED_PIN     2   // LED internal ESP32 (GPIO 2)/18
125: 49: ResetEnergy resetEnergy(RESET_ENERGY_BUTTON_PIN, RESET_ENERGY_LED_PIN);
126: 50: 
127: 51: // ================== MQTT TOPICS ==================
128: 52: // Voltage (V)
129: 53: const char* workshop_v[]        = {"lutpiii/workshop/voltage/l1",        "lutpiii/workshop/voltage/l2",        "lutpiii/workshop/voltage/l3"};
130: 54: // Current (A)
131: 55: const char* workshop_i[]        = {"lutpiii/workshop/current/l1",        "lutpiii/workshop/current/l2",        "lutpiii/workshop/current/l3"};
132: 56: // Frequency (Hz)
133: 57: const char* workshop_f[]        = {"lutpiii/workshop/frequency/l1",      "lutpiii/workshop/frequency/l2",      "lutpiii/workshop/frequency/l3"};
134: 58: // Apparent Power (VA)
135: 59: const char* workshop_va[]       = {"lutpiii/workshop/apparent_power/l1", "lutpiii/workshop/apparent_power/l2", "lutpiii/workshop/apparent_power/l3"};
136: 60: // Reactive Power (VAr)
137: 61: const char* workshop_var[]      = {"lutpiii/workshop/reactive_power/l1", "lutpiii/workshop/reactive_power/l2", "lutpiii/workshop/reactive_power/l3"};
138: 62: // Active Power (W)
139: 63: const char* workshop_p[]        = {"lutpiii/workshop/active_power/l1",   "lutpiii/workshop/active_power/l2",   "lutpiii/workshop/active_power/l3"};
140: 64: // Power Factor
141: 65: const char* workshop_pf[]       = {"lutpiii/workshop/power_factor/l1",   "lutpiii/workshop/power_factor/l2",   "lutpiii/workshop/power_factor/l3"};
142: 66: // Energy (kWh)
143: 67: const char* workshop_e[]        = {"lutpiii/workshop/active_energy/l1",  "lutpiii/workshop/active_energy/l2",  "lutpiii/workshop/active_energy/l3"};
144: 68: // Status Main Power
145: 69: const char* line_status[]       = {"lutpiii/workshop/line_status/l1",    "lutpiii/workshop/line_status/l2",    "lutpiii/workshop/line_status/l3"};
146: 70: 
147: 71: // ================== ENUM LINE STATUS ==================
148: 72: enum LineStatus { LINE_GOOD, LINE_UNDERVOLTAGE, LINE_ISSUED };
149: 73: LineStatus lineStatus[NUM_PZEMS];  // status tiap line 
150: 74: 
151: 75: // =============== Buffer untuk OLED ===============
152: 76: float vol[3], cur[3], powe[3], pff[3], freqArr[3], ener[3];
153: 77: int st[3];
154: 78: 
155: 79: // ================== SETUP WIFI ==================
156: 80: void setup_wifi() {
157: 81:   if (!wifiManager.connectBestNetwork()) {
158: 82:     Serial.println("Tidak bisa terhubung ke jaringan mana pun!");
159: 83:   } else {
160: 84:     wifiManager.printStatus();
161: 85:   }
162: 86: }
163: 87: 
164: 88: // ================== MQTT CONNECT ==================
165: 89: void reconnect() {
166: 90:   while (!client.connected()) {
167: 91:     Serial.print("Attempting MQTT connection...");
168: 92:     String clientId = "ESP32Client-";
169: 93:     clientId += String(random(0xffff), HEX);
170: 94:     if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
171: 95:       Serial.println("✅MQTT Server Connected!");
172: 96:     } else {
173: 97:       Serial.print("❌failed, rc=");
174: 98:       Serial.print(client.state());
175: 99:       Serial.println(" retry in 5s");
176: 100:       delay(5000);
177: 101:     }
178: 102:   }
179: 103: }
180: 104: 
181: 105: float randFloat(float minVal, float maxVal) {
182: 106:   return minVal + ((float)random(0, 10000) / 10000.0) * (maxVal - minVal);
183: 107: }
184: 108: 
185: 109: void generateDummyData(float &voltage, float &current, float &power, float &energy, float &freq, float &pf) {
186: 110:   voltage = randFloat(200, 220);      // 180–240 V (normal + undervolt simulation)
187: 111:   current = randFloat(48.0, 50.0);    // 0.1–50 A
188: 112:   freq    = randFloat(50.0, 50.5);    // 50Hz +- tolerance
189: 113:   pf      = randFloat(0.6, 0.8);      // 0.5–1.0 realistic PF
190: 114: 
191: 115:   power   = voltage * current * pf;   // P = V × I × pf
192: 116:   energy += randFloat(2200, 4000);    // simulate energy increasing (Wh)
193: 117: }
194: 118: 
195: 119: // ================== SETUP ==================
196: 120: void setup() {
197: 121:   Serial.begin(115200);
198: 122:   setup_wifi();
199: 123: 
200: 124:   resetEnergy.begin();
201: 125:   oled.begin();
202: 126:   oled.enableAutoScroll(false);
203: 127: 
204: 128:   client.setServer(MQTT_SERVER, MQTTS_PORT);
205: 129:   // espClient.setInsecure(); // WARNING: tidak aman
206: 130:   espClient.setCACert(CA_CERT);
207: 131: 
208: 132:   // randomSeed(analogRead(0));  // seed PRNG
209: 133:   // Init Serial2 untuk PZEM
210: 134:   // PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
211: 135: }
212: 136: 
213: 137: // ================== LOOP ==================
214: 138: void loop() {
215: 139:   if (!client.connected()) reconnect();
216: 140:   client.loop();
217: 141: 
218: 142:   btn.update();
219: 143: 
220: 144:   // WiFi auto check tiap 30 detik
221: 145:   static unsigned long lastWiFiCheck = 0;
222: 146:   if (millis() - lastWiFiCheck > 30000) {
223: 147:     lastWiFiCheck = millis();
224: 148:     if (!wifiManager.isConnected()) {
225: 149:       Serial.println("WiFi terputus, mencoba koneksi ulang...");
226: 150:       wifiManager.connectBestNetwork();
227: 151:     } else if(wifiManager.isConnected()) {
228: 152:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
229: 153:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
230: 154:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
231: 155:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
232: 156:       Serial.println("🔄🔄🔄🔄🔄 Wifi Re-checker 🔄🔄🔄🔄🔄");
233: 157:     }
234: 158:   }
235: 159: 
236: 160:   // resetEnergy.handle(pzems, NUM_PZEMS);
237: 161:   if (btn.isShortPressed()) {
238: 162:     uint8_t next = (oled.getPage() + 1) % 3;
239: 163:     oled.setPage(next);
240: 164:   }
241: 165: 
242: 166:   if (btn.isLongPressed()) {
243: 167:     // panggil reset untuk semua PZEM
244: 168:     resetEnergy.resetAll(pzems, NUM_PZEMS);
245: 169:   }
246: 170: 
247: 171:   unsigned long now = millis();
248: 172:   if (now - lastMsg > 500) {  // interval 1 detik
249: 173:     lastMsg = now;
250: 174: 
251: 175:     for (int i = 0; i < NUM_PZEMS; i++) {
252: 176:       float voltage = pzems[i].voltage();
253: 177:       float current = pzems[i].current();
254: 178:       float power   = pzems[i].power();
255: 179:       float energy  = pzems[i].energy()*1000;
256: 180:       float freq    = pzems[i].frequency();
257: 181:       float pf      = pzems[i].pf();
258: 182: 
259: 183:       // pzems[i].voltage();
260: 184:       // pzems[i].current();
261: 185:       // pzems[i].power();
262: 186:       // pzems[i].energy();
263: 187:       // pzems[i].frequency();
264: 188:       // pzems[i].pf();
265: 189: 
266: 190:       // float voltage, current, power, energy = 0, freq, pf;
267: 191:       // generateDummyData(voltage, current, power, energy, freq, pf);
268: 192: 
269: 193:       // Apparent Power & Reactive Power
270: 194:       float va = voltage * current;
271: 195:       float var = sqrt((va * va) - (power * power));
272: 196: 
273: 197:       // Simpan ke buffer
274: 198:       vol[i]      = voltage;
275: 199:       cur[i]      = current;
276: 200:       powe[i]     = power;
277: 201:       pff[i]      = pf;
278: 202:       freqArr[i]  = freq;
279: 203:       ener[i]     = energy;
280: 204:       st[i]       = lineStatus[i];
281: 205: 
282: 206:       // if(!isnan(voltage)){
283: 207:       //   lineStatus[i] = LINE_GOOD;
284: 208:       //   // Debug Serial
285: 209:       //   // Serial.print("PZEM "); Serial.print(i+1);
286: 210:       //   // Serial.print(" | V: "); Serial.print(voltage);
287: 211:       //   // Serial.print("V | I: "); Serial.print(current);
288: 212:       //   // Serial.print("A | f: "); Serial.print(freq);
289: 213:       //   // Serial.print("Hz | VA: "); Serial.print(va);
290: 214:       //   // Serial.print("VA | VAR: "); Serial.print(var);
291: 215:       //   // Serial.print("VAR | P: "); Serial.print(power);
292: 216:       //   // Serial.print("W | PF: "); Serial.print(pf);
293: 217:       //   // Serial.print(" | E: "); Serial.print(energy, 2);
294: 218:       //   // Serial.println("Wh");
295: 219:       // }else if(voltage < 199 && voltage > 1){
296: 220:       //   lineStatus[i] = LINE_UNDERVOLTAGE;
297: 221:       //   // Serial.println("⚡WARNING: 3-Phase is Running UNDERVOLTAGE!!!");
298: 222:       // }else{
299: 223:       //   lineStatus[i] = LINE_ISSUED;
300: 224:       //   voltage = current = power = freq = va = var = pf = 0;
301: 225:       //   // Serial.println("❌MAIN POWER ISSUED | PLEASE CHECK THE PROBE!!");
302: 226:       // }
303: 227: 
304: 228:       if (isnan(voltage) || voltage <= 0) {
305: 229:         lineStatus[i] = LINE_ISSUED; // Tidak ada data / probe error
306: 230:         voltage = current = power = freq = va = var = pf = 0;
307: 231:       }
308: 232:       else if (voltage < 199) {
309: 233:         lineStatus[i] = LINE_UNDERVOLTAGE; // Tegangan rendah
310: 234:       }
311: 235:       else {
312: 236:         lineStatus[i] = LINE_GOOD; // Normal
313: 237:       }
314: 238: 
315: 239:       // ============= SERIAL MONITOR =============
316: 240:       switch (lineStatus[i]) {
317: 241:         case LINE_GOOD:
318: 242:           Serial.printf("✅ Line %d: GOOD (V: %.1fV", i + 1, voltage);
319: 243:           // Serial.print("PZEM "); Serial.print(i+1);
320: 244:           Serial.print(" | I: "); Serial.print(current);
321: 245:           Serial.print("A | f: "); Serial.print(freq);
322: 246:           Serial.print("Hz | VA: "); Serial.print(va);
323: 247:           Serial.print("VA | VAR: "); Serial.print(var);
324: 248:           Serial.print("VAR | P: "); Serial.print(power);
325: 249:           Serial.print("W | PF: "); Serial.print(pf);
326: 250:           Serial.print(" | E: "); Serial.print(energy, 2);
327: 251:           Serial.println("Wh)");
328: 252:           break;
329: 253:         case LINE_UNDERVOLTAGE:
330: 254:           Serial.printf("⚡ Line %d: UNDERVOLTAGE (V: %.1fV", i + 1, voltage);
331: 255:           Serial.print(" | I: "); Serial.print(current);
332: 256:           Serial.print("A | f: "); Serial.print(freq);
333: 257:           Serial.print("Hz | VA: "); Serial.print(va);
334: 258:           Serial.print("VA | VAR: "); Serial.print(var);
335: 259:           Serial.print("VAR | P: "); Serial.print(power);
336: 260:           Serial.print("W | PF: "); Serial.print(pf);
337: 261:           Serial.print(" | E: "); Serial.print(energy, 2);
338: 262:           Serial.println("Wh)");
339: 263:           break;
340: 264:         case LINE_ISSUED:
341: 265:           Serial.printf("❌ Line %d: ISSUED / PROBE ERROR\n", i + 1);
342: 266:           break;
343: 267:       }
344: 268: 
345: 269: 
346: 270:       // Publish ke MQTT
347: 271:       client.publish(workshop_v[i], String(voltage).c_str());
348: 272:       client.publish(workshop_i[i], String(current).c_str());
349: 273:       client.publish(workshop_p[i], String(power).c_str());
350: 274:       client.publish(workshop_e[i], String(energy).c_str());
351: 275:       client.publish(workshop_f[i], String(freq).c_str());
352: 276:       client.publish(workshop_pf[i], String(pf).c_str());
353: 277:       client.publish(workshop_va[i], String(va).c_str());
354: 278:       client.publish(workshop_var[i], String(var).c_str());
355: 279: 
356: 280:       // Publish status line
357: 281:       const char* statusStr = (lineStatus[i] == LINE_GOOD) ? "good"
358: 282:                             : (lineStatus[i] == LINE_UNDERVOLTAGE) ? "undervoltage"
359: 283:                             : "issued";
360: 284:       client.publish(line_status[i], statusStr);
361: 285:     }
362: 286: 
363: 287:     Serial.println("=== Data Published ===");
364: 288:     oled.update(vol, cur, powe, pff, freqArr, ener, st);
365: 289:   }
366: 290: }
367: 
368: (End of file - total 290 lines)
369: </content>
370: ```
371: 
372: **Tool: read**
373: 
374: **Input:**
375: ```json
376: {
377:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.cpp"
378: }
379: ```
380: 
381: **Output:**
382: ```
383: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.cpp</path>
384: <type>file</type>
385: <content>
386: 1: /******************************************************
387: 2:  * Project   : ESP32 MQTTs Publisher
388: 3:  * File      : credentials.h
389: 4:  * Author    : Muhammad Lutfi Nur Anendi
390: 5:  * Created   : 5 August 2025
391: 6:  * Last Edit : 23 October 2025
392: 7:  *
393: 8:  * Description:
394: 9:  *   Configuration file for WiFi, MQTT, and SSL credentials.
395: 10:  *   Rename this file from `credentials_example.cpp` → `credentials.cpp`
396: 11:  *   before compiling the project.
397: 12:  *
398: 13:  * IMPORTANT:
399: 14:  *   - DO NOT commit this file to any public repository.
400: 15:  *   - Keep your credentials private.
401: 16:  *   - Modify only the fields below as required.
402: 17:  *
403: 18:  * MQTT Notes:
404: 19:  *   - MQTT_PORT  = 1883 for non-SSL
405: 20:  *   - MQTTS_PORT = 8883 for SSL (secure)
406: 21:  *   - MQTT_TOPIC example: "user_topic/#"
407: 22:  *
408: 23:  * SSL Notes:
409: 24:  *   - CA_CERT should contain your Root Certificate between
410: 25:  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
411: 26:  *
412: 27:  * Developer Info:
413: 28:  *   GitHub   : https://github.com/lutfi-rpi5
414: 29:  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
415: 30:  *   Instagram: https://instagram.com/lutfiianendi
416: 31:  *
417: 32:  * Revision History:
418: 33:  *   v1.0  Initial release with MQTT & SSL credentials
419: 34:  *   v1.1  Added developer info and usage guide
420: 35:  ******************************************************/
421: 36: 
422: 37: #include "credentials.h"
423: 38: 
424: 39: // ---------- WiFi Configuration ----------
425: 40: const char* WIFI_SSID1 = "PTBA-LOGGER";
426: 41: const char* WIFI_PASS1 = "katekpasswordnyo";
427: 42: 
428: 43: const char* WIFI_SSID2 = "FLA 2.4G";
429: 44: const char* WIFI_PASS2 = "20092009";
430: 45: 
431: 46: const char* WIFI_SSID3 = "PTBA-KERTAPATI";
432: 47: const char* WIFI_PASS3 = "Spirit7jutaton";
433: 48: 
434: 49: const char* WIFI_SSID4 = "rpi-5";
435: 50: const char* WIFI_PASS4 = "getalifebro";
436: 51: 
437: 52: // ---------- WiFi Networks List ----------
438: 53: WiFiCredential wifiList[] = {
439: 54:   {WIFI_SSID2, WIFI_PASS2, 0},   // Highest priority
440: 55:   {WIFI_SSID4, WIFI_PASS4, 1},
441: 56:   {WIFI_SSID1, WIFI_PASS1, 2},
442: 57:   {WIFI_SSID3, WIFI_PASS3, 3}
443: 58: };
444: 59: const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);
445: 60: 
446: 61: // ---------- MQTT Broker Configuration ----------
447: 62: const char* MQTT_SERVER = "broker.avisha.id"; // Broker address (Avisha Broker) 103.127.97.247
448: 63: const int   MQTT_PORT   = 1883;               // Non-SSL port
449: 64: const int   MQTTS_PORT  = 8884;               // SSL port
450: 65: const int   WS_PORT     = 8083;               // Non-SSL port
451: 66: const int   WSS_PORT    = 8084;               // SSL port
452: 67: const char* MQTT_USER   = "lutpiii";          // MQTT Username
453: 68: const char* MQTT_PASS   = "lutpiiiMQTT";      // MQTT Password
454: 69: const char* MQTT_TOPIC  = "lutpiii/";         // Topic prefix (optional)
455: 70: 
456: 71: // ---------- Optional API Key ----------
457: 72: const char* API_KEY = "";  // For future REST API integration
458: 73: 
459: 74: // ---------- Root CA Certificate ----------
460: 75: // const char* CA_CERT = 
461: 76: // "-----BEGIN CERTIFICATE-----\n"
462: 77: // // Drop your CA certificate content here
463: 78: // "-----END CERTIFICATE-----\n";
464: 79: 
465: 80: const char *CA_CERT = R"EOF(
466: 81: -----BEGIN CERTIFICATE-----
467: 82: MIIDtTCCAp2gAwIBAgIUOKMAqMXFCssUwLZtEx+8/dj+d/AwDQYJKoZIhvcNAQEL
468: 83: BQAwajELMAkGA1UEBhMCSUQxEDAOBgNVBAgMB0pha2FydGExEDAOBgNVBAcMB0ph
469: 84: a2FydGExDTALBgNVBAoMBEVNUVgxDTALBgNVBAsMBE1RVFQxGTAXBgNVBAMMEGJy
470: 85: b2tlci5hdmlzaGEuaWQwHhcNMjUxMTAyMTEzODM2WhcNMjYxMTAyMTEzODM2WjBq
471: 86: MQswCQYDVQQGEwJJRDEQMA4GA1UECAwHSmFrYXJ0YTEQMA4GA1UEBwwHSmFrYXJ0
472: 87: YTENMAsGA1UECgwERU1RWDENMAsGA1UECwwETVFUVDEZMBcGA1UEAwwQYnJva2Vy
473: 88: LmF2aXNoYS5pZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMAvKNx4
474: 89: /EBrDB/vhjL6L+hnoRuM623QFqkmje+I7YjsL0ehNTBnTCTbI0giflQlx0lE1OUC
475: 90: r3HbahOFovFJGbMbLF3JgKvJ7rtmfEhaURyuSNCAVJRxmnEFt+OgB6SntSQaJ0zP
476: 91: q5DT2EapWdiwbFkYsDTO6kyRtkBReZVc4rhTCaHFJnxEab3DbzVzteC2g8GSv0NX
477: 92: clO3+6yi9bRh0uH0u3rfux4I9r4GMYF9vdC7NDDaVFeHIsQRXKW1dh4KajdMnJvl
478: 93: K25whh4HAEryZ5JJsyCviWcmjZhvPW/WEha1ORPjpcw3dbCSa40zSQVoFKgJAPvJ
479: 94: mcsCKk6TUT8ENiUCAwEAAaNTMFEwHQYDVR0OBBYEFHDNv7W0hpVlgFt82uqmGU0O
480: 95: AnPuMB8GA1UdIwQYMBaAFHDNv7W0hpVlgFt82uqmGU0OAnPuMA8GA1UdEwEB/wQF
481: 96: MAMBAf8wDQYJKoZIhvcNAQELBQADggEBAIOK9N+TF7t/2bsbX3RlFzKG6Nn+qkA4
482: 97: KNXEbfHhgKN/xRrZWw5ay8v+mhwTKh/bgv2qb8sEVN4+79cgio+azzGGVTOsYQ0R
483: 98: bWblQy40xPZSzylEHPLtxDEvy/iQLeenV3MFIGRNgzDOzum5zEH6Vna81LaGyqOf
484: 99: W2fzRa0QuyV8i+NzUIDwKd2S6UBMqAqCkmoL3c8cAFXMabVuN3Lz+co0yebDX2kW
485: 100: SihneS6XYxxRAgoien4o814k4rP1OYarFUNSK7JFOOj7uyAr/3VkgX3e0n1M0IfD
486: 101: vzvlEDsvCUU3FP8IN5bhgF+sth/w6K4qUsmsWzdds4HJoMjFBpCQwyA=
487: 102: -----END CERTIFICATE-----
488: 103: )EOF";
489: 104: 
490: 105: 
491: 
492: (End of file - total 105 lines)
493: </content>
494: ```
495: 
496: **Tool: read**
497: 
498: **Input:**
499: ```json
500: {
501:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.h"
502: }
503: ```
504: 
505: **Output:**
506: ```
507: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credentials.h</path>
508: <type>file</type>
509: <content>
510: 1: /******************************************************
511: 2:  * Project   : ESP32 MQTTs Publisher
512: 3:  * File      : credentials.h
513: 4:  * Author    : Muhammad Lutfi Nur Anendi
514: 5:  * Created   : 5 August 2025
515: 6:  * Last Edit : 23 October 2025
516: 7:  *
517: 8:  * Description:
518: 9:  *   Configuration file for WiFi, MQTT, and SSL credentials.
519: 10:  *   Rename this file from `credentials_example.h` → `credentials.h`
520: 11:  *   before compiling the project.
521: 12:  *
522: 13:  * IMPORTANT:
523: 14:  *   - DO NOT commit this file to any public repository.
524: 15:  *   - Keep your credentials private.
525: 16:  *   - Modify only the fields below as required.
526: 17:  *
527: 18:  * MQTT Notes:
528: 19:  *   - MQTT_PORT  = 1883 for non-SSL
529: 20:  *   - MQTTS_PORT = 8883 for SSL (secure)
530: 21:  *   - MQTT_TOPIC example: "user_topic/#"
531: 22:  *
532: 23:  * SSL Notes:
533: 24:  *   - CA_CERT should contain your Root Certificate between
534: 25:  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
535: 26:  *
536: 27:  * Developer Info:
537: 28:  *   GitHub   : https://github.com/lutfi-rpi5
538: 29:  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
539: 30:  *   Instagram: https://instagram.com/lutfiianendi
540: 31:  *
541: 32:  * Revision History:
542: 33:  *   v1.0  Initial release with MQTT & SSL credentials
543: 34:  *   v1.1  Added developer info and usage guide
544: 35:  ******************************************************/
545: 36: 
546: 37: /******************************************************
547: 38:  * Project   : ESP32 MQTTs Publisher
548: 39:  * File      : credentials.h
549: 40:  * Author    : Muhammad Lutfi Nur Anendi
550: 41:  * Created   : 5 August 2025
551: 42:  * Last Edit : 23 October 2025
552: 43:  *
553: 44:  * Description:
554: 45:  *   Configuration header for WiFi, MQTT, and SSL credentials.
555: 46:  ******************************************************/
556: 47: 
557: 48: #ifndef CREDENTIALS_H
558: 49: #define CREDENTIALS_H
559: 50: 
560: 51: // ---------- WiFi Credential Struct ----------
561: 52: struct WiFiCredential {
562: 53:   const char* ssid;
563: 54:   const char* password;
564: 55:   int priority;
565: 56: };
566: 57: 
567: 58: // ---------- WiFi Configuration ----------
568: 59: extern const char* WIFI_SSID1;
569: 60: extern const char* WIFI_PASS1;
570: 61: extern const char* WIFI_SSID2;
571: 62: extern const char* WIFI_PASS2;
572: 63: extern const char* WIFI_SSID3;
573: 64: extern const char* WIFI_PASS3;
574: 65: extern const char* WIFI_SSID4;
575: 66: extern const char* WIFI_PASS4;
576: 67: 
577: 68: extern WiFiCredential wifiList[];
578: 69: extern const int WIFI_COUNT;
579: 70: 
580: 71: // ---------- MQTT Broker Configuration ----------
581: 72: extern const char* MQTT_SERVER;
582: 73: extern const int   MQTT_PORT;
583: 74: extern const int   MQTTS_PORT;
584: 75: extern const int   WS_PORT;
585: 76: extern const int   WSS_PORT;
586: 77: extern const char* MQTT_USER;
587: 78: extern const char* MQTT_PASS;
588: 79: extern const char* MQTT_TOPIC;
589: 80: 
590: 81: // ---------- Optional API Key ----------
591: 82: extern const char* API_KEY;
592: 83: 
593: 84: // ---------- Root CA Certificate ----------
594: 85: extern const char* CA_CERT;
595: 86: 
596: 87: #endif  // CREDENTIALS_H
597: 88: 
598: 89: 
599: 
600: (End of file - total 89 lines)
601: </content>
602: ```
603: 
604: **Tool: read**
605: 
606: **Input:**
607: ```json
608: {
609:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/button.cpp"
610: }
611: ```
612: 
613: **Output:**
614: ```
615: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/button.cpp</path>
616: <type>file</type>
617: <content>
618: 1: #include "Button.h"
619: 2: 
620: 3: Button::Button(uint8_t pin, unsigned long longPressTime, unsigned long debounceTime)
621: 4:     : _pin(pin), _longPressTime(longPressTime), _debounceTime(debounceTime) {
622: 5:     pinMode(_pin, INPUT_PULLUP);
623: 6:     _state = HIGH;
624: 7:     _lastState = HIGH;
625: 8:     _pressStart = 0;
626: 9:     _longPressHandled = false;
627: 10:     _shortPressDetected = false;
628: 11:     _longPressDetected = false;
629: 12: }
630: 13: 
631: 14: void Button::update() {
632: 15:     bool reading = digitalRead(_pin);
633: 16: 
634: 17:     if (reading != _lastState) {
635: 18:         delay(_debounceTime);
636: 19:     }
637: 20: 
638: 21:     if (reading != _state) {
639: 22:         _state = reading;
640: 23: 
641: 24:         if (_state == LOW) {
642: 25:             _pressStart = millis();
643: 26:             _longPressHandled = false;
644: 27:         } else {
645: 28:             unsigned long pressDuration = millis() - _pressStart;
646: 29:             if (pressDuration < _longPressTime && !_longPressHandled) {
647: 30:                 _shortPressDetected = true;
648: 31:             }
649: 32:         }
650: 33:     }
651: 34: 
652: 35:     if (_state == LOW && !_longPressHandled && (millis() - _pressStart >= _longPressTime)) {
653: 36:         _longPressHandled = true;
654: 37:         _longPressDetected = true;
655: 38:     }
656: 39: 
657: 40:     _lastState = reading;
658: 41: }
659: 42: 
660: 43: bool Button::isShortPressed() {
661: 44:     if (_shortPressDetected) {
662: 45:         _shortPressDetected = false;
663: 46:         return true;
664: 47:     }
665: 48:     return false;
666: 49: }
667: 50: 
668: 51: bool Button::isLongPressed() {
669: 52:     if (_longPressDetected) {
670: 53:         _longPressDetected = false;
671: 54:         return true;
672: 55:     }
673: 56:     return false;
674: 57: }
675: 
676: (End of file - total 57 lines)
677: </content>
678: ```
679: 
680: **Tool: read**
681: 
682: **Input:**
683: ```json
684: {
685:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/button.h"
686: }
687: ```
688: 
689: **Output:**
690: ```
691: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/button.h</path>
692: <type>file</type>
693: <content>
694: 1: #ifndef BUTTON_H
695: 2: #define BUTTON_H
696: 3: 
697: 4: #include <Arduino.h>
698: 5: 
699: 6: class Button {
700: 7: public:
701: 8:     Button(uint8_t pin, unsigned long longPressTime = 5000, unsigned long debounceTime = 50);
702: 9: 
703: 10:     void update();
704: 11:     bool isShortPressed();
705: 12:     bool isLongPressed();
706: 13: 
707: 14: private:
708: 15:     uint8_t _pin;
709: 16:     unsigned long _longPressTime;
710: 17:     unsigned long _debounceTime;
711: 18: 
712: 19:     bool _state;
713: 20:     bool _lastState;
714: 21:     unsigned long _pressStart;
715: 22:     bool _longPressHandled;
716: 23:     bool _shortPressDetected;
717: 24:     bool _longPressDetected;
718: 25: };
719: 26: 
720: 27: #endif
721: 
722: (End of file - total 27 lines)
723: </content>
724: ```
725: 
726: **Tool: read**
727: 
728: **Input:**
729: ```json
730: {
731:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/DisplayOLED.cpp"
732: }
733: ```
734: 
735: **Output:**
736: ```
737: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/DisplayOLED.cpp</path>
738: <type>file</type>
739: <content>
740: 1: #include "DisplayOLED.h"
741: 2: 
742: 3: DisplayOLED::DisplayOLED(uint8_t address)
743: 4:   : display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1), page(0), lastUpdate(0), autoScroll(true)
744: 5: {}
745: 6: 
746: 7: void DisplayOLED::begin() {
747: 8:   Wire.begin(); // default SDA=21 SCL=22, override di main kalau perlu
748: 9:   if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
749: 10:     Serial.println("[OLED] SSD1306 allocation failed");
750: 11:     return;
751: 12:   }
752: 13:   display.clearDisplay();
753: 14:   display.setTextSize(1);
754: 15:   display.setTextColor(SSD1306_WHITE);
755: 16:   display.setCursor(0,0);
756: 17:   display.println("Power Logger init...");
757: 18:   display.display();
758: 19:   delay(500);
759: 20:   display.clearDisplay();
760: 21: }
761: 22: 
762: 23: void DisplayOLED::setPage(uint8_t p) {
763: 24:   page = p % 3;
764: 25:   // paksa refresh berikutnya
765: 26:   lastUpdate = 0;
766: 27: }
767: 28: 
768: 29: void DisplayOLED::enableAutoScroll(bool en) {
769: 30:   autoScroll = en;
770: 31: }
771: 32: 
772: 33: const char* DisplayOLED::statusToText(int st) {
773: 34:   switch(st) {
774: 35:     case 0: return "OK";
775: 36:     case 1: return "LOW";
776: 37:     default: return "ERR";
777: 38:   }
778: 39: }
779: 40: 
780: 41: void DisplayOLED::update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]) {
781: 42:   unsigned long now = millis();
782: 43:   if (now - lastUpdate < interval) return;
783: 44:   lastUpdate = now;
784: 45: 
785: 46:   display.clearDisplay();
786: 47:   display.setCursor(0,0);
787: 48: 
788: 49:   switch(page) {
789: 50:     case 0: drawPage1(v, i); break;
790: 51:     case 1: drawPage2(p, pf, f); break;
791: 52:     case 2: drawPage3(e, status); break;
792: 53:     default: drawPage1(v,i); break;
793: 54:   }
794: 55: 
795: 56:   display.display();
796: 57: 
797: 58:   // hanya auto-advance jika diijinkan
798: 59:   if (autoScroll) {
799: 60:     page = (page + 1) % 3;
800: 61:   }
801: 62: }
802: 63: 
803: 64: void DisplayOLED::drawPage1(float v[3], float i[3]) {
804: 65:   display.setTextSize(1);
805: 66:   display.println("    V     I");
806: 67:   for (int l = 0; l < 3; ++l) {
807: 68:     // rapiin agar muat 128x64
808: 69:     char buf[32];
809: 70:     snprintf(buf, sizeof(buf), "L%d %3.0fV %4.1fA", l+1, v[l], i[l]);
810: 71:     display.println(buf);
811: 72:   }
812: 73: }
813: 74: 
814: 75: void DisplayOLED::drawPage2(float p[3], float pf[3], float f[3]) {
815: 76:   display.setTextSize(1);
816: 77:   display.println("    P(W)   PF   F(Hz)");
817: 78:   for (int l = 0; l < 3; ++l) {
818: 79:     char buf[40];
819: 80:     snprintf(buf, sizeof(buf), "L%d %5.0fW  %.2f  %.1f", l+1, p[l], pf[l], f[l]);
820: 81:     display.println(buf);
821: 82:   }
822: 83: }
823: 84: 
824: 85: void DisplayOLED::drawPage3(float e[3], int status[3]) {
825: 86:   display.setTextSize(1);
826: 87:   display.println("   E(kWh)    STATUS");
827: 88:   for (int l = 0; l < 3; ++l) {
828: 89:     char buf[40];
829: 90:     // tampilkan energy dalam kWh (assume e[] dalam Wh, jika beda sesuaikan)
830: 91:     snprintf(buf, sizeof(buf), "L%d %.2fkWh     %s", l+1, e[l]/1000.0, statusToText(status[l]));
831: 92:     display.println(buf);
832: 93:   }
833: 94: }
834: 
835: (End of file - total 94 lines)
836: </content>
837: ```
838: 
839: **Tool: read**
840: 
841: **Input:**
842: ```json
843: {
844:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/DisplayOLED.h"
845: }
846: ```
847: 
848: **Output:**
849: ```
850: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/DisplayOLED.h</path>
851: <type>file</type>
852: <content>
853: 1: #pragma once
854: 2: #include <Arduino.h>
855: 3: #include <Wire.h>
856: 4: #include <Adafruit_GFX.h>
857: 5: #include <Adafruit_SSD1306.h>
858: 6: 
859: 7: #define OLED_WIDTH 128
860: 8: #define OLED_HEIGHT 64
861: 9: #define OLED_ADDR 0x3C
862: 10: 
863: 11: class DisplayOLED {
864: 12: public:
865: 13:     DisplayOLED(uint8_t address = OLED_ADDR);
866: 14:     void begin();
867: 15:     // update dipanggil tiap loop agar non-blocking
868: 16:     void update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]);
869: 17: 
870: 18:     // Kontrol page dari luar (button)
871: 19:     void setPage(uint8_t p);
872: 20:     void enableAutoScroll(bool en); // kalau false, page hanya diganti oleh setPage
873: 21:     uint8_t getPage() const { return page; }
874: 22: 
875: 23: private:
876: 24:     Adafruit_SSD1306 display;
877: 25:     uint8_t page;
878: 26:     unsigned long lastUpdate;
879: 27:     const uint16_t interval = 800; // refresh tiap 800ms
880: 28: 
881: 29:     bool autoScroll;
882: 30: 
883: 31:     void drawPage1(float v[3], float i[3]);
884: 32:     void drawPage2(float p[3], float pf[3], float f[3]);
885: 33:     void drawPage3(float e[3], int status[3]);
886: 34:     const char* statusToText(int st);
887: 35: };
888: 
889: (End of file - total 35 lines)
890: </content>
891: ```
892: 
893: **Tool: read**
894: 
895: **Input:**
896: ```json
897: {
898:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/ResetEnergy.cpp"
899: }
900: ```
901: 
902: **Output:**
903: ```
904: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/ResetEnergy.cpp</path>
905: <type>file</type>
906: <content>
907: 1: #include "ResetEnergy.h"
908: 2: 
909: 3: ResetEnergy::ResetEnergy(uint8_t buttonPin, uint8_t ledPin)
910: 4:   : _buttonPin(buttonPin), _ledPin(ledPin)
911: 5: {}
912: 6: 
913: 7: void ResetEnergy::begin() {
914: 8:   pinMode(_buttonPin, INPUT_PULLUP);
915: 9:   pinMode(_ledPin, OUTPUT);
916: 10:   digitalWrite(_ledPin, LOW);
917: 11: }
918: 12: 
919: 13: // handle() kamu mungkin sudah ada — jangan ubah kalau jalan.
920: 14: // Namun kita tetap sediakan resetAll() untuk dipanggil dari main ketika long press terdeteksi.
921: 15: void ResetEnergy::handle(PZEM004Tv30 pzems[], int count) {
922: 16:   // Bisa diisi dengan logic lama jika mau; tapi untuk modularity kita biarkan main memanggil resetAll()
923: 17:   // sehingga handle tetap optional. Jika kamu ingin, implementasi lama bisa ditempatkan di sini.
924: 18: }
925: 19: 
926: 20: // resetAll: kirim perintah reset energy ke tiap PZEM.
927: 21: // Perhatikan: library pzem biasanya punya method resetEnergy() atau resetEnergy(addr). 
928: 22: // Kita asumsikan instance PZEM004Tv30 punya method resetEnergy()
929: 23: void ResetEnergy::resetAll(PZEM004Tv30 pzems[], int count) {
930: 24:   // Indikasi: LED nyala menandakan reset segera dilakukan
931: 25:   digitalWrite(_ledPin, HIGH);
932: 26:   delay(100);
933: 27: 
934: 28:   for (int i = 0; i < count; ++i) {
935: 29:     // Cek apakah instance valid, lalu panggil reset
936: 30:     // fungsi API library PZEM biasanya: pzems[i].resetEnergy();
937: 31:     // Jika library kamu berbeda, ganti panggilan ini sesuai API.
938: 32:     Serial.printf("[ResetEnergy] Resetting PZEM %d...\n", i+1);
939: 33:     pzems[i].resetEnergy(); // <- asumsikan ada
940: 34:     // beri jeda singkat supaya bus serial/uart tidak kebanjiran
941: 35:     delay(200);
942: 36:   }
943: 37: 
944: 38:   // Blink LED 5x sebagai konfirmasi (final)
945: 39:   blinkLed(5, 120, 120);
946: 40: 
947: 41:   // Matikan LED
948: 42:   digitalWrite(_ledPin, LOW);
949: 43: }
950: 44: 
951: 45: void ResetEnergy::blinkLed(int times, unsigned int onMs, unsigned int offMs) {
952: 46:   for (int t = 0; t < times; ++t) {
953: 47:     digitalWrite(_ledPin, HIGH);
954: 48:     delay(onMs);
955: 49:     digitalWrite(_ledPin, LOW);
956: 50:     delay(offMs);
957: 51:   }
958: 52: }
959: 
960: (End of file - total 52 lines)
961: </content>
962: ```
963: 
964: **Tool: read**
965: 
966: **Input:**
967: ```json
968: {
969:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/ResetEnergy.h"
970: }
971: ```
972: 
973: **Output:**
974: ```
975: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/ResetEnergy.h</path>
976: <type>file</type>
977: <content>
978: 1: #pragma once
979: 2: #include <Arduino.h>
980: 3: #include <PZEM004Tv30.h>
981: 4: 
982: 5: class ResetEnergy {
983: 6: public:
984: 7:     ResetEnergy(uint8_t buttonPin, uint8_t ledPin);
985: 8:     void begin();
986: 9:     // dipanggil rutin di loop untuk handle long press flow (sudah kamu punya sebelumnya)
987: 10:     void handle(PZEM004Tv30 pzems[], int count);
988: 11: 
989: 12:     // fungsi publik baru yang bisa dipanggil kapanpun untuk reset semua PZEM
990: 13:     // fungsi ini blocking ringan: mengirim perintah satu-per-satu dengan delay kecil
991: 14:     void resetAll(PZEM004Tv30 pzems[], int count);
992: 15: 
993: 16: private:
994: 17:     uint8_t _buttonPin;
995: 18:     uint8_t _ledPin;
996: 19: 
997: 20:     // internal helper
998: 21:     void blinkLed(int times, unsigned int onMs = 150, unsigned int offMs = 150);
999: 22: };
1000: 
1001: (End of file - total 22 lines)
1002: </content>
1003: ```
1004: 
1005: **Tool: read**
1006: 
1007: **Input:**
1008: ```json
1009: {
1010:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/WifiSelector.cpp"
1011: }
1012: ```
1013: 
1014: **Output:**
1015: ```
1016: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/WifiSelector.cpp</path>
1017: <type>file</type>
1018: <content>
1019: 1: #include "WiFiSelector.h"
1020: 2: 
1021: 3: WiFiSelector::WiFiSelector(unsigned long timeout) {
1022: 4:     _timeout = timeout;
1023: 5: }
1024: 6: 
1025: 7: bool WiFiSelector::connectBestNetwork() {
1026: 8:     Serial.println("\n[WiFiSelector] Memindai jaringan...");
1027: 9:     int n = WiFi.scanNetworks();
1028: 10: 
1029: 11:     if (n == 0) {
1030: 12:         Serial.println("Tidak ada jaringan ditemukan!");
1031: 13:         return false;
1032: 14:     }
1033: 15: 
1034: 16:     for (int i = 0; i < WIFI_COUNT; i++) {
1035: 17:         for (int j = 0; j < n; j++) {
1036: 18:             String ssid = WiFi.SSID(j);
1037: 19:             if (ssid == wifiList[i].ssid) {
1038: 20:                 Serial.printf("Mencoba koneksi ke: %s (prioritas %d)\n", wifiList[i].ssid, wifiList[i].priority);
1039: 21:                 if (connectToNetwork(wifiList[i])) {
1040: 22:                     Serial.printf("Terhubung ke %s\n", wifiList[i].ssid);
1041: 23:                     return true;
1042: 24:                 }
1043: 25:             }
1044: 26:         }
1045: 27:     }
1046: 28: 
1047: 29:     Serial.println("Gagal terhubung ke semua jaringan.");
1048: 30:     return false;
1049: 31: }
1050: 32: 
1051: 33: bool WiFiSelector::connectToNetwork(const WiFiCredential& cred) {
1052: 34:     WiFi.begin(cred.ssid, cred.password);
1053: 35:     unsigned long startAttemptTime = millis();
1054: 36: 
1055: 37:     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < _timeout) {
1056: 38:         delay(500);
1057: 39:         Serial.print(".");
1058: 40:     }
1059: 41: 
1060: 42:     if (WiFi.status() == WL_CONNECTED) {
1061: 43:         Serial.printf("\nTerhubung ke %s, IP: %s\n", cred.ssid, WiFi.localIP().toString().c_str());
1062: 44:         return true;
1063: 45:     }
1064: 46: 
1065: 47:     Serial.printf("\nGagal konek ke %s\n", cred.ssid);
1066: 48:     WiFi.disconnect();
1067: 49:     return false;
1068: 50: }
1069: 51: 
1070: 52: bool WiFiSelector::isConnected() {
1071: 53:     return WiFi.status() == WL_CONNECTED;
1072: 54: }
1073: 55: 
1074: 56: void WiFiSelector::printStatus() {
1075: 57:     if (isConnected()) {
1076: 58:         Serial.printf("WiFi Aktif: %s | IP: %s\n",
1077: 59:                       WiFi.SSID().c_str(),
1078: 60:                       WiFi.localIP().toString().c_str());
1079: 61:     } else {
1080: 62:         Serial.println("WiFi tidak terhubung.");
1081: 63:     }
1082: 64: }
1083: 
1084: (End of file - total 64 lines)
1085: </content>
1086: ```
1087: 
1088: **Tool: read**
1089: 
1090: **Input:**
1091: ```json
1092: {
1093:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/WifiSelector.h"
1094: }
1095: ```
1096: 
1097: **Output:**
1098: ```
1099: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/WifiSelector.h</path>
1100: <type>file</type>
1101: <content>
1102: 1: #ifndef WIFI_SELECTOR_H
1103: 2: #define WIFI_SELECTOR_H
1104: 3: 
1105: 4: #include <WiFi.h>
1106: 5: #include "credentials.h"
1107: 6: 
1108: 7: class WiFiSelector {
1109: 8: public:
1110: 9:     WiFiSelector(unsigned long timeout = 10000);  // default 10 detik
1111: 10:     bool connectBestNetwork();                     // koneksi ke WiFi terbaik
1112: 11:     bool isConnected();                            // cek koneksi
1113: 12:     void printStatus();                            // tampilkan info koneksi
1114: 13: 
1115: 14: private:
1116: 15:     unsigned long _timeout;
1117: 16:     bool connectToNetwork(const WiFiCredential& cred);
1118: 17: };
1119: 18: 
1120: 19: #endif
1121: 
1122: (End of file - total 19 lines)
1123: </content>
1124: ```
1125: 
1126: **Tool: read**
1127: 
1128: **Input:**
1129: ```json
1130: {
1131:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credential_example.cpp"
1132: }
1133: ```
1134: 
1135: **Output:**
1136: ```
1137: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credential_example.cpp</path>
1138: <type>file</type>
1139: <content>
1140: 1: // /******************************************************
1141: 2: //  * Project   : ESP32 MQTTs Publisher
1142: 3: //  * File      : credentials.h
1143: 4: //  * Author    : Muhammad Lutfi Nur Anendi
1144: 5: //  * Created   : 5 August 2025
1145: 6: //  * Last Edit : 23 October 2025
1146: 7: //  *
1147: 8: //  * Description:
1148: 9: //  *   Configuration file for WiFi, MQTT, and SSL credentials.
1149: 10: //  *   Rename this file from `credentials_example.cpp` → `credentials.cpp`
1150: 11: //  *   before compiling the project.
1151: 12: //  *
1152: 13: //  * IMPORTANT:
1153: 14: //  *   - DO NOT commit this file to any public repository.
1154: 15: //  *   - Keep your credentials private.
1155: 16: //  *   - Modify only the fields below as required.
1156: 17: //  *
1157: 18: //  * MQTT Notes:
1158: 19: //  *   - MQTT_PORT  = 1883 for non-SSL
1159: 20: //  *   - MQTTS_PORT = 8883 for SSL (secure)
1160: 21: //  *   - MQTT_TOPIC example: "user_topic/#"
1161: 22: //  *
1162: 23: //  * SSL Notes:
1163: 24: //  *   - CA_CERT should contain your Root Certificate between
1164: 25: //  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
1165: 26: //  *
1166: 27: //  * Developer Info:
1167: 28: //  *   GitHub   : https://github.com/lutfi-rpi5
1168: 29: //  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
1169: 30: //  *   Instagram: https://instagram.com/lutfiianendi
1170: 31: //  *
1171: 32: //  * Revision History:
1172: 33: //  *   v1.0  Initial release with MQTT & SSL credentials
1173: 34: //  *   v1.1  Added developer info and usage guide
1174: 35: //  ******************************************************/
1175: 36: 
1176: 37: // #include "credentials.h"
1177: 38: 
1178: 39: // // ---------- WiFi Configuration ----------
1179: 40: // const char* WIFI_SSID1 = "";
1180: 41: // const char* WIFI_PASS1 = "";
1181: 42: 
1182: 43: // const char* WIFI_SSID2 = "";
1183: 44: // const char* WIFI_PASS2 = "";
1184: 45: 
1185: 46: // const char* WIFI_SSID3 = "";
1186: 47: // const char* WIFI_PASS3 = "";
1187: 48: 
1188: 49: // const char* WIFI_SSID4 = "";
1189: 50: // const char* WIFI_PASS4 = "";
1190: 51: 
1191: 52: // // ---------- WiFi Networks List ----------
1192: 53: // WiFiCredential wifiList[] = {
1193: 54: //   {WIFI_SSID1, WIFI_PASS1, 0},   // Highest priority
1194: 55: //   {WIFI_SSID4, WIFI_PASS4, 1},
1195: 56: //   {WIFI_SSID2, WIFI_PASS2, 2},
1196: 57: //   {WIFI_SSID3, WIFI_PASS3, 3}
1197: 58: // };
1198: 59: // const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);
1199: 60: 
1200: 61: // // ---------- MQTT Broker Configuration ----------
1201: 62: // const char* MQTT_SERVER = "";                 // Broker address
1202: 63: // const int   MQTT_PORT   = 1883;               // Non-SSL port
1203: 64: // const int   MQTTS_PORT  = 8883;               // SSL port
1204: 65: // const char* MQTT_USER   = "";                 // MQTT Username
1205: 66: // const char* MQTT_PASS   = "";                 // MQTT Password
1206: 67: // const char* MQTT_TOPIC  = "";                 // Topic prefix (optional)
1207: 68: 
1208: 69: // // ---------- Optional API Key ----------
1209: 70: // const char* API_KEY = "";  // For future REST API integration
1210: 71: 
1211: 72: // // ---------- Root CA Certificate ----------
1212: 73: // const char* CA_CERT = 
1213: 74: // "-----BEGIN CERTIFICATE-----\n"
1214: 75: // // Drop your CA certificate content here
1215: 76: // "-----END CERTIFICATE-----\n";
1216: 77: 
1217: 78: 
1218: 
1219: (End of file - total 78 lines)
1220: </content>
1221: ```
1222: 
1223: ---
1224: 
1225: ## Assistant (Plan · DeepSeek V4 Flash Free · 5.1s)
1226: 
1227: _Thinking:_
1228: 
1229: Now let me also check the brief files for additional context.
1230: 
1231: **Tool: read**
1232: 
1233: **Input:**
1234: ```json
1235: {
1236:   "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/brief.md"
1237: }
1238: ```
1239: 
1240: **Output:**
1241: ```
1242: <path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/brief.md</path>
1243: <type>file</type>
1244: <content>
1245: 1: ini adalah project yang sudah jadi dan sudah rilis v1.0.0
1246: 2: ini penjelasan project yang sekarang dan yang akan di update firmware nya
1247: 3: saya lampirkan program yang akan di kembangkan menjadi v2.0.0
1248: 4: 
1249: 5: Nama Project  : Data Logger 3 Phase
1250: 6: 
1251: 7: Overview Alat (Hardware)      : konteksnya alat ini hanya berfungsi untuk akuisisi data dan mengirimkan ke mqtt broker (tidak melakukan data logger), data logger sdh ada unit website yang khusus menangani data logging yang datanya di ambil dari mqtt broker
1252: 8:                                 1. Akuisisi Data dari 3 buah PZEM-004T (Masing2 PZEM handle 1 Line RST to Netral (WYE/220V))
1253: 9:                                 2. Pengolahan Data
1254: 10:                                 3. Publish Data ke MQTT Broker (per topic)
1255: 11:                                 4. Handle Display OLED 0.96" (display tampilan sangat alakadarnya) dan push button untuk ganti slide page parameter 3 phase
1256: 12:                                 5. bisa management signal wi-fi dan prioritasnya (masih manual (hard-code) atur priotitas)
1257: 13:                                 6. ada fitur debug berupa kirim data random ke broker namun masih harus buka tutup komentar di baris program (dan masih cuma punya 1 jenis debug aja)
1258: 14:                                 7. masih ada beberapa kekurangan/bug sistem seperti:
1259: 15:                                   a. [asinkron data] data antar parameter yang sampai ke broker atau website tidak serentak/tidak bersamaan (ada yang terlalu cepat/terlalu lambat sepersekian detik) karena masih publish mentah per topic dan belum pakai json
1260: 16:                                   b. [blocking] saat booting esp32 terus melakukan connecting ke wifi dan tidak akan masuk display utama sampai esp32 terkoneksi ke wifi
1261: 17:                                   c. belum ada fitur kalibrasi
1262: 18:                                   d. belum ada webserver & webpage (untuk config dan kalibrasi)
1263: 19:                                   e. masih ada sistem blocking
1264: 20:                                   f. belum ada state machine & self-healing kalau error di lapangan
1265: 21:                                   g. display OLED masih polosan raw text dan susunan parameter masih berantakan
1266: 22:                                   h. Wifi Selector belum seperti pintar network management windows maupun smartphone yang mempunyai known network dan pintar mencari prioritas serta otomatis tersambung 
1267: 23: 
1268: 24: Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin (Arduino IDE v2.0 & Board Package ESP32 v2.0.14)
1269: 25:                                 2. PZEM-004T 100A (3 buah, masing2 handle 1 fasa dari RST ke Netral (WYE/220V)) 
1270: 26:                                 3. OLED 0.96" I2C
1271: 27:                                 4. Push Button (Normally Open, Not latching) untuk Menu OLED
1272: 28:                                 5. Built-in LED ESP32 untuk signal indikator seperti reset kWh
1273: 29: 
1274: 30: Konfigurasi Pinout Electrical : 
1275: 31: //************* PZEM ****************//
1276: 32: - PZEM Fasa R dan S dalam satu hardware serial 1
1277: 33: - PZEM Fasa T berada dalam hardware serial 2 
1278: 34: 
1279: 35: #define PZEM_RX1_PIN 4
1280: 36: #define PZEM_TX1_PIN 15
1281: 37: #define PZEM_RX2_PIN 17
1282: 38: #define PZEM_TX2_PIN 16
1283: 39: // #define PZEM_SERIAL Serial2
1284: 40: HardwareSerial PZEMSerial1(1);
1285: 41: HardwareSerial PZEMSerial2(2);
1286: 42: #define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM
1287: 43: 
1288: 44: PZEM004Tv30 pzems[NUM_PZEMS] = {
1289: 45:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
1290: 46:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
1291: 47:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
1292: 48: };
1293: 49: //************* PZEM ****************//
1294: 50: 
1295: 51: //************* OLED 0.96" I2C ****************//
1296: 52: SDA= GPIO 21
1297: 53: SCL= GPIO 22
1298: 54: //************* OLED 0.96" I2C ****************//
1299: 55: 
1300: 56: //*************** Button & LED ****************//
1301: 57: BUTTON  5
1302: 58: LED     2
1303: 59: //*************** Button & LED ****************//
1304: 60: 
1305: 61: * Board Package & Libraries yang sudah diinstall di firmware Data Logger v1.0.0 ini (install via Board & Library Manager):
1306: 62: *   Board Package   : Board Package ESP32 v2.0.14 (pada firmware data logger v2.0.0 gunakan ESP32 v3.3.0)
1307: 63: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
1308: 64: *   - PZEM004Tv30          by Jakub Mandula     v1.2.1
1309: 65: *   - Adafruit SSD1306     by Adafruit          v2.5.16
1310: 66: *   - Adafruit GFX         by Adafruit          v1.12.5
1311: 67: *   - PubSubClient         by Nick O'Leary      v2.8
1312: 68: *
1313: 69: * Board Package & Libraries yang perlu diinstall untuk pembaruan firmware Data Logger di versi v2.0.0 nanti (install via Board & Library Manager):
1314: 70: *   Board Package   : Board Package ESP32 v3.3.0
1315: 71: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
1316: 72: *   - ArduinoJson          by Benoit Blanchon   v6.21.6
1317: 73: *   - ESP Async WebServer  by ESP32Async        v3.11.0
1318: 74: *   - Async TCP            by ESP32Async        v3.4.10
1319: 75: 
1320: 76: 
1321: 77: Yang Harus diperbaiki/diupdate adalah:
1322: 78: 1. Semua bagian sistem dan fungsi harus FULL asynchronous task/non blocking system kalau perlu gunakan dua core esp32 atau gunakan FreeRTOS
1323: 79: 2. [hardcode & webpage config] sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded (hanya bisa diakses oleh programmer lewat program) dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
1324: 80: 3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker dengan topic {PREFIX_TOPIC/MQTT_TOPIC} (prefix dan topic diambil dari eeprom berdasarkan configurasi web config) (buatlah file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js)
1325: 81:   struktur json harus seperti ini
1326: 82:   ```json
1327: 83: {
1328: 84:   "device": {
1329: 85:     "id":     "3ph-logger-001",   → DEVICE_ID dari config.h
1330: 86:     "fw":     "v2.1.0",           → FW_VERSION dari config.h
1331: 87:     "uptime": 3600,               → detik sejak boot (uint32)
1332: 88:     "heap":   148432,             → free heap bytes
1333: 89:     "rssi":   -65                 → sinyal WiFi dBm
1334: 90:   },
1335: 91:   "phases": {
1336: 92:     "R": {   → valid = true
1337: 93:       "v":      "220.1",      → voltage (V), 1 desimal
1338: 94:       "i":      "12.34",      → current (A), 2 desimal
1339: 95:       "p":      "2650.0",     → active power (W)
1340: 96:       "s":      "2714.6",     → apparent power (VA)
1341: 97:       "q":      "582.1",      → reactive power (VAr) = √(S²-P²)
1342: 98:       "pf":     "0.97",       → power factor (0.0–1.0), 2 desimal
1343: 99:       "f":      "50.0",       → frequency (Hz)
1344: 100:       "e":      "15230.0",    → energy kumulatif (Wh)
1345: 101:       "status": "OK"          → "OK" / "UNDER" / "OVER" / "LOST"
1346: 102:     },
1347: 103:     "S": {   → valid = true
1348: 104:       "v":      "200.6",
1349: 105:       "i":      "12.34",
1350: 106:       "p":      "2650.0",
1351: 107:       "s":      "2714.6",
1352: 108:       "q":      "582.1",
1353: 109:       "pf":     "0.97",
1354: 110:       "f":      "50.0",
1355: 111:       "e":      "15230.0",
1356: 112:       "status": "UNDER"
1357: 113:     },
1358: 114:     "T": {   → valid = false → early return
1359: 115:       "status": "LOST" → jika salah satu atau lebih dari 1 fasa mengalami lost voltage maka esp32 hanya akan mengirimkan status "LOST" saja
1360: 116:     },
1361: 117:     "unbalance": {
1362: 118:       "value": "4.25"
1363: 119:     }
1364: 120:   },
1365: 121:   "ts": 3601234  → millis() saat publish, bukan Unix time
1366: 122: }
1367: 123: ```
1368: 124: 4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi atau mqtt dulu baru bisa masuk ke sistem (tetap buat opening screen OLED dengan Loading bar progress nya)
1369: 125:     seharusnya sistem terus berjalan meskipun:
1370: 126:     a. tidak terhubung ke wi-fi dan/atau mqtt, atau
1371: 127:     b. hanya terhubung ke wi-fi namun tidak terhubung ke mqtt server
1372: 128: 5. [Tambah] tambah status line dari OK/UNDER/LOST menjadi OK/UNDER/OVER/LOST. serta memindahkan line status threshold dari hardcoded jadi eeprom di config.h
1373: 129:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
1374: 130:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
1375: 131:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
1376: 132: 7. [Tambah] tambahkan parameter unbalanced antar phase berdasarkan standard di indonesia, masukkan fungsi di DataAcquisition.h/.cpp
1377: 133:         ALGORITMA HITUNG VOLTAGE UNBALANCE 3 PHASE
1378: 134:         Input:
1379: 135:             VR = Tegangan phase R ke netral
1380: 136:             VS = Tegangan phase S ke netral
1381: 137:             VT = Tegangan phase T ke netral
1382: 138:         Langkah 1:
1383: 139:             Hitung tegangan rata-rata
1384: 140:             VAVG = (VR + VS + VT) / 3
1385: 141:         Langkah 2:
1386: 142:             Hitung deviasi tiap phase terhadap rata-rata
1387: 143:             DEV_R = |VR - VAVG|
1388: 144:             DEV_S = |VS - VAVG|
1389: 145:             DEV_T = |VT - VAVG|
1390: 146:         Langkah 3:
1391: 147:             Tentukan deviasi terbesar
1392: 148:             MAX_DEV = nilai terbesar dari:
1393: 149:                       DEV_R, DEV_S, DEV_T
1394: 150:         Langkah 4:
1395: 151:             Hitung persentase unbalance
1396: 152:             UNBALANCE = (MAX_DEV / VAVG) × 100
1397: 153:         Langkah 5:
1398: 154:             Tampilkan hasil unbalance dalam persen (%)
1399: 155: 
1400: 156:         --------------------------------------------------
1401: 157: 
1402: 158:         CONTOH DATA:
1403: 159:             VR = 220V
1404: 160:             VS = 210V
1405: 161:             VT = 228V
1406: 162:         PROSES:
1407: 163:         1. Hitung rata-rata
1408: 164:             VAVG = (220 + 210 + 228) / 3
1409: 165:             VAVG = 219.33V
1410: 166:         2. Hitung deviasi
1411: 167:             DEV_R = |220 - 219.33|
1412: 168:             DEV_R = 0.67V
1413: 169:             DEV_S = |210 - 219.33|
1414: 170:             DEV_S = 9.33V
1415: 171:             DEV_T = |228 - 219.33|
1416: 172:             DEV_T = 8.67V
1417: 173:         3. Cari deviasi terbesar
1418: 174:             MAX_DEV = 9.33V
1419: 175:         4. Hitung unbalance
1420: 176:             UNBALANCE = (9.33 / 219.33) × 100
1421: 177:             UNBALANCE = 4.25%
1422: 178:         HASIL:
1423: 179:             Voltage Unbalance = 4.25%
1424: 180: 
1425: 181: 8. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
1426: 182:   5a. Rule set Display:
1427: 183:     - display OLED 0.96" SSD1306 sepenuhnya dikontrol dengan 1 push button
1428: 184:     - kuncinya cuma ada 2     : 1. jika push button di tekan dengan interval biasa maka itu untuk mengganti page/memindahkan kursor menu ">"
1429: 185:                                 2. jika push button di tekan dengan interval 2 detik (customable webpage config) maka itu untuk melakukan "Enter" pada menu yang di pilih atau khususnya pada Monitoring Mode adalah untuk berpindah ke Menu Mode
1430: 186:     - saat di Monitoring Mode : 1. tekan biasa push button untuk slide page ke page berikutnya

(Output capped at 50 KB. Showing lines 1-1430. Use offset=1431 to continue.)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents/skills"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents/skills</path>
<type>directory</type>
<entries>
log-skill/

(1 entries)
</entries>
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/*.py"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/CMakeLists.txt"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/*.json"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/*.yaml"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/*.yml"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/*.toml"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/Makefile"
}
```

**Output:**
```
No files found
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/Dockerfile*"
}
```

**Output:**
```
No files found
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 33.3s)

_Thinking:_

Now I have a thorough understanding of the repository. Let me summarize my findings:

**Repository: Power Logger Hardware ESP32 (3-Phase Data Logger)**

This is an Arduino-based ESP32 firmware project for a 3-phase power data logger. It's a production-deployed v1.0.0 codebase undergoing a major refactor to v2.0.0.

**Key facts:**

1. **Project structure:**
   - Root has: `Data_Logger_Monitor/` (main firmware), `setAddressPZEM/` (utility sketches), `Data_Logger_Monitor.zip`
   - No README, no AGENTS.md, no package.json, no build system beyond Arduino IDE
   - No CI/CD, no linting, no testing infrastructure

2. **Current firmware (v1.0.0):**
   - Single `.ino` file (Data_Logger_Monitor.ino) serving as the main entry point
   - Modular classes: Button, WiFiSelector, DisplayOLED, ResetEnergy
   - Uses: PubSubClient, PZEM004Tv30, Adafruit SSD1306/GFX
   - Hardcoded credentials in `credentials.cpp` (gitignored: `credentials.cpp`)
   - `credential_example.cpp` provided as template

3. **Target v2.0.0 requirements (documented in `brief.md` and `revision to 2.0.0.md`):**
   - Full async/non-blocking with FreeRTOS
   - Centralized config (`config.h`) with hardcoded + webpage-editable sections
   - Switch from per-topic MQTT publishing to structured JSON payload
   - JSON schema: `{seq, device{id,fw,uptime,heap,rssi}, phases{R,S,T{valid,v,i,p,s,q,pf,f,e,status}}, unbalance, ts}`
   - MQTT topic hierarchy: `{prefix}/{topic}/{device_id}`
   - New line status: OK/UNDER/OVER/LOST (with configurable thresholds)
   - Voltage unbalance calculation
   - Professional OLED UI with Menu Mode and Monitoring Mode (3 pages)
   - Web server + AP with responsive HTML/CSS config pages
   - Storage via EEPROM/NVS
   - Calibration system with offset model
   - Self-healing mechanisms
   - LED signal indicators

4. **Hardware:**
   - MCU: ESP32 DevKit V1 30-pin
   - Sensors: 3x PZEM-004T 100A (WYE 220V system)
   - Display: OLED SSD1306 128x64 I2C
   - Input: 1x push button (GPIO5)
   - LED: Built-in GPIO2
   - PZEM serial: UART1 (RX=4, TX=15) for phases R,S; UART2 (RX=17, TX=16) for phase T
   - OLED I2C: SDA=21, SCL=22

5. **Libraries v1.0.0:**
   - ESP32 Board Package v2.0.14
   - PZEM004Tv30 v1.2.1
   - Adafruit SSD1306 v2.5.16
   - Adafruit GFX v1.12.5
   - PubSubClient v2.8

6. **New libraries for v2.0.0:**
   - ESP32 Board Package v3.3.0
   - ArduinoJson v6.21.6
   - ESP Async WebServer v3.11.0
   - Async TCP v3.4.10

7. **Repository quirks:**
   - `.gitignore` is a generic Node.js template (not really relevant for Arduino but does ignore `credentials.cpp`)
   - No test infrastructure (Arduino firmware - testing is manual on hardware)
   - No build system (Arduino IDE only)
   - Target language: English for code, docs, comments
   - Author: Muhammad Lutfi Nur Anendi

8. **Agent skills directory:** `.agents/skills/log-skill/` exists but I haven't read it.

Now let me write the AGENTS.md file. It should be compact, high-signal, and repo-specific.

Key points an agent would miss without help:
- This is an Arduino ESP32 project, not a standard web app
- v1.0.0 is production; v2.0.0 is the target refactor
- No package.json, no testing, no CI - it's Arduino IDE based
- Pin configuration is non-standard
- MQTT JSON schema is precisely specified
- `credentials.cpp` is gitignored; copy from `credential_example.cpp`
- Board package and library versions matter
- The brief.md and revision to 2.0.0.md contain the full v2 spec
- The OLED UI has exact ASCII-art layouts that must be followed
- Button logic (single button, short=nav, long=enter)
- These `brief-cl.md` files are for AI coding sessions and contain the development brief

Let me write a concise AGENTS.md.

Now I have a thorough understanding of the repository. Let me write the `AGENTS.md`.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents/skills/log-skill"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents/skills/log-skill</path>
<type>directory</type>
<entries>
SKILL.md

(1 entries)
</entries>
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 17.1s)

_Thinking:_

Let me check this skill file for any relevant context.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents/skills/log-skill/SKILL.md"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/.agents/skills/log-skill/SKILL.md</path>
<type>file</type>
<content>
1: ---
2: name: log-skill
3: description: Berikan log yang sudah dikerjakan
4: license: MIT
5: compatibility: opencode
6: metadata:
7:   audience: maintainers
8: ---
9: 
10: ## What I do
11: 
12: - Tambahkan log yang sudah dikerjakan di progress.txt
13: 
14: ## When To Use Me
15: 
16: - Setelah selesai membuat file, refactor, menjalankan task, atau memperbaiki bug
17: - setiap kali ada perubahan kode
18: 
19: ## How I Do It
20: 
21: 1. Read `progress.txt` untuk melihat log terakhir
22: 2. Append entry baru di bagian bawah file
23: 3. Group berdasarkan tanggal (YYYY/MM/DD), gunakan bullet points
24: 4. Format: aksi (`Modified`, `Created`, `Fixed`, `Deleted`)
25: 
26: ## Log Format
27: 
28: ```
29: ## 2025/05/30
30: 
31: ### Kategori Task
32: - [Modified] Mengubah `src/public/css/style.css` - ubah family-font 
33: - [Created] Membuat `src/middleware/auth.js` - membuat middleware untuk auth login
34: - [Fixed] Memperbaiki `src/public/pages/dashboard.html` - Memperbaiki div .value-card saling overlaping
35: - [Deleted] Menghapus `src/controllers/exportController.js` - File & Fitur di hapus
36: ```
37: **PENTING:** Selalu append, jangan overwrite file yang sudah ada 

(End of file - total 37 lines)
</content>
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 23.6s)

_Thinking:_

Now I have all the context I need. Let me write the AGENTS.md file. This is a compact, high-signal instruction file for the repository.

**Tool: write**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/AGENTS.md",
  "content": "# AGENTS.md — Power Logger Hardware ESP32\n\n## Project overview\n\nArduino-based ESP32 firmware for a 3-phase power data logger (3× PZEM-004T). v1.0.0 is production-deployed; v2.0.0 is an in-progress ground-up refactor. The device acquires data, publishes JSON to MQTT, and displays on OLED — it does **not** log data locally (that's handled by a backend subscribed to the broker).\n\n## Build & toolchain\n\n- **IDE:** Arduino IDE v2.0 (or PlatformIO as alternative)\n- **Board package:** ESP32 v2.0.14 (v1.x), upgrade to **v3.3.0** for v2.0.0\n- **Board URL:** `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-packages/package_esp32_index.json`\n- **No** package.json, CMakeLists, tests, CI, or linter — zero standard project infra\n- Verify by compiling in Arduino IDE; no automated tests exist\n\n## Key directories\n\n| Path | Purpose |\n|---|---|\n| `Data_Logger_Monitor/` | Main firmware entrypoint (`Data_Logger_Monitor.ino`) and all modules |\n| `Data_Logger_Monitor/.agents/skills/log-skill/` | Session log skill (append-only progress tracking) |\n| `setAddressPZEM/` | Standalone sketches for configuring PZEM-004T addresses |\n| `brief-cl.md`, `brief.md`, `revision to 2.0.0.md` | AI session prompts and full v2 spec — read before working on v2 features |\n\n## Current firmware (v1.0.0) architecture\n\n**Modular classes:** `Button`, `WiFiSelector`, `DisplayOLED`, `ResetEnergy` — each has `.h`/`.cpp`. The main `.ino` is monolithic (~290 lines).\n\n**Critical bugs to fix (documented in brief.md):**\n- MQTT publishes per-topic (desynced timestamps) — must switch to single JSON payload\n- Wi-Fi connect blocks boot — must become async/non-blocking\n- No calibration, no webserver, no state machine, no self-healing\n- OLED display is raw text\n- Debug requires commenting/uncommenting code\n\n## Hardware pinout (non-negotiable)\n\n```\nPZEM:  UART1 RX=4  TX=15  (phases R, S)   |   UART2 RX=17 TX=16  (phase T)\nOLED:  SDA=21 SCL=22  I2C addr 0x3C\nBUTTON: GPIO5  (INPUT_PULLUP)\nLED:    GPIO2  (built-in)\n```\n\n3× PZEM-004T all share address space: `0x10` (R), `0x11` (S), `0x12` (T).\n\n## Libraries\n\n**Installed (v1.x):** PZEM004Tv30 v1.2.1, Adafruit SSD1306 v2.5.16, Adafruit GFX v1.12.5, PubSubClient v2.8\n\n**Add for v2.0.0:** ArduinoJson v6.21.6, ESP Async WebServer v3.11.0, Async TCP v3.4.10\n\n## Credentials\n\n`credentials.cpp` is **gitignored**. Copy `Data_Logger_Monitor/credential_example.cpp` → `credentials.cpp` and fill in real values. Never commit credentials.\n\n## Target JSON schema (v2.0.0 MQTT payload)\n\nTopic: `{prefix}/{topic}/{device_id}` (hierarchical). Backend subscribes to `+/telemetry/+`.\n\n```json\n{\n  \"seq\": 1247,\n  \"device\": { \"id\": \"3ph-logger-001\", \"fw\": \"v2.1.0\", \"uptime\": 3600, \"heap\": 148432, \"rssi\": -65 },\n  \"phases\": {\n    \"R\": { \"valid\": true,  \"v\": 220.1, \"i\": 12.34, \"p\": 2650.0, \"s\": 2714.6, \"q\": 582.1, \"pf\": 0.97, \"f\": 50.0, \"e\": 15230.0, \"status\": \"OK\" },\n    \"S\": { \"valid\": true,  \"v\": 200.6, \"i\": 12.34, \"p\": 2650.0, \"s\": 2714.6, \"q\": 582.1, \"pf\": 0.97, \"f\": 50.0, \"e\": 15230.0, \"status\": \"UNDER\" },\n    \"T\": { \"valid\": false, \"v\": 0.0,   \"i\": 0.0,   \"p\": 0.0,    \"s\": 0.0,    \"q\": 0.0,   \"pf\": 0.0, \"f\": 0.0, \"e\": 0.0,    \"status\": \"LOST\" }\n  },\n  \"unbalance\": 4.25,\n  \"ts\": 1717001234\n}\n```\n\n- Numeric values as raw numbers (not strings), `ts` = Unix epoch (NTP), `valid: false` phases still send full schema with zeros.\n- Calibration offsets on V/I propagate to all derived params (VA, VAr, W, Wh, PF — but not Hz).\n\n## OLED UI requirements\n\nSingle button: **short press** = navigate/page/cursor; **long press (2s)** = enter/confirm/back.\n\nTwo modes with exact ASCII layouts in `brief.md` (reproduce faithfully):\n\n### Monitoring Mode (3 pages)\n1. Voltage/Current/Line Status + Unbalance %\n2. Frequency/Apparent Power/Reactive Power (auto-scale VA↔kVA, VAr↔kVAr)\n3. Power Factor/Active Power/Energy (auto-scale W↔kW, Wh↔kWh)\n\nTitle bar: `\"3-Phase Data Logger\"` with independent `W` (Wi‑Fi) and `Q` (MQTT) icons. Warning blinking for LOST/UNDER/OVER and unbalance threshold `[!]`.\n\n### Menu Mode (5 pages)\nConfig → Reset kWh → Reboot Device → Reset to Factory → Back to Monitoring. Each has confirmation/countdown screens.\n\n## Web server (v2.0.0)\n\nESP32 AP mode + HTTP server with responsive HTML/CSS. Pages: Manage Known Networks (table + add/delete), MQTT Config (server/ports/SSL/WS/topic), PZEM Calibration (V/I offsets per phase + thresholds). Login page default user/pass in `config.h`. All config stored in EEPROM/NVS.\n\n## Line status thresholds\n\nConfigurable from webpage (defaults in `config.h`):\n```\nVOLTAGE_OK_MIN    = 180.0f  // Below → UNDER\nVOLTAGE_LOST_MAX  = 80.0f   // Below → LOST\nVOLTAGE_OVER_MAX  = 240.0f  // Above → OVER\nUNBALANCE_MAX     = 3.0f    // Above → OLED warning [!]\n```\n\n## Non-blocking rules\n\n- No `delay()` except on boot screens or where truly unavoidable\n- Wi-Fi/MQTT connect must be async — device runs immediately without network\n- OLED must remain responsive during network ops\n- Use `millis()` scheduling, FreeRTOS tasks, and state machines\n- Watchdog-safe task design required\n\n## Development workflow notes\n\n- All code, comments, docs in **English**\n- Target audience: beginners — keep code readable with inline docs\n- `brief-cl.md` and `brief.md` are AI briefs for the v2 refactor; read them before implementing any v2 feature\n- Session logs go in `Data_Logger_Monitor/.agents/skills/log-skill/` via `progress.txt` (append only)\n- After any code change, update `progress.txt` following the log-skill format\n"
}
```

**Output:**
```
Wrote file successfully.
```

---

## Assistant (Build · DeepSeek V4 Flash Free · 10.8s)

_Thinking:_

The AGENTS.md file has been created. Let me verify it looks good by reading it back.

Written `AGENTS.md` to the repo root. It covers:

- **Project identity**: Arduino ESP32 firmware, not a web app — no tests/CI/linter
- **Build info**: IDE, board package versions, board URL, library versions for v1.x and v2.x
- **Key directories**: what each folder/file is for
- **v1.0.0 pain points**: per-topic MQTT, blocking Wi-Fi boot, missing features
- **Pinout**: exact non-negotiable GPIO assignments
- **Credentials**: gitignore + setup workflow
- **v2 JSON schema**: the precise shape backend expects
- **OLED UI**: short/long press semantics, monitoring pages, menu tree, title bar rules
- **Web server**: AP mode, pages, storage
- **Thresholds**: voltage/LOST/UNDER/OVER/unbalance defaults
- **Non-blocking rules**: no delay(), async connect, watchdog safety
- **Workflow conventions**: English only, beginner-targeted docs, session log format

Every line answers "would an agent miss this without help?" — fluff excluded.

---

## User

rancang firmware Data_Logger_Monitor v2.0.0 berdasarkan file Data_Logger_Monitor/revision to v2.0.0.md dengan membuat folder baru Data_Logger_Monitor_v2.0.0 di root folder ini, bagaimana plan mu?

---

## Assistant (Plan · DeepSeek V4 Flash Free · 5.8s)

_Thinking:_

The user wants me to plan the creation of a new v2.0.0 firmware based on the specification. Let me first read the full specification document and understand the requirements before presenting a plan.

Since I'm in plan mode (read-only), I should only read files and think about the plan, not make any edits.

Let me re-read the key spec files to make sure I have a comprehensive understanding.

Let me re-read the full v2 specification to ensure my plan is complete.

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/revision to 2.0.0.md"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/revision to 2.0.0.md</path>
<type>file</type>
<content>
1: ini adalah project yang sudah jadi dan sudah rilis v1.0.0
2: ini penjelasan project yang sekarang dan yang akan di update firmware nya
3: saya lampirkan program yang akan di kembangkan menjadi v2.0.0
4: 
5: Nama Project  : Data Logger 3 Phase
6: 
7: Overview Alat (Hardware)      : konteksnya alat ini hanya berfungsi untuk akuisisi data dan mengirimkan ke mqtt broker (tidak melakukan data logger), data logger sdh ada unit website yang khusus menangani data logging yang datanya di ambil dari mqtt broker
8:                                 1. Akuisisi Data dari 3 buah PZEM-004T (Masing2 PZEM handle 1 Line RST to Netral (WYE/220V))
9:                                 2. Pengolahan Data
10:                                 3. Publish Data ke MQTT Broker (per topic)
11:                                 4. Handle Display OLED 0.96" (display tampilan sangat alakadarnya) dan push button untuk ganti slide page parameter 3 phase
12:                                 5. bisa management signal wi-fi dan prioritasnya (masih manual (hard-code) atur priotitas)
13:                                 6. ada fitur debug berupa kirim data random ke broker namun masih harus buka tutup komentar di baris program (dan masih cuma punya 1 jenis debug aja)
14:                                 7. masih ada beberapa kekurangan/bug sistem seperti:
15:                                   a. [asinkron data] data antar parameter yang sampai ke broker atau website tidak serentak/tidak bersamaan (ada yang terlalu cepat/terlalu lambat sepersekian detik) karena masih publish mentah per topic dan belum pakai json
16:                                   b. [blocking] saat booting esp32 terus melakukan connecting ke wifi dan tidak akan masuk display utama sampai esp32 terkoneksi ke wifi
17:                                   c. belum ada fitur kalibrasi
18:                                   d. belum ada webserver & webpage (untuk config dan kalibrasi)
19:                                   e. masih ada sistem blocking
20:                                   f. belum ada state machine & self-healing kalau error di lapangan
21:                                   g. display OLED masih polosan raw text dan susunan parameter masih berantakan
22:                                   h. Wifi Selector belum seperti pintar network management windows maupun smartphone yang mempunyai known network dan pintar mencari prioritas serta otomatis tersambung 
23: 
24: Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin (Arduino IDE v2.0 & Board Package ESP32 v2.0.14 upgrade ke v3.3.0 untuk firmware datalogger v2.0.0)
25:                                 2. PZEM-004T 100A (3 buah, masing2 handle 1 fasa dari RST ke Netral (WYE/220V)) 
26:                                 3. OLED 0.96" I2C
27:                                 4. Push Button (Normally Open, Not latching) untuk Menu OLED
28:                                 5. Built-in LED ESP32 untuk signal indikator seperti reset kWh
29: 
30: Konfigurasi Pinout Electrical : 
31: //************* PZEM ****************//
32: - PZEM Fasa R dan S dalam satu hardware serial 1
33: - PZEM Fasa T berada dalam hardware serial 2 
34: 
35: #define PZEM_RX1_PIN 4
36: #define PZEM_TX1_PIN 15
37: #define PZEM_RX2_PIN 17
38: #define PZEM_TX2_PIN 16
39: // #define PZEM_SERIAL Serial2
40: HardwareSerial PZEMSerial1(1);
41: HardwareSerial PZEMSerial2(2);
42: #define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM
43: 
44: PZEM004Tv30 pzems[NUM_PZEMS] = {
45:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
46:   PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
47:   PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
48: };
49: //************* PZEM ****************//
50: 
51: //************* OLED 0.96" I2C ****************//
52: SDA= GPIO 21
53: SCL= GPIO 22
54: //************* OLED 0.96" I2C ****************//
55: 
56: //*************** Button & LED ****************//
57: BUTTON  5
58: LED     2
59: //*************** Button & LED ****************//
60: 
61: * Board Package & Libraries yang sudah diinstall di firmware Data Logger v1.0.0 ini (install via Board & Library Manager):
62: *   Board Package   : Board Package ESP32 v2.0.14 (pada firmware data logger v2.0.0 gunakan ESP32 v3.3.0)
63: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
64: *   - PZEM004Tv30          by Jakub Mandula     v1.2.1
65: *   - Adafruit SSD1306     by Adafruit          v2.5.16
66: *   - Adafruit GFX         by Adafruit          v1.12.5
67: *   - PubSubClient         by Nick O'Leary      v2.8
68: *
69: * Board Package & Libraries yang perlu diinstall untuk pembaruan firmware Data Logger di versi v2.0.0 nanti (install via Board & Library Manager):
70: *   Board Package   : Board Package ESP32 v3.3.0
71: *   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
72: *   - ArduinoJson          by Benoit Blanchon   v6.21.6
73: *   - ESP Async WebServer  by ESP32Async        v3.11.0
74: *   - Async TCP            by ESP32Async        v3.4.10
75: 
76: 
77: Yang Harus diperbaiki/diupdate adalah:
78: 1. Semua bagian sistem dan fungsi harus FULL asynchronous task/non blocking system kalau perlu gunakan dua core esp32 atau gunakan FreeRTOS
79: 2. [hardcode & webpage config] sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded (hanya bisa diakses oleh programmer lewat program) dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
80: 3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker dengan topic hierarchical `{PREFIX_TOPIC}/{MQTT_TOPIC}/{DEVICE_ID}` (prefix dan topic diambil dari eeprom berdasarkan configurasi web config)
81:   buatlah file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js
82: 
83:   **Topik MQTT:** `lutpiii/telemetry/{device_id}` (hierarchical, bukan flat)
84:   Backend subscribe ke `lutpiii/telemetry/+` untuk menerima data dari semua device.
85: 
86:   **Final Schema JSON (v2.0.0):**
87:   ```json
88: {
89:   "seq": 1247,
90:   "device": {
91:     "id":     "3ph-logger-001",
92:     "fw":     "v2.1.0",
93:     "uptime": 3600,
94:     "heap":   148432,
95:     "rssi":   -65
96:   },
97:   "phases": {
98:     "R": {
99:       "valid":  true,
100:       "v":      220.1,
101:       "i":      12.34,
102:       "p":      2650.0,
103:       "s":      2714.6,
104:       "q":      582.1,
105:       "pf":     0.97,
106:       "f":      50.0,
107:       "e":      15230.0,
108:       "status": "OK"
109:     },
110:     "S": {
111:       "valid":  true,
112:       "v":      200.6,
113:       "i":      12.34,
114:       "p":      2650.0,
115:       "s":      2714.6,
116:       "q":      582.1,
117:       "pf":     0.97,
118:       "f":      50.0,
119:       "e":      15230.0,
120:       "status": "UNDER"
121:     },
122:     "T": {
123:       "valid":  false,
124:       "v":      0.0,
125:       "i":      0.0,
126:       "p":      0.0,
127:       "s":      0.0,
128:       "q":      0.0,
129:       "pf":     0.0,
130:       "f":      0.0,
131:       "e":      0.0,
132:       "status": "LOST"
133:     }
134:   },
135:   "unbalance": 4.25,
136:   "ts": 1717001234
137: }
138: ```
139: 
140:   **Perubahan dari JSON versi awal:**
141: 
142:   | Aspek | Sebelum | Sesudah | Alasan |
143:   |---|---|---|---|
144:   | Topic | `lutpiii/telemetry` (flat) | `lutpiii/telemetry/{device_id}` (hierarchical) | Backend filter by device via MQTT routing |
145:   | Numeric values | String `"220.1"` | Number `220.1` | Backend langsung pakai, payload lebih kecil |
146:   | Schema phase invalid | Parsial (hanya `status`) | Full schema + `valid: false` | Satu parser untuk semua kondisi |
147:   | `ts` | `millis()` lokal device | Unix epoch seconds (dari NTP) | Waktu absolut untuk sorting/graphing antar device |
148:   | `unbalance` | Di dalam object `phases` | Level terpisah di root | Milik agregat 3-fasa, bukan per-phase |
149:   | `seq` | Tidak ada | `uint32` increment setiap publish | Deteksi missed message / data gap |
150: 4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi atau mqtt dulu baru bisa masuk ke sistem (tetap buat opening screen OLED dengan Loading bar progress nya)
151:     seharusnya sistem terus berjalan meskipun:
152:     a. tidak terhubung ke wi-fi dan/atau mqtt, atau
153:     b. hanya terhubung ke wi-fi namun tidak terhubung ke mqtt server
154: 5. [Tambah] tambah status line dari OK/UNDER/LOST menjadi OK/UNDER/OVER/LOST. serta memindahkan line status threshold dari hardcoded jadi eeprom di config.h
155:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
156:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
157:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
158: 6. [Tambah] tambahkan parameter unbalanced antar phase berdasarkan standard di indonesia, masukkan fungsi di DataAcquisition.h/.cpp
159:         ALGORITMA HITUNG VOLTAGE UNBALANCE 3 PHASE
160:         Input:
161:             VR = Tegangan phase R ke netral
162:             VS = Tegangan phase S ke netral
163:             VT = Tegangan phase T ke netral
164:         Langkah 1:
165:             Hitung tegangan rata-rata
166:             VAVG = (VR + VS + VT) / 3
167:         Langkah 2:
168:             Hitung deviasi tiap phase terhadap rata-rata
169:             DEV_R = |VR - VAVG|
170:             DEV_S = |VS - VAVG|
171:             DEV_T = |VT - VAVG|
172:         Langkah 3:
173:             Tentukan deviasi terbesar
174:             MAX_DEV = nilai terbesar dari:
175:                       DEV_R, DEV_S, DEV_T
176:         Langkah 4:
177:             Hitung persentase unbalance
178:             UNBALANCE = (MAX_DEV / VAVG) × 100
179:         Langkah 5:
180:             Tampilkan hasil unbalance dalam persen (%)
181: 
182:         --------------------------------------------------
183: 
184:         CONTOH DATA:
185:             VR = 220V
186:             VS = 210V
187:             VT = 228V
188:         PROSES:
189:         1. Hitung rata-rata
190:             VAVG = (220 + 210 + 228) / 3
191:             VAVG = 219.33V
192:         2. Hitung deviasi
193:             DEV_R = |220 - 219.33|
194:             DEV_R = 0.67V
195:             DEV_S = |210 - 219.33|
196:             DEV_S = 9.33V
197:             DEV_T = |228 - 219.33|
198:             DEV_T = 8.67V
199:         3. Cari deviasi terbesar
200:             MAX_DEV = 9.33V
201:         4. Hitung unbalance
202:             UNBALANCE = (9.33 / 219.33) × 100
203:             UNBALANCE = 4.25%
204:         HASIL:
205:             Voltage Unbalance = 4.25%
206: 
207: 7. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
208:   Rule set Display:
209:     - display OLED 0.96" SSD1306 sepenuhnya dikontrol dengan 1 push button
210:     - kuncinya cuma ada 2     : 1. jika push button di tekan dengan interval biasa maka itu untuk mengganti page/memindahkan kursor menu ">"
211:                                 2. jika push button di tekan dengan interval 2 detik (customable webpage config) maka itu untuk melakukan "Enter" pada menu yang di pilih atau khususnya pada Monitoring Mode adalah untuk berpindah ke Menu Mode
212:     - saat di Monitoring Mode : 1. tekan biasa push button untuk slide page ke page berikutnya
213:                                 2. tekan selama 2 detik untuk masuk ke "Menu Mode" (berlaku di monitoring page manapun untuk berpindah ke menu mode)
214:     - saat di Menu Mode       : 1. push button sebagai kursor untuk memilih menu ke bawah, dengan cara menekan biasa agar kursor berpindah ke menu dibawahnya
215:                                 2. jika kursor sudah pada menu paling bawah dan button di tekan lagi, maka kursor akan kembali ke menu paling atas lagi (berlaku juga untuk menu confirm "YES" or "NO" yang secara horizontal kanan-kiri)
216:                                 3. untuk masuk ke menu pilihan, maka button harus ditekan selama 2 detik sebagai tombol "Enter" nya
217:                                 4. setiap menu harus ada "Back" untuk keluar dari menu dengan cara menekan yang sama seperti masuk ke dalam menu (2 detik)
218:                                 5. setiap menu harus ada "Back" kecuali "Back to Monitoring", jika itu ditekan selama 2 detik maka akan kembali ke "Monitoring Mode"
219:  *    1. Monitoring Mode:
220:  *    Page 1 [Monitoring Mode]:
221:  *   ┌────────────────────────────┐
222:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
223:  *   │   R         S         T    │  ← Phase Name         
224:  *   │  220V      220V      220V  │  ← Voltage Line to Netral   
225:  *   │  100A      100A      100A  │  ← Current per phase   (0-100)
226:  *   │   OK        OK        OK   │  ← Line_Status   (LOST/UNDER/OK)
227:  *   │  Unbalance = 2.5%          │  ← Unbalance param
228:  *   └────────────────────────────┘
229:  *
230:  *   ┌────────────────────────────┐
231:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
232:  *   │   R         S         T    │  ← Phase Name         
233:  *   │  220V      220V        0V  │  ← Voltage Line to Netral   
234:  *   │  100A      100A      100A  │  ← Current per phase   (0-100)
235:  *   │   OK       UNDER     LOST  │  ← Line_Status   (LOST/UNDER/OK), muncul dengan efek beeping/kedip untuk warning ketika status LOST / UNDER / OVER
236:  *   │  Unbalance = 5.45%   [!]   │  ← Unbalance param ketika hit threshold max unbalance di web config, [!] icon ini juga akan muncul dengan efek beeping/kedip untuk warning
237:  *   └────────────────────────────┘
238:  *
239:  *    Page 2 [Monitoring Mode]:
240:  *   ┌────────────────────────────┐
241:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
242:  *   │   R         S         T    │  ← Phase Name         
243:  *   │  60Hz      60Hz      60Hz  │  ← Frequency     
244:  *   │ 100VA     1.2kVA    10kVA  │  ← Apparent Power (L to N) (jika dibawah 1000 maka satuannya "VA", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVA")  
245:  *   │ 100VAr    2.0kVAr   13kVAr │  ← Reactive Power (L to N) (jika dibawah 1000 maka satuannya "VAR", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVAr")         
246:  *   └────────────────────────────┘
247:  *
248:  *    Page 3 [Monitoring Mode]:
249:  *   ┌────────────────────────────┐
250:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
251:  *   │   R         S         T    │  ← Phase Name         
252:  *   │ 0.8PF     1.0PF     0.7PF  │  ← Power Factor (0.0 - 1.0)     
253:  *   │ 100W       12kW     2.2kW  │  ← Active Power (jika dibawah 1000 maka satuannya "W", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kW")   
254:  *   │ 300Wh     1.4kWh     30kWh │  ← Total Active Energy (jika dibawah 1000 maka satuannya "Wh", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kWh")          
255:  *   └────────────────────────────┘
256:  *
257:  *    2. Menu Mode:
258:  *    Page 1 [Menu Mode]: (jika menu highlighted maka ada indentasi maju 1 atau 2 karakter untuk menunjukkan posisi kursor pada OLED di menu mana)
259:  *   ┌────────────────────────────┐
260:  *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
261:  *   │  > Config                  │  ← Config Menu         
262:  *   │  > Reset kWh               │  ← Reset kWh semua fasa jadi 0kWh
263:  *   │  > Reboot Device           │  ← Reboot ESP32 lewat program setara dengan tekan tombol RST di board
264:  *   │  > Reset to Factory        │  ← Reset to Factory (reset configurasi yang ada di EEPROM, sama saja dengan reset EEPROM melalui web config, bedanya ini lewat menu OLED, yang satunya lewat web config)
265:  *   │  > Back to Monitoring      │  ← Kembali ke Page Monitoring Mode (jika kursor di "Back to Monitoring" dan tombol ditekan selama 2 detik maka akan kembali ke Page 1 Monitoring Mode )
266:  *   └────────────────────────────┘
267:  *
268:  *    Page 2 [Menu Mode > Config]:
269:  *    Note: Di menu Config ini hanya difungsikan untuk user mengaktifkan webserver, selebihnya config akan dilakukan melalui webpage dan akan di simpan ke dalam EEPROM ESP32
270:  *   ┌────────────────────────────┐
271:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
272:  *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
273:  *   │  PW  : 12345678            │  ← Password dari SSID ESP32
274:  *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
275:  *   │  > Active         Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
276:  *   └────────────────────────────┘
277:  *   ┌────────────────────────────┐
278:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
279:  *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
280:  *   │  PW  : 12345678            │  ← Password dari SSID ESP32
281:  *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
282:  *   │  Inactive       > Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
283:  *   └────────────────────────────┘
284:  *
285:  *    Page 3 [Menu Mode > Reset kWh]:
286:  *    Note: Di menu Reset kWh ini hanya difungsikan untuk user melakukan Reset Total kWh pada semua module PZEM-004T
287:  *   ┌────────────────────────────┐
288:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
289:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
290:  *   │        Reset kWh?          │  
291:  *   │                            │  
292:  *   │  > Reset          Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
293:  *   └────────────────────────────┘
294:  *   ┌────────────────────────────┐
295:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
296:  *   │                            │  ← Teks Konfirmasi         
297:  *   │     Reset kWh Berhasil     │  ← Teks Output Reset (Reset kWh Berhasil/Reset kWh Gagal) tampil selama 2 detik lalu otomatis kembali ke page Menu Mode
298:  *   │                            │  
299:  *   │                            │  
300:  *   └────────────────────────────┘
301:  *   ┌────────────────────────────┐
302:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
303:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
304:  *   │        Reset kWh?          │  
305:  *   │                            │  
306:  *   │   Reset         > Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
307:  *   └────────────────────────────┘
308:  *
309:  *    Page 4 [Menu Mode > Reboot Device]:
310:  *    Note: Di menu Config ini hanya difungsikan untuk user melakukan rebooting device ESP32
311:  *   ┌────────────────────────────┐
312:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
313:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
314:  *   │       Reboot Device?       │  
315:  *   │                            │  
316:  *   │  > Reboot         Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
317:  *   └────────────────────────────┘
318:  *   ┌────────────────────────────┐
319:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
320:  *   │                            │      
321:  *   │     Device akan Reboot     │  ← Teks Countdown 3 detik sebelum system otomatis reboot (dalam 3 detik, dalam 2 detik, dalam 1 detik, dalam 0 detik)
322:  *   │       dalam 3 detik        │  
323:  *   │                            │  
324:  *   └────────────────────────────┘
325:  *   ┌────────────────────────────┐
326:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
327:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
328:  *   │       Reboot Device?       │  
329:  *   │                            │  
330:  *   │   Reboot        > Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
331:  *   └────────────────────────────┘
332:  *
333:  *    Page 5 [Menu Mode > Reset to Factory]:
334:  *    Note: -
335:  *   ┌────────────────────────────┐
336:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
337:  *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
338:  *   │        Reset Device?       │  
339:  *   │                            │  
340:  *   │  > Reset          Back     │  ← (Confirm Choise) jika menu "Reset" ditekan selama 2 detik maka esp32 akan melakukan reset EEPROM dan menampilkan teks output jika berhasil selama 2 detik, kemudian otomatis akan melakukan reboot esp32 by system, namun jika "tidak" ditekan selama 2 detik maka akan kembali ke Menu Config
341:  *   └────────────────────────────┘
342:  *   ┌────────────────────────────┐
343:  *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
344:  *   │                            │      
345:  *   │      Reset to Factory      │  ← Teks Output Reset (Reset EEPROM/preferences Berhasil/Reset EEPROM Gagal) tampil selama 2 detik lalu otomatis melakukan reboot esp32 by system
346:  *   │          berhasil          │  
347:  *   │                            │  
348:  *   └────────────────────────────┘
349: 
350: 8. [HTTP Web-server & Access Point] 
351:   a. buat webpage config yang bisa diakses lewat smartphone atau laptop yang terhubung melalui IP Address yang ESP32 keluarkan saat Config pada Menu Mode "Active"
352:   b. jika saat access point aktif dan ESP32 sedang terhubung ke jaringan yang ada internetnya, maka teruskan internet itu ke access point ESP32 agar Laptop/smartphone yang sedang melakukan config/terhubung ke access point esp32 mendapatkan jaringan internet untuk tetap online (jika tidak, Laptop/smartphone tidak apa-apa dalam kondisi offline) 
353:   c. semua parameter config yang ada di webpage tersimpan di dalam EEPROM ESP32 (jadi device baru harus config terlebih dahulu untuk terkoneksi ke internet/MQTT)
354:   d. buatkan login page sebelum masuk ke main page config, dengan user dan pass yang hanya bisa diubah di hardcode config.h, dengan default USERNAME = ADMIN, PASS = 18273645
355:   e. page 1 [Config] HTML + CSS:
356:     - Manage Known Network   (Big/Medium Card with Link to "Manage Known Network")
357:     - MQTT Connection Config (Big/Medium Card with Link to "MQTT Connection Config")
358:     - PZEM-004T Calibration  (Big/Medium Card with Link to "PZEM-004T Calibration")
359: 
360:     {HARDRESET EEPROM}
361: 
362:   f. page 1 [Config > Manage Known Network] HTML + CSS:
363:       tampilkan semua jaringan yang diketahui (EEPROM) dalam tabel dengan kolom dan terdapat text box untuk menambahkan jaringan baru di bawah table dan tombol add network untuk menyimpan jaringan baru ke EEPROM
364:               Manage Known Network
365: 
366:       | No. | SSID | PASS     |   Aksi   |
367:       | 1.  | ABC  | 18273645 | {Delete} |
368:       | 2.  | esp  | 18273645 | {Delete} |
369: 
370:       Add Network
371:       [ SSID....       ]
372:       [ PASS....       ]
373: 
374:       {ADD NETWORK}
375: 
376:   g. page 2 [Config > MQTT Connection Config] HTML + CSS:
377:       tampilkan data koneksi MQTT dalam bentuk teks dari EEPROM
378:       MQTT Server = broker.avisha.id
379:       MQTT_PORT   = 1883               // Non-SSL port
380:       MQTTS_PORT  = 8884               // SSL port
381:       WS_PORT     = 8083               // Non-SSL port
382:       WSS_PORT    = 8084               // SSL port
383:       MQTT_USER   = "lutpiii"          // MQTT Username
384:       MQTT_PASS   = "lutpiiiMQTT"      // MQTT Password
385:       PREFIX_TOPIC= "lutpiii"          // Topic prefix (wajib)
386:       MQTT_TOPIC  = "telemetry"        // Topic (wajib)
387:       USE SSL     = TRUE
388:       USE WS      = FALSE              
389:       
390: 
391:       {EDIT}        {SAVE CONFIG}
392: 
393:       // jika WS=0 & SSL=0 gunakan MQTT
394:       // jika WS=0 & SSL=1 gunakan MQTTS
395:       // jika WS=1 & SSL=0 gunakan WS
396:       // jika WS=1 & SSL=1 gunakan WSS
397: 
398:   h. page 3 [Config > PZEM-004T Calibration] HTML + CSS:
399:       buat kalibrasi dengan model offset (-/+) untuk parameter Voltage & Current masing-masing Fasa dengan presisi 0.000 perubahan, threshold LOST, UNDER, OVER dan unbalance. dan tampilkan juga offset yang tersimpan
400:       dengan model text box
401: 
402:       {SAVE CALIBRATION}
403: 
404:   i. gunakan saja template yang sudah saya buat ini, namun bagusin lagi homa page bagian text link nya, jgn pakai text link lagi, gunakan card medium atau besar ke bawah saja susunannya, jumlah card = jumlah menu
405: String WebServerManager::_htmlHead(const char* title) {
406: return String(R"(<!DOCTYPE html><html><head>
407: <meta charset="UTF-8">
408: <meta name="viewport" content="width=device-width,initial-scale=1">
409: <meta name="color-scheme" content="light">
410: <title>)") + title + (R"(</title>
411: <style>
412: :root{
413:   --bg:#f7f8fa;
414:   --surface:#ffffff;
415:   --surface-2:#f3f7f4;
416:   --text:#1f2937;
417:   --muted:#6b7280;
418:   --line:#dbe3dc;
419:   --primary:#16a34a;
420:   --primary-hover:#15803d;
421:   --primary-soft:#dcfce7;
422:   --danger:#dc2626;
423:   --danger-hover:#b91c1c;
424:   --shadow:0 6px 18px rgba(15,23,42,.06);
425:   --radius:10px;
426: }
427: 
428: *{box-sizing:border-box}
429: html{-webkit-text-size-adjust:100%}
430: body{
431:   font-family:system-ui,-apple-system,"Segoe UI",Roboto,Arial,sans-serif;
432:   max-width:600px;
433:   margin:30px auto;
434:   padding:0 16px;
435:   background:var(--bg);
436:   color:var(--text);
437:   line-height:1.5;
438: }
439: 
440: h1{
441:   color:var(--text);
442:   font-size:1.4rem;
443:   margin:0 0 12px;
444:   letter-spacing:-0.02em;
445: }
446: 
447: h2{
448:   margin:0 0 10px;
449:   font-size:1rem;
450:   color:var(--primary);
451:   background:var(--primary-soft);
452:   padding:8px 12px;
453:   border-radius:8px;
454:   font-weight:700;
455: }
456: 
457: a{
458:   color:var(--primary);
459:   text-decoration:none;
460: }
461: a:hover{text-decoration:underline}
462: 
463: nav{
464:   margin:12px 0;
465:   display:flex;
466:   flex-wrap:wrap;
467:   gap:8px;
468: }
469: nav a{
470:   display:inline-flex;
471:   align-items:center;
472:   justify-content:center;
473:   padding:8px 12px;
474:   border:1px solid var(--line);
475:   border-radius:999px;
476:   background:var(--surface);
477:   color:var(--text);
478:   text-decoration:none;
479:   box-shadow:0 1px 2px rgba(15,23,42,.04);
480: }
481: nav a:hover{
482:   background:var(--surface-2);
483:   text-decoration:none;
484: }
485: 
486: hr{
487:   border:none;
488:   height:1px;
489:   background:var(--line);
490:   margin:14px 0;
491: }
492: 
493: .card{
494:   background:var(--surface);
495:   border:1px solid var(--line);
496:   border-left:4px solid var(--primary);
497:   border-radius:var(--radius);
498:   padding:16px;
499:   margin:12px 0;
500:   box-shadow:var(--shadow);
501: }
502: 
503: label{
504:   display:block;
505:   font-size:.85rem;
506:   color:var(--muted);
507:   margin:10px 0 6px;
508: }
509: 
510: .hint{
511:   font-size:.78rem;
512:   color:var(--muted);
513:   margin:-4px 0 8px;
514: }
515: 
516: input[type=text],
517: input[type=password],
518: input[type=number],
519: textarea{
520:   width:100%;
521:   box-sizing:border-box;
522:   padding:9px 10px;
523:   background:#fff;
524:   color:var(--text);
525:   border:1px solid var(--line);
526:   border-radius:8px;
527:   margin:4px 0 10px;
528:   font:inherit;
529:   outline:none;
530: }
531: 
532: input[type=text]:focus,
533: input[type=password]:focus,
534: input[type=number]:focus,
535: textarea:focus{
536:   border-color:rgba(22,163,74,.55);
537:   box-shadow:0 0 0 3px rgba(22,163,74,.12);
538: }
539: 
540: textarea{
541:   resize:vertical;
542:   min-height:120px;
543: }
544: 
545: button,
546: .btn,
547: .danger{
548:   background:var(--primary);
549:   color:#fff;
550:   border:none;
551:   padding:10px 16px;
552:   border-radius:8px;
553:   cursor:pointer;
554:   font-size:.92rem;
555:   font-weight:600;
556:   transition:background .15s ease, transform .05s ease;
557: }
558: 
559: button:hover,
560: .btn:hover{
561:   background:var(--primary-hover);
562: }
563: 
564: button:active,
565: .btn:active,
566: .danger:active{
567:   transform:translateY(1px);
568: }
569: 
570: .btn-del{
571:   background:#6b7280;
572: }
573: .btn-del:hover{
574:   background:#4b5563;
575: }
576: 
577: .danger{
578:   background:var(--danger);
579: }
580: .danger:hover{
581:   background:var(--danger-hover);
582: }
583: 
584: table{
585:   width:100%;
586:   border-collapse:collapse;
587:   margin:8px 0;
588: }
589: th,td{
590:   text-align:left;
591:   padding:8px;
592:   border-bottom:1px solid var(--line);
593:   vertical-align:top;
594: }
595: th{
596:   color:var(--primary);
597:   background:#f9fffb;
598: }
599: 
600: input[type=checkbox]{
601:   accent-color:var(--primary);
602:   transform:translateY(1px);
603: }
604: 
605: form{margin:0}
606: 
607: @media (max-width:640px){
608:   body{
609:     margin:18px auto;
610:     padding:0 12px;
611:   }
612:   nav a{
613:     width:calc(50% - 4px);
614:   }
615: }
616: </style></head><body>
617: <h1>&#9889; 3-Phase Data Logger</h1>
618: <nav>
619: <a href="/">Home</a>
620: <a href="/wifi">WiFi</a>
621: <a href="/mqtt">MQTT</a>
622: <a href="/calibration">Calibration</a>
623: </nav>
624: <hr>
625: )");
626: }
627: 
628: String WebServerManager::_htmlFoot() {
629:     return R"(<hr><p style="font-size:.75rem;color:#6b7280;text-align:center">Muhammad Lutfi Nur Anendi &bull; )" FW_VERSION R"(</p></body></html>)";
630: }
631: 
632: // ─── Page: Index ──────────────────────────────────────────
633: 
634: String WebServerManager::_pageIndex(StorageManager& s) {
635:     String h = _htmlHead("Config");
636:     h += R"HTML(<div class="card">
637: <h2>Configuration Pages</h2>
638: <p><a href="/wifi">&#128246; Manage Known Networks</a></p>
639: <p><a href="/mqtt">&#128272; MQTT Connection Config</a></p>
640: <p><a href="/calibration">&#9881; PZEM-004T Calibration &amp; Thresholds</a></p>
641: </div>
642: <div class="card">
643: <h2>&#9888; Danger Zone</h2>
644: <form method="POST" action="/factoryreset"
645:   onsubmit="return confirm('ERASE ALL settings? This cannot be undone.')">
646: <button class="danger" type="submit">Hard Reset EEPROM</button>
647: </form>
648: </div>
649: )HTML";
650:     h += _htmlFoot();
651:     return h;
652: }
653: 
654: // ─── Page: WiFi ───────────────────────────────────────────
655: 
656: String WebServerManager::_pageWifi(StorageManager& s) {
657:     String h = _htmlHead("Manage WiFi");
658:     h += "<h2>Known Networks</h2><div class='card'>";
659:     h += "<table><tr><th>No.</th><th>SSID</th><th>Action</th></tr>";
660: 
661:     int n = s.getWifiCount();
662:     for (int i = 0; i < n; i++) {
663:         WiFiEntry e = s.getWifiEntry(i);
664:         h += "<tr><td>" + String(i + 1) + "</td><td>" + String(e.ssid) + "</td><td>";
665:         h += "<form method='POST' action='/wifi/delete' style='display:inline'>";
666:         h += "<input type='hidden' name='idx' value='" + String(i) + "'>";
667:         h += "<button class='btn btn-del' type='submit'>Delete</button></form>";
668:         h += "</td></tr>";
669:     }
670:     if (n == 0) h += "<tr><td colspan='3'>No networks stored.</td></tr>";
671:     h += "</table></div>";
672: 
673:     h += R"(<div class="card"><h2>Add Network</h2>
674: <form method="POST" action="/wifi/add">
675: <label>SSID</label>
676: <input type="text" name="ssid" placeholder="Network name" required>
677: <label>Password</label>
678: <input type="password" name="pass" placeholder="Password">
679: <button type="submit">Add Network</button>
680: </form></div>)";
681: 
682:     h += _htmlFoot();
683:     return h;
684: }
685: 
686: // ─── Page: MQTT ───────────────────────────────────────────
687: 
688: String WebServerManager::_pageMQTT(StorageManager& s) {
689:     MQTTConfig cfg = s.getMQTTConfig();
690:     String h = _htmlHead("MQTT Config");
691:     h += "<h2>MQTT Broker</h2><div class='card'>";
692:     h += "<form method='POST' action='/mqtt'>";
693: 
694:     auto field = [&](const char* label, const char* name, const char* val, const char* type = "text") {
695:         h += String("<label>") + label + "</label>";
696:         h += String("<input type='") + type + "' name='" + name + "' value='" + val + "'>";
697:     };
698:     auto numField = [&](const char* label, const char* name, int val) {
699:         h += String("<label>") + label + "</label>";
700:         h += String("<input type='number' name='") + name + "' value='" + val + "'>";
701:     };
702:     auto checkbox = [&](const char* label, const char* name, bool checked) {
703:         h += String("<label><input type='checkbox' name='") + name + "' " +
704:              (checked ? "checked" : "") + "> " + label + "</label><br>";
705:     };
706: 
707:     field("MQTT Server",       "server",   cfg.server);
708:     numField("Port (non-SSL)", "port",     cfg.port);
709:     numField("Port (SSL)",     "sslport",  cfg.sslPort);
710:     field("Username",          "user",     cfg.user);
711:     field("Password",          "pass",     cfg.pass, "password");
712:     field("Topic Prefix",      "topic",    cfg.topic);
713:     checkbox("Use SSL",        "usessl",   cfg.useSSL);
714:     checkbox("Use WebSocket",  "usews",    cfg.useWS);
715: 
716:     h += "<label>CA Certificate (PEM)</label>";
717:     h += String("<textarea name='cacert' rows='5' placeholder='-----BEGIN CERTIFICATE-----...'>")
718:          + cfg.caCert + "</textarea>";
719:     h += "<button type='submit'>Save Config</button></form></div>";
720:     h += _htmlFoot();
721:     return h;
722: }
723: 
724: // ─── Page: Calibration + Thresholds ──────────────────────
725: 
726: String WebServerManager::_pageCalibration(StorageManager& s) {
727:     CalibrationConfig cal = s.getCalibration();
728:     ThresholdConfig   thr = s.getThresholds();
729: 
730:     String h = _htmlHead("Calibration & Thresholds");
731:     h += "<form method='POST' action='/calibration'>";
732: 
733:     h += "<h2>PZEM-004T Calibration</h2>";
734:     h += "<p class='hint'>Offset is additive: Reading = Raw + Offset. Use negative to subtract.</p>";
735:     h += "<div class='card'>";
736: 
737:     const char* phases[3] = {"R", "S", "T"};
738:     for (int i = 0; i < 3; i++) {
739:         h += String("<h2>Phase ") + phases[i] + "</h2>";
740:         h += String("<label>Voltage Offset (V) &nbsp; Current stored: ") +
741:              String(cal.voltageOffset[i], 3) + " V</label>";
742:         h += String("<input type='number' step='0.001' name='voff_") +
743:              phases[i] + "' value='" + String(cal.voltageOffset[i], 3) + "'>";
744:         h += String("<label>Current Offset (A) &nbsp; Current stored: ") +
745:              String(cal.currentOffset[i], 3) + " A</label>";
746:         h += String("<input type='number' step='0.001' name='ioff_") +
747:              phases[i] + "' value='" + String(cal.currentOffset[i], 3) + "'>";
748:     }
749:     h += "</div>";
750: 
751:     h += "<h2>Line Status Thresholds</h2>";
752:     h += "<p class='hint'>Thresholds used to determine line status: LOST / UNDER / OK / OVER.</p>";
753:     h += "<div class='card'>";
754: 
755:     h += "<label>LOST threshold (V) &ndash; voltage below this &rarr; LOST</label>";
756:     h += "<p class='hint'>Current: " + String(thr.voltageLost, 1) + " V</p>";
757:     h += "<input type='number' step='0.1' name='th_vlost' value='" + String(thr.voltageLost, 1) + "'>";
758: 
759:     h += "<label>UNDER threshold (V) &ndash; voltage below this &rarr; UNDER (above LOST)</label>";
760:     h += "<p class='hint'>Current: " + String(thr.voltageUnder, 1) + " V</p>";
761:     h += "<input type='number' step='0.1' name='th_vunder' value='" + String(thr.voltageUnder, 1) + "'>";
762: 
763:     h += "<label>OVER threshold (V) &ndash; voltage above this &rarr; OVER</label>";
764:     h += "<p class='hint'>Current: " + String(thr.voltageOver, 1) + " V</p>";
765:     h += "<input type='number' step='0.1' name='th_vover' value='" + String(thr.voltageOver, 1) + "'>";
766:     h += "</div>";
767: 
768:     h += "<h2>Voltage Unbalance Threshold</h2>";
769:     h += "<p class='hint'>Berdasarkan standar SNI/SPLN Indonesia. Jika unbalance &gt; threshold unbalance maka OLED menampilkan peringatan [!]</p>";
770:     h += "<br>";
771:     h += "<p class='hint'>3-phase voltage unbalance computed using NEMA method.</p>";
772:     h += "<div class='card'>";
773:     h += "<label>Max Unbalance (%) &ndash; above this &rarr; warning [!] on OLED &amp; MQTT</label>";
774:     h += "<p class='hint'>Current: " + String(thr.unbalanceMax, 2) + " %</p>";
775:     h += "<input type='number' step='0.01' name='th_unbal' value='" + String(thr.unbalanceMax, 2) + "'>";
776:     h += "</div>";
777: 
778:     h += "<button type='submit'>Save Calibration &amp; Thresholds</button>";
779:     h += "</form>";
780:     h += _htmlFoot();
781:     return h;
782: }
783:  
784: 
785: 9. [IMPORTANT!!!] Kalibrasi Tegangan & Arus PZEM harus mempengaruhi semua parameter turunan (VA, VAr, W, Wh, PF kecuali Hz). Karena parameter lain bergantung pada V dan I, maka setelah kalibrasi offset V/I, semua parameter hitung ulang agar konsisten.
786: 
787: 10. [Tambah] di web config, tambahkan config untuk nilai LOST, UNDER, OVER voltage, serta persentase max unbalance phase di halaman PZEM Calibration
788:     #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
789:     #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
790:     #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
791:     #define VOLTAGE_UNBALANCE_MAX    3.0f  // Above this → OVER UNBALANCE (satuan persen)
792: 
793: 11. LED Signal 
794:   - sebagai indikator terhadap aksi-aksi dari fungsi/module program seperti Booting, Reset kWh, active/deactive webserver, dll.
795: 
796: 12. [Tambah] tambahkan fitur auto self-healing ketika esp32 mengalami freeze/hang/stuck/runningout memory atau sejenisnya atau sesuatu yang membuat esp32 malfungsi sistem, maka harus ditambahkan self healing dengan metode yang paling cocok untuk sistem ini
797: 
798: 13. Struktur File Project gunakan OOP dan seperti ROS2 (Robot Operating System 2) dengan dokumentasi-dokumentasi melimpahnya dan program mudah dipahami, mudah di konfigurasi, dan bisa scalable saat penambahan fitur/sensor baru
799:   (jumlah file menyesuaikan, ini hanya sebagai contoh):
800:   - main.ino
801:   - config.h
802:   - data_acquisition.h/.cpp
803:   - calibration.h/.cpp
804:   - display.h/.cpp
805:   - menu.h/.cpp
806:   - webserver.h/.cpp
807:   - types.h
808:   - system_state.h
809:   - task_manager.h/.cpp
810:   - wifi_manager.h/.cpp
811:   - mqtt_manager.h/.cpp
812:   - json_builder.h/.cpp
813:   - storage_manager.h/.cpp
814:   - led_manager.h/.cpp
815:   - diagnostics.h/.cpp
816:   - dll
817: 
818: ### Catatan untuk Backend Multi-Device (JSON Parser)
819: 1. Subscribe ke `lutpiii/telemetry/+` (wildcard) untuk menerima data dari semua device
820: 2. Indexing data berdasarkan `device.id` + `ts` (Unix timestamp) untuk query perangkat per periode waktu
821: 3. Gunakan field `seq` untuk mendeteksi apakah ada data terlewat (cek gap sequence tiap device)
822: 4. Field `valid` pada tiap phase memberi tahu bahwa data phase tersebut bisa diabaikan (tanpa perlu conditional parsing)
823: 5. Jika ESP32 belum sync NTP, kirim nilai `ts = 0` dan backend fallback ke MQTT broker timestamp
824: 

(End of file - total 824 lines)
</content>
```

**Tool: read**

**Input:**
```json
{
  "filePath": "/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credential_example.cpp"
}
```

**Output:**
```
<path>/mnt/d/project web/power-logger-hardware-esp32/Data_Logger_Monitor/credential_example.cpp</path>
<type>file</type>
<content>
1: // /******************************************************
2: //  * Project   : ESP32 MQTTs Publisher
3: //  * File      : credentials.h
4: //  * Author    : Muhammad Lutfi Nur Anendi
5: //  * Created   : 5 August 2025
6: //  * Last Edit : 23 October 2025
7: //  *
8: //  * Description:
9: //  *   Configuration file for WiFi, MQTT, and SSL credentials.
10: //  *   Rename this file from `credentials_example.cpp` → `credentials.cpp`
11: //  *   before compiling the project.
12: //  *
13: //  * IMPORTANT:
14: //  *   - DO NOT commit this file to any public repository.
15: //  *   - Keep your credentials private.
16: //  *   - Modify only the fields below as required.
17: //  *
18: //  * MQTT Notes:
19: //  *   - MQTT_PORT  = 1883 for non-SSL
20: //  *   - MQTTS_PORT = 8883 for SSL (secure)
21: //  *   - MQTT_TOPIC example: "user_topic/#"
22: //  *
23: //  * SSL Notes:
24: //  *   - CA_CERT should contain your Root Certificate between
25: //  *     -----BEGIN CERTIFICATE----- and -----END CERTIFICATE-----
26: //  *
27: //  * Developer Info:
28: //  *   GitHub   : https://github.com/lutfi-rpi5
29: //  *   LinkedIn : https://linkedin.com/in/muhammad-lutfi-nur-anendi
30: //  *   Instagram: https://instagram.com/lutfiianendi
31: //  *
32: //  * Revision History:
33: //  *   v1.0  Initial release with MQTT & SSL credentials
34: //  *   v1.1  Added developer info and usage guide
35: //  ******************************************************/
36: 
37: // #include "credentials.h"
38: 
39: // // ---------- WiFi Configuration ----------
40: // const char* WIFI_SSID1 = "";
41: // const char* WIFI_PASS1 = "";
42: 
43: // const char* WIFI_SSID2 = "";
44: // const char* WIFI_PASS2 = "";
45: 
46: // const char* WIFI_SSID3 = "";
47: // const char* WIFI_PASS3 = "";
48: 
49: // const char* WIFI_SSID4 = "";
50: // const char* WIFI_PASS4 = "";
51: 
52: // // ---------- WiFi Networks List ----------
53: // WiFiCredential wifiList[] = {
54: //   {WIFI_SSID1, WIFI_PASS1, 0},   // Highest priority
55: //   {WIFI_SSID4, WIFI_PASS4, 1},
56: //   {WIFI_SSID2, WIFI_PASS2, 2},
57: //   {WIFI_SSID3, WIFI_PASS3, 3}
58: // };
59: // const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);
60: 
61: // // ---------- MQTT Broker Configuration ----------
62: // const char* MQTT_SERVER = "";                 // Broker address
63: // const int   MQTT_PORT   = 1883;               // Non-SSL port
64: // const int   MQTTS_PORT  = 8883;               // SSL port
65: // const char* MQTT_USER   = "";                 // MQTT Username
66: // const char* MQTT_PASS   = "";                 // MQTT Password
67: // const char* MQTT_TOPIC  = "";                 // Topic prefix (optional)
68: 
69: // // ---------- Optional API Key ----------
70: // const char* API_KEY = "";  // For future REST API integration
71: 
72: // // ---------- Root CA Certificate ----------
73: // const char* CA_CERT = 
74: // "-----BEGIN CERTIFICATE-----\n"
75: // // Drop your CA certificate content here
76: // "-----END CERTIFICATE-----\n";
77: 
78: 

(End of file - total 78 lines)
</content>
```

---

## Assistant (Plan · DeepSeek V4 Flash Free · 126.9s)

_Thinking:_

Now I have a comprehensive understanding of the entire specification. Let me formulate a complete plan.

The user wants:
1. Create a new folder `Data_Logger_Monitor_v2.0.0` in the root
2. Design all firmware files based on the v2.0.0 specification in `revision to 2.0.0.md`

Key requirements from the spec:
1. Full async/non-blocking system using FreeRTOS tasks
2. Centralized config: `config.h` (hardcoded) + webpage config (EEPROM/NVS)
3. MQTT: single JSON payload, hierarchical topic, structured schema
4. No blocking at boot - device runs without network
5. Line status: OK/UNDER/OVER/LOST with configurable thresholds
6. Voltage unbalance calculation (NEMA method)
7. OLED: Professional UI with Monitoring Mode (3 pages) + Menu Mode (5 pages)
8. Web server: AP mode + HTTP server with HTML/CSS pages for WiFi/MQTT/Calibration
9. Calibration offset on V/I affecting all derived parameters
10. LED signal indicators
11. Self-healing mechanisms
12. OOP architecture, modular files

File structure from the spec:
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
- credentials.h
- credential_example.cpp

Let me organize the plan into a logical build order.

Questions I should ask the user:
1. Do they want me to include all files listed in the spec, or are there any they want to add/remove/simplify?
2. Any preference on the exact architecture details (e.g., Preferences vs EEPROM for storage)?
3. The spec mentions "Reset to Factory" in the OLED menu but the template code has "Hard Reset EEPROM" - should both be included?
4. The spec mentions MQTT port logic (WS=0/SSL=0 → MQTT, etc.) - confirm the exact logic.

Actually, I think the spec is quite detailed already. Let me just present the plan and ask if they want any adjustments before I start implementing.

Let me organize the build order:
1. Foundation: `config.h` (all hardcoded thresholds, pins, version), `types.h` (shared structs)
2. Storage: `storage_manager.h/.cpp` (EEPROM/NVS read/write for WiFi list, MQTT config, calibration)
3. Button: `button_manager.h/.cpp` (debounce, long-press detection)
4. Data Acquisition: `data_acquisition.h/.cpp` (PZEM reads, unbalance calc, line status)
5. Calibration: `calibration.h/.cpp` (offset apply + propagate to derived params)
6. WiFi: `wifi_manager.h/.cpp` (async connect, known networks from storage)
7. MQTT: `mqtt_manager.h/.cpp` (async connect, publish loop)
8. JSON: `json_builder.h/.cpp` (build the schema)
9. Display: `display_manager.h/.cpp` + `menu_manager.h/.cpp` (OLED UI)
10. LED: `led_manager.h/.cpp`
11. Web: `webserver_manager.h/.cpp` (AP + HTTP + HTML)
12. System: `system_state.h` (state machine), `task_manager.h/.cpp`, `diagnostics.h/.cpp`
13. Main: `main.ino` (wiring everything together)
14. Credentials: `credentials.h`, `credential_example.cpp`

Actually, I need to be careful about the order of file creation because of dependencies. Let me think about a more logical order:

Phase 1 - Core types & config:
- config.h
- types.h
- credentials.h
- credential_example.cpp

Phase 2 - Infrastructure:
- storage_manager.h / .cpp (depends on config.h, types.h)
- system_state.h (enum state machine)
- diagnostics.h / .cpp

Phase 3 - Hardware drivers:
- button_manager.h / .cpp (standalone, no deps)
- led_manager.h / .cpp (standalone)
- data_acquisition.h / .cpp (depends on config.h, calibration.h)
- calibration.h / .cpp (standalone algorithm)

Phase 4 - Connectivity:
- wifi_manager.h / .cpp (depends on storage_manager, credentials)
- mqtt_manager.h / .cpp (depends on storage_manager, credentials)
- json_builder.h / .cpp (standalone, builds JSON)

Phase 5 - Display:
- display_manager.h / .cpp (depends on types.h)
- menu_manager.h / .cpp (depends on display_manager, button_manager)

Phase 6 - Web:
- webserver_manager.h / .cpp (depends on storage_manager)

Phase 7 - Task orchestration:
- task_manager.h / .cpp (depends on everything)

Phase 8 - Main:
- main.ino (wires everything)

This is a lot of files. Let me present this plan clearly to the user.

Here is the complete plan based on the `revision to 2.0.0.md` specification.

---

## Folder tree

```
Data_Logger_Monitor_v2.0.0/
├── main.ino                  # Entry point, wiring, state machine loop
├── config.h                  # Hardcoded programmer settings (pins, thresholds, version)
├── types.h                   # Shared structs & enums
├── system_state.h            # Global state machine enum + flags
├── credentials.h             # WiFi/MQTT/SSL credential declarations
├── credential_example.cpp    # Template for credentials copy
│
├── storage_manager.h/.cpp    # EEPROM/NVS: save/load WiFi list, MQTT config, calibration, thresholds
├── button_manager.h/.cpp     # Single button: debounce, long-press (2s), short-press events
├── led_manager.h/.cpp        # GPIO2 indicator: patterns for boot, reset, AP active, heartbeat
├── diagnostics.h/.cpp        # Log levels, fake data injection, memory & timing diag
│
├── calibration.h/.cpp        # V/I offset model, propagate to VA/VAr/W/Wh/PF (not Hz)
├── data_acquisition.h/.cpp   # PZEM reads, line status (LOST/UNDER/OK/OVER), unbalance %
│
├── wifi_manager.h/.cpp       # Async connect/disconnect, known network mgmt from storage
├── mqtt_manager.h/.cpp       # Async connect/disconnect, reconnect via state machine
├── json_builder.h/.cpp       # Build the JSON payload (seq, device, phases, unbalance, ts)
│
├── display_manager.h/.cpp    # OLED rendering: title bar with Q/W icons, partial redraw
├── menu_manager.h/.cpp       # Menu state machine: monitor (3 pages) ↔ menu (5 pages), cursor
│
├── webserver_manager.h/.cpp  # AP mode + HTTP, login page, WiFi/MQTT/Calibration HTML forms
│
└── task_manager.h/.cpp       # FreeRTOS task scheduling, watchdog config, timing diagnostics
```

## Build order (14 filesets)

| Phase | Files | What they do |
|-------|-------|-------------|
| **1 — Config & Types** | `config.h`, `types.h`, `credentials.h` + `credential_example.cpp` | Pins, thresholds, firmware version, structs, enums, credential declarations |
| **2 — Storage** | `storage_manager.h/.cpp` | Read/write WiFi entries, MQTT config, calibration offsets, thresholds via Preferences/NVS |
| **3 — State & Diag** | `system_state.h`, `diagnostics.h/.cpp` | Global app state enum, logging levels, fake data injection, heap/RSSI diag |
| **4 — Drivers** | `button_manager.h/.cpp`, `led_manager.h/.cpp` | Debounced nav button (short/long), LED blink patterns |
| **5 — Calibration** | `calibration.h/.cpp` | Apply V/I offset, recompute VA, VAr, W, Wh, PF (not Hz) |
| **6 — Data** | `data_acquisition.h/.cpp` | Read 3× PZEM-004T, compute VA/VAr, line status, unbalance % |
| **7 — Networking** | `wifi_manager.h/.cpp`, `mqtt_manager.h/.cpp` | Async connect to best known network, async MQTT with reconnect state machine |
| **8 — JSON** | `json_builder.h/.cpp` | Build the `{seq, device, phases, unbalance, ts}` payload |
| **9 — OLED** | `display_manager.h/.cpp`, `menu_manager.h/.cpp` | Title bar (Q/W icons), 3 monitoring pages, 5 menu pages, cursor, confirm screens |
| **10 — Web** | `webserver_manager.h/.cpp` | Login page, WiFi/MQTT/Calibration HTML forms, inline CSS from spec |
| **11 — Tasks** | `task_manager.h/.cpp` | FreeRTOS task creation, watchdog, millis()-based scheduling |
| **12 — Main** | `main.ino` | Wire all managers, `setup()` + `loop()`, global state machine |

## Key design decisions to confirm

1. **Storage**: Preferences (NVS) instead of EEPROM? The spec says "EEPROM or Preferences/NVS" — NVS is preferred on ESP32 Arduino v3.x for smaller writes and no wear limits.

2. **MQTT library**: The spec says to add `ESP Async WebServer` + `Async TCP`, but doesn't mention an **Async MQTT** library. For non-blocking MQTT we can use `AsyncMqttClient` in addition to `PubSubClient`. AsyncMqttClient fits the non-blocking requirement better. Do you want async MQTT, or keep PubSubClient with non-blocking polling?

3. **OLED menu**: The spec lists 5 menu items (Config, Reset kWh, Reboot Device, Reset to Factory, Back to Monitoring) but the earlier template in the spec shows 4 items (no Reset to Factory). Which set should I implement — 5 items?

4. **NTP / `ts`**: The JSON schema uses Unix epoch for `ts`, but sync requires NTP. Should I implement NTP sync as part of wifi_manager, or send `ts=0` until synced as the spec suggests?

**Tool: question**

**Input:**
```json
{
  "questions": [
    {
      "question": "Storage: Use Preferences (NVS) instead of raw EEPROM? NVS is preferred for ESP32 Arduino v3.x — smaller writes, no wear limits, and simpler API.",
      "header": "Storage Backend",
      "options": [
        {
          "label": "Preferences / NVS (Recommended)",
          "description": "ESP32 native key-value store, faster, no wear limits"
        },
        {
          "label": "EEPROM library",
          "description": "Traditional Arduino EEPROM, compatible but slower writes"
        }
      ]
    },
    {
      "question": "MQTT library for async non-blocking: AsyncMqttClient integrates naturally with the async TCP lib already required. PubSubClient works but needs polling.",
      "header": "MQTT Library",
      "options": [
        {
          "label": "AsyncMqttClient (Recommended)",
          "description": "Fully async, event-driven, pairs with Async TCP"
        },
        {
          "label": "PubSubClient (keep v1 style)",
          "description": "Polling-based, need care to avoid blocking"
        }
      ]
    },
    {
      "question": "OLED menu items: The spec template shows both 4 items and 5. Which should I implement?",
      "header": "Menu Items",
      "options": [
        {
          "label": "5 items (Recommended)",
          "description": "Config, Reset kWh, Reboot Device, Reset to Factory, Back to Monitoring"
        },
        {
          "label": "4 items",
          "description": "Config, Reset kWh, Reboot Device, Back to Monitoring (omit Reset to Factory)"
        }
      ]
    },
    {
      "question": "NTP sync for ts field: Should I implement NTP client for Unix epoch timestamps, or send ts=0 until synced?",
      "header": "NTP Timestamp",
      "options": [
        {
          "label": "NTP + fallback ts=0 (Recommended)",
          "description": "Implement NTP sync, send ts=0 until first successful sync (matches spec)"
        },
        {
          "label": "Just use millis() as ts",
          "description": "Simple local timestamp, not Unix epoch (simpler but less accurate)"
        }
      ]
    }
  ]
}
```

**Output:**
```
User has answered your questions: "Storage: Use Preferences (NVS) instead of raw EEPROM? NVS is preferred for ESP32 Arduino v3.x — smaller writes, no wear limits, and simpler API."="Preferences / NVS (Recommended)", "MQTT library for async non-blocking: AsyncMqttClient integrates naturally with the async TCP lib already required. PubSubClient works but needs polling."="AsyncMqttClient (Recommended)", "OLED menu items: The spec template shows both 4 items and 5. Which should I implement?"="5 items (Recommended)", "NTP sync for ts field: Should I implement NTP client for Unix epoch timestamps, or send ts=0 until synced?"="NTP + fallback ts=0 (Recommended)". You can now continue with the user's answers in mind.
```

---

## Assistant (Plan · DeepSeek V4 Flash Free · 9.5s)

_Thinking:_

The user confirmed all my recommendations. Now let me present the finalized plan and ask if they're happy before I start implementing.

Let me write a clear plan summary.

Semua opsi terkonfirmasi. Berikut ringkasan final plan-nya:

## Final Plan

**Folder:** `Data_Logger_Monitor_v2.0.0/` (14 file sets, urutan build sesuai dependensi)

| # | Files | Tech decisions |
|---|-------|---------------|
| 1 | `config.h`, `types.h`, `credentials.h`, `credential_example.cpp` | Pins from v1 (4/15/17/16, 21/22, GPIO5, GPIO2), thresholds, `FW_VERSION = "v2.0.0"` |
| 2 | `storage_manager.h/.cpp` | **Preferences (NVS)** untuk WiFi list, MQTT config, calibration offsets, thresholds |
| 3 | `system_state.h`, `diagnostics.h/.cpp` | Enum state machine, serial log levels, fake data mode |
| 4 | `button_manager.h/.cpp`, `led_manager.h/.cpp` | Debounce, short=nav, long=enter (2s), LED blink pattern per aksi |
| 5 | `calibration.h/.cpp` | V/I offset additive, propagate ke VA/VAr/W/Wh/PF, Hz tidak terpengaruh |
| 6 | `data_acquisition.h/.cpp` | PZEM read 3 fasa, line status LOST/UNDER/OK/OVER, unbalance NEMA |
| 7 | `wifi_manager.h/.cpp`, `mqtt_manager.h/.cpp` | **AsyncMqttClient**, async connect, reconnect state machine |
| 8 | `json_builder.h/.cpp` | `{seq, device, phases{R,S,T}, unbalance, ts}` — numeric values, not strings |
| 9 | `display_manager.h/.cpp`, `menu_manager.h/.cpp` | Title bar Q/W, 3 monitor pages + 5 menu pages, cursor, confirm/countdown |
| 10 | `webserver_manager.h/.cpp` | AP + HTTP, login (`ADMIN`/`18273645`), WiFi/MQTT/Calibration forms, CSS from spec |
| 11 | `task_manager.h/.cpp` | FreeRTOS task scheduling, watchdog, timing diag |
| 12 | `main.ino` | Wiring, global state machine, boot sequence (non-blocking) |

**NTP:** Implement NTP sync → `ts` = Unix epoch, kirim `ts=0` sampai sync pertama berhasil.

Setuju? Jika ya, saya mulai implementasi.

---

## User

main.ino ganti menjadi Data_Logger_Monitor_v2.0.0.ino dan tambah README.md dengan dokumentasi selengkap2nya

---

## Assistant (Plan · DeepSeek V4 Flash Free · 9.2s)

_Thinking:_

The user wants two adjustments to my plan:

1. Rename `main.ino` to `Data_Logger_Monitor_v2.0.0.ino` — this makes sense since Arduino IDE requires the .ino file name to match its folder name.

2. Add a comprehensive `README.md` with full documentation.

Since I'm in plan mode, I cannot make any edits. I should confirm these changes to the plan and present the updated plan for approval.

Let me update the plan to reflect these two changes, and ask if they're ready for me to start implementation.

Noted. Dua perubahan:

1. **`main.ino` → `Data_Logger_Monitor_v2.0.0.ino`** — Arduino IDE convention (nama folder = nama .ino). Ditambahkan skill directory `log-skill/` di `.agents/skills/` seperti v1.

2. **`README.md`** di root folder v2.0.0 — berisi:
   - Deskripsi project, target hardware, pinout
   - Arsitektur firmware (file-by-file)
   - Daftar library + versi
   - Petunjuk setup (copy credentials, install board package, install libraries)
   - State machine diagram
   - MQTT JSON schema documentation
   - OLED UI reference (monitoring pages + menu tree)
   - Webpage configuration guide
   - Cara flashing dengan Arduino IDE

**Final tree:**

```
Data_Logger_Monitor_v2.0.0/
├── Data_Logger_Monitor_v2.0.0.ino    # Entry point
├── README.md                          # Full documentation
├── .agents/
│   └── skills/
│       └── log-skill/SKILL.md         # Session log skill
├── config.h
├── types.h
├── system_state.h
├── credentials.h
├── credential_example.cpp
├── storage_manager.h / .cpp
├── button_manager.h / .cpp
├── led_manager.h / .cpp
├── diagnostics.h / .cpp
├── calibration.h / .cpp
├── data_acquisition.h / .cpp
├── wifi_manager.h / .cpp
├── mqtt_manager.h / .cpp
├── json_builder.h / .cpp
├── display_manager.h / .cpp
├── menu_manager.h / .cpp
├── webserver_manager.h / .cpp
└── task_manager.h / .cpp
```

Konfirmasi: saya mulai implementasi?

---

## User

.agents tidak perlu dimasukkan

---

## Assistant (Plan · DeepSeek V4 Flash Free · 6.4s)

_Thinking:_

The user corrected me - no `.agents` directory needed in the v2.0.0 folder. Noted. Let me confirm the final tree without it and ask if I should proceed.

OK, dihapus. Final tree tanpa `.agents`:

```
Data_Logger_Monitor_v2.0.0/
├── Data_Logger_Monitor_v2.0.0.ino
├── README.md
├── config.h
├── types.h
├── system_state.h
├── credentials.h
├── credential_example.cpp
├── storage_manager.h / .cpp
├── button_manager.h / .cpp
├── led_manager.h / .cpp
├── diagnostics.h / .cpp
├── calibration.h / .cpp
├── data_acquisition.h / .cpp
├── wifi_manager.h / .cpp
├── mqtt_manager.h / .cpp
├── json_builder.h / .cpp
├── display_manager.h / .cpp
├── menu_manager.h / .cpp
├── webserver_manager.h / .cpp
└── task_manager.h / .cpp
```

**20 source files + 1 README** — 21 total. Mulai implementasi?

---


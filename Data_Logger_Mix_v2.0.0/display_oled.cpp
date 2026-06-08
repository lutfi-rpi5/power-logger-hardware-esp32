#include "display_oled.h"
#include "diagnostics.h"
#include <string.h>

/**
 * @file display_oled.cpp
 * @brief SSD1306 128×64 OLED rendering for monitoring and menu pages.
 *
 * Layout reference (all units in pixels, origin at top-left):
 * ```
 * Row  0: Title bar         "3Ph Data Logger" + [W][Q] icons (right-aligned)
 * Row  9: Separator line    ────────────────────────
 * Row 11: Phase headers     R         S         T
 * Row 19: Data row 1        Values    Values    Values
 * Row 27: Data row 2        Values    Values    Values
 * Row 35: Data row 3        Values    Values    Values
 * Row 43: Data row 4        Unbalance or menu items
 * Row 51: Data row 5        Reserved
 * ```
 *
 * Column centres: 21, 64, 107 (three equal 42-pixel divisions).
 * Font: Adafruit 5×7 (one char = 6 px width including spacing).
 */

DisplayOLED::DisplayOLED(uint8_t i2cAddr)
    : _disp(OLED_WIDTH, OLED_HEIGHT, &Wire, -1)
{}

/**
 * @brief Initialise the I2C bus and SSD1306 display controller.
 *
 * On failure (no display connected, wrong address, etc.), logs an error
 * but does NOT halt — the device can run headless.
 */
void DisplayOLED::begin() {
    Wire.begin(PIN_SDA, PIN_SCL);
    if (!_disp.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        diag.error("OLED", "SSD1306 init failed!");
        return;
    }
    _disp.setTextColor(SSD1306_WHITE);
    _disp.setTextSize(1);
    _disp.cp437(true);  // Enable Code Page 437 character set
    diag.info("OLED", "SSD1306 ready.");
    drawBoot(0);  // Show initial boot screen at 0%
}

/**
 * @brief Clear the display (black) and flush to hardware.
 */
void DisplayOLED::clear() { _clear(); commit(); }

/**
 * @brief Flush the internal framebuffer to SSD1306 over I2C.
 */
void DisplayOLED::commit() { _disp.display(); }

/**
 * @brief Draw the boot progress screen.
 *
 * Layout:
 * ```
 * 3-Phase Data Logger
 * v2.1.0
 * [████████░░░░░░░░░░░░]  40%
 * ```
 *
 * @param progressPercent 0–100 (fill width of the progress bar).
 */
void DisplayOLED::drawBoot(int progressPercent) {
    _clear();
    _disp.setTextSize(1);
    _disp.setCursor(10, 10);
    _disp.print("3-Phase Data Logger");
    _disp.setCursor(10, 24);
    _disp.print(FW_VERSION);

    // Progress bar: outer rectangle 108×10 px, inner fill proportional
    _disp.drawRect(10, 40, 108, 10, SSD1306_WHITE);
    int fillW = (108 * progressPercent) / 100;
    if (fillW > 0) {
        _disp.fillRect(11, 41, fillW, 8, SSD1306_WHITE);
    }
    commit();
}

// ─── Private helpers ─────────────────────────────────────────────────

void DisplayOLED::_clear() {
    _disp.clearDisplay();
    _disp.setTextSize(1);
}

/**
 * @brief Draw the title bar with connection status icons.
 *
 * Title is left-aligned. Icons are right-aligned, placed at pixel 127
 * and stepping left by 6 px per icon (one character width).
 *
 * Icons:
 * - "W" = Wi-Fi connected (shown rightmost)
 * - "Q" = MQTT connected (shown immediately left of W)
 *
 * @param title  Title string (truncated to fit ~18 characters).
 * @param wifiOk true = show "W" icon.
 * @param mqttOk true = show "Q" icon.
 */
void DisplayOLED::_header(const char* title, bool wifiOk, bool mqttOk) {
    _disp.setCursor(0, ROW_TITLE);
    _disp.print(title);
    // Right-align status icons at pixel column 127
    // Each icon is 6 px wide (one 5×7 character + 1 px spacing)
    int16_t x = 127;
    if (wifiOk) { x -= 6; _disp.setCursor(x, ROW_TITLE); _disp.print("W"); }
    if (mqttOk) { x -= 6; _disp.setCursor(x, ROW_TITLE); _disp.print("Q"); }
}

/**
 * @brief Draw a horizontal separator line at ROW_SEP (row 9).
 */
void DisplayOLED::_separator() {
    _disp.drawLine(0, ROW_SEP, 127, ROW_SEP, SSD1306_WHITE);
}

/**
 * @brief Draw three column headers: "R", "S", "T" at ROW_0.
 */
void DisplayOLED::_phaseHeaders() {
    _printCol(0, ROW_0, "R");
    _printCol(1, ROW_0, "S");
    _printCol(2, ROW_0, "T");
}

/**
 * @brief Print a string centred horizontally on a given X position.
 *
 * Calculates the starting X position as: cx - (strlen × 6 / 2).
 * Uses Adafruit 5×7 font where each character is 6 px wide
 * (5 px glyph + 1 px spacing).
 *
 * @param cx Centre X in display pixels.
 * @param y  Y position (row, in display pixels).
 * @param str Null-terminated string to print.
 */
void DisplayOLED::_printCX(int16_t cx, int16_t y, const char* str) {
    int16_t len = (int16_t)strlen(str) * 6;  // 6 px per character
    int16_t x   = cx - len / 2;
    if (x < 0) x = 0;
    _disp.setCursor(x, y);
    _disp.print(str);
}

/**
 * @brief Print a string in one of the three phase columns.
 * @param col Column index: 0=R, 1=S, 2=T.
 * @param y   Y position in pixels.
 * @param str String to print.
 */
void DisplayOLED::_printCol(uint8_t col, int16_t y, const char* str) {
    static const int16_t cx[3] = { COL0_CX, COL1_CX, COL2_CX };
    _printCX(cx[col < 3 ? col : 0], y, str);
}

/**
 * @name Value Formatting Helpers
 * Each formats a float into a display string with auto-scaling.
 *
 * Auto-scale thresholds:
 * - < 1: show "---V" for voltage (unreadable)
 * - < 10: 2 decimal places (precision important at low current)
 * - < 1000: base unit (W, VA, VAr, Wh)
 * - >= 1000: kilo prefix (kW, kVA, kVAr, kWh) with 2 decimals for energy
 * @{
 */

void DisplayOLED::_fmtVoltage(char* b, float v) {
    if (v < 1) snprintf(b, 10, "---V");    // Sensor fault / LOST
    else        snprintf(b, 10, "%.0fV", v);
}

void DisplayOLED::_fmtCurrent(char* b, float i) {
    if (i < 10) snprintf(b, 10, "%.2fA", i);  // Low current: more precision
    else         snprintf(b, 10, "%.1fA", i);
}

/// Auto-scale: W ↔ kW at 1000 threshold
void DisplayOLED::_fmtPower(char* b, float w) {
    if (w < 1000) snprintf(b, 10, "%.0fW",  w);
    else           snprintf(b, 10, "%.1fkW", w / 1000.0f);
}

/// Auto-scale: VA ↔ kVA at 1000 threshold
void DisplayOLED::_fmtApparent(char* b, float va) {
    if (va < 1000) snprintf(b, 10, "%.0fVA",  va);
    else            snprintf(b, 10, "%.1fkVA", va / 1000.0f);
}

/// Auto-scale: VAr ↔ kVAr at 1000 threshold
void DisplayOLED::_fmtReactive(char* b, float var) {
    if (var < 1000) snprintf(b, 10, "%.0fVAr",  var);
    else             snprintf(b, 10, "%.1fkVAr", var / 1000.0f);
}

/// Auto-scale: Wh ↔ kWh at 1000 threshold (2 decimals for kWh)
void DisplayOLED::_fmtEnergy(char* b, float wh) {
    if (wh < 1000) snprintf(b, 10, "%.0fWh",  wh);
    else            snprintf(b, 10, "%.2fkWh", wh / 1000.0f);
}

void DisplayOLED::_fmtFreq(char* b, float hz) {
    snprintf(b, 10, "%.1fHz", hz);
}

void DisplayOLED::_fmtPF(char* b, float pf) {
    snprintf(b, 10, "%.2fPF", pf);
}
/// @}

/**
 * @brief Draw a single menu line with optional selection cursor.
 * @param y        Y position (pixels).
 * @param selected If true, prepend "> " cursor.
 * @param text     Menu item text.
 */
void DisplayOLED::_menuLine(int16_t y, bool selected, const char* text) {
    _disp.setCursor(0, y);
    _disp.print(selected ? "> " : "  ");
    _disp.print(text);
}

/**
 * @brief Draw a two-option confirmation row with cursor selection.
 *
 * Left option is left-aligned; right option is right-aligned at
 * pixel 127 (minus text width). The cursor ">" is placed before
 * the selected option.
 *
 * @param y      Y position (pixels).
 * @param cursor 0 = select left, 1 = select right.
 * @param left   Left option label.
 * @param right  Right option label.
 */
void DisplayOLED::_confirmRow(int16_t y, int cursor, const char* left, const char* right) {
    _disp.setCursor(0, y);
    if (cursor == 0) {
        _disp.print("> "); _disp.print(left);
        int16_t rx = 127 - (int16_t)strlen(right) * 6;
        _disp.setCursor(rx, y); _disp.print(right);
    } else {
        _disp.print("  "); _disp.print(left);
        int16_t rx = 127 - ((int16_t)strlen(right) * 6 + 12);  // 12 = "> " width
        _disp.setCursor(rx, y); _disp.print("> "); _disp.print(right);
    }
}

// ─── Monitoring Pages ────────────────────────────────────────────────

/**
 * @brief Monitoring Page 1: Voltage / Current / Status / Unbalance.
 *
 * ```
 * 3Ph Data Logger  QW
 * ────────────────────
 *   R         S         T
 *  220V      220V      220V
 *  10.0A     10.0A     10.0A
 *   OK        OK        OK
 * Unbalance=2.50%
 * ```
 *
 * Status text blinks (alternates every OLED_BLINK_INTERVAL_MS)
 * when a phase has LOST/UNDER/OVER status.
 * [!] icon blinks next to the unbalance value if it exceeds the threshold.
 */
void DisplayOLED::drawMonitorPage1(const SystemState& s) {
    _clear();
    _header("3Ph Data Logger", s.wifiConnected, s.mqttConnected);
    _separator();
    _phaseHeaders();

    bool blinkOn = ((millis() / OLED_BLINK_INTERVAL_MS) % 2) == 0;
    char buf[24];

    for (int i = 0; i < 3; i++) {
        const PhaseReading& r = s.phases[i];
        _fmtVoltage(buf, r.valid ? r.voltage : 0);
        _printCol(i, ROW_1, buf);
        _fmtCurrent(buf, r.valid ? r.current : 0);
        _printCol(i, ROW_2, buf);
        // Blink status text if the phase is not OK
        bool isWarning = (r.status != LineStatus::OK);
        if (!isWarning || blinkOn) {
            _printCol(i, ROW_3, lineStatusStr(r.status));
        }
    }

    // Unbalance row with optional [!] warning
    float unbal    = s.unbalance;
    float unbalMax = s.thresholdUnbalanceMax;
    bool  unbalWarn = (unbal > unbalMax);
    if (unbalWarn) {
        if (blinkOn) snprintf(buf, sizeof(buf), "Unbalance=%.2f%% [!]", unbal);
        else         snprintf(buf, sizeof(buf), "Unbalance=%.2f%%", unbal);
    } else {
        snprintf(buf, sizeof(buf), "Unbalance=%.2f%%", unbal);
    }
    _disp.setCursor(0, ROW_4);
    _disp.print(buf);
    commit();
}

/**
 * @brief Monitoring Page 2: Frequency / Apparent Power / Reactive Power.
 *
 * ```
 * 3Ph Data Logger  QW
 * ────────────────────
 *   R         S         T
 * 50.0Hz    50.0Hz    50.0Hz
 * 2200VA    2300VA    2100VA
 * 1200VAr   1300VAr   1100VAr
 * ```
 *
 * Auto-scales VA↔kVA and VAr↔kVAr at the 1000 threshold.
 */
void DisplayOLED::drawMonitorPage2(const SystemState& s) {
    _clear();
    _header("3Ph Data Logger", s.wifiConnected, s.mqttConnected);
    _separator();
    _phaseHeaders();
    char buf[12];
    for (int i = 0; i < 3; i++) {
        const PhaseReading& r = s.phases[i];
        _fmtFreq(buf, r.valid ? r.frequency : 0);
        _printCol(i, ROW_1, buf);
        _fmtApparent(buf, r.valid ? r.apparentPower : 0);
        _printCol(i, ROW_2, buf);
        _fmtReactive(buf, r.valid ? r.reactivePower : 0);
        _printCol(i, ROW_3, buf);
    }
    commit();
}

/**
 * @brief Monitoring Page 3: Power Factor / Active Power / Energy.
 *
 * ```
 * 3Ph Data Logger  QW
 * ────────────────────
 *   R         S         T
 * 0.97PF    0.95PF    0.96PF
 * 2100W     2200W     2000W
 * 15230Wh   14800Wh   16000Wh
 * ```
 *
 * Auto-scales W↔kW and Wh↔kWh at the 1000 threshold.
 */
void DisplayOLED::drawMonitorPage3(const SystemState& s) {
    _clear();
    _header("3Ph Data Logger", s.wifiConnected, s.mqttConnected);
    _separator();
    _phaseHeaders();
    char buf[12];
    for (int i = 0; i < 3; i++) {
        const PhaseReading& r = s.phases[i];
        _fmtPF(buf, r.valid ? r.powerFactor : 0);
        _printCol(i, ROW_1, buf);
        _fmtPower(buf, r.valid ? r.activePower : 0);
        _printCol(i, ROW_2, buf);
        _fmtEnergy(buf, r.valid ? r.energyWh : 0);
        _printCol(i, ROW_3, buf);
    }
    commit();
}

// ─── Menu Pages ─────────────────────────────────────────────────────

/**
 * @brief Draw the main menu with 4 items.
 *
 * ```
 * 3Ph Data Logger  QW
 * ────────────────────
 * > Config
 *   Reset kWh
 *   Reboot Device
 *   Back to Monitoring
 * ```
 *
 * @param cursor Currently selected item (0–3).
 */
void DisplayOLED::drawMenuMain(int cursor, bool wifiOk, bool mqttOk) {
    _clear();
    _header("3Ph Data Logger", wifiOk, mqttOk);
    _separator();
    _menuLine(ROW_0, cursor == 0, "Config");
    _menuLine(ROW_1, cursor == 1, "Reset kWh");
    _menuLine(ROW_2, cursor == 2, "Reboot Device");
    _menuLine(ROW_3, cursor == 3, "Back to Monitoring");
    commit();
}

/**
 * @brief Draw the web configuration (AP toggle) screen.
 *
 * ```
 *     CONFIGURATION     QW
 * ────────────────────────
 * SSID: DataLogger
 * PW  : 12345678
 * 192.168.4.1
 * > Active         Back
 * ```
 *
 * @param cursor 0 = toggle Active/Inactive, 1 = Back.
 * @param active Current server state (shown as left option label).
 */
void DisplayOLED::drawMenuConfig(int cursor, bool active,
                                  const char* ssid, const char* pass, const char* ip,
                                  bool wifiOk, bool mqttOk)
{
    _clear();
    _header("CONFIGURATION", wifiOk, mqttOk);
    _separator();
    char buf[32];
    snprintf(buf, sizeof(buf), "SSID: %s", ssid);
    _disp.setCursor(0, ROW_0); _disp.print(buf);
    snprintf(buf, sizeof(buf), "PW  : %s", pass);
    _disp.setCursor(0, ROW_1); _disp.print(buf);
    _disp.setCursor(0, ROW_2); _disp.print(ip);
    const char* leftLabel = active ? "Active" : "Inactive";
    _confirmRow(ROW_3, cursor, leftLabel, "Back");
    commit();
}

/**
 * @brief Draw the Reset kWh confirmation screen.
 *
 * ```
 *     RESET kWh         QW
 * ────────────────────────
 * Apakah anda yakin
 * untuk Reset kWh?
 *
 * > Reset          Back
 * ```
 *
 * @param cursor 0 = Reset, 1 = Back.
 */
void DisplayOLED::drawMenuResetKwh(int cursor, bool wifiOk, bool mqttOk) {
    _clear();
    _header("RESET kWh", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_0); _disp.print("Apakah anda yakin");
    _disp.setCursor(0, ROW_1); _disp.print("untuk Reset kWh?");
    _confirmRow(ROW_3, cursor, "Reset", "Back");
    commit();
}

/**
 * @brief Draw the result of a kWh reset attempt.
 *
 * Auto-returns to main menu after MENU_AUTO_RETURN_MS (2 s).
 *
 * @param success true = show success message, false = show failure.
 */
void DisplayOLED::drawMenuResetResult(bool success, bool wifiOk, bool mqttOk) {
    _clear();
    _header("RESET kWh", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_1);
    _disp.print(success ? "  Reset kWh Berhasil" : "  Reset kWh Gagal!");
    commit();
}

/**
 * @brief Draw the Reboot confirmation screen.
 *
 * ```
 *     REBOOT            QW
 * ────────────────────────
 * Apakah anda yakin
 * untuk Reboot Device?
 *
 * > Reboot         Back
 * ```
 *
 * @param cursor 0 = Reboot, 1 = Back.
 */
void DisplayOLED::drawMenuReboot(int cursor, bool wifiOk, bool mqttOk) {
    _clear();
    _header("REBOOT", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_0); _disp.print("Apakah anda yakin");
    _disp.setCursor(0, ROW_1); _disp.print("untuk Reboot Device?");
    _confirmRow(ROW_3, cursor, "Reboot", "Back");
    commit();
}

/**
 * @brief Draw the reboot countdown screen.
 *
 * ```
 *     REBOOT            QW
 * ────────────────────────
 *   Device akan Reboot
 *     dalam 3 detik
 * ```
 *
 * Counts down from REBOOT_COUNTDOWN_SEC (3) to 0, then ESP.restart().
 *
 * @param secsLeft Seconds remaining before reboot.
 */
void DisplayOLED::drawMenuRebootCountdown(int secsLeft, bool wifiOk, bool mqttOk) {
    _clear();
    _header("REBOOT", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_1); _disp.print("  Device akan Reboot");
    char buf[24];
    snprintf(buf, sizeof(buf), "    dalam %d detik", secsLeft);
    _disp.setCursor(0, ROW_2); _disp.print(buf);
    commit();
}

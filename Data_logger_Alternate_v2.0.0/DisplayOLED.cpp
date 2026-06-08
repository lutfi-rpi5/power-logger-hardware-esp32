/******************************************************
 * File      : DisplayOLED.cpp
 ******************************************************/

#include "DisplayOLED.h"
#include <string.h>

DisplayOLED::DisplayOLED(uint8_t i2cAddr)
    : _disp(OLED_WIDTH, OLED_HEIGHT, &Wire, -1)
{}

void DisplayOLED::begin() {
    Wire.begin(PIN_SDA, PIN_SCL);
    if (!_disp.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        Serial.println("[OLED] SSD1306 init failed!");
        return;
    }
    _disp.setTextColor(SSD1306_WHITE);
    _disp.setTextSize(1);
    _disp.cp437(true);
    Serial.println("[OLED] SSD1306 ready.");
    drawBoot("Booting...");
}

void DisplayOLED::commit() {
    _disp.display();
}

// ─── Boot Screen ──────────────────────────────────────────

void DisplayOLED::drawBoot(const char* msg) {
    _clear();
    _disp.setTextSize(1);
    _disp.setCursor(0, 0);
    _disp.println("  3-Phase Data Logger");
    _disp.drawLine(0, 9, 127, 9, SSD1306_WHITE);
    _disp.setCursor(0, 14);
    _disp.println("    " FW_VERSION);
    _disp.setCursor(0, 28);
    if (msg) _disp.println(msg);
    _disp.setCursor(0, 44);
    _disp.println("  github/lutfi-rpi5");
    commit();
}

// ─── Common Helpers ───────────────────────────────────────

void DisplayOLED::_clear() {
    _disp.clearDisplay();
    _disp.setTextSize(1);
}

void DisplayOLED::_header(const char* title, bool wifiOk, bool mqttOk) {
    _disp.setCursor(0, ROW_TITLE);
    _disp.print(title);

    // Right-align connectivity icons
    // Q = MQTT, W = WiFi (shown only when connected)
    int16_t x = 127;
    if (wifiOk) {
        x -= 6;
        _disp.setCursor(x, ROW_TITLE);
        _disp.print("W");
    }
    if (mqttOk) {
        x -= 6;
        _disp.setCursor(x, ROW_TITLE);
        _disp.print("Q");
    }
}

void DisplayOLED::_separator() {
    _disp.drawLine(0, ROW_SEP, 127, ROW_SEP, SSD1306_WHITE);
}

void DisplayOLED::_phaseHeaders() {
    _printCol(0, ROW_0, "R");
    _printCol(1, ROW_0, "S");
    _printCol(2, ROW_0, "T");
}

void DisplayOLED::_printCX(int16_t cx, int16_t y, const char* str) {
    int16_t len = (int16_t)strlen(str) * 6;
    int16_t x   = cx - len / 2;
    if (x < 0) x = 0;
    _disp.setCursor(x, y);
    _disp.print(str);
}

void DisplayOLED::_printCol(uint8_t col, int16_t y, const char* str) {
    static const int16_t cx[3] = { COL0_CX, COL1_CX, COL2_CX };
    _printCX(cx[col < 3 ? col : 0], y, str);
}

// ─── Formatters ───────────────────────────────────────────

void DisplayOLED::_fmtVoltage(char* b, float v) {
    if (v < 1) snprintf(b, 10, "---V");
    else        snprintf(b, 10, "%.0fV", v);
}

void DisplayOLED::_fmtCurrent(char* b, float i) {
    if (i < 10) snprintf(b, 10, "%.2fA", i);
    else         snprintf(b, 10, "%.1fA", i);
}

void DisplayOLED::_fmtPower(char* b, float w) {
    if (w < 1000) snprintf(b, 10, "%.0fW",  w);
    else           snprintf(b, 10, "%.1fkW", w / 1000.0f);
}

void DisplayOLED::_fmtApparent(char* b, float va) {
    if (va < 1000) snprintf(b, 10, "%.0fVA",  va);
    else            snprintf(b, 10, "%.1fkVA", va / 1000.0f);
}

void DisplayOLED::_fmtReactive(char* b, float var) {
    if (var < 1000) snprintf(b, 10, "%.0fVAr",  var);
    else             snprintf(b, 10, "%.1fkVAr", var / 1000.0f);
}

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

// ─── Menu line helpers ────────────────────────────────────

void DisplayOLED::_menuLine(int16_t y, bool selected, const char* text) {
    _disp.setCursor(0, y);
    _disp.print(selected ? "> " : "  ");
    _disp.print(text);
}

// cursor==0 → "> Left    Right"   cursor==1 → "  Left  > Right"
void DisplayOLED::_confirmRow(int16_t y, int cursor, const char* left, const char* right) {
    _disp.setCursor(0, y);
    if (cursor == 0) {
        _disp.print("> ");
        _disp.print(left);
        // right-align second option
        int16_t rx = 127 - (int16_t)strlen(right) * 6;
        _disp.setCursor(rx, y);
        _disp.print(right);
    } else {
        _disp.print("  ");
        _disp.print(left);
        int16_t rx = 127 - ((int16_t)strlen(right) * 6 + 12);
        _disp.setCursor(rx, y);
        _disp.print("> ");
        _disp.print(right);
    }
}

// ════════════════════════════════════════════════════════════
// MONITORING PAGES
// ════════════════════════════════════════════════════════════

void DisplayOLED::drawMonitorPage1(const SystemState& s) {
    _clear();
    _header("3Ph Data Logger", s.wifiConnected, s.mqttConnected);
    _separator();
    _phaseHeaders();

    // Blink state: toggles every OLED_BLINK_INTERVAL_MS
    // Used to flash warning status text and unbalance [!] icon
    bool blinkOn = ((millis() / OLED_BLINK_INTERVAL_MS) % 2) == 0;

    char buf[24];
    for (int i = 0; i < 3; i++) {
        const PhaseReading& r = s.phases[i];

        _fmtVoltage(buf, r.valid ? r.voltage : 0);
        _printCol(i, ROW_1, buf);

        _fmtCurrent(buf, r.valid ? r.current : 0);
        _printCol(i, ROW_2, buf);

        // Status: blink when LOST / UNDER / OVER (alarm condition)
        bool isWarning = (r.status != LineStatus::OK);
        if (!isWarning || blinkOn) {
            _printCol(i, ROW_3, lineStatusStr(r.status));
        }
        // When blinkOn==false for a warning status, that column is left blank → blink effect
    }

    // ── Unbalance row ────────────────────────────────────
    float unbal    = s.unbalance;
    float unbalMax = s.thresholdUnbalanceMax;
    bool  unbalWarn = (unbal > unbalMax);

    if (unbalWarn) {
        // Blink between "Unbalance=X.XX% [!]" and "Unbalance=X.XX%"
        if (blinkOn) {
            snprintf(buf, sizeof(buf), "Unbalance=%.2f%% [!]", unbal);
        } else {
            snprintf(buf, sizeof(buf), "Unbalance=%.2f%%", unbal);
        }
    } else {
        snprintf(buf, sizeof(buf), "Unbalance=%.2f%%", unbal);
    }
    _disp.setCursor(0, ROW_4);
    _disp.print(buf);

    commit();
}

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

// ════════════════════════════════════════════════════════════
// MENU SCREENS
// ════════════════════════════════════════════════════════════

// cursor: 0=Config, 1=Reset kWh, 2=Reboot, 3=Back to Monitoring
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

// cursor: 0=Active/Inactive toggle, 1=Back
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

// cursor: 0=Reset, 1=Back
void DisplayOLED::drawMenuResetKwh(int cursor, bool wifiOk, bool mqttOk) {
    _clear();
    _header("RESET kWh", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_0); _disp.print("Apakah anda yakin");
    _disp.setCursor(0, ROW_1); _disp.print("untuk Reset kWh?");
    // ROW_2 empty
    _confirmRow(ROW_3, cursor, "Reset", "Back");
    commit();
}

void DisplayOLED::drawMenuResetResult(bool success, bool wifiOk, bool mqttOk) {
    _clear();
    _header("RESET kWh", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_1);
    _disp.print(success ? "  Reset kWh Berhasil" : "  Reset kWh Gagal!");
    commit();
}

// cursor: 0=Reboot, 1=Back
void DisplayOLED::drawMenuReboot(int cursor, bool wifiOk, bool mqttOk) {
    _clear();
    _header("REBOOT", wifiOk, mqttOk);
    _separator();
    _disp.setCursor(0, ROW_0); _disp.print("Apakah anda yakin");
    _disp.setCursor(0, ROW_1); _disp.print("untuk Reboot Device?");
    _confirmRow(ROW_3, cursor, "Reboot", "Back");
    commit();
}

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

#pragma once

/******************************************************
 * File      : DisplayOLED.h
 * Description:
 *   Low-level OLED SSD1306 rendering layer.
 *   Draws all screen layouts (monitoring pages + menus).
 *   MenuSystem calls these; DisplayOLED knows nothing
 *   about navigation logic.
 *
 *   Display: 128×64 px, text size 1 → 21 chars × 8 rows
 *   Column layout (3-phase): col0=x0-42, col1=x43-85, col2=x86-127
 *   Column centers: 21, 64, 107
 ******************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "PhaseData.h"

// Y positions for each row (px)
#define ROW_TITLE   0
#define ROW_SEP     9   // separator line
#define ROW_0      11   // first data row
#define ROW_1      19
#define ROW_2      27
#define ROW_3      35
#define ROW_4      43
#define ROW_5      51

// Column center X positions
#define COL0_CX    21
#define COL1_CX    64
#define COL2_CX   107

struct MenuDrawState {
    int   monitorPage   = 0;     // 0, 1, 2
    int   menuCursor    = 0;
    bool  webServerOn   = false;
    char  apSSID[32]    = "";
    char  apPass[32]    = "";
    char  apIP[16]      = "";
    bool  resetSuccess  = false;
    int   rebootSecs    = 3;
};

class DisplayOLED {
public:
    explicit DisplayOLED(uint8_t i2cAddr = OLED_I2C_ADDR);

    void begin();

    // ── Monitoring screens ───────────────────────────────
    void drawMonitorPage1(const SystemState& s);  // V, I, Status
    void drawMonitorPage2(const SystemState& s);  // F, S(VA), Q(VAr)
    void drawMonitorPage3(const SystemState& s);  // PF, P(W), E(Wh)

    // ── Menu screens ─────────────────────────────────────
    void drawMenuMain(int cursor, bool wifiOk, bool mqttOk);
    void drawMenuConfig(int cursor, bool active,
                        const char* ssid, const char* pass, const char* ip,
                        bool wifiOk, bool mqttOk);
    void drawMenuResetKwh(int cursor, bool wifiOk, bool mqttOk);
    void drawMenuResetResult(bool success, bool wifiOk, bool mqttOk);
    void drawMenuReboot(int cursor, bool wifiOk, bool mqttOk);
    void drawMenuRebootCountdown(int secsLeft, bool wifiOk, bool mqttOk);

    // ── Boot/splash screen ───────────────────────────────
    void drawBoot(const char* msg = nullptr);

    void commit();  // push buffer to display

private:
    Adafruit_SSD1306 _disp;

    // ── Common helpers ───────────────────────────────────
    void _clear();
    void _header(const char* title, bool wifiOk, bool mqttOk);
    void _separator();
    void _phaseHeaders();

    // Print text centered around pixel x, at y
    void _printCX(int16_t cx, int16_t y, const char* str);
    // Print text in 3-column layout (col = 0/1/2)
    void _printCol(uint8_t col, int16_t y, const char* str);

    // Value formatters (write to buf)
    static void _fmtVoltage (char* b, float v);
    static void _fmtCurrent (char* b, float i);
    static void _fmtPower   (char* b, float w);       // W / kW
    static void _fmtApparent(char* b, float va);      // VA / kVA
    static void _fmtReactive(char* b, float var);     // VAr / kVAr
    static void _fmtEnergy  (char* b, float wh);      // Wh / kWh
    static void _fmtFreq    (char* b, float hz);
    static void _fmtPF      (char* b, float pf);

    // Cursor line: ">" prefix + item text, with indentation for non-selected
    void _menuLine(int16_t y, bool selected, const char* text);
    // Horizontal confirm row: "> Left      Right" or "  Left    > Right"
    void _confirmRow(int16_t y, int cursor, const char* left, const char* right);
};

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "types.h"

// ============================================================
// OLED display manager — SSD1306 128x64 I2C
// ============================================================
// Handles title bar with Q/W icons, renders monitoring pages,
// and provides helpers for menu rendering.
// ============================================================

class DisplayManager {
public:
    DisplayManager();

    void begin();
    void clear();
    void display();

    // Title bar (shown on every page)
    void drawTitleBar(const char* title, bool wifiOn, bool mqttOn);

    // Monitoring pages
    void drawMonitorPage1(const PhaseData phases[3], float unbalance, const ThresholdData& thr);
    void drawMonitorPage2(const PhaseData phases[3]);
    void drawMonitorPage3(const PhaseData phases[3]);

    // Menu helpers
    void drawMenuTitle(const char* title, bool wifiOn, bool mqttOn);
    void drawMenuItems(const char* items[], int count, int cursor, int cols);
    void drawConfirmPrompt(const char* line1, const char* line2,
                           const char* leftLabel, const char* rightLabel,
                           bool cursorOnLeft);
    void drawStatusMessage(const char* line1, const char* line2);

    // Config page
    void drawConfigPage(const char* ssid, const char* pass, const char* ip,
                        bool active, bool cursorOnLeft);

    // Boot screen
    void drawBootScreen(int progressPercent);

    // utility
    static String formatUnit(float value, const char* unit, const char* kUnit, uint16_t threshold = 1000);

private:
    Adafruit_SSD1306 _display;
    unsigned long _lastDraw;
    uint16_t _refreshInterval;

    // Internal helpers
    void _drawPhaseHeader(int y);
    void _drawPhaseRow(int y, const char* col1, const char* col2, const char* col3);
    const char* _statusText(LineStatus s);
};

#endif

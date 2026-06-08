#include "display_manager.h"
#include "config.h"
#include "diagnostics.h"
#include <math.h>

DisplayManager::DisplayManager()
    : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1)
    , _lastDraw(0)
    , _refreshInterval(OLED_REFRESH_INTERVAL_MS)
{}

void DisplayManager::begin() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        diag.error("OLED", "SSD1306 init failed");
        return;
    }
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.display();
    diag.info("OLED", "Display initialized");
}

void DisplayManager::clear() {
    _display.clearDisplay();
}

void DisplayManager::display() {
    _display.display();
}

void DisplayManager::drawTitleBar(const char* title, bool wifiOn, bool mqttOn) {
    _display.setTextSize(1);
    _display.setCursor(0, 0);

    // Title (truncate if needed)
    String t = title;
    int maxLen = 16;  // 128px / ~8px per char at size 1
    if (t.length() > (unsigned)maxLen) t = t.substring(0, maxLen);
    _display.print(t);

    // Q/W icons (right-aligned)
    int x = OLED_WIDTH - 2;
    if (mqttOn) {
        x -= 8;
        _display.setCursor(x, 0);
        _display.print("Q");
    }
    if (wifiOn) {
        x -= 8;
        _display.setCursor(x, 0);
        _display.print("W");
    }

    // Separator line
    _display.drawLine(0, 9, OLED_WIDTH - 1, 9, SSD1306_WHITE);
}

void DisplayManager::drawMonitorPage1(const PhaseData phases[3], float unbalance, const ThresholdData& thr) {
    _display.setTextSize(1);
    int y = 14;

    // Phase header
    _display.setCursor(8, y);  _display.print("R");
    _display.setCursor(52, y); _display.print("S");
    _display.setCursor(96, y); _display.print("T");
    y += 10;

    // Voltage
    for (int i = 0; i < 3; i++) {
        char buf[12];
        snprintf(buf, sizeof(buf), "%.0fV", phases[i].voltage);
        _display.setCursor(2 + i * 44, y);
        _display.print(buf);
    }
    y += 10;

    // Current
    for (int i = 0; i < 3; i++) {
        char buf[12];
        snprintf(buf, sizeof(buf), "%.1fA", phases[i].current);
        _display.setCursor(2 + i * 44, y);
        _display.print(buf);
    }
    y += 10;

    // Status
    for (int i = 0; i < 3; i++) {
        _display.setCursor(2 + i * 44, y);
        _display.print(_statusText(phases[i].status));
    }
    y += 10;

    // Unbalance
    char buf[20];
    bool warn = unbalance > thr.unbalanceMax;
    snprintf(buf, sizeof(buf), "Unbal=%.1f%%", unbalance);
    _display.setCursor(2, y);
    _display.print(buf);
    if (warn) {
        _display.setCursor(100, y);
        _display.print("[!]");
    }
}

void DisplayManager::drawMonitorPage2(const PhaseData phases[3]) {
    _display.setTextSize(1);
    int y = 14;

    // Phase header
    _display.setCursor(8, y);  _display.print("R");
    _display.setCursor(52, y); _display.print("S");
    _display.setCursor(96, y); _display.print("T");
    y += 10;

    // Frequency
    for (int i = 0; i < 3; i++) {
        char buf[12];
        snprintf(buf, sizeof(buf), "%.1fHz", phases[i].frequency);
        _display.setCursor(2 + i * 44, y);
        _display.print(buf);
    }
    y += 10;

    // Apparent power (auto-scale VA/kVA)
    for (int i = 0; i < 3; i++) {
        String s = formatUnit(phases[i].apparent, "VA", "kVA");
        _display.setCursor(2 + i * 44, y);
        _display.print(s);
    }
    y += 10;

    // Reactive power (auto-scale VAr/kVAr)
    for (int i = 0; i < 3; i++) {
        String s = formatUnit(phases[i].reactive, "VAr", "kVAr");
        _display.setCursor(2 + i * 44, y);
        _display.print(s);
    }
}

void DisplayManager::drawMonitorPage3(const PhaseData phases[3]) {
    _display.setTextSize(1);
    int y = 14;

    // Phase header
    _display.setCursor(8, y);  _display.print("R");
    _display.setCursor(52, y); _display.print("S");
    _display.setCursor(96, y); _display.print("T");
    y += 10;

    // Power factor
    for (int i = 0; i < 3; i++) {
        char buf[12];
        snprintf(buf, sizeof(buf), "%.2fPF", phases[i].pf);
        _display.setCursor(2 + i * 44, y);
        _display.print(buf);
    }
    y += 10;

    // Active power (auto-scale W/kW)
    for (int i = 0; i < 3; i++) {
        String s = formatUnit(phases[i].power, "W", "kW");
        _display.setCursor(2 + i * 44, y);
        _display.print(s);
    }
    y += 10;

    // Energy (auto-scale Wh/kWh)
    for (int i = 0; i < 3; i++) {
        String s = formatUnit(phases[i].energy, "Wh", "kWh");
        _display.setCursor(2 + i * 44, y);
        _display.print(s);
    }
}

void DisplayManager::drawMenuTitle(const char* title, bool wifiOn, bool mqttOn) {
    drawTitleBar(title, wifiOn, mqttOn);
}

void DisplayManager::drawMenuItems(const char* items[], int count, int cursor, int cols) {
    _display.setTextSize(1);
    int y = 14;

    for (int i = 0; i < count; i++) {
        int col = i % cols;
        int row = i / cols;
        int x = col * (OLED_WIDTH / cols) + 2;
        int yp = y + row * 10;

        _display.setCursor(x, yp);
        if (i == cursor) {
            _display.print(">");
            _display.setCursor(x + 8, yp);
        } else {
            _display.setCursor(x + 8, yp);
        }
        _display.print(items[i]);
    }
}

void DisplayManager::drawConfirmPrompt(const char* line1, const char* line2,
                                        const char* leftLabel, const char* rightLabel,
                                        bool cursorOnLeft) {
    _display.setTextSize(1);
    int y = 16;

    _display.setCursor(10, y); _display.print(line1);
    y += 10;
    _display.setCursor(10, y); _display.print(line2);
    y += 14;

    // Left option
    _display.setCursor(20, y);
    if (cursorOnLeft) _display.print(">");
    else _display.print(" ");
    _display.print(" ");
    _display.print(leftLabel);

    // Right option
    _display.setCursor(80, y);
    if (!cursorOnLeft) _display.print(">");
    else _display.print(" ");
    _display.print(" ");
    _display.print(rightLabel);
}

void DisplayManager::drawStatusMessage(const char* line1, const char* line2) {
    _display.setTextSize(1);
    _display.setCursor(10, 20);
    _display.print(line1);
    _display.setCursor(10, 32);
    _display.print(line2);
}

void DisplayManager::drawConfigPage(const char* ssid, const char* pass, const char* ip,
                                     bool active, bool cursorOnLeft) {
    _display.setTextSize(1);
    _display.setCursor(2, 14);
    _display.print("SSID: ");
    _display.print(ssid);
    _display.setCursor(2, 24);
    _display.print("PW  : ");
    _display.print(pass);
    _display.setCursor(2, 34);
    _display.print(ip);
    _display.setCursor(2, 48);
    if (cursorOnLeft) _display.print("> ");
    else _display.print("  ");
    _display.print(active ? "Active" : "Inactive");
    _display.setCursor(80, 48);
    if (!cursorOnLeft) _display.print("> ");
    else _display.print("  ");
    _display.print("Back");
}

void DisplayManager::drawBootScreen(int progressPercent) {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setCursor(10, 10);
    _display.print("3-Phase Data Logger");
    _display.setCursor(10, 24);
    _display.print("v2.0.0");

    // Progress bar
    _display.drawRect(10, 40, 108, 10, SSD1306_WHITE);
    int fillW = (108 * progressPercent) / 100;
    if (fillW > 0) {
        _display.fillRect(11, 41, fillW, 8, SSD1306_WHITE);
    }
    _display.display();
}

String DisplayManager::formatUnit(float value, const char* unit, const char* kUnit, uint16_t threshold) {
    char buf[16];
    if (value >= (float)threshold) {
        snprintf(buf, sizeof(buf), "%.1f%s", value / 1000.0f, kUnit);
    } else {
        snprintf(buf, sizeof(buf), "%.1f%s", value, unit);
    }
    return String(buf);
}

const char* DisplayManager::_statusText(LineStatus s) {
    switch (s) {
        case LineStatus::LOST:  return "LOST";
        case LineStatus::UNDER: return "UNDER";
        case LineStatus::OVER:  return "OVER";
        default:                return "OK";
    }
}

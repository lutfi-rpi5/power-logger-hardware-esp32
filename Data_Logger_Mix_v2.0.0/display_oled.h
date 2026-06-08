#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "types.h"

#define ROW_TITLE   0
#define ROW_SEP     9
#define ROW_0      11
#define ROW_1      19
#define ROW_2      27
#define ROW_3      35
#define ROW_4      43
#define ROW_5      51

#define COL0_CX    21
#define COL1_CX    64
#define COL2_CX   107

class DisplayOLED {
public:
    explicit DisplayOLED(uint8_t i2cAddr = OLED_I2C_ADDR);
    void begin();

    void drawMonitorPage1(const SystemState& s);
    void drawMonitorPage2(const SystemState& s);
    void drawMonitorPage3(const SystemState& s);

    void drawMenuMain(int cursor, bool wifiOk, bool mqttOk);
    void drawMenuConfig(int cursor, bool active,
                        const char* ssid, const char* pass, const char* ip,
                        bool wifiOk, bool mqttOk);
    void drawMenuResetKwh(int cursor, bool wifiOk, bool mqttOk);
    void drawMenuResetResult(bool success, bool wifiOk, bool mqttOk);
    void drawMenuReboot(int cursor, bool wifiOk, bool mqttOk);
    void drawMenuRebootCountdown(int secsLeft, bool wifiOk, bool mqttOk);

    void drawBoot(int progressPercent);
    void clear();
    void commit();

private:
    Adafruit_SSD1306 _disp;

    void _clear();
    void _header(const char* title, bool wifiOk, bool mqttOk);
    void _separator();
    void _phaseHeaders();
    void _printCX(int16_t cx, int16_t y, const char* str);
    void _printCol(uint8_t col, int16_t y, const char* str);

    static void _fmtVoltage (char* b, float v);
    static void _fmtCurrent (char* b, float i);
    static void _fmtPower   (char* b, float w);
    static void _fmtApparent(char* b, float va);
    static void _fmtReactive(char* b, float var);
    static void _fmtEnergy  (char* b, float wh);
    static void _fmtFreq    (char* b, float hz);
    static void _fmtPF      (char* b, float pf);

    void _menuLine(int16_t y, bool selected, const char* text);
    void _confirmRow(int16_t y, int cursor, const char* left, const char* right);
};

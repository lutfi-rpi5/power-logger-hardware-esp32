#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C

class DisplayOLED {
public:
    DisplayOLED();
    void begin();
    void update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]);

private:
    Adafruit_SSD1306 display;
    uint8_t page = 0;
    unsigned long lastUpdate = 0;
    const uint16_t interval = 3000; // 1s async refresh

    void drawPage1(float v[3], float i[3]);
    void drawPage2(float p[3], float pf[3], float f[3]);
    void drawPage3(float e[3], int status[3]);
    const char* statusToText(int st);
};

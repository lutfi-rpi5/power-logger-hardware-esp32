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
    DisplayOLED(uint8_t address = OLED_ADDR);
    void begin();
    // update dipanggil tiap loop agar non-blocking
    void update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]);

    // Kontrol page dari luar (button)
    void setPage(uint8_t p);
    void enableAutoScroll(bool en); // kalau false, page hanya diganti oleh setPage
    uint8_t getPage() const { return page; }

private:
    Adafruit_SSD1306 display;
    uint8_t page;
    unsigned long lastUpdate;
    const uint16_t interval = 800; // refresh tiap 800ms

    bool autoScroll;

    void drawPage1(float v[3], float i[3]);
    void drawPage2(float p[3], float pf[3], float f[3]);
    void drawPage3(float e[3], int status[3]);
    const char* statusToText(int st);
};

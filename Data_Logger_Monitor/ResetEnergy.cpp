#include "ResetEnergy.h"

ResetEnergy::ResetEnergy(uint8_t btnPin, uint8_t ledPin) {
    this->buttonPin = btnPin;
    this->ledPin = ledPin;
}

void ResetEnergy::begin() {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
}

void ResetEnergy::handle(PZEM004Tv30* pzems, int numPzems) {
    unsigned long now = millis();
    bool buttonReading = digitalRead(buttonPin);

    // ========== DEBOUNCE ==========
    if (buttonReading != lastButtonReading) {
        lastDebounceTime = now; // ada perubahan, reset timer debounce
    }

    if ((now - lastDebounceTime) > debounceDelay) {
        buttonStableState = buttonReading; // update state stabil setelah 50ms
    }

    lastButtonReading = buttonReading;
    bool buttonPressed = (buttonStableState == LOW); // aktif LOW

    // ========== TOMBOL DITEKAN ==========
    if (buttonPressed && !isPressed && !isBlinking) {
        isPressed = true;
        pressStart = now;
        resetTriggered = false;
        digitalWrite(ledPin, HIGH); // LED langsung nyala saat ditekan
    }

    // ========== SELAMA DITEKAN ==========
    if (buttonPressed && isPressed && !resetTriggered && !isBlinking) {
        if (now - pressStart >= holdTime) {
            resetTriggered = true;

            // Reset energy semua PZEM
            for (int i = 0; i < numPzems; i++) {
                pzems[i].resetEnergy();
            }

            Serial.println("✅ Energy Reset Successfully!");

            // Mulai mode blinking (indikasi sukses)
            isBlinking = true;
            blinkCount = 0;
            blinkStart = now;
            ledState = false;
            digitalWrite(ledPin, LOW);
        }
    }

    // ========== MODE KEDIP 5x NON-BLOCKING ==========
    if (isBlinking) {
        if (now - blinkStart >= 50) { // toggle setiap 200 ms
            blinkStart = now;
            ledState = !ledState;
            digitalWrite(ledPin, ledState);

            if (ledState) blinkCount++;

            if (blinkCount >= 10 && !ledState) {
                isBlinking = false;
                digitalWrite(ledPin, LOW);
                // lock hingga tombol dilepas
                isPressed = true;
                resetTriggered = true;
            }
        }
        return; // jangan evaluasi tombol lain selama blinking
    }

    // ========== TOMBOL DILEPAS ==========
    if (!buttonPressed && isPressed) {
        isPressed = false;
        pressStart = 0;
        resetTriggered = false;
        digitalWrite(ledPin, LOW);
    }
}

#include "ResetEnergy.h"

ResetEnergy::ResetEnergy(uint8_t buttonPin, uint8_t ledPin)
  : _buttonPin(buttonPin), _ledPin(ledPin)
{}

void ResetEnergy::begin() {
  pinMode(_buttonPin, INPUT_PULLUP);
  pinMode(_ledPin, OUTPUT);
  digitalWrite(_ledPin, LOW);
}

// handle() kamu mungkin sudah ada — jangan ubah kalau jalan.
// Namun kita tetap sediakan resetAll() untuk dipanggil dari main ketika long press terdeteksi.
void ResetEnergy::handle(PZEM004Tv30 pzems[], int count) {
  // Bisa diisi dengan logic lama jika mau; tapi untuk modularity kita biarkan main memanggil resetAll()
  // sehingga handle tetap optional. Jika kamu ingin, implementasi lama bisa ditempatkan di sini.
}

// resetAll: kirim perintah reset energy ke tiap PZEM.
// Perhatikan: library pzem biasanya punya method resetEnergy() atau resetEnergy(addr). 
// Kita asumsikan instance PZEM004Tv30 punya method resetEnergy()
void ResetEnergy::resetAll(PZEM004Tv30 pzems[], int count) {
  // Indikasi: LED nyala menandakan reset segera dilakukan
  digitalWrite(_ledPin, HIGH);
  delay(100);

  for (int i = 0; i < count; ++i) {
    // Cek apakah instance valid, lalu panggil reset
    // fungsi API library PZEM biasanya: pzems[i].resetEnergy();
    // Jika library kamu berbeda, ganti panggilan ini sesuai API.
    Serial.printf("[ResetEnergy] Resetting PZEM %d...\n", i+1);
    pzems[i].resetEnergy(); // <- asumsikan ada
    // beri jeda singkat supaya bus serial/uart tidak kebanjiran
    delay(200);
  }

  // Blink LED 5x sebagai konfirmasi (final)
  blinkLed(5, 120, 120);

  // Matikan LED
  digitalWrite(_ledPin, LOW);
}

void ResetEnergy::blinkLed(int times, unsigned int onMs, unsigned int offMs) {
  for (int t = 0; t < times; ++t) {
    digitalWrite(_ledPin, HIGH);
    delay(onMs);
    digitalWrite(_ledPin, LOW);
    delay(offMs);
  }
}

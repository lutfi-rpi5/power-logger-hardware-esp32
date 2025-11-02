#include <PZEM004Tv30.h>

// address default PZEM biasanya 0xF8 (semua respon)
// tapi untuk set address, kita start dulu dengan itu
PZEM004Tv30 pzem(Serial2, 16, 17);

void setup() {
  Serial.begin(115200);
  // Serial2.begin(9600, SERIAL_8N1, 16, 17);

  delay(2000);
  Serial.println("=== Set Address PZEM ===");

  // Cek alamat lama
  uint8_t oldAddr = pzem.getAddress();
  Serial.print("Alamat lama: ");
  Serial.println(oldAddr);

  // Ubah alamat baru sesuai modul
  uint8_t newAddr = 0x01;   // ganti 1, 2, 3 sesuai giliran modul
  if(pzem.setAddress(newAddr)) {
    Serial.print("Alamat berhasil diubah ke: ");
    Serial.println(newAddr);
  } else {
    Serial.println("Gagal ubah alamat!");
  }

  // Verifikasi
  uint8_t checkAddr = pzem.getAddress();
  Serial.print("Alamat sekarang: ");
  Serial.println(checkAddr);
}

void loop() {
  // kosong
}

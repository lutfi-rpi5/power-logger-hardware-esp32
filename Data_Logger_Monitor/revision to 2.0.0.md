ini adalah project yang sudah jadi dan sudah rilis v1.0.0
ini penjelasan project yang sekarang dan yang akan di update firmware nya
saya lampirkan program yang akan di kembangkan menjadi v2.0.0

Nama Project  : Data Logger 3 Phase

Overview Alat (Hardware)      : konteksnya alat ini hanya berfungsi untuk akuisisi data dan mengirimkan ke mqtt broker (tidak melakukan data logger), data logger sdh ada unit website yang khusus menangani data logging yang datanya di ambil dari mqtt broker
                                1. Akuisisi Data dari 3 buah PZEM-004T (Masing2 PZEM handle 1 Line RST to Netral (WYE/220V))
                                2. Pengolahan Data
                                3. Publish Data ke MQTT Broker (per topic)
                                4. Handle Display OLED 0.96" (display tampilan sangat alakadarnya) dan push button untuk ganti slide page parameter 3 phase
                                5. bisa management signal wi-fi dan prioritasnya (masih manual (hard-code) atur priotitas)
                                6. ada fitur debug berupa kirim data random ke broker namun masih harus buka tutup komentar di baris program (dan masih cuma punya 1 jenis debug aja)
                                7. masih ada beberapa kekurangan/bug sistem seperti:
                                  a. [asinkron data] data antar parameter yang sampai ke broker atau website tidak serentak/tidak bersamaan (ada yang terlalu cepat/terlalu lambat sepersekian detik) karena masih publish mentah per topic dan belum pakai json
                                  b. [blocking] saat booting esp32 terus melakukan connecting ke wifi dan tidak akan masuk display utama sampai esp32 terkoneksi ke wifi
                                  c. belum ada fitur kalibrasi
                                  d. belum ada webserver & webpage (untuk config dan kalibrasi)
                                  e. masih ada sistem blocking
                                  f. belum ada state machine & self-healing kalau error di lapangan
                                  g. display OLED masih polosan raw text dan susunan parameter masih berantakan
                                  h. Wifi Selector belum seperti pintar network management windows maupun smartphone yang mempunyai known network dan pintar mencari prioritas serta otomatis tersambung 

Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin (Arduino IDE v2.0 & Board Package ESP32 v2.0.14 upgrade ke v3.3.0 untuk firmware datalogger v2.0.0)
                                2. PZEM-004T 100A (3 buah, masing2 handle 1 fasa dari RST ke Netral (WYE/220V)) 
                                3. OLED 0.96" I2C
                                4. Push Button (Normally Open, Not latching) untuk Menu OLED
                                5. Built-in LED ESP32 untuk signal indikator seperti reset kWh

Konfigurasi Pinout Electrical : 
//************* PZEM ****************//
- PZEM Fasa R dan S dalam satu hardware serial 1
- PZEM Fasa T berada dalam hardware serial 2 

#define PZEM_RX1_PIN 4
#define PZEM_TX1_PIN 15
#define PZEM_RX2_PIN 17
#define PZEM_TX2_PIN 16
// #define PZEM_SERIAL Serial2
HardwareSerial PZEMSerial1(1);
HardwareSerial PZEMSerial2(2);
#define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM

PZEM004Tv30 pzems[NUM_PZEMS] = {
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
  PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
};
//************* PZEM ****************//

//************* OLED 0.96" I2C ****************//
SDA= GPIO 21
SCL= GPIO 22
//************* OLED 0.96" I2C ****************//

//*************** Button & LED ****************//
BUTTON  5
LED     2
//*************** Button & LED ****************//

* Board Package & Libraries yang sudah diinstall di firmware Data Logger v1.0.0 ini (install via Board & Library Manager):
*   Board Package   : Board Package ESP32 v2.0.14 (pada firmware data logger v2.0.0 gunakan ESP32 v3.3.0)
*   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
*   - PZEM004Tv30          by Jakub Mandula     v1.2.1
*   - Adafruit SSD1306     by Adafruit          v2.5.16
*   - Adafruit GFX         by Adafruit          v1.12.5
*   - PubSubClient         by Nick O'Leary      v2.8
*
* Board Package & Libraries yang perlu diinstall untuk pembaruan firmware Data Logger di versi v2.0.0 nanti (install via Board & Library Manager):
*   Board Package   : Board Package ESP32 v3.3.0
*   Board reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
*   - ArduinoJson          by Benoit Blanchon   v6.21.6
*   - ESP Async WebServer  by ESP32Async        v3.11.0
*   - Async TCP            by ESP32Async        v3.4.10


Yang Harus diperbaiki/diupdate adalah:
1. Semua bagian sistem dan fungsi harus FULL asynchronous task/non blocking system kalau perlu gunakan dua core esp32 atau gunakan FreeRTOS
2. [hardcode & webpage config] sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded (hanya bisa diakses oleh programmer lewat program) dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker dengan topic hierarchical `{PREFIX_TOPIC}/{MQTT_TOPIC}/{DEVICE_ID}` (prefix dan topic diambil dari eeprom berdasarkan configurasi web config)
  buatlah file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js

  **Topik MQTT:** `lutpiii/telemetry/{device_id}` (hierarchical, bukan flat)
  Backend subscribe ke `lutpiii/telemetry/+` untuk menerima data dari semua device.

  **Final Schema JSON (v2.0.0):**
  ```json
{
  "seq": 1247,
  "device": {
    "id":     "3ph-logger-001",
    "fw":     "v2.1.0",
    "uptime": 3600,
    "heap":   148432,
    "rssi":   -65
  },
  "phases": {
    "R": {
      "valid":  true,
      "v":      220.1,
      "i":      12.34,
      "p":      2650.0,
      "s":      2714.6,
      "q":      582.1,
      "pf":     0.97,
      "f":      50.0,
      "e":      15230.0,
      "status": "OK"
    },
    "S": {
      "valid":  true,
      "v":      200.6,
      "i":      12.34,
      "p":      2650.0,
      "s":      2714.6,
      "q":      582.1,
      "pf":     0.97,
      "f":      50.0,
      "e":      15230.0,
      "status": "UNDER"
    },
    "T": {
      "valid":  false,
      "v":      0.0,
      "i":      0.0,
      "p":      0.0,
      "s":      0.0,
      "q":      0.0,
      "pf":     0.0,
      "f":      0.0,
      "e":      0.0,
      "status": "LOST"
    }
  },
  "unbalance": 4.25,
  "ts": 1717001234
}
```

  **Perubahan dari JSON versi awal:**

  | Aspek | Sebelum | Sesudah | Alasan |
  |---|---|---|---|
  | Topic | `lutpiii/telemetry` (flat) | `lutpiii/telemetry/{device_id}` (hierarchical) | Backend filter by device via MQTT routing |
  | Numeric values | String `"220.1"` | Number `220.1` | Backend langsung pakai, payload lebih kecil |
  | Schema phase invalid | Parsial (hanya `status`) | Full schema + `valid: false` | Satu parser untuk semua kondisi |
  | `ts` | `millis()` lokal device | Unix epoch seconds (dari NTP) | Waktu absolut untuk sorting/graphing antar device |
  | `unbalance` | String `"4.25"` | Number `4.25` | Backend langsung pakai sebagai number |
  | `seq` | Tidak ada | `uint32` increment setiap publish | Deteksi missed message / data gap |
4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi atau mqtt dulu baru bisa masuk ke sistem (tetap buat opening screen OLED dengan Loading bar progress nya)
    seharusnya sistem terus berjalan meskipun:
    a. tidak terhubung ke wi-fi dan/atau mqtt, atau
    b. hanya terhubung ke wi-fi namun tidak terhubung ke mqtt server
5. [Tambah] tambah status line dari OK/UNDER/LOST menjadi OK/UNDER/OVER/LOST. serta memindahkan line status threshold dari hardcoded jadi eeprom di config.h
    #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
    #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
    #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
6. [Tambah] tambahkan parameter unbalanced antar phase berdasarkan standard di indonesia, masukkan fungsi di DataAcquisition.h/.cpp
        ALGORITMA HITUNG VOLTAGE UNBALANCE 3 PHASE
        Input:
            VR = Tegangan phase R ke netral
            VS = Tegangan phase S ke netral
            VT = Tegangan phase T ke netral
        Langkah 1:
            Hitung tegangan rata-rata
            VAVG = (VR + VS + VT) / 3
        Langkah 2:
            Hitung deviasi tiap phase terhadap rata-rata
            DEV_R = |VR - VAVG|
            DEV_S = |VS - VAVG|
            DEV_T = |VT - VAVG|
        Langkah 3:
            Tentukan deviasi terbesar
            MAX_DEV = nilai terbesar dari:
                      DEV_R, DEV_S, DEV_T
        Langkah 4:
            Hitung persentase unbalance
            UNBALANCE = (MAX_DEV / VAVG) × 100
        Langkah 5:
            Tampilkan hasil unbalance dalam persen (%)

        --------------------------------------------------

        CONTOH DATA:
            VR = 220V
            VS = 210V
            VT = 228V
        PROSES:
        1. Hitung rata-rata
            VAVG = (220 + 210 + 228) / 3
            VAVG = 219.33V
        2. Hitung deviasi
            DEV_R = |220 - 219.33|
            DEV_R = 0.67V
            DEV_S = |210 - 219.33|
            DEV_S = 9.33V
            DEV_T = |228 - 219.33|
            DEV_T = 8.67V
        3. Cari deviasi terbesar
            MAX_DEV = 9.33V
        4. Hitung unbalance
            UNBALANCE = (9.33 / 219.33) × 100
            UNBALANCE = 4.25%
        HASIL:
            Voltage Unbalance = 4.25%

7. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
  Rule set Display:
    - display OLED 0.96" SSD1306 sepenuhnya dikontrol dengan 1 push button
    - kuncinya cuma ada 2     : 1. jika push button di tekan dengan interval biasa maka itu untuk mengganti page/memindahkan kursor menu ">"
                                2. jika push button di tekan dengan interval 2 detik (customable webpage config) maka itu untuk melakukan "Enter" pada menu yang di pilih atau khususnya pada Monitoring Mode adalah untuk berpindah ke Menu Mode
    - saat di Monitoring Mode : 1. tekan biasa push button untuk slide page ke page berikutnya
                                2. tekan selama 2 detik untuk masuk ke "Menu Mode" (berlaku di monitoring page manapun untuk berpindah ke menu mode)
    - saat di Menu Mode       : 1. push button sebagai kursor untuk memilih menu ke bawah, dengan cara menekan biasa agar kursor berpindah ke menu dibawahnya
                                2. jika kursor sudah pada menu paling bawah dan button di tekan lagi, maka kursor akan kembali ke menu paling atas lagi (berlaku juga untuk menu confirm "YES" or "NO" yang secara horizontal kanan-kiri)
                                3. untuk masuk ke menu pilihan, maka button harus ditekan selama 2 detik sebagai tombol "Enter" nya
                                4. setiap menu harus ada "Back" untuk keluar dari menu dengan cara menekan yang sama seperti masuk ke dalam menu (2 detik)
                                5. setiap menu harus ada "Back" kecuali "Back to Monitoring", jika itu ditekan selama 2 detik maka akan kembali ke "Monitoring Mode"
 *    1. Monitoring Mode:
 *    Page 1 [Monitoring Mode]:
 *   ┌────────────────────────────┐
 *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │   R         S         T    │  ← Phase Name         
 *   │  220V      220V      220V  │  ← Voltage Line to Netral   
 *   │  100A      100A      100A  │  ← Current per phase   (0-100)
 *   │   OK        OK        OK   │  ← Line_Status   (LOST/UNDER/OK)
 *   │  Unbalance = 2.5%          │  ← Unbalance param
 *   └────────────────────────────┘
 *
 *   ┌────────────────────────────┐
 *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │   R         S         T    │  ← Phase Name         
 *   │  220V      220V        0V  │  ← Voltage Line to Netral   
 *   │  100A      100A      100A  │  ← Current per phase   (0-100)
 *   │   OK       UNDER     LOST  │  ← Line_Status   (LOST/UNDER/OK), muncul dengan efek beeping/kedip untuk warning ketika status LOST / UNDER / OVER
 *   │  Unbalance = 5.45%   [!]   │  ← Unbalance param ketika hit threshold max unbalance di web config, [!] icon ini juga akan muncul dengan efek beeping/kedip untuk warning
 *   └────────────────────────────┘
 *
 *    Page 2 [Monitoring Mode]:
 *   ┌────────────────────────────┐
 *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │   R         S         T    │  ← Phase Name         
 *   │  60Hz      60Hz      60Hz  │  ← Frequency     
 *   │ 100VA     1.2kVA    10kVA  │  ← Apparent Power (L to N) (jika dibawah 1000 maka satuannya "VA", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVA")  
 *   │ 100VAr    2.0kVAr   13kVAr │  ← Reactive Power (L to N) (jika dibawah 1000 maka satuannya "VAR", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kVAr")         
 *   └────────────────────────────┘
 *
 *    Page 3 [Monitoring Mode]:
 *   ┌────────────────────────────┐
 *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │   R         S         T    │  ← Phase Name         
 *   │ 0.8PF     1.0PF     0.7PF  │  ← Power Factor (0.0 - 1.0)     
 *   │ 100W       12kW     2.2kW  │  ← Active Power (jika dibawah 1000 maka satuannya "W", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kW")   
 *   │ 300Wh     1.4kWh     30kWh │  ← Total Active Energy (jika dibawah 1000 maka satuannya "Wh", tetapi jika nilainya => 1000 atau lebih maka berubah jadi "kWh")          
 *   └────────────────────────────┘
 *
 *    2. Menu Mode:
 *    Page 1 [Menu Mode]: (jika menu highlighted maka ada indentasi maju 1 atau 2 karakter untuk menunjukkan posisi kursor pada OLED di menu mana)
 *   ┌────────────────────────────┐
 *   │    3-Phase Data Logger  QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  > Config                  │  ← Config Menu         
 *   │  > Reset kWh               │  ← Reset kWh semua fasa jadi 0kWh
 *   │  > Reboot Device           │  ← Reboot ESP32 lewat program setara dengan tekan tombol RST di board
 *   │  > Reset to Factory        │  ← Reset to Factory (reset configurasi yang ada di EEPROM, sama saja dengan reset EEPROM melalui web config, bedanya ini lewat menu OLED, yang satunya lewat web config)
 *   │  > Back to Monitoring      │  ← Kembali ke Page Monitoring Mode (jika kursor di "Back to Monitoring" dan tombol ditekan selama 2 detik maka akan kembali ke Page 1 Monitoring Mode )
 *   └────────────────────────────┘
 *
 *    Page 2 [Menu Mode > Config]:
 *    Note: Di menu Config ini hanya difungsikan untuk user mengaktifkan webserver, selebihnya config akan dilakukan melalui webpage dan akan di simpan ke dalam EEPROM ESP32
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
 *   │  PW  : 12345678            │  ← Password dari SSID ESP32
 *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
 *   │  > Active         Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
 *   └────────────────────────────┘
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  SSID: LOGGER              │  ← SSID ESP32 untuk masuk ke IP Address ESP32         
 *   │  PW  : 12345678            │  ← Password dari SSID ESP32
 *   │  192.168.1.5               │  ← IP Address yang dikeluarkan webserver ESP32 agar Laptop/Smartphone bisa akses pakai browser
 *   │  Inactive       > Back     │  ← jika menu "Active" ditekan selama 2 detik maka webserver dan SSID esp32 akan aktif, untuk non aktifkannya lakukan hal yang sama seperti saat mengaktifkannya. dan "Back" untuk Kembali ke Page Menu Mode
 *   └────────────────────────────┘
 *
 *    Page 3 [Menu Mode > Reset kWh]:
 *    Note: Di menu Reset kWh ini hanya difungsikan untuk user melakukan Reset Total kWh pada semua module PZEM-004T
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
 *   │        Reset kWh?          │  
 *   │                            │  
 *   │  > Reset          Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
 *   └────────────────────────────┘
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │                            │  ← Teks Konfirmasi         
 *   │     Reset kWh Berhasil     │  ← Teks Output Reset (Reset kWh Berhasil/Reset kWh Gagal) tampil selama 2 detik lalu otomatis kembali ke page Menu Mode
 *   │                            │  
 *   │                            │  
 *   └────────────────────────────┘
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
 *   │        Reset kWh?          │  
 *   │                            │  
 *   │   Reset         > Back     │  ← (Confirm Choose) jika menu "Reset" ditekan selama 2 detik maka esp32 akan reset total kWh pada semua PZEM menjadi 0, jika sudah reset tampilkan outputnya Berhasil/Tidak (selama 2 detik) lalu langsung kembali otomatis ke page Menu Mode.dan jika tidak jadi melakukan reset tinggal pilih "Back" untuk Kembali ke Page Menu Mode
 *   └────────────────────────────┘
 *
 *    Page 4 [Menu Mode > Reboot Device]:
 *    Note: Di menu Config ini hanya difungsikan untuk user melakukan rebooting device ESP32
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
 *   │       Reboot Device?       │  
 *   │                            │  
 *   │  > Reboot         Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
 *   └────────────────────────────┘
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │                            │      
 *   │     Device akan Reboot     │  ← Teks Countdown 3 detik sebelum system otomatis reboot (dalam 3 detik, dalam 2 detik, dalam 1 detik, dalam 0 detik)
 *   │       dalam 3 detik        │  
 *   │                            │  
 *   └────────────────────────────┘
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
 *   │       Reboot Device?       │  
 *   │                            │  
 *   │   Reboot        > Back     │  ← (Confirm Choose) jika menu "Reboot" ditekan selama 2 detik maka esp32 akan melakukan reboot by system, jika tombol "Reboot" sudah ditekan maka tampilkan countdown 3 detik menuju Reboot system dan system otomatis akan Reboot. dan jika tidak jadi melakukan Reboot tinggal pilih "Back" untuk Kembali ke Page Menu Mode
 *   └────────────────────────────┘
 *
 *    Page 5 [Menu Mode > Reset to Factory]:
 *    Note: -
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │  Apakah anda yakin untuk   │  ← Teks Konfirmasi         
 *   │        Reset Device?       │  
 *   │                            │  
 *   │  > Reset          Back     │  ← (Confirm Choise) jika menu "Reset" ditekan selama 2 detik maka esp32 akan melakukan reset EEPROM dan menampilkan teks output jika berhasil selama 2 detik, kemudian otomatis akan melakukan reboot esp32 by system, namun jika "tidak" ditekan selama 2 detik maka akan kembali ke Menu Config
 *   └────────────────────────────┘
 *   ┌────────────────────────────┐
 *   │       CONFIGURATION     QW │  ← Title, MQTT Icon gunakan huruf "Q" sebagai simbol dan wifi icon "W" di sebelah kanan judul jika terkoneksi ke MQTT/wifi (icon Q/W Independent) (jangan tampilkan jika tidak terkoneksi ke Wi-Fi) & list separator under the title
 *   │                            │      
 *   │      Reset to Factory      │  ← Teks Output Reset (Reset EEPROM/preferences Berhasil/Reset EEPROM Gagal) tampil selama 2 detik lalu otomatis melakukan reboot esp32 by system
 *   │          berhasil          │  
 *   │                            │  
 *   └────────────────────────────┘

8. [HTTP Web-server & Access Point] 
  a. buat webpage config yang bisa diakses lewat smartphone atau laptop yang terhubung melalui IP Address yang ESP32 keluarkan saat Config pada Menu Mode "Active"
  b. jika saat access point aktif dan ESP32 sedang terhubung ke jaringan yang ada internetnya, maka teruskan internet itu ke access point ESP32 agar Laptop/smartphone yang sedang melakukan config/terhubung ke access point esp32 mendapatkan jaringan internet untuk tetap online (jika tidak, Laptop/smartphone tidak apa-apa dalam kondisi offline) 
  c. semua parameter config yang ada di webpage tersimpan di dalam EEPROM ESP32 (jadi device baru harus config terlebih dahulu untuk terkoneksi ke internet/MQTT)
  d. buatkan login page sebelum masuk ke main page config, dengan user dan pass yang hanya bisa diubah di hardcode config.h, dengan default USERNAME = ADMIN, PASS = 18273645
  e. page 1 [Config] HTML + CSS:
    - Manage Known Network   (Big/Medium Card with Link to "Manage Known Network")
    - MQTT Connection Config (Big/Medium Card with Link to "MQTT Connection Config")
    - PZEM-004T Calibration  (Big/Medium Card with Link to "PZEM-004T Calibration")

    {HARDRESET EEPROM}

  f. page 1 [Config > Manage Known Network] HTML + CSS:
      tampilkan semua jaringan yang diketahui (EEPROM) dalam tabel dengan kolom dan terdapat text box untuk menambahkan jaringan baru di bawah table dan tombol add network untuk menyimpan jaringan baru ke EEPROM
              Manage Known Network

      | No. | SSID | PASS     |   Aksi   |
      | 1.  | ABC  | 18273645 | {Delete} |
      | 2.  | esp  | 18273645 | {Delete} |

      Add Network
      [ SSID....       ]
      [ PASS....       ]

      {ADD NETWORK}

  g. page 2 [Config > MQTT Connection Config] HTML + CSS:
      tampilkan data koneksi MQTT dalam bentuk teks dari EEPROM
      MQTT Server = broker.avisha.id
      MQTT_PORT   = 1883               // Non-SSL port
      MQTTS_PORT  = 8884               // SSL port
      WS_PORT     = 8083               // Non-SSL port
      WSS_PORT    = 8084               // SSL port
      MQTT_USER   = "lutpiii"          // MQTT Username
      MQTT_PASS   = "lutpiiiMQTT"      // MQTT Password
      PREFIX_TOPIC= "lutpiii"          // Topic prefix (wajib)
      MQTT_TOPIC  = "telemetry"        // Topic (wajib)
      USE SSL     = TRUE
      USE WS      = FALSE              
      

      {EDIT}        {SAVE CONFIG}

      // jika WS=0 & SSL=0 gunakan MQTT
      // jika WS=0 & SSL=1 gunakan MQTTS
      // jika WS=1 & SSL=0 gunakan WS
      // jika WS=1 & SSL=1 gunakan WSS

  h. page 3 [Config > PZEM-004T Calibration] HTML + CSS:
      buat kalibrasi dengan model offset (-/+) untuk parameter Voltage & Current masing-masing Fasa dengan presisi 0.000 perubahan, threshold LOST, UNDER, OVER dan unbalance. dan tampilkan juga offset yang tersimpan
      dengan model text box

      {SAVE CALIBRATION}

  i. gunakan saja template yang sudah saya buat ini, namun bagusin lagi homa page bagian text link nya, jgn pakai text link lagi, gunakan card medium atau besar ke bawah saja susunannya, jumlah card = jumlah menu
String WebServerManager::_htmlHead(const char* title) {
return String(R"(<!DOCTYPE html><html><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="light">
<title>)") + title + (R"(</title>
<style>
:root{
  --bg:#f7f8fa;
  --surface:#ffffff;
  --surface-2:#f3f7f4;
  --text:#1f2937;
  --muted:#6b7280;
  --line:#dbe3dc;
  --primary:#16a34a;
  --primary-hover:#15803d;
  --primary-soft:#dcfce7;
  --danger:#dc2626;
  --danger-hover:#b91c1c;
  --shadow:0 6px 18px rgba(15,23,42,.06);
  --radius:10px;
}

*{box-sizing:border-box}
html{-webkit-text-size-adjust:100%}
body{
  font-family:system-ui,-apple-system,"Segoe UI",Roboto,Arial,sans-serif;
  max-width:600px;
  margin:30px auto;
  padding:0 16px;
  background:var(--bg);
  color:var(--text);
  line-height:1.5;
}

h1{
  color:var(--text);
  font-size:1.4rem;
  margin:0 0 12px;
  letter-spacing:-0.02em;
}

h2{
  margin:0 0 10px;
  font-size:1rem;
  color:var(--primary);
  background:var(--primary-soft);
  padding:8px 12px;
  border-radius:8px;
  font-weight:700;
}

a{
  color:var(--primary);
  text-decoration:none;
}
a:hover{text-decoration:underline}

nav{
  margin:12px 0;
  display:flex;
  flex-wrap:wrap;
  gap:8px;
}
nav a{
  display:inline-flex;
  align-items:center;
  justify-content:center;
  padding:8px 12px;
  border:1px solid var(--line);
  border-radius:999px;
  background:var(--surface);
  color:var(--text);
  text-decoration:none;
  box-shadow:0 1px 2px rgba(15,23,42,.04);
}
nav a:hover{
  background:var(--surface-2);
  text-decoration:none;
}

hr{
  border:none;
  height:1px;
  background:var(--line);
  margin:14px 0;
}

.card{
  background:var(--surface);
  border:1px solid var(--line);
  border-left:4px solid var(--primary);
  border-radius:var(--radius);
  padding:16px;
  margin:12px 0;
  box-shadow:var(--shadow);
}

label{
  display:block;
  font-size:.85rem;
  color:var(--muted);
  margin:10px 0 6px;
}

.hint{
  font-size:.78rem;
  color:var(--muted);
  margin:-4px 0 8px;
}

input[type=text],
input[type=password],
input[type=number],
textarea{
  width:100%;
  box-sizing:border-box;
  padding:9px 10px;
  background:#fff;
  color:var(--text);
  border:1px solid var(--line);
  border-radius:8px;
  margin:4px 0 10px;
  font:inherit;
  outline:none;
}

input[type=text]:focus,
input[type=password]:focus,
input[type=number]:focus,
textarea:focus{
  border-color:rgba(22,163,74,.55);
  box-shadow:0 0 0 3px rgba(22,163,74,.12);
}

textarea{
  resize:vertical;
  min-height:120px;
}

button,
.btn,
.danger{
  background:var(--primary);
  color:#fff;
  border:none;
  padding:10px 16px;
  border-radius:8px;
  cursor:pointer;
  font-size:.92rem;
  font-weight:600;
  transition:background .15s ease, transform .05s ease;
}

button:hover,
.btn:hover{
  background:var(--primary-hover);
}

button:active,
.btn:active,
.danger:active{
  transform:translateY(1px);
}

.btn-del{
  background:#6b7280;
}
.btn-del:hover{
  background:#4b5563;
}

.danger{
  background:var(--danger);
}
.danger:hover{
  background:var(--danger-hover);
}

table{
  width:100%;
  border-collapse:collapse;
  margin:8px 0;
}
th,td{
  text-align:left;
  padding:8px;
  border-bottom:1px solid var(--line);
  vertical-align:top;
}
th{
  color:var(--primary);
  background:#f9fffb;
}

input[type=checkbox]{
  accent-color:var(--primary);
  transform:translateY(1px);
}

form{margin:0}

@media (max-width:640px){
  body{
    margin:18px auto;
    padding:0 12px;
  }
  nav a{
    width:calc(50% - 4px);
  }
}
</style></head><body>
<h1>&#9889; 3-Phase Data Logger</h1>
<nav>
<a href="/">Home</a>
<a href="/wifi">WiFi</a>
<a href="/mqtt">MQTT</a>
<a href="/calibration">Calibration</a>
</nav>
<hr>
)");
}

String WebServerManager::_htmlFoot() {
    return R"(<hr><p style="font-size:.75rem;color:#6b7280;text-align:center">Muhammad Lutfi Nur Anendi &bull; )" FW_VERSION R"(</p></body></html>)";
}

// ─── Page: Index ──────────────────────────────────────────

String WebServerManager::_pageIndex(StorageManager& s) {
    String h = _htmlHead("Config");
    h += R"HTML(<div class="card">
<h2>Configuration Pages</h2>
<p><a href="/wifi">&#128246; Manage Known Networks</a></p>
<p><a href="/mqtt">&#128272; MQTT Connection Config</a></p>
<p><a href="/calibration">&#9881; PZEM-004T Calibration &amp; Thresholds</a></p>
</div>
<div class="card">
<h2>&#9888; Danger Zone</h2>
<form method="POST" action="/factoryreset"
  onsubmit="return confirm('ERASE ALL settings? This cannot be undone.')">
<button class="danger" type="submit">Hard Reset EEPROM</button>
</form>
</div>
)HTML";
    h += _htmlFoot();
    return h;
}

// ─── Page: WiFi ───────────────────────────────────────────

String WebServerManager::_pageWifi(StorageManager& s) {
    String h = _htmlHead("Manage WiFi");
    h += "<h2>Known Networks</h2><div class='card'>";
    h += "<table><tr><th>No.</th><th>SSID</th><th>Action</th></tr>";

    int n = s.getWifiCount();
    for (int i = 0; i < n; i++) {
        WiFiEntry e = s.getWifiEntry(i);
        h += "<tr><td>" + String(i + 1) + "</td><td>" + String(e.ssid) + "</td><td>";
        h += "<form method='POST' action='/wifi/delete' style='display:inline'>";
        h += "<input type='hidden' name='idx' value='" + String(i) + "'>";
        h += "<button class='btn btn-del' type='submit'>Delete</button></form>";
        h += "</td></tr>";
    }
    if (n == 0) h += "<tr><td colspan='3'>No networks stored.</td></tr>";
    h += "</table></div>";

    h += R"(<div class="card"><h2>Add Network</h2>
<form method="POST" action="/wifi/add">
<label>SSID</label>
<input type="text" name="ssid" placeholder="Network name" required>
<label>Password</label>
<input type="password" name="pass" placeholder="Password">
<button type="submit">Add Network</button>
</form></div>)";

    h += _htmlFoot();
    return h;
}

// ─── Page: MQTT ───────────────────────────────────────────

String WebServerManager::_pageMQTT(StorageManager& s) {
    MQTTConfig cfg = s.getMQTTConfig();
    String h = _htmlHead("MQTT Config");
    h += "<h2>MQTT Broker</h2><div class='card'>";
    h += "<form method='POST' action='/mqtt'>";

    auto field = [&](const char* label, const char* name, const char* val, const char* type = "text") {
        h += String("<label>") + label + "</label>";
        h += String("<input type='") + type + "' name='" + name + "' value='" + val + "'>";
    };
    auto numField = [&](const char* label, const char* name, int val) {
        h += String("<label>") + label + "</label>";
        h += String("<input type='number' name='") + name + "' value='" + val + "'>";
    };
    auto checkbox = [&](const char* label, const char* name, bool checked) {
        h += String("<label><input type='checkbox' name='") + name + "' " +
             (checked ? "checked" : "") + "> " + label + "</label><br>";
    };

    field("MQTT Server",       "server",   cfg.server);
    numField("Port (non-SSL)", "port",     cfg.port);
    numField("Port (SSL)",     "sslport",  cfg.sslPort);
    field("Username",          "user",     cfg.user);
    field("Password",          "pass",     cfg.pass, "password");
    field("Topic Prefix",      "topic",    cfg.topic);
    checkbox("Use SSL",        "usessl",   cfg.useSSL);
    checkbox("Use WebSocket",  "usews",    cfg.useWS);

    h += "<label>CA Certificate (PEM)</label>";
    h += String("<textarea name='cacert' rows='5' placeholder='-----BEGIN CERTIFICATE-----...'>")
         + cfg.caCert + "</textarea>";
    h += "<button type='submit'>Save Config</button></form></div>";
    h += _htmlFoot();
    return h;
}

// ─── Page: Calibration + Thresholds ──────────────────────

String WebServerManager::_pageCalibration(StorageManager& s) {
    CalibrationConfig cal = s.getCalibration();
    ThresholdConfig   thr = s.getThresholds();

    String h = _htmlHead("Calibration & Thresholds");
    h += "<form method='POST' action='/calibration'>";

    h += "<h2>PZEM-004T Calibration</h2>";
    h += "<p class='hint'>Offset is additive: Reading = Raw + Offset. Use negative to subtract.</p>";
    h += "<div class='card'>";

    const char* phases[3] = {"R", "S", "T"};
    for (int i = 0; i < 3; i++) {
        h += String("<h2>Phase ") + phases[i] + "</h2>";
        h += String("<label>Voltage Offset (V) &nbsp; Current stored: ") +
             String(cal.voltageOffset[i], 3) + " V</label>";
        h += String("<input type='number' step='0.001' name='voff_") +
             phases[i] + "' value='" + String(cal.voltageOffset[i], 3) + "'>";
        h += String("<label>Current Offset (A) &nbsp; Current stored: ") +
             String(cal.currentOffset[i], 3) + " A</label>";
        h += String("<input type='number' step='0.001' name='ioff_") +
             phases[i] + "' value='" + String(cal.currentOffset[i], 3) + "'>";
    }
    h += "</div>";

    h += "<h2>Line Status Thresholds</h2>";
    h += "<p class='hint'>Thresholds used to determine line status: LOST / UNDER / OK / OVER.</p>";
    h += "<div class='card'>";

    h += "<label>LOST threshold (V) &ndash; voltage below this &rarr; LOST</label>";
    h += "<p class='hint'>Current: " + String(thr.voltageLost, 1) + " V</p>";
    h += "<input type='number' step='0.1' name='th_vlost' value='" + String(thr.voltageLost, 1) + "'>";

    h += "<label>UNDER threshold (V) &ndash; voltage below this &rarr; UNDER (above LOST)</label>";
    h += "<p class='hint'>Current: " + String(thr.voltageUnder, 1) + " V</p>";
    h += "<input type='number' step='0.1' name='th_vunder' value='" + String(thr.voltageUnder, 1) + "'>";

    h += "<label>OVER threshold (V) &ndash; voltage above this &rarr; OVER</label>";
    h += "<p class='hint'>Current: " + String(thr.voltageOver, 1) + " V</p>";
    h += "<input type='number' step='0.1' name='th_vover' value='" + String(thr.voltageOver, 1) + "'>";
    h += "</div>";

    h += "<h2>Voltage Unbalance Threshold</h2>";
    h += "<p class='hint'>Berdasarkan standar SNI/SPLN Indonesia. Jika unbalance &gt; threshold unbalance maka OLED menampilkan peringatan [!]</p>";
    h += "<br>";
    h += "<p class='hint'>3-phase voltage unbalance computed using NEMA method.</p>";
    h += "<div class='card'>";
    h += "<label>Max Unbalance (%) &ndash; above this &rarr; warning [!] on OLED &amp; MQTT</label>";
    h += "<p class='hint'>Current: " + String(thr.unbalanceMax, 2) + " %</p>";
    h += "<input type='number' step='0.01' name='th_unbal' value='" + String(thr.unbalanceMax, 2) + "'>";
    h += "</div>";

    h += "<button type='submit'>Save Calibration &amp; Thresholds</button>";
    h += "</form>";
    h += _htmlFoot();
    return h;
}
 

9. [IMPORTANT!!!] Kalibrasi Tegangan & Arus PZEM harus mempengaruhi semua parameter turunan (VA, VAr, W, Wh, PF kecuali Hz). Karena parameter lain bergantung pada V dan I, maka setelah kalibrasi offset V/I, semua parameter hitung ulang agar konsisten.

10. [Tambah] di web config, tambahkan config untuk nilai LOST, UNDER, OVER voltage, serta persentase max unbalance phase di halaman PZEM Calibration
    #define VOLTAGE_OK_MIN      180.0f  // Below this → UNDER
    #define VOLTAGE_LOST_MAX    80.0f   // Below this → LOST
    #define VOLTAGE_OVER_MAX    240.0f  // Above this → OVER
    #define VOLTAGE_UNBALANCE_MAX    3.0f  // Above this → OVER UNBALANCE (satuan persen)

11. LED Signal 
  - sebagai indikator terhadap aksi-aksi dari fungsi/module program seperti Booting, Reset kWh, active/deactive webserver, dll.

12. [Tambah] tambahkan fitur auto self-healing ketika esp32 mengalami freeze/hang/stuck/runningout memory atau sejenisnya atau sesuatu yang membuat esp32 malfungsi sistem, maka harus ditambahkan self healing dengan metode yang paling cocok untuk sistem ini

13. Struktur File Project gunakan OOP dan seperti ROS2 (Robot Operating System 2) dengan dokumentasi-dokumentasi melimpahnya dan program mudah dipahami, mudah di konfigurasi, dan bisa scalable saat penambahan fitur/sensor baru
  (jumlah file menyesuaikan, ini hanya sebagai contoh):
  - main.ino
  - config.h
  - data_acquisition.h/.cpp
  - calibration.h/.cpp
  - display.h/.cpp
  - menu.h/.cpp
  - webserver.h/.cpp
  - types.h
  - system_state.h
  - task_manager.h/.cpp
  - wifi_manager.h/.cpp
  - mqtt_manager.h/.cpp
  - json_builder.h/.cpp
  - storage_manager.h/.cpp
  - led_manager.h/.cpp
  - diagnostics.h/.cpp
  - dll

### Catatan untuk Backend Multi-Device (JSON Parser)
1. Subscribe ke `lutpiii/telemetry/+` (wildcard) untuk menerima data dari semua device
2. Indexing data berdasarkan `device.id` + `ts` (Unix timestamp) untuk query perangkat per periode waktu
3. Gunakan field `seq` untuk mendeteksi apakah ada data terlewat (cek gap sequence tiap device)
4. Field `valid` pada tiap phase memberi tahu bahwa data phase tersebut bisa diabaikan (tanpa perlu conditional parsing)
5. Jika ESP32 belum sync NTP, kirim nilai `ts = 0` dan backend fallback ke MQTT broker timestamp


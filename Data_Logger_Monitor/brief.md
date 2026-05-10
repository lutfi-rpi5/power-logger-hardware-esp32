project yang sudah jadi dan sudah rilis
ini penjelasan project yang sekarang dan yang akan di update firmware nya
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

Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin (Arduino IDE v2.0 & Board Package ESP32 v2.0.14)
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


Yang Harus diperbaiki/diupdate adalah:
1. Semua bagian sistem dan fungsi harus FULL asynchronous task/non blocking system kalau perlu gunakan dua core esp32
2. [hardcode & webpage config] sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded (hanya bisa diakses oleh programmer lewat program) dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker (desainlah struktur file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js)
4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi dulu baru bisa masuk ke sistem (tetap buat opening screen OLED)
5. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
  5a. Rule set Display:
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
 *   │   > Config                 │  ← Config Menu         
 *   │  > Reset kWh               │  ← Reset kWh semua fasa jadi 0kWh
 *   │  > Reboot Device           │  ← Reboot ESP32 lewat program setara dengan tekan tombol RST di board
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

6. [HTTP Web-server & Access Point] 
  a. buat webpage config yang bisa diakses lewat smartphone atau laptop yang terhubung melalui IP Address yang ESP32 keluarkan saat Config pada Menu Mode "Active"
  b. jika saat access point aktif dan ESP32 sedang terhubung ke jaringan yang ada internetnya, maka teruskan internet itu ke access point ESP32 agar Laptop/smartphone yang sedang melakukan config/terhubung ke access point esp32 mendapatkan jaringan internet untuk tetap online (jika tidak, Laptop/smartphone tidak apa-apa dalam kondisi offline) 
  c. semua parameter config yang ada di webpage tersimpan di dalam EEPROM ESP32 (jadi device baru harus config terlebih dahulu untuk terkoneksi ke internet/MQTT)
  d. page 1 [Config] HTML + CSS:
    - Manage Known Network (text Link to "Manage Known Network")
    - MQTT Connection Config (text Link to "MQTT Connection Config")
    - PZEM-004T Calibration (text Link to "PZEM-004T Calibration")

  e. page 1 [Config > Manage Known Network] HTML + CSS:
      tampilkan semua jaringan yang diketahui (EEPROM) dalam tabel dengan kolom dan terdapat text box untuk menambahkan jaringan baru di bawah table dan tombol add network untuk menyimpan jaringan baru ke EEPROM
              Manage Known Network

      | No. | SSID | PASS     |   Aksi   |
      | 1.  | ABC  | 18273645 | {Delete} |
      | 2.  | esp  | 18273645 | {Delete} |

      Add Network
      [ SSID....       ]
      [ PASS....       ]

      {ADD NETWORK}

  e. page 2 [Config > MQTT Connection Config] HTML + CSS:
      tampilkan data koneksi MQTT dalam bentuk teks dari EEPROM
      MQTT Server = broker.avisha.id
      MQTT_PORT   = 1883               // Non-SSL port
      MQTTS_PORT  = 8884               // SSL port
      WS_PORT     = 8083               // Non-SSL port
      WSS_PORT    = 8084               // SSL port
      MQTT_USER   = "lutpiii"          // MQTT Username
      MQTT_PASS   = "lutpiiiMQTT"      // MQTT Password
      MQTT_TOPIC  = "lutpiii/"         // Topic prefix (optional)
      USE SSL     = TRUE
      USE WS      = FALSE

      {EDIT}        {SAVE CONFIG}

  f. page 3 [Config > PZEM-004T Calibration] HTML + CSS:
      buat kalibrasi dengan model offset (-/+) untuk parameter Voltage & Current masing-masing Fasa, dan tampilkan juga offset yang tersimpan
      dengan model text box

      {SAVE CALIBRATION}

7. LED Signal 
  - sebagai indikator terhadap aksi-aksi dari fungsi/module program seperti Booting, Reset kWh

8. Struktur File Project menggunakan OOP dan seperti ROS2 dengan dokumentasi-dokumentasi melimpahnya dan program mudah dipahami, mudah di konfigurasi, dan bisa scalable saat penambahan fitur/sensor baru
  (jumlah file menyesuaikan, ini hanya sebagai contoh):
  - main.ino
  - config.h
  - data_acquisition.h/.cpp
  - calibration.h/.cpp
  - display.h/cpp
  - menu.h/cpp
  - webserver.h/.cpp
  - etc




































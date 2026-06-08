Spesifikasi sistem:
 * Board     : ESP32 Devkit V1 (30-pin)
 * IDE       : Arduino IDE v2.x
 * Package   : esp32 by Espressif System: ESP32 Dev Module v3.3.0
 * Board Package Reference : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * Library required: ESPAsyncWebServer by ESP32Async v3.11.0
 *                            AsyncTCP by ESP32Async v3.4.10

Revisi sistem:
1. perubahan rencana, dari hasil perancangan untuk meneruskan internet dari STA ke AP, mendapati sistem tak pernah stabil.
maka dari itu, hilangkan fitur NAT Passthrough, biar kan sistem lebih stabil tanpa fitur itu,
jadi client saat terhubung ke AP esp32 biarkan client tidak mendapat internet karna esp32 tidak perlu lagi meneruskan internet dari router

2. dan terdapat bug juga ketika esp32 terhubung ke jaringan dan jaringan router tersebut tidak ada internet, 
saat esp32 mencoba koneksi ke mqtt otomatis esp32 nya langsung reboot, 
kasus ini terjadi ketika esp32 ingin melakukan koneksi ke mqtt lewat jaringan yang dia sudah terhubung, 
namun jaringan tersebut tidak memiliki jaringan. seharusnya biarkan saja ketika sistem tidak bisa terhubung ke mqtt, buat saja perilakunya sama seperti ketika esp32 tidak mendapatkan jaringan wi-fi 

bug didapatkan dengan indikasi serial monitor sebagai berikut:

11:51:55.054 -> [Menu] System ready.
11:51:55.054 -> E (3271) task_wdt: esp_task_wdt_init(517): TWDT already initialized
11:51:55.054 -> [WDT] Task watchdog initialized: timeout=30s, panic=enabled
11:51:55.101 -> [DAQ] Reader task started on Core 0
11:51:55.739 -> [Main] Setup complete. Entering main loop on Core 1.
11:51:56.107 -> [Main] WiFi connected → LED signal
11:51:56.141 -> [WiFi] Active: rpi-5 | IP: 172.20.168.58 | RSSI: -50 dBm
11:51:56.764 -> [MQTT] Connecting to broker.avisha.id...E (20894) task_wdt: Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:
11:52:02.093 -> E (20894) task_wdt:  - loopTask (CPU 1)
11:52:02.093 -> E (20894) task_wdt: Tasks currently running:
11:52:02.093 -> E (20894) task_wdt: CPU 0: IDLE0
11:52:02.138 -> E (20894) task_wdt: CPU 1: IDLE1
11:52:02.138 -> E (20894) task_wdt: Aborting.
11:52:02.138 -> E (20894) task_wdt: Print CPU 1 backtrace
11:52:02.138 -> Backtrace: 0x40089efb:0x3ffccb10 0x400efd5e:0x3ffccb30 0x4008db5b:0x3ffccb50 0x4008cb91:0x3ffccb70
11:52:02.138 -> ELF file SHA256: 21b0df990
11:52:02.387 -> Rebooting...
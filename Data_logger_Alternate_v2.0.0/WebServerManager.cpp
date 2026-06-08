/******************************************************
 * File      : WebServerManager.cpp
 *
 * v2.1.0: NAT/internet passthrough dihapus.
 *   ESP32 AP hanya digunakan untuk WebConfig (192.168.4.1).
 *   Client yang terhubung ke AP tidak mendapat internet.
 *   Ini disengaja — sistem lebih stabil tanpa NAPT.
 ******************************************************/

#include "WebServerManager.h"

WebServerManager::WebServerManager(StorageManager& storage)
    : _storage(storage), _server(80)
{}

// ─── Begin (register routes, don't start yet) ────────────

void WebServerManager::begin() {
    _setupRoutes();
    Serial.println("[Web] Routes registered. Server not active yet.");
}

// ─── Activate AP + Server ─────────────────────────────────

void WebServerManager::activate() {
    if (_active) return;

    APConfig ap = _storage.getAPConfig();

    // Mode AP_STA: STA tetap jalan (untuk MQTT), AP untuk WebConfig
    if (WiFi.getMode() != WIFI_AP_STA) {
        WiFi.mode(WIFI_AP_STA);
        delay(100);
    }

    WiFi.softAPConfig(
        IPAddress(192, 168, 4, 1),
        IPAddress(192, 168, 4, 1),
        IPAddress(255, 255, 255, 0)
    );
    WiFi.softAP(ap.ssid, ap.pass, /*channel=*/6, /*hidden=*/0, /*max_conn=*/4);
    delay(100);

    Serial.printf("[Web] AP started: %s  IP: %s\n",
        ap.ssid, WiFi.softAPIP().toString().c_str());

    _server.begin();
    _active = true;

    updateState([&](SystemState& s) {
        s.webServerActive = true;
        strncpy(s.apSSID, ap.ssid, sizeof(s.apSSID));
        strncpy(s.apPass, ap.pass, sizeof(s.apPass));
        WiFi.softAPIP().toString().toCharArray(s.apIP, sizeof(s.apIP));
    });

    Serial.println("[Web] HTTP server started on port 80");
}

// ─── Deactivate ───────────────────────────────────────────

void WebServerManager::deactivate() {
    if (!_active) return;
    _server.end();
    WiFi.softAPdisconnect(true);
    _active = false;
    updateState([](SystemState& s) {
        s.webServerActive = false;
    });
    Serial.println("[Web] Server and AP stopped.");
}

String WebServerManager::getIP() const {
    return _active ? WiFi.softAPIP().toString() : "";
}

// ─── Routes ───────────────────────────────────────────────

void WebServerManager::_setupRoutes() {
    // ── Index ─────────────────────────────────────────────
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* req) {
        req->send(200, "text/html", _pageIndex(_storage));
    });

    // ── WiFi ──────────────────────────────────────────────
    _server.on("/wifi", HTTP_GET, [this](AsyncWebServerRequest* req) {
        req->send(200, "text/html", _pageWifi(_storage));
    });

    _server.on("/wifi/add", HTTP_POST, [this](AsyncWebServerRequest* req) {
        String ssid = req->hasParam("ssid", true) ? req->getParam("ssid", true)->value() : "";
        String pass = req->hasParam("pass", true) ? req->getParam("pass", true)->value() : "";
        if (ssid.length() > 0) {
            _storage.addWifiEntry(ssid.c_str(), pass.c_str());
        }
        req->redirect("/wifi");
    });

    _server.on("/wifi/delete", HTTP_POST, [this](AsyncWebServerRequest* req) {
        if (req->hasParam("idx", true)) {
            int idx = req->getParam("idx", true)->value().toInt();
            _storage.deleteWifiEntry(idx);
        }
        req->redirect("/wifi");
    });

    // ── MQTT ──────────────────────────────────────────────
    _server.on("/mqtt", HTTP_GET, [this](AsyncWebServerRequest* req) {
        req->send(200, "text/html", _pageMQTT(_storage));
    });

    _server.on("/mqtt", HTTP_POST, [this](AsyncWebServerRequest* req) {
        MQTTConfig cfg = _storage.getMQTTConfig();

        auto get = [&](const char* name) -> String {
            return req->hasParam(name, true) ? req->getParam(name, true)->value() : "";
        };

        String srv = get("server"); if (srv.length()) srv.toCharArray(cfg.server, sizeof(cfg.server));
        String u   = get("user");   if (u.length())   u.toCharArray(cfg.user,   sizeof(cfg.user));
        String p   = get("pass");   if (p.length())   p.toCharArray(cfg.pass,   sizeof(cfg.pass));
        String t   = get("topic");  if (t.length())   t.toCharArray(cfg.topic,  sizeof(cfg.topic));
        String ca  = get("cacert"); if (ca.length())  ca.toCharArray(cfg.caCert, sizeof(cfg.caCert));

        String portStr    = get("port");    if (portStr.length())    cfg.port    = portStr.toInt();
        String sslPortStr = get("sslport"); if (sslPortStr.length()) cfg.sslPort = sslPortStr.toInt();

        cfg.useSSL = req->hasParam("usessl", true);
        cfg.useWS  = req->hasParam("usews",  true);

        _storage.saveMQTTConfig(cfg);
        if (onMQTTSaved) onMQTTSaved();
        req->redirect("/mqtt");
    });

    // ── Calibration + Thresholds ──────────────────────────
    _server.on("/calibration", HTTP_GET, [this](AsyncWebServerRequest* req) {
        req->send(200, "text/html", _pageCalibration(_storage));
    });

    _server.on("/calibration", HTTP_POST, [this](AsyncWebServerRequest* req) {
        CalibrationConfig cal = _storage.getCalibration();
        const char* vKeys[3] = {"voff_R", "voff_S", "voff_T"};
        const char* iKeys[3] = {"ioff_R", "ioff_S", "ioff_T"};
        for (int i = 0; i < 3; i++) {
            if (req->hasParam(vKeys[i], true))
                cal.voltageOffset[i] = req->getParam(vKeys[i], true)->value().toFloat();
            if (req->hasParam(iKeys[i], true))
                cal.currentOffset[i] = req->getParam(iKeys[i], true)->value().toFloat();
        }
        _storage.saveCalibration(cal);
        if (onCalibrationSaved) onCalibrationSaved();

        ThresholdConfig thr = _storage.getThresholds();
        if (req->hasParam("th_vlost",  true)) thr.voltageLost  = req->getParam("th_vlost",  true)->value().toFloat();
        if (req->hasParam("th_vunder", true)) thr.voltageUnder = req->getParam("th_vunder", true)->value().toFloat();
        if (req->hasParam("th_vover",  true)) thr.voltageOver  = req->getParam("th_vover",  true)->value().toFloat();
        if (req->hasParam("th_unbal",  true)) thr.unbalanceMax = req->getParam("th_unbal",  true)->value().toFloat();
        _storage.saveThresholds(thr);
        if (onThresholdSaved) onThresholdSaved();

        req->redirect("/calibration");
    });

    // ── Factory Reset ─────────────────────────────────────
    _server.on("/factoryreset", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _storage.factoryReset();
        req->send(200, "text/html",
            _htmlHead("Factory Reset") +
            "<h2>All settings cleared.</h2>"
            "<p>Device will reboot in 3 seconds...</p>"
            "<script>setTimeout(()=>location.href='/',3000)</script>" +
            _htmlFoot());
        delay(3500);
        ESP.restart();
    });

    // ── Catch-all: redirect ke WebConfig ─────────────────
    // Semua request yang tidak cocok diarahkan ke halaman utama.
    // Ini menangani captive portal detection dari Android/iOS/Windows:
    // OS akan melihat redirect → memunculkan notifikasi "Sign in to DataLogger"
    // → user tahu ini AP lokal tanpa internet (perilaku jujur & stabil).
    _server.onNotFound([](AsyncWebServerRequest* req) {
        req->redirect("http://192.168.4.1/");
    });
}

// ─── HTML Helpers ─────────────────────────────────────────

String WebServerManager::_htmlHead(const char* title) {
    return String(R"(<!DOCTYPE html><html><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>)") + title + R"(</title>
<style>
body{font-family:sans-serif;max-width:600px;margin:30px auto;padding:0 16px;background:#1a1a2e;color:#eee}
h1{color:#e94560;font-size:1.4rem}h2{color:#0f3460;background:#e94560;padding:8px 12px;border-radius:4px;font-size:1rem;color:#fff}
a{color:#e94560;text-decoration:none}a:hover{text-decoration:underline}
.card{background:#16213e;border-radius:8px;padding:16px;margin:12px 0}
input[type=text],input[type=password],input[type=number],textarea{width:100%;box-sizing:border-box;padding:8px;background:#0f3460;color:#eee;border:1px solid #e94560;border-radius:4px;margin:4px 0 10px}
button,.btn{background:#e94560;color:#fff;border:none;padding:10px 18px;border-radius:4px;cursor:pointer;font-size:.9rem}
button:hover{background:#c73652}.btn-del{background:#555}.btn-del:hover{background:#333}
table{width:100%;border-collapse:collapse;margin:8px 0}th,td{text-align:left;padding:8px;border-bottom:1px solid #0f3460}th{color:#e94560}
label{font-size:.85rem;color:#aaa}nav{margin:12px 0}nav a{margin-right:12px}
.danger{background:#e94560;color:white;border:none;padding:10px 18px;border-radius:4px;cursor:pointer}
.hint{font-size:.78rem;color:#888;margin:-6px 0 8px}
</style></head><body>
<h1>&#9889; 3-Phase Data Logger</h1>
<nav>
<a href="/">Home</a>
<a href="/wifi">WiFi</a>
<a href="/mqtt">MQTT</a>
<a href="/calibration">Calibration</a>
</nav>
<hr>
)";
}

String WebServerManager::_htmlFoot() {
    return R"(<hr><p style="font-size:.75rem;color:#666">Muhammad Lutfi Nur Anendi &bull; )" FW_VERSION R"(</p></body></html>)";
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

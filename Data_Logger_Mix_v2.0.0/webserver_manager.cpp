#include "webserver_manager.h"
#include "diagnostics.h"

WebServerManager::WebServerManager(StorageManager& storage)
    : _storage(storage), _server(80)
{}

void WebServerManager::begin() {
    _setupRoutes();
    diag.info("WEB", "Routes registered. Server not active yet.");
}

void WebServerManager::activate() {
    if (_active) return;

    APConfig ap = _storage.getAPConfig();

    if (WiFi.getMode() != WIFI_AP_STA) {
        WiFi.mode(WIFI_AP_STA);
        delay(100);
    }

    WiFi.softAPConfig(
        IPAddress(192, 168, 4, 1),
        IPAddress(192, 168, 4, 1),
        IPAddress(255, 255, 255, 0)
    );
    WiFi.softAP(ap.ssid, ap.pass, 6, 0, 4);
    delay(100);

    diag.info("WEB", "AP started: %s  IP: %s", ap.ssid, WiFi.softAPIP().toString().c_str());

    _server.begin();
    _active = true;

    updateState([&](SystemState& s) {
        s.webServerActive = true;
        strncpy(s.apSSID, ap.ssid, sizeof(s.apSSID));
        strncpy(s.apPass, ap.pass, sizeof(s.apPass));
        WiFi.softAPIP().toString().toCharArray(s.apIP, sizeof(s.apIP));
    });

    diag.info("WEB", "HTTP server started on port 80");
}

void WebServerManager::deactivate() {
    if (!_active) return;
    _server.end();
    WiFi.softAPdisconnect(true);
    _active = false;
    updateState([](SystemState& s) {
        s.webServerActive = false;
    });
    diag.info("WEB", "Server and AP stopped.");
}

String WebServerManager::getIP() const {
    return _active ? WiFi.softAPIP().toString() : "";
}

// ─── Auth ──────────────────────────────────────────────────

bool WebServerManager::_authenticate(AsyncWebServerRequest* request) {
    return request->authenticate(WEB_USERNAME, WEB_PASSWORD);
}

void WebServerManager::_handleLogin(AsyncWebServerRequest* request) {
    AsyncWebServerResponse* resp = request->beginResponse(401, "text/html", _pageLogin());
    resp->addHeader("WWW-Authenticate", "Basic realm=\"3-Phase Logger\"");
    request->send(resp);
}

// ─── Routes ────────────────────────────────────────────────

void WebServerManager::_setupRoutes() {
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* req) {
        if (!_authenticate(req)) { _handleLogin(req); return; }
        req->send(200, "text/html", _pageIndex(_storage));
    });

    _server.on("/wifi", HTTP_GET, [this](AsyncWebServerRequest* req) {
        if (!_authenticate(req)) { _handleLogin(req); return; }
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

    _server.on("/mqtt", HTTP_GET, [this](AsyncWebServerRequest* req) {
        if (!_authenticate(req)) { _handleLogin(req); return; }
        req->send(200, "text/html", _pageMQTT(_storage));
    });

    _server.on("/mqtt", HTTP_POST, [this](AsyncWebServerRequest* req) {
        MQTTConfig cfg;
        memset(&cfg, 0, sizeof(cfg));

        auto get = [&](const char* name) -> String {
            return req->hasParam(name, true) ? req->getParam(name, true)->value() : "";
        };

        String srv = get("server"); srv.toCharArray(cfg.server, sizeof(cfg.server));
        cfg.port    = get("port").toInt();
        cfg.sslPort = get("sslport").toInt();
        cfg.wsPort  = get("wsport").toInt();
        cfg.wssPort = get("wssport").toInt();
        String u   = get("user");   u.toCharArray(cfg.user,   sizeof(cfg.user));
        String p   = get("pass");   p.toCharArray(cfg.pass,   sizeof(cfg.pass));
        String pr  = get("prefix"); pr.toCharArray(cfg.prefix, sizeof(cfg.prefix));
        String t   = get("topic");  t.toCharArray(cfg.topic,  sizeof(cfg.topic));
        cfg.useSSL = req->hasParam("usessl", true);
        String ca  = get("cacert"); ca.toCharArray(cfg.caCert, sizeof(cfg.caCert));

        _storage.saveMQTTConfig(cfg);
        if (onMQTTSaved) onMQTTSaved();
        req->redirect("/mqtt");
    });

    _server.on("/calibration", HTTP_GET, [this](AsyncWebServerRequest* req) {
        if (!_authenticate(req)) { _handleLogin(req); return; }
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

    _server.on("/factoryreset", HTTP_POST, [this](AsyncWebServerRequest* req) {
        if (!_authenticate(req)) { _handleLogin(req); return; }
        _storage.factoryReset();
        String h = _htmlHead("Reset");
        h += "<div class='card'><h2>Factory Reset Complete</h2><p>All settings erased. Device will reboot.</p></div>";
        h += _htmlFoot();
        req->send(200, "text/html", h);
        delay(1000);
        ESP.restart();
    });

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
:root{--bg:#f7f8fa;--surface:#fff;--text:#1f2937;--muted:#6b7280;--line:#dbe3dc;--primary:#16a34a;--primary-hover:#15803d;--primary-soft:#dcfce7;--danger:#dc2626;--danger-hover:#b91c1c;--radius:10px;--shadow:0 6px 18px rgba(15,23,42,.06)}
*{box-sizing:border-box}body{font-family:system-ui,-apple-system,sans-serif;max-width:600px;margin:30px auto;padding:0 16px;background:var(--bg);color:var(--text);line-height:1.5}
h1{color:var(--text);font-size:1.4rem;margin:0 0 12px}
h2{margin:0 0 10px;font-size:1rem;color:var(--primary);background:var(--primary-soft);padding:8px 12px;border-radius:8px;font-weight:700}
a{color:var(--primary);text-decoration:none}a:hover{text-decoration:underline}
.card{background:var(--surface);border:1px solid var(--line);border-left:4px solid var(--primary);border-radius:var(--radius);padding:16px;margin:12px 0;box-shadow:var(--shadow)}
label{display:block;font-size:.85rem;color:var(--muted);margin:10px 0 6px}
.hint{font-size:.78rem;color:var(--muted);margin:-4px 0 8px}
input[type=text],input[type=password],input[type=number],textarea{width:100%;padding:9px 10px;background:#fff;border:1px solid var(--line);border-radius:8px;margin:4px 0 10px;font:inherit;outline:none}
input:focus,textarea:focus{border-color:rgba(22,163,74,.55);box-shadow:0 0 0 3px rgba(22,163,74,.12)}
textarea{resize:vertical;min-height:80px}
button,.btn{background:var(--primary);color:#fff;border:none;padding:10px 16px;border-radius:8px;cursor:pointer;font-size:.92rem;font-weight:600}
button:hover{background:var(--primary-hover)}
.btn-del{background:#6b7280}.btn-del:hover{background:#4b5563}
.danger{background:var(--danger)}.danger:hover{background:var(--danger-hover)}
table{width:100%;border-collapse:collapse;margin:8px 0}
th,td{text-align:left;padding:8px;border-bottom:1px solid var(--line)}
th{color:var(--primary);background:#f9fffb}
form{margin:0}
nav{margin:12px 0;display:flex;flex-wrap:wrap;gap:8px}
nav a{display:inline-flex;align-items:center;padding:8px 12px;border:1px solid var(--line);border-radius:999px;background:var(--surface);color:var(--text);text-decoration:none}
nav a:hover{text-decoration:none}
hr{border:none;height:1px;background:var(--line);margin:14px 0}
@media (max-width:640px){body{margin:18px auto;padding:0 12px}nav a{width:calc(50% - 4px)}}
</style></head><body>
<h1>&#9889; 3-Phase Data Logger</h1>
<nav><a href='/'>Home</a><a href='/wifi'>WiFi</a><a href='/mqtt'>MQTT</a><a href='/calibration'>Calibration</a></nav>
<hr>)";
}

String WebServerManager::_htmlFoot() {
    return String(R"(<hr><p style="font-size:.75rem;color:#6b7280;text-align:center">)") +
           FW_VERSION + R"( &bull; Muhammad Lutfi Nur Anendi</p></body></html>)";
}

// ─── Page: Login ───────────────────────────────────────────

String WebServerManager::_pageLogin() {
    String h = _htmlHead("Login");
    h += R"(<div class='card'><h2>Authentication Required</h2>
<p>Please enter your username and password.</p></div>)";
    h += _htmlFoot();
    return h;
}

// ─── Page: Index ──────────────────────────────────────────

String WebServerManager::_pageIndex(StorageManager& s) {
    String h = _htmlHead("Config");
    h += R"HTML(<div class="card"><h2>Configuration Pages</h2>
<p><a href="/wifi">&#128246; Manage Known Networks</a></p>
<p><a href="/mqtt">&#128272; MQTT Connection Config</a></p>
<p><a href="/calibration">&#9881; PZEM-004T Calibration &amp; Thresholds</a></p>
</div>
<div class="card"><h2>&#9888; Danger Zone</h2>
<form method="POST" action="/factoryreset"
  onsubmit="return confirm('ERASE ALL settings? This cannot be undone.')">
<button class="danger" type="submit">Hard Reset EEPROM</button>
</form></div>)HTML";
    h += _htmlFoot();
    return h;
}

// ─── Page: WiFi ───────────────────────────────────────────

String WebServerManager::_pageWifi(StorageManager& s) {
    String h = _htmlHead("Manage WiFi");
    h += "<h2>Known Networks</h2><div class='card'><table><tr><th>No.</th><th>SSID</th><th>Action</th></tr>";

    int n = s.getWifiCount();
    for (int i = 0; i < n; i++) {
        WiFiEntry e = s.getWifiEntry(i);
        h += "<tr><td>" + String(i + 1) + "</td><td>" + String(e.ssid) + "</td><td>"
             "<form method='POST' action='/wifi/delete' style='display:inline'>"
             "<input type='hidden' name='idx' value='" + String(i) + "'>"
             "<button class='btn btn-del' type='submit'>Delete</button></form></td></tr>";
    }
    if (n == 0) h += "<tr><td colspan='3'>No networks stored.</td></tr>";
    h += "</table></div>";

    h += R"(<div class="card"><h2>Add Network</h2>
<form method="POST" action="/wifi/add">
<label>SSID</label><input type="text" name="ssid" placeholder="Network name" required>
<label>Password</label><input type="password" name="pass" placeholder="Password">
<button type="submit">Add Network</button>
</form></div>)";
    h += _htmlFoot();
    return h;
}

// ─── Page: MQTT ───────────────────────────────────────────

String WebServerManager::_pageMQTT(StorageManager& s) {
    MQTTConfig cfg = s.getMQTTConfig();
    String h = _htmlHead("MQTT Config");
    h += "<h2>MQTT Broker</h2><div class='card'><form method='POST' action='/mqtt'>";

    auto field = [&](const char* label, const char* name, const char* val, const char* type = "text") {
        h += String("<label>") + label + "</label>"
             "<input type='" + type + "' name='" + name + "' value='" + val + "'>";
    };
    auto numF = [&](const char* label, const char* name, int val) {
        h += String("<label>") + label + "</label>"
             "<input type='number' name='" + name + "' value='" + val + "'>";
    };
    auto cb = [&](const char* label, const char* name, bool checked) {
        h += String("<label><input type='checkbox' name='") + name + "' " +
             (checked ? "checked" : "") + "> " + label + "</label><br>";
    };

    field("MQTT Server",        "server",   cfg.server);
    numF("Port (MQTT)",         "port",     cfg.port);
    numF("Port (MQTTS)",        "sslport",  cfg.sslPort);
    numF("Port (WS)",           "wsport",   cfg.wsPort);
    h += "<p class='hint'>Port WS disimpan untuk referensi. Koneksi via PubSubClient (raw TCP).</p>";
    numF("Port (WSS)",          "wssport",  cfg.wssPort);
    h += "<p class='hint'>Port WSS disimpan untuk referensi. Gunakan checklist SSL untuk koneksi aman.</p>";
    field("Username",           "user",     cfg.user);
    field("Password",           "pass",     cfg.pass, "password");
    field("Topic Prefix",       "prefix",   cfg.prefix);
    h += "<p class='hint'>Program otomatis tambah '/' antara prefix dan topic.</p>";
    field("Topic Name",         "topic",    cfg.topic);
    cb("Use SSL/TLS",           "usessl",   cfg.useSSL);

    h += "<label>CA Certificate (PEM)</label>"
         "<textarea name='cacert' rows='5'>" + String(cfg.caCert) + "</textarea>"
         "<button type='submit'>Save Config</button></form></div>";
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
        h += String("<label>Voltage Offset (V) &nbsp; Stored: ") +
             String(cal.voltageOffset[i], 3) + " V</label>";
        h += String("<input type='number' step='0.001' name='voff_") +
             phases[i] + "' value='" + String(cal.voltageOffset[i], 3) + "'>";
        h += String("<label>Current Offset (A) &nbsp; Stored: ") +
             String(cal.currentOffset[i], 3) + " A</label>";
        h += String("<input type='number' step='0.001' name='ioff_") +
             phases[i] + "' value='" + String(cal.currentOffset[i], 3) + "'>";
    }
    h += "</div>";

    h += "<h2>Line Status Thresholds</h2><div class='card'>";
    h += "<label>LOST threshold (V) &nbsp; Current: " + String(thr.voltageLost, 1) + " V</label>"
         "<input type='number' step='0.1' name='th_vlost' value='" + String(thr.voltageLost, 1) + "'>";
    h += "<label>UNDER threshold (V) &nbsp; Current: " + String(thr.voltageUnder, 1) + " V</label>"
         "<input type='number' step='0.1' name='th_vunder' value='" + String(thr.voltageUnder, 1) + "'>";
    h += "<label>OVER threshold (V) &nbsp; Current: " + String(thr.voltageOver, 1) + " V</label>"
         "<input type='number' step='0.1' name='th_vover' value='" + String(thr.voltageOver, 1) + "'>";
    h += "</div>";

    h += "<h2>Voltage Unbalance</h2><div class='card'>";
    h += "<label>Max Unbalance (%) &nbsp; Current: " + String(thr.unbalanceMax, 2) + " %</label>"
         "<input type='number' step='0.01' name='th_unbal' value='" + String(thr.unbalanceMax, 2) + "'>";
    h += "</div>";

    h += "<button type='submit'>Save Calibration &amp; Thresholds</button></form>";
    h += _htmlFoot();
    return h;
}

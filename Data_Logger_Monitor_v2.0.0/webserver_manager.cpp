#include "webserver_manager.h"
#include "config.h"
#include "diagnostics.h"

WebServerManager::WebServerManager(StorageManager* storage)
    : _server(80)
    , _storage(storage)
{}

void WebServerManager::begin() {
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* r) {
        if (!_authenticate(r)) { _handleLogin(r); return; }
        _handleIndex(r);
    });

    _server.on("/wifi", HTTP_GET, [this](AsyncWebServerRequest* r) {
        if (!_authenticate(r)) { _handleLogin(r); return; }
        _handleWiFi(r);
    });

    _server.on("/wifi/add", HTTP_POST, [this](AsyncWebServerRequest* r) {
        _handleWiFiAdd(r);
    });

    _server.on("/wifi/delete", HTTP_POST, [this](AsyncWebServerRequest* r) {
        _handleWiFiDelete(r);
    });

    _server.on("/mqtt", HTTP_GET, [this](AsyncWebServerRequest* r) {
        if (!_authenticate(r)) { _handleLogin(r); return; }
        _handleMQTT(r);
    });

    _server.on("/mqtt", HTTP_POST, [this](AsyncWebServerRequest* r) {
        _handleMQTTSave(r);
    });

    _server.on("/calibration", HTTP_GET, [this](AsyncWebServerRequest* r) {
        if (!_authenticate(r)) { _handleLogin(r); return; }
        _handleCalibration(r);
    });

    _server.on("/calibration", HTTP_POST, [this](AsyncWebServerRequest* r) {
        _handleCalibrationSave(r);
    });

    _server.on("/factoryreset", HTTP_POST, [this](AsyncWebServerRequest* r) {
        if (!_authenticate(r)) { _handleLogin(r); return; }
        _handleFactoryReset(r);
    });

    _server.begin();
    diag.info("WEB", "Web server started on port 80");
}

void WebServerManager::update() {
    // AsyncWebServer handles internally; no periodic task needed
}

// ─── Auth ───────────────────────────────────────────────────

bool WebServerManager::_authenticate(AsyncWebServerRequest* request) {
    if (!request->authenticate(WEB_USERNAME, WEB_PASSWORD)) {
        return false;
    }
    return true;
}

void WebServerManager::_handleLogin(AsyncWebServerRequest* request) {
    AsyncWebServerResponse* resp = request->beginResponse(401, "text/html", _pageLogin());
    resp->addHeader("WWW-Authenticate", "Basic realm=\"3-Phase Logger\"");
    request->send(resp);
}

// ─── HTML Templates ─────────────────────────────────────────

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
nav a:hover{background:var(--surface-2);text-decoration:none}
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

// ─── Pages ──────────────────────────────────────────────────

String WebServerManager::_pageLogin() {
    String h = _htmlHead("Login");
    h += R"(<div class='card'><h2>Authentication Required</h2>
<p>Please enter your username and password.</p></div>)";
    h += _htmlFoot();
    return h;
}

String WebServerManager::_pageIndex() {
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

String WebServerManager::_pageWiFi() {
    String h = _htmlHead("Manage WiFi");
    h += "<h2>Known Networks</h2><div class='card'><table><tr><th>No.</th><th>SSID</th><th>Action</th></tr>";

    int n = _storage->getWifiCount();
    for (int i = 0; i < n; i++) {
        WiFiEntry e = _storage->getWifiEntry(i);
        h += "<tr><td>" + String(i + 1) + "</td><td>" + String(e.ssid) + "</td><td>"
             "<form method='POST' action='/wifi/delete' style='display:inline'>"
             "<input type='hidden' name='idx' value='" + String(i) + "'>"
             "<button class='btn btn-del' type='submit'>Delete</button></form></td></tr>";
    }
    if (n == 0) h += "<tr><td colspan='3'>No networks stored.</td></tr>";
    h += "</table></div>";

    h += R"(<div class='card'><h2>Add Network</h2>
<form method='POST' action='/wifi/add'>
<label>SSID</label><input type='text' name='ssid' placeholder='Network name' required>
<label>Password</label><input type='password' name='pass' placeholder='Password'>
<button type='submit'>Add Network</button>
</form></div>)";
    h += _htmlFoot();
    return h;
}

String WebServerManager::_pageMQTT() {
    MQTTConfig cfg = _storage->getMQTTConfig();
    String h = _htmlHead("MQTT Config");
    h += "<h2>MQTT Broker</h2><div class='card'><form method='POST' action='/mqtt'>";

    auto field = [&](const char* label, const char* name, const char* val, const char* type = "text") {
        h += "<label>" + String(label) + "</label>"
             "<input type='" + String(type) + "' name='" + String(name) + "' value='" + String(val) + "'>";
    };
    auto numF = [&](const char* label, const char* name, int val) {
        h += "<label>" + String(label) + "</label>"
             "<input type='number' name='" + String(name) + "' value='" + String(val) + "'>";
    };
    auto cb = [&](const char* label, const char* name, bool checked) {
        h += "<label><input type='checkbox' name='" + String(name) + "' " +
             (checked ? "checked" : "") + "> " + String(label) + "</label><br>";
    };

    field("MQTT Server",        "server",   cfg.server);
    numF("Port (non-SSL)",      "port",     cfg.port);
    numF("Port (SSL)",          "sslport",  cfg.sslPort);
    field("Username",           "user",     cfg.user);
    field("Password",           "pass",     cfg.pass, "password");
    field("Topic Prefix",       "prefix",   cfg.prefix);
    field("Topic",              "topic",    cfg.topic);
    cb("Use SSL",               "usessl",   cfg.useSSL);
    cb("Use WebSocket",         "usews",    cfg.useWS);

    h += "<label>CA Certificate (PEM)</label>"
         "<textarea name='cacert' rows='5'>" + String(cfg.caCert) + "</textarea>"
         "<button type='submit'>Save Config</button></form></div>";
    h += _htmlFoot();
    return h;
}

String WebServerManager::_pageCalibration() {
    CalibrationData cal = _storage->getCalibration();
    ThresholdData thr = _storage->getThresholds();
    String h = _htmlHead("Calibration & Thresholds");
    h += "<form method='POST' action='/calibration'>";

    h += "<h2>PZEM-004T Calibration</h2>";
    h += "<p class='hint'>Offset is additive: Reading = Raw + Offset. Use negative to subtract.</p>";
    h += "<div class='card'>";
    const char* phases[3] = {"R", "S", "T"};
    for (int i = 0; i < 3; i++) {
        h += "<h2>Phase " + String(phases[i]) + "</h2>";
        h += "<label>Voltage Offset (V) &nbsp; Stored: " + String(cal.voltageOffset[i], 3) + " V</label>";
        h += "<input type='number' step='0.001' name='voff_" + String(phases[i]) +
             "' value='" + String(cal.voltageOffset[i], 3) + "'>";
        h += "<label>Current Offset (A) &nbsp; Stored: " + String(cal.currentOffset[i], 3) + " A</label>";
        h += "<input type='number' step='0.001' name='ioff_" + String(phases[i]) +
             "' value='" + String(cal.currentOffset[i], 3) + "'>";
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

// ─── Route Handlers ─────────────────────────────────────────

void WebServerManager::_handleIndex(AsyncWebServerRequest* request) {
    request->send(200, "text/html", _pageIndex());
}

void WebServerManager::_handleWiFi(AsyncWebServerRequest* request) {
    request->send(200, "text/html", _pageWiFi());
}

void WebServerManager::_handleWiFiAdd(AsyncWebServerRequest* request) {
    String ssid = request->arg("ssid");
    String pass = request->arg("pass");
    if (ssid.length() > 0) {
        _storage->addWifiEntry(ssid.c_str(), pass.c_str());
        diag.info("WEB", "WiFi added: %s", ssid.c_str());
    }
    request->redirect("/wifi");
}

void WebServerManager::_handleWiFiDelete(AsyncWebServerRequest* request) {
    int idx = request->arg("idx").toInt();
    _storage->deleteWifiEntry(idx);
    diag.info("WEB", "WiFi deleted index %d", idx);
    request->redirect("/wifi");
}

void WebServerManager::_handleMQTT(AsyncWebServerRequest* request) {
    request->send(200, "text/html", _pageMQTT());
}

void WebServerManager::_handleMQTTSave(AsyncWebServerRequest* request) {
    MQTTConfig cfg;
    memset(&cfg, 0, sizeof(cfg));

    strlcpy(cfg.server, request->arg("server").c_str(), sizeof(cfg.server));
    cfg.port    = request->arg("port").toInt();
    cfg.sslPort = request->arg("sslport").toInt();
    strlcpy(cfg.user, request->arg("user").c_str(), sizeof(cfg.user));
    strlcpy(cfg.pass, request->arg("pass").c_str(), sizeof(cfg.pass));
    strlcpy(cfg.prefix, request->arg("prefix").c_str(), sizeof(cfg.prefix));
    strlcpy(cfg.topic, request->arg("topic").c_str(), sizeof(cfg.topic));
    cfg.useSSL = request->hasArg("usessl");
    cfg.useWS  = request->hasArg("usews");
    strlcpy(cfg.caCert, request->arg("cacert").c_str(), sizeof(cfg.caCert));

    _storage->saveMQTTConfig(cfg);
    diag.info("WEB", "MQTT config saved");
    request->redirect("/mqtt");
}

void WebServerManager::_handleCalibration(AsyncWebServerRequest* request) {
    request->send(200, "text/html", _pageCalibration());
}

void WebServerManager::_handleCalibrationSave(AsyncWebServerRequest* request) {
    CalibrationData cal;
    ThresholdData thr;

    const char* phases[3] = {"R", "S", "T"};
    for (int i = 0; i < 3; i++) {
        String vk = "voff_" + String(phases[i]);
        String ik = "ioff_" + String(phases[i]);
        cal.voltageOffset[i] = request->arg(vk).toFloat();
        cal.currentOffset[i] = request->arg(ik).toFloat();
    }
    _storage->saveCalibration(cal);

    thr.voltageLost  = request->arg("th_vlost").toFloat();
    thr.voltageUnder = request->arg("th_vunder").toFloat();
    thr.voltageOver  = request->arg("th_vover").toFloat();
    thr.unbalanceMax = request->arg("th_unbal").toFloat();
    _storage->saveThresholds(thr);

    diag.info("WEB", "Calibration saved");
    request->redirect("/calibration");
}

void WebServerManager::_handleFactoryReset(AsyncWebServerRequest* request) {
    _storage->factoryReset();
    diag.warn("WEB", "Factory reset triggered!");
    String h = _htmlHead("Reset");
    h += "<div class='card'><h2>Factory Reset Complete</h2><p>All settings erased. Device will reboot.</p></div>";
    h += _htmlFoot();
    request->send(200, "text/html", h);
    delay(1000);
    ESP.restart();
}

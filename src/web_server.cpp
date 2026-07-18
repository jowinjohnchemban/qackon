#include "web_server.h"
#include "config.h"
#include "gps.h"
#include "imu.h"
#include "beacon.h"
#include "gsm.h"
#include "wifi_manager.h"
#include "ota.h"

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

static AsyncWebServer server(80);

static void handleGetStatus(AsyncWebServerRequest* req) {
  GpsFix fix = gpsGetFix();
  JsonDocument doc;

  doc["deviceName"] = cfg.deviceName;
  doc["version"] = QACKON_VERSION;
  doc["wifiConnected"] = wifiIsConnected();
  doc["setupMode"] = wifiIsInSetupMode();
  doc["gsmRegistered"] = gsmIsRegistered();
  doc["beaconArmed"] = beaconIsArmed();
  doc["imuMagnitudeG"] = imuLastMagnitudeG();

  JsonObject gps = doc["gps"].to<JsonObject>();
  gps["valid"] = fix.valid;
  gps["lat"] = fix.lat;
  gps["lng"] = fix.lng;
  gps["satellites"] = fix.satellites;
  gps["speedKmh"] = fix.speedKmh;
  gps["altitudeM"] = fix.altitudeM;

  String out;
  serializeJson(doc, out);
  req->send(200, "application/json", out);
}

static void handleGetConfig(AsyncWebServerRequest* req) {
  JsonDocument doc;
  doc["deviceName"] = cfg.deviceName;
  doc["ownerName"] = cfg.ownerName;
  doc["emergencyContact"] = cfg.emergencyContact;
  doc["wifiSSID"] = cfg.wifiSSID;
  // wifiPass intentionally omitted from GET — write-only field, same as
  // WLED does for its AP password in /json/cfg.
  doc["cloudURL"] = cfg.cloudURL;
  doc["cloudIntervalSec"] = cfg.cloudIntervalSec;
  doc["gsmEnabled"] = cfg.gsmEnabled;
  doc["beaconEnabled"] = cfg.beaconEnabled;
  doc["beaconIntervalMs"] = cfg.beaconIntervalMs;
  doc["motionThresholdG"] = cfg.motionThresholdG;
  doc["submersionAutoArm"] = cfg.submersionAutoArm;
  doc["geofenceEnabled"] = cfg.geofenceEnabled;
  doc["geofenceLat"] = cfg.geofenceLat;
  doc["geofenceLng"] = cfg.geofenceLng;
  doc["geofenceRadiusM"] = cfg.geofenceRadiusM;

  String out;
  serializeJson(doc, out);
  req->send(200, "application/json", out);
}

// Body arrives in chunks via this callback for POST /api/config.
static void handlePostConfigBody(AsyncWebServerRequest* req, uint8_t* data,
                                  size_t len, size_t index, size_t total) {
  static String body;
  if (index == 0) body = "";
  body += String((char*)data, len);
  if (index + len != total) return; // wait for the rest

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    req->send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }

  if (doc["deviceName"].is<const char*>())
    strlcpy(cfg.deviceName, doc["deviceName"], sizeof(cfg.deviceName));
  if (doc["ownerName"].is<const char*>())
    strlcpy(cfg.ownerName, doc["ownerName"], sizeof(cfg.ownerName));
  if (doc["emergencyContact"].is<const char*>())
    strlcpy(cfg.emergencyContact, doc["emergencyContact"], sizeof(cfg.emergencyContact));
  if (doc["wifiSSID"].is<const char*>())
    strlcpy(cfg.wifiSSID, doc["wifiSSID"], sizeof(cfg.wifiSSID));
  if (doc["wifiPass"].is<const char*>())
    strlcpy(cfg.wifiPass, doc["wifiPass"], sizeof(cfg.wifiPass));
  if (doc["cloudURL"].is<const char*>())
    strlcpy(cfg.cloudURL, doc["cloudURL"], sizeof(cfg.cloudURL));
  if (doc["cloudIntervalSec"].is<uint16_t>())
    cfg.cloudIntervalSec = doc["cloudIntervalSec"];
  if (doc["gsmEnabled"].is<bool>())
    cfg.gsmEnabled = doc["gsmEnabled"];
  if (doc["beaconEnabled"].is<bool>())
    cfg.beaconEnabled = doc["beaconEnabled"];
  if (doc["beaconIntervalMs"].is<uint16_t>())
    cfg.beaconIntervalMs = doc["beaconIntervalMs"];
  if (doc["motionThresholdG"].is<float>())
    cfg.motionThresholdG = doc["motionThresholdG"];
  if (doc["submersionAutoArm"].is<bool>())
    cfg.submersionAutoArm = doc["submersionAutoArm"];
  if (doc["geofenceEnabled"].is<bool>())
    cfg.geofenceEnabled = doc["geofenceEnabled"];
  if (doc["geofenceLat"].is<double>())
    cfg.geofenceLat = doc["geofenceLat"];
  if (doc["geofenceLng"].is<double>())
    cfg.geofenceLng = doc["geofenceLng"];
  if (doc["geofenceRadiusM"].is<float>())
    cfg.geofenceRadiusM = doc["geofenceRadiusM"];

  configSave();
  req->send(200, "application/json", "{\"ok\":true,\"note\":\"reboot to apply WiFi changes\"}");
}

static void handlePostArm(AsyncWebServerRequest* req) {
  bool arm = true;
  if (req->hasParam("state", true))
    arm = req->getParam("state", true)->value() == "on";

  if (arm) beaconArm(); else beaconDisarm();
  req->send(200, "application/json", "{\"ok\":true}");
}

void webServerBegin() {
  if (!LittleFS.begin(true)) {
    Serial.println("[web] LittleFS mount failed");
  }

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on("/api/status", HTTP_GET, handleGetStatus);
  server.on("/api/config", HTTP_GET, handleGetConfig);
  server.on("/api/config", HTTP_POST, [](AsyncWebServerRequest* req) {},
            nullptr, handlePostConfigBody);
  server.on("/api/arm", HTTP_POST, handlePostArm);

  otaRegisterRoutes(server);

  // Captive portal: any unknown path redirects to the setup page while in
  // AP mode, so phones auto-open the config UI (same trick WLED/WiFiManager use).
  server.onNotFound([](AsyncWebServerRequest* req) {
    if (wifiIsInSetupMode()) {
      req->redirect("/");
    } else {
      req->send(404, "text/plain", "Not found");
    }
  });

  server.begin();
  Serial.println("[web] server started on port 80");
}

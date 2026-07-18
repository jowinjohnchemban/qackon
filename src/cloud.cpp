#include "cloud.h"
#include "config.h"
#include "wifi_manager.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

static unsigned long lastUploadMs = 0;

void cloudLoop(const GpsFix& fix) {
  if (strlen(cfg.cloudURL) == 0) return;
  if (!wifiIsConnected()) return;
  if (millis() - lastUploadMs < (unsigned long)cfg.cloudIntervalSec * 1000UL) return;
  lastUploadMs = millis();

  JsonDocument doc;
  doc["device"] = cfg.deviceName;
  doc["lat"] = fix.lat;
  doc["lng"] = fix.lng;
  doc["valid"] = fix.valid;
  doc["satellites"] = fix.satellites;
  doc["speedKmh"] = fix.speedKmh;
  doc["uptimeMs"] = millis();

  String body;
  serializeJson(doc, body);

  HTTPClient http;
  http.begin(cfg.cloudURL);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(body);
  Serial.printf("[cloud] upload -> %d\n", code);
  http.end();
}

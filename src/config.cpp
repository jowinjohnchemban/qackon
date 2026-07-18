#include "config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

QackonConfig cfg;

static const char* CFG_PATH = "/cfg.json";

bool configLoad() {
  if (!LittleFS.exists(CFG_PATH)) {
    Serial.println("[cfg] no cfg.json found, using defaults");
    return false;
  }

  File f = LittleFS.open(CFG_PATH, "r");
  if (!f) {
    Serial.println("[cfg] failed to open cfg.json");
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    Serial.printf("[cfg] parse error: %s\n", err.c_str());
    return false;
  }

  strlcpy(cfg.deviceName,       doc["deviceName"]       | cfg.deviceName,       sizeof(cfg.deviceName));
  strlcpy(cfg.ownerName,        doc["ownerName"]         | cfg.ownerName,        sizeof(cfg.ownerName));
  strlcpy(cfg.emergencyContact, doc["emergencyContact"]  | cfg.emergencyContact, sizeof(cfg.emergencyContact));
  strlcpy(cfg.wifiSSID,         doc["wifiSSID"]          | cfg.wifiSSID,         sizeof(cfg.wifiSSID));
  strlcpy(cfg.wifiPass,         doc["wifiPass"]          | cfg.wifiPass,         sizeof(cfg.wifiPass));
  strlcpy(cfg.cloudURL,         doc["cloudURL"]          | cfg.cloudURL,         sizeof(cfg.cloudURL));
  strlcpy(cfg.gsmAPN,           doc["gsmAPN"]            | cfg.gsmAPN,           sizeof(cfg.gsmAPN));

  cfg.cloudIntervalSec  = doc["cloudIntervalSec"]  | cfg.cloudIntervalSec;
  cfg.gsmEnabled        = doc["gsmEnabled"]        | cfg.gsmEnabled;
  cfg.beaconEnabled     = doc["beaconEnabled"]     | cfg.beaconEnabled;
  cfg.beaconIntervalMs  = doc["beaconIntervalMs"]  | cfg.beaconIntervalMs;
  cfg.motionThresholdG  = doc["motionThresholdG"]  | cfg.motionThresholdG;
  cfg.submersionAutoArm = doc["submersionAutoArm"] | cfg.submersionAutoArm;
  cfg.geofenceEnabled   = doc["geofenceEnabled"]   | cfg.geofenceEnabled;
  cfg.geofenceLat       = doc["geofenceLat"]       | cfg.geofenceLat;
  cfg.geofenceLng       = doc["geofenceLng"]       | cfg.geofenceLng;
  cfg.geofenceRadiusM   = doc["geofenceRadiusM"]   | cfg.geofenceRadiusM;

  Serial.println("[cfg] loaded cfg.json");
  return true;
}

bool configSave() {
  JsonDocument doc;

  doc["deviceName"]       = cfg.deviceName;
  doc["ownerName"]        = cfg.ownerName;
  doc["emergencyContact"] = cfg.emergencyContact;
  doc["wifiSSID"]         = cfg.wifiSSID;
  doc["wifiPass"]         = cfg.wifiPass;
  doc["cloudURL"]         = cfg.cloudURL;
  doc["cloudIntervalSec"] = cfg.cloudIntervalSec;
  doc["gsmAPN"]           = cfg.gsmAPN;
  doc["gsmEnabled"]       = cfg.gsmEnabled;
  doc["beaconEnabled"]    = cfg.beaconEnabled;
  doc["beaconIntervalMs"] = cfg.beaconIntervalMs;
  doc["motionThresholdG"] = cfg.motionThresholdG;
  doc["submersionAutoArm"]= cfg.submersionAutoArm;
  doc["geofenceEnabled"]  = cfg.geofenceEnabled;
  doc["geofenceLat"]      = cfg.geofenceLat;
  doc["geofenceLng"]      = cfg.geofenceLng;
  doc["geofenceRadiusM"]  = cfg.geofenceRadiusM;

  File f = LittleFS.open(CFG_PATH, "w");
  if (!f) {
    Serial.println("[cfg] failed to open cfg.json for write");
    return false;
  }
  serializeJson(doc, f);
  f.close();
  Serial.println("[cfg] saved cfg.json");
  return true;
}

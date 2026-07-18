#pragma once
#include <Arduino.h>

// All user-editable settings live here. Loaded from LittleFS /cfg.json at
// boot, saved back whenever the web UI or BLE config changes something.
// Keeping this as one flat struct (rather than scattering settings across
// modules) is what makes a single web form / JSON API possible — same
// pattern WLED uses for its cfg.json.

struct QackonConfig {
  // --- Identity ---
  char deviceName[32]      = "Qackon-Beacon";
  char ownerName[32]       = "";
  char emergencyContact[20] = ""; // phone number for SMS alerts

  // --- WiFi (station mode) ---
  char wifiSSID[32]  = "";
  char wifiPass[64]  = "";

  // --- Cloud upload ---
  char cloudURL[128] = "";       // HTTPS endpoint to POST location JSON to
  uint16_t cloudIntervalSec = 60;

  // --- GSM / SIM ---
  char gsmAPN[32]     = "";
  bool gsmEnabled      = true;

  // --- Beacon transmitter ---
  bool beaconEnabled       = true;
  uint16_t beaconIntervalMs = 1000; // pulse repetition interval

  // --- Activation sensitivity ---
  float motionThresholdG   = 1.5f;  // accel delta to trigger activation
  bool submersionAutoArm   = true;

  // --- Geofence (optional) ---
  bool geofenceEnabled = false;
  double geofenceLat   = 0.0;
  double geofenceLng   = 0.0;
  float geofenceRadiusM = 500.0f;
};

extern QackonConfig cfg;

// Loads /cfg.json from LittleFS into `cfg`. Returns false (and leaves
// defaults in place) if no config file exists yet — that's the normal
// first-boot case, not an error.
bool configLoad();

// Serializes `cfg` back to /cfg.json. Call after any change via web/BLE.
bool configSave();

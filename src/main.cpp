#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "gps.h"
#include "imu.h"
#include "gsm.h"
#include "beacon.h"
#include "ble_config.h"
#include "cloud.h"

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.printf("\n[qackon] booting, firmware %s\n", QACKON_VERSION);

  configLoad();       // pull saved settings before anything else needs them
  beaconBegin();
  gpsBegin();
  imuBegin();          // ok to fail — logs and continues without it
  gsmBegin();          // ok to fail — logs and continues without it

  wifiManagerBegin();  // joins WiFi, or opens the Qackon-Setup AP
  webServerBegin();    // UI + JSON API, works in both modes

  if (wifiIsInSetupMode()) {
    bleConfigBegin();  // also offer BLE setup for phones without WiFi UI flow
  }

  Serial.println("[qackon] setup complete");
}

void loop() {
  wifiManagerLoop();
  gpsLoop();
  imuLoop();
  gsmLoop();
  beaconLoop();
  if (wifiIsInSetupMode()) {
    bleConfigLoop();
  }

  // Auto-activation: an IMU-detected impact/motion event arms the beacon
  // and (if a GSM link exists) sends the emergency contact an SMS with the
  // last known fix. This is the core safety behavior — everything else
  // (web UI, cloud upload) is secondary to this working even with no
  // connectivity at all.
  if (imuActivationTriggered()) {
    Serial.println("[main] activation triggered by IMU");
    beaconArm();
    GpsFix fix = gpsGetFix();
    if (fix.valid && strlen(cfg.emergencyContact) > 0) {
      gsmSendLocationSms(fix.lat, fix.lng);
    }
  }

  cloudLoop(gpsGetFix());
}

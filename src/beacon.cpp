#include "beacon.h"
#include "config.h"

// Enable/trigger pin to the external 457 kHz beacon transmitter module.
// Pull HIGH briefly to fire one pulse; the transmitter board handles the
// actual RF generation and antenna matching.
static const int BEACON_TRIGGER_PIN = 25;
static const int STATUS_LED_PIN = 2; // onboard LED, mirrors beacon state

static bool armed = false;
static unsigned long lastPulseMs = 0;

void beaconBegin() {
  pinMode(BEACON_TRIGGER_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(BEACON_TRIGGER_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
}

void beaconArm() {
  if (!cfg.beaconEnabled) {
    Serial.println("[beacon] arm requested but beacon disabled in config");
    return;
  }
  armed = true;
  digitalWrite(STATUS_LED_PIN, HIGH);
  Serial.println("[beacon] armed");
}

void beaconDisarm() {
  armed = false;
  digitalWrite(BEACON_TRIGGER_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  Serial.println("[beacon] disarmed");
}

bool beaconIsArmed() { return armed; }

void beaconLoop() {
  if (!armed) return;
  if (millis() - lastPulseMs < cfg.beaconIntervalMs) return;
  lastPulseMs = millis();

  // Short trigger pulse — width depends on your transmitter board's spec,
  // 10ms is a placeholder. Check the module datasheet before relying on this.
  digitalWrite(BEACON_TRIGGER_PIN, HIGH);
  delay(10);
  digitalWrite(BEACON_TRIGGER_PIN, LOW);
}

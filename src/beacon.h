#pragma once
#include <Arduino.h>

// The 457 kHz RF transmission itself happens on the dedicated beacon
// transmitter board (see README hardware section) — the ESP32 only
// controls its enable/trigger line and pulse timing. This module does not
// synthesize RF signals itself.

void beaconBegin();
void beaconLoop();

void beaconArm();     // start pulsing at cfg.beaconIntervalMs
void beaconDisarm();  // stop pulsing
bool beaconIsArmed();

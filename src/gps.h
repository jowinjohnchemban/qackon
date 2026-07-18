#pragma once
#include <Arduino.h>

struct GpsFix {
  bool valid = false;
  double lat = 0.0;
  double lng = 0.0;
  float speedKmh = 0.0f;
  float altitudeM = 0.0f;
  uint8_t satellites = 0;
  uint32_t ageMs = 0xFFFFFFFF; // time since last fix update
};

// Ublox NEO-6M on UART2 (GPIO16 RX / GPIO17 TX by default — adjust in .cpp
// to match your wiring).
void gpsBegin();

// Feed available serial bytes into the parser. Call every loop() iteration;
// non-blocking.
void gpsLoop();

// Latest known fix. `valid` is false until the module gets its first lock.
GpsFix gpsGetFix();

#pragma once
#include <Arduino.h>

// Mirrors the WLED WiFi pattern:
//  1. If wifiSSID is set in config, try to join it for a few seconds.
//  2. On failure (or if no SSID configured yet), fall back to a
//     "Qackon-Setup" access point with a captive portal so the user can
//     enter WiFi credentials from their phone with no app needed.
// Call wifiManagerBegin() once in setup(), then wifiManagerLoop() every
// loop() iteration to service the captive-portal DNS server while in AP mode.

void wifiManagerBegin();
void wifiManagerLoop();

// True once we're joined to the configured network (station mode, got IP).
bool wifiIsConnected();

// True while running as a fallback AP for setup.
bool wifiIsInSetupMode();

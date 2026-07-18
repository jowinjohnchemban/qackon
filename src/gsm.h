#pragma once
#include <Arduino.h>

// SIM800L on UART1. Used for two things:
//  1. Sending an SMS with current coordinates to cfg.emergencyContact when
//     the beacon activates and no WiFi is available.
//  2. (Optional, future) GPRS fallback so cloud uploads still work with no
//     WiFi in range — stubbed for now, see README roadmap.

bool gsmBegin();
void gsmLoop();
bool gsmIsRegistered(); // true once registered on the cellular network

// Sends "lat,lng" as a plain-text SMS to cfg.emergencyContact. Returns false
// if the module isn't registered or the send fails.
bool gsmSendLocationSms(double lat, double lng);

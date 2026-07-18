#pragma once

// Exposes one BLE GATT characteristic that accepts the same JSON schema as
// /cfg.json over the web API. Lets the companion app configure the device
// (WiFi credentials, emergency contact, thresholds) before it's ever joined
// a WiFi network — same role WLED's "improv" BLE setup plays.

void bleConfigBegin();
void bleConfigLoop();

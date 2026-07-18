#pragma once

// Serves the single-page UI from LittleFS (/data) and exposes:
//   GET  /api/status  -> live JSON (fix, battery, arm state, signal)
//   GET  /api/config  -> current cfg.json contents
//   POST /api/config  -> merge posted JSON into cfg + save + apply
//   POST /api/arm     -> manually arm/disarm the beacon
//   POST /api/update  -> OTA firmware upload (multipart), see ota.h

void webServerBegin();

#pragma once
#include <ESPAsyncWebServer.h>

// Registers POST /api/update on the given server: accepts a multipart
// firmware .bin upload and flashes it via the Update library, then reboots.
// This is what makes "flash once over USB, update forever over WiFi" work.
void otaRegisterRoutes(AsyncWebServer& server);

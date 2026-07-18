#include "ota.h"
#include <Update.h>

static void handleUpdateUpload(AsyncWebServerRequest* req, String filename,
                                size_t index, uint8_t* data, size_t len, bool final) {
  if (index == 0) {
    Serial.printf("[ota] update start: %s\n", filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }

  if (final) {
    if (Update.end(true)) {
      Serial.printf("[ota] update complete, %u bytes\n", (unsigned)(index + len));
    } else {
      Update.printError(Serial);
    }
  }
}

void otaRegisterRoutes(AsyncWebServer& server) {
  server.on(
      "/api/update", HTTP_POST,
      [](AsyncWebServerRequest* req) {
        bool ok = !Update.hasError();
        AsyncWebServerResponse* res = req->beginResponse(
            200, "application/json", ok ? "{\"ok\":true}" : "{\"ok\":false}");
        res->addHeader("Connection", "close");
        req->send(res);
        if (ok) {
          delay(500);
          ESP.restart();
        }
      },
      handleUpdateUpload);
}

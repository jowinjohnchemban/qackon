#include "ble_config.h"
#include "config.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <ArduinoJson.h>

// Standard-ish custom UUIDs for the Qackon config service. Generate your own
// with `python -c "import uuid; print(uuid.uuid4())"` before shipping more
// than one unit on the same network of BLE scanners.
#define QACKON_SERVICE_UUID        "6e400001-0001-4a1f-9e6d-b1a2c3d4e5f6"
#define QACKON_CONFIG_CHAR_UUID    "6e400002-0001-4a1f-9e6d-b1a2c3d4e5f6"

static BLEServer* server = nullptr;
static BLECharacteristic* configChar = nullptr;

class ConfigWriteCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    std::string value = c->getValue();
    if (value.empty()) return;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, value);
    if (err) {
      Serial.printf("[ble] bad JSON from client: %s\n", err.c_str());
      return;
    }

    // Only accept the fields relevant to first-time setup over BLE — full
    // config editing is the web UI's job once WiFi is up.
    if (doc["wifiSSID"].is<const char*>())
      strlcpy(cfg.wifiSSID, doc["wifiSSID"], sizeof(cfg.wifiSSID));
    if (doc["wifiPass"].is<const char*>())
      strlcpy(cfg.wifiPass, doc["wifiPass"], sizeof(cfg.wifiPass));
    if (doc["emergencyContact"].is<const char*>())
      strlcpy(cfg.emergencyContact, doc["emergencyContact"], sizeof(cfg.emergencyContact));

    configSave();
    Serial.println("[ble] config updated, will apply on next reboot");
  }
};

void bleConfigBegin() {
  BLEDevice::init(cfg.deviceName);
  server = BLEDevice::createServer();
  BLEService* service = server->createService(QACKON_SERVICE_UUID);

  configChar = service->createCharacteristic(
      QACKON_CONFIG_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  configChar->setCallbacks(new ConfigWriteCallback());

  service->start();
  server->getAdvertising()->start();
  Serial.println("[ble] config service advertising");
}

void bleConfigLoop() {
  // Nothing to poll — writes arrive via the callback above. Reserved for
  // future connection-state handling (e.g. re-advertise after disconnect).
}

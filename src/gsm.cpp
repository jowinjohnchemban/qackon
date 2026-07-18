#include "gsm.h"
#include "config.h"

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <HardwareSerial.h>

// Wiring: SIM800L TXD -> ESP32 GPIO26 (RX1), SIM800L RXD -> ESP32 GPIO27 (TX1)
// SIM800L needs its own 3.7-4.2V supply capable of ~2A peaks — do NOT power
// it from the ESP32's 3.3V rail directly.
static const int GSM_RX_PIN = 26;
static const int GSM_TX_PIN = 27;
static const uint32_t GSM_BAUD = 9600;

static HardwareSerial gsmSerial(1); // UART1
static TinyGsm modem(gsmSerial);
static bool registered = false;
static unsigned long lastCheckMs = 0;
static const unsigned long CHECK_INTERVAL_MS = 10000;

bool gsmBegin() {
  if (!cfg.gsmEnabled) {
    Serial.println("[gsm] disabled in config, skipping init");
    return false;
  }
  gsmSerial.begin(GSM_BAUD, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  Serial.println("[gsm] initializing modem...");
  if (!modem.restart()) {
    Serial.println("[gsm] modem restart failed — check wiring/power");
    return false;
  }
  Serial.printf("[gsm] modem info: %s\n", modem.getModemInfo().c_str());
  return true;
}

void gsmLoop() {
  if (!cfg.gsmEnabled) return;
  if (millis() - lastCheckMs < CHECK_INTERVAL_MS) return;
  lastCheckMs = millis();
  registered = modem.isNetworkConnected();
}

bool gsmIsRegistered() { return registered; }

bool gsmSendLocationSms(double lat, double lng) {
  if (!registered) {
    Serial.println("[gsm] not registered, can't send SMS");
    return false;
  }
  char msg[96];
  snprintf(msg, sizeof(msg),
           "Qackon alert: %s activated. Location: %.6f,%.6f "
           "https://maps.google.com/?q=%.6f,%.6f",
           cfg.deviceName, lat, lng, lat, lng);

  bool ok = modem.sendSMS(cfg.emergencyContact, String(msg));
  Serial.printf("[gsm] SMS send %s\n", ok ? "OK" : "FAILED");
  return ok;
}

#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>
#include <DNSServer.h>

static DNSServer dnsServer;
static bool inSetupMode = false;
static bool staConnected = false;
static unsigned long staAttemptStart = 0;
static const unsigned long STA_TIMEOUT_MS = 8000;

static const byte DNS_PORT = 53;
static IPAddress apIP(4, 3, 2, 1);

static void startSetupAP() {
  inSetupMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Qackon-Setup", "qackon123");
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("[wifi] setup AP started: Qackon-Setup / qackon123");
  Serial.println("[wifi] connect and visit http://4.3.2.1 to configure");
}

void wifiManagerBegin() {
  if (strlen(cfg.wifiSSID) == 0) {
    startSetupAP();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg.wifiSSID, cfg.wifiPass);
  staAttemptStart = millis();
  Serial.printf("[wifi] joining '%s'...\n", cfg.wifiSSID);

  while (WiFi.status() != WL_CONNECTED &&
         millis() - staAttemptStart < STA_TIMEOUT_MS) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    staConnected = true;
    Serial.printf("[wifi] connected, IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("[wifi] join failed, falling back to setup AP");
    startSetupAP();
  }
}

void wifiManagerLoop() {
  if (inSetupMode) {
    dnsServer.processNextRequest();
  } else if (WiFi.status() != WL_CONNECTED && staConnected) {
    // Dropped connection after having been up — WLED just lets Wifi.begin's
    // auto-reconnect handle it, but we track state so the UI can show it.
    staConnected = false;
    Serial.println("[wifi] connection lost, attempting reconnect");
    WiFi.reconnect();
  } else if (WiFi.status() == WL_CONNECTED && !staConnected) {
    staConnected = true;
    Serial.printf("[wifi] reconnected, IP: %s\n", WiFi.localIP().toString().c_str());
  }
}

bool wifiIsConnected() { return staConnected; }
bool wifiIsInSetupMode() { return inSetupMode; }

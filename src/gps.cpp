#include "gps.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Wiring: NEO-6M TX -> ESP32 GPIO16 (RX2), NEO-6M RX -> ESP32 GPIO17 (TX2)
static const int GPS_RX_PIN = 16;
static const int GPS_TX_PIN = 17;
static const uint32_t GPS_BAUD = 9600;

static HardwareSerial gpsSerial(2); // UART2
static TinyGPSPlus gps;

void gpsBegin() {
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("[gps] UART2 started at 9600 baud");
}

void gpsLoop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
}

GpsFix gpsGetFix() {
  GpsFix fix;
  fix.valid = gps.location.isValid() && gps.location.isUpdated() || gps.location.isValid();
  if (gps.location.isValid()) {
    fix.lat = gps.location.lat();
    fix.lng = gps.location.lng();
    fix.ageMs = gps.location.age();
  }
  if (gps.speed.isValid())    fix.speedKmh   = gps.speed.kmph();
  if (gps.altitude.isValid()) fix.altitudeM  = gps.altitude.meters();
  if (gps.satellites.isValid()) fix.satellites = gps.satellites.value();
  return fix;
}

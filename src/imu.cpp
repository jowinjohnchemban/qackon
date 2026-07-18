#include "imu.h"
#include "config.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

static Adafruit_MPU6050 mpu;
static bool mpuOk = false;
static float lastMagG = 1.0f;
static bool triggered = false;
static unsigned long lastPollMs = 0;
static const unsigned long POLL_INTERVAL_MS = 100; // 10 Hz is plenty for impact/submersion

bool imuBegin() {
  Wire.begin(); // default SDA=21 SCL=22 on most ESP32 dev boards
  mpuOk = mpu.begin();
  if (mpuOk) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("[imu] MPU6050 detected");
  } else {
    Serial.println("[imu] MPU6050 not found — activation sensing disabled");
  }
  return mpuOk;
}

void imuLoop() {
  if (!mpuOk) return;
  if (millis() - lastPollMs < POLL_INTERVAL_MS) return;
  lastPollMs = millis();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Magnitude of the acceleration vector in g. At rest this reads ~1.0g
  // (gravity). A sudden jump/drop (impact, free-fall, water entry shock)
  // shows up as a spike away from that baseline.
  float magG = sqrtf(a.acceleration.x * a.acceleration.x +
                      a.acceleration.y * a.acceleration.y +
                      a.acceleration.z * a.acceleration.z) / 9.80665f;
  lastMagG = magG;

  float delta = fabsf(magG - 1.0f);
  if (delta >= cfg.motionThresholdG) {
    triggered = true;
  }

  // NOTE: submersion detection isn't something an accelerometer can do on
  // its own — this only covers the "impact/motion" half of auto-activation.
  // A real submersion sensor (conductivity probe or pressure sensor) should
  // hook in here alongside the IMU check; see README "Hardware gaps".
}

bool imuActivationTriggered() {
  if (triggered) {
    triggered = false;
    return true;
  }
  return false;
}

float imuLastMagnitudeG() { return lastMagG; }

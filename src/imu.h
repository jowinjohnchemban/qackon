#pragma once
#include <Arduino.h>

// Wraps the MPU6050 to answer one question the rest of the firmware cares
// about: "should the beacon auto-activate right now?" Threshold comes from
// cfg.motionThresholdG so it's tunable from the web UI without a rebuild.

bool imuBegin();       // returns false if the sensor isn't detected on I2C
void imuLoop();        // poll accelerometer, update internal activation state
bool imuActivationTriggered(); // true once, then auto-clears (edge-triggered)
float imuLastMagnitudeG();     // last measured acceleration magnitude, for the UI

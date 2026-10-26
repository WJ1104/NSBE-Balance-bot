#pragma once
// ===== imu.h =====
// Reads the IMU and produces a filtered tilt angle.
// If your team uses a different IMU, this is the only file you should need to change.
//
// Public interface:
//   bool  imuInit();                 // returns false if the sensor doesn't respond
//   void  imuCalibrate();            // robot must be still
//   void  imuUpdate(float dt);       // call every loop
//   float imuAngle;                  // filtered tilt, degrees
//   float imuRate;                   // angular velocity, deg/s

#include <Wire.h>
#include "config.h"

float imuAngle = 0;
float imuRate  = 0;
static float gyroBias = 0;

static void imuWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

// Returns false if the read fails
static bool imuRead(float &ax, float &az, float &gyroY) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint8_t)IMU_ADDR, (uint8_t)14) != 14) return false;

  int16_t rawAx = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();
  int16_t rawAz = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();
  Wire.read(); Wire.read();
  int16_t rawGy = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();

  ax = rawAx;
  az = rawAz;
  gyroY = rawGy / 131.0f;
  return true;
}

bool imuInit() {
  Wire.begin();
  Wire.setClock(400000);

  Wire.beginTransmission(IMU_ADDR);
  if (Wire.endTransmission() != 0) return false;   // not found

  imuWrite(0x6B, 0x00);   // wake
  imuWrite(0x1A, 0x03);   // low-pass filter ~44 Hz
  imuWrite(0x1B, 0x00);   // gyro +/-250 deg/s
  imuWrite(0x1C, 0x00);   // accel +/-2 g
  return true;
}

void imuCalibrate() {
  const int N = 500;
  float sum = 0;
  int good = 0;
  for (int i = 0; i < N; i++) {
    float ax, az, gy;
    if (imuRead(ax, az, gy)) { sum += gy; good++; }
    delay(3);
  }
  gyroBias = good > 0 ? sum / good : 0;

  float ax, az, gy;
  if (imuRead(ax, az, gy)) {
    imuAngle = ANGLE_SIGN * atan2(ax, az) * 180.0f / PI;
  }
}

// Returns false if the sensor read failed (caller should stop the motors)
bool imuUpdate(float dt) {
  float ax, az, gy;
  if (!imuRead(ax, az, gy)) return false;

  imuRate = ANGLE_SIGN * (gy - gyroBias);
  float accelAngle = ANGLE_SIGN * atan2(ax, az) * 180.0f / PI;

  // Complementary filter: gyro for fast changes, accel to cancel drift
  imuAngle = FILTER_ALPHA * (imuAngle + imuRate * dt) + (1.0f - FILTER_ALPHA) * accelAngle;
  return true;
}

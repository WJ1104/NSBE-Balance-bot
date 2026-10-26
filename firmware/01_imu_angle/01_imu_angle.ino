// Step 2: Read the IMU and print a tilt angle.
//
// Example hardware: MPU6050 over I2C (address 0x68). No library needed,
// we talk to the chip directly so you can see how it works.
//
// What to do:
//   1. Upload, then open Serial Monitor at 115200 baud.
//   2. Keep the board still for ~2 seconds while it calibrates the gyro.
//   3. Tilt the board forward and back. "angle" should change smoothly.
//   4. If the sign is backwards or the wrong axis moves, change the
//      accel angle line below (swap ax/ay, flip a sign).
//
// Columns printed: accelAngle, gyroOnlyAngle, filteredAngle (degrees)
// Try the Serial Plotter (Tools menu) to watch all three at once.
// You should see: accel is noisy, gyro drifts, filtered is smooth and stable.

#include <Wire.h>

const uint8_t MPU_ADDR = 0x68;
const float   ALPHA    = 0.98;   // trust gyro 98%, accel 2%
const float   LOOP_HZ  = 200.0;
const unsigned long LOOP_US = 1000000UL / LOOP_HZ;

float gyroBias = 0;       // deg/s measured while still
float filtered = 0;       // complementary filter output (deg)
float gyroOnly = 0;       // pure gyro integration (deg), to show drift
unsigned long lastMicros;
unsigned long lastPrint = 0;

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

// Reads accel X/Z and gyro Y (the axes used for pitch in this example)
void readIMU(float &ax, float &az, float &gyroY) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                       // first accel register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)14);

  int16_t rawAx = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();               // skip accel Y
  int16_t rawAz = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();               // skip temperature
  Wire.read(); Wire.read();               // skip gyro X
  int16_t rawGy = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();               // skip gyro Z

  ax    = rawAx;
  az    = rawAz;
  gyroY = rawGy / 131.0;                  // +/-250 deg/s range -> 131 LSB per deg/s
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  writeReg(0x6B, 0x00);   // wake up (clear sleep bit)
  writeReg(0x1A, 0x03);   // digital low-pass filter ~44 Hz
  writeReg(0x1B, 0x00);   // gyro range +/-250 deg/s
  writeReg(0x1C, 0x00);   // accel range +/-2 g

  Serial.println("Calibrating gyro. Keep the board still...");
  const int N = 500;
  float sum = 0;
  for (int i = 0; i < N; i++) {
    float ax, az, gy;
    readIMU(ax, az, gy);
    sum += gy;
    delay(3);
  }
  gyroBias = sum / N;
  Serial.print("Gyro bias (deg/s): ");
  Serial.println(gyroBias);

  // Start the filter at the accelerometer's angle
  float ax, az, gy;
  readIMU(ax, az, gy);
  filtered = gyroOnly = atan2(ax, az) * 180.0 / PI;
  lastMicros = micros();
}

void loop() {
  // Run at a fixed rate
  unsigned long now = micros();
  if (now - lastMicros < LOOP_US) return;
  float dt = (now - lastMicros) / 1000000.0;
  lastMicros = now;

  float ax, az, gy;
  readIMU(ax, az, gy);
  gy -= gyroBias;

  float accelAngle = atan2(ax, az) * 180.0 / PI;   // <- change axes/sign if needed
  gyroOnly += gy * dt;
  filtered  = ALPHA * (filtered + gy * dt) + (1.0 - ALPHA) * accelAngle;

  // Print at ~20 Hz so the Serial Monitor stays readable
  if (millis() - lastPrint > 50) {
    lastPrint = millis();
    Serial.print(accelAngle);  Serial.print(',');
    Serial.print(gyroOnly);    Serial.print(',');
    Serial.println(filtered);
  }
}

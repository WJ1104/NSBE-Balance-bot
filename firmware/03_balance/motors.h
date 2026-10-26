#pragma once
// ===== motors.h =====
// Everything about the motor driver lives here. If your team uses a
// different driver, this is the only file you should need to change.
//
// Public interface:
//   motorsInit();
//   setMotors(left, right);   // each -1.0 .. +1.0
//   motorsStop();

#include "config.h"

static void driveOne(int pwmPin, int in1, int in2, float speed) {
  if (speed > 1.0f)  speed = 1.0f;
  if (speed < -1.0f) speed = -1.0f;

  // Skip the dead zone, then rescale so small commands still move the wheel
  float mag = fabs(speed);
  if (mag > 0.001f) {
    mag = MOTOR_DEADBAND + mag * (1.0f - MOTOR_DEADBAND);
  }

  digitalWrite(in1, speed > 0 ? HIGH : LOW);
  digitalWrite(in2, speed < 0 ? HIGH : LOW);
  analogWrite(pwmPin, (int)(mag * 255));
}

void motorsInit() {
  int pins[] = {L_PWM_PIN, L_IN1_PIN, L_IN2_PIN, R_PWM_PIN, R_IN1_PIN, R_IN2_PIN};
  for (int p : pins) pinMode(p, OUTPUT);
}

void setMotors(float left, float right) {
  driveOne(L_PWM_PIN, L_IN1_PIN, L_IN2_PIN, left  * LEFT_DIR);
  driveOne(R_PWM_PIN, R_IN1_PIN, R_IN2_PIN, right * RIGHT_DIR);
}

void motorsStop() {
  analogWrite(L_PWM_PIN, 0);
  analogWrite(R_PWM_PIN, 0);
  digitalWrite(L_IN1_PIN, LOW); digitalWrite(L_IN2_PIN, LOW);
  digitalWrite(R_IN1_PIN, LOW); digitalWrite(R_IN2_PIN, LOW);
}

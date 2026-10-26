#pragma once
// ===== pid.h =====
// A small PID controller.
//
//   P: push in proportion to the lean (error)
//   I: slowly add up leftover error to remove steady lean
//   D: push against how fast the robot is falling (uses the gyro directly,
//      which is smoother than differentiating the angle)

struct PID {
  float integral = 0;
  float outMin, outMax;

  PID(float lo, float hi) : outMin(lo), outMax(hi) {}

  void reset() { integral = 0; }

  // error: setpoint - measured angle (deg)
  // rate:  measured angular velocity (deg/s), positive = leaning more positive
  // dt:    seconds since last update
  float update(float error, float rate, float dt, float kp, float ki, float kd) {
    integral += error * dt;

    // Anti-windup: don't let the I term alone exceed the output range
    if (ki > 0) {
      float lim = outMax / ki;
      if (integral >  lim) integral =  lim;
      if (integral < -lim) integral = -lim;
    }

    // d(error)/dt = -rate when the setpoint is constant
    float out = kp * error + ki * integral - kd * rate;

    if (out > outMax) out = outMax;
    if (out < outMin) out = outMin;
    return out;
  }
};

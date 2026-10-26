#pragma once
// ===== config.h =====
// Edit THIS file first when adapting to your team's robot.
// Keep it in sync with docs/pin_map.md.

// ----- Motor driver pins -----
#define L_PWM_PIN 5
#define L_IN1_PIN 4
#define L_IN2_PIN 7
#define R_PWM_PIN 6
#define R_IN1_PIN 8
#define R_IN2_PIN 9

// Flip these to -1 if a wheel spins the wrong way (easier than rewiring)
#define LEFT_DIR   1
#define RIGHT_DIR  1

// ----- IMU -----
#define IMU_ADDR 0x68

// If the robot leans forward and the angle goes NEGATIVE, set this to -1
#define ANGLE_SIGN 1

// ----- Control loop -----
#define LOOP_HZ 200.0f
#define LOOP_US (1000000UL / 200UL)

// Complementary filter: fraction of the gyro to trust each step
#define FILTER_ALPHA 0.98f

// ----- PID gains (starting guesses, you WILL need to tune these) -----
// Output is a motor command in [-1, 1], input is degrees.
float KP = 0.05f;
float KI = 0.0f;
float KD = 0.002f;

// Balance point offset in degrees (center of mass is rarely exactly at 0)
float ANGLE_TRIM = 0.0f;

// ----- Motors -----
// Motors don't move at tiny PWM values. This is the smallest command (0-1)
// that makes the wheels actually turn. Find it with 02_motor_test.
#define MOTOR_DEADBAND 0.05f

// ----- Safety -----
#define TILT_CUTOFF_DEG   35.0f   // beyond this, motors turn off
#define START_WINDOW_DEG   3.0f   // robot must be within this of vertical to start
#define START_HOLD_MS    1000     // ...and held there this long

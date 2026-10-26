// Step 4: Self-balancing robot, main sketch.
//
// Ties together: imu.h (angle), pid.h (controller), motors.h (outputs).
// Settings live in config.h. Pin wiring lives in docs/pin_map.md.
//
// State machine:
//   CALIBRATING -> IDLE -> BALANCING -> FALLEN -> IDLE ...
//
// Live tuning over Serial (115200 baud, "Newline" line ending):
//   p 0.05   set Kp        i 0.1   set Ki        d 0.002   set Kd
//   t 1.5    set trim (deg)                      s         print settings

#include "config.h"
#include "imu.h"
#include "motors.h"
#include "pid.h"

enum State { CALIBRATING, IDLE, BALANCING, FALLEN };
State state = CALIBRATING;

PID pid(-1.0f, 1.0f);

unsigned long lastLoopUs = 0;
unsigned long nearVerticalSince = 0;
unsigned long lastPrintMs = 0;

const char* stateName(State s) {
  switch (s) {
    case CALIBRATING: return "CALIBRATING";
    case IDLE:        return "IDLE";
    case BALANCING:   return "BALANCING";
    default:          return "FALLEN";
  }
}

void printSettings() {
  Serial.print("Kp="); Serial.print(KP, 4);
  Serial.print(" Ki="); Serial.print(KI, 4);
  Serial.print(" Kd="); Serial.print(KD, 4);
  Serial.print(" trim="); Serial.println(ANGLE_TRIM, 2);
}

// Parses commands like "p 0.05"
void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  char cmd = line.charAt(0);
  float val = line.substring(1).toFloat();

  switch (cmd) {
    case 'p': KP = val; break;
    case 'i': KI = val; pid.reset(); break;
    case 'd': KD = val; break;
    case 't': ANGLE_TRIM = val; break;
    case 's': break;
    default:
      Serial.println("Unknown command. Use p, i, d, t, or s.");
      return;
  }
  printSettings();
}

void setup() {
  Serial.begin(115200);
  motorsInit();
  motorsStop();

  if (!imuInit()) {
    Serial.println("ERROR: IMU not found. Check wiring and docs/pin_map.md.");
    while (true) {                       // halt, motors stay off
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(150);
    }
  }

  Serial.println("Calibrating. Keep the robot still...");
  imuCalibrate();
  Serial.println("Done. Hold the robot upright to start.");
  printSettings();

  pinMode(LED_BUILTIN, OUTPUT);
  state = IDLE;
  lastLoopUs = micros();
}

void loop() {
  handleSerial();

  // Fixed-rate loop
  unsigned long nowUs = micros();
  if (nowUs - lastLoopUs < LOOP_US) return;
  float dt = (nowUs - lastLoopUs) / 1000000.0f;
  lastLoopUs = nowUs;

  // 1) Sense. If the sensor stops responding, stop the motors.
  if (!imuUpdate(dt)) {
    motorsStop();
    state = FALLEN;
    return;
  }

  float angle = imuAngle - ANGLE_TRIM;

  // 2) Think + act, depending on the state
  switch (state) {
    case IDLE:
      motorsStop();
      pid.reset();
      digitalWrite(LED_BUILTIN, LOW);
      // Start once the robot has been held near vertical for a moment
      if (fabs(angle) < START_WINDOW_DEG) {
        if (nearVerticalSince == 0) nearVerticalSince = millis();
        if (millis() - nearVerticalSince > START_HOLD_MS) {
          state = BALANCING;
          Serial.println("BALANCING");
        }
      } else {
        nearVerticalSince = 0;
      }
      break;

    case BALANCING: {
      digitalWrite(LED_BUILTIN, HIGH);

      if (fabs(angle) > TILT_CUTOFF_DEG) {
        motorsStop();
        state = FALLEN;
        Serial.println("FALLEN: tilt cutoff exceeded. Motors off.");
        break;
      }

      // Setpoint is 0 degrees (upright), so error = -angle
      float cmd = pid.update(-angle, imuRate, dt, KP, KI, KD);

      // Lean forward (+angle) -> wheels must drive forward to get under the robot.
      // If your robot drives the WRONG way and falls faster, flip LEFT_DIR/RIGHT_DIR
      // or ANGLE_SIGN in config.h.
      setMotors(-cmd, -cmd);
      break;
    }

    case FALLEN:
      motorsStop();
      pid.reset();
      // Stand it back up near vertical to re-arm
      if (fabs(angle) < START_WINDOW_DEG) {
        nearVerticalSince = 0;
        state = IDLE;
        Serial.println("IDLE: hold upright to start");
      }
      break;

    default:
      break;
  }

  // 3) Print a status line ~10 Hz (angle, command state) for plotting
  if (millis() - lastPrintMs > 100) {
    lastPrintMs = millis();
    Serial.print(stateName(state));
    Serial.print(',');
    Serial.println(angle, 2);
  }
}

// Step 3: Spin the motors.
//
// !!! WHEELS OFF THE GROUND (prop the robot up on a box) !!!
//
// Example hardware: dual H-bridge driver with PWM + two direction pins per motor.
// Update the pins to match docs/pin_map.md.
//
// Sequence (repeats): left forward, right forward, both backward, stop.
// If a wheel spins the wrong way, swap that motor's IN1/IN2 pins in code
// (or swap its two motor wires). Write down which fix you chose in pin_map.md.

// ----- Pins (match config.h) -----
const int L_PWM = 5, L_IN1 = 4, L_IN2 = 7;
const int R_PWM = 6, R_IN1 = 8, R_IN2 = 9;

void setMotor(int pwmPin, int in1, int in2, float speed) {
  // speed: -1.0 (full reverse) to +1.0 (full forward)
  speed = constrain(speed, -1.0, 1.0);
  digitalWrite(in1, speed > 0 ? HIGH : LOW);
  digitalWrite(in2, speed < 0 ? HIGH : LOW);
  analogWrite(pwmPin, (int)(fabs(speed) * 255));
}

void setup() {
  int pins[] = {L_PWM, L_IN1, L_IN2, R_PWM, R_IN1, R_IN2};
  for (int p : pins) pinMode(p, OUTPUT);
  Serial.begin(115200);
  Serial.println("Motor test starting in 3 seconds. Wheels off the ground!");
  delay(3000);
}

void loop() {
  Serial.println("Left forward");
  setMotor(L_PWM, L_IN1, L_IN2, 0.5);
  setMotor(R_PWM, R_IN1, R_IN2, 0.0);
  delay(1500);

  Serial.println("Right forward");
  setMotor(L_PWM, L_IN1, L_IN2, 0.0);
  setMotor(R_PWM, R_IN1, R_IN2, 0.5);
  delay(1500);

  Serial.println("Both backward");
  setMotor(L_PWM, L_IN1, L_IN2, -0.5);
  setMotor(R_PWM, R_IN1, R_IN2, -0.5);
  delay(1500);

  Serial.println("Stop");
  setMotor(L_PWM, L_IN1, L_IN2, 0.0);
  setMotor(R_PWM, R_IN1, R_IN2, 0.0);
  delay(2000);
}

# Control Loop Overview

## Block diagram

```
                 +-------------------------------------------------+
                 |                  every 5 ms (200 Hz)             |
                 |                                                 |
  IMU  ──────►  Angle estimate  ──►  error = target - angle  ──►  PID  ──►  Motor command  ──►  Wheels
 (accel+gyro)   (complementary        (target = 0 deg + trim)     (Kp,Ki,Kd)   (-1 to 1)
                  filter)                                           ▲
                    │                                               │
                    └──── gyro rate (deg/s) feeds the D term ───────┘
                 |                                                 |
                 +----────────────  Safety check: tilt > cutoff? ──► motors OFF
```

## Why each piece exists

| Block | Purpose |
|-------|---------|
| Angle estimate | The accelerometer alone is noisy when the robot shakes. The gyro alone drifts over time. Blending them gives a clean, stable angle |
| PID | **P** pushes proportional to the lean. **D** pushes against how fast it's falling (damping). **I** removes slow steady-state lean |
| Motor command | A normalized value from -1 to 1, so the controller doesn't need to know about PWM or driver pins |
| Safety cutoff | Stops motors when the robot has clearly fallen, so it doesn't drive itself across the room |

## State machine

```
CALIBRATING ──► IDLE ──► BALANCING ──► FALLEN
                 ▲                        │
                 └────────────────────────┘
          (robot is stood back up near vertical)
```

- **CALIBRATING:** keep the robot still. The code measures the gyro's resting bias.
- **IDLE:** motors off, waiting for the robot to be held near vertical.
- **BALANCING:** the PID loop is running.
- **FALLEN:** tilt exceeded the cutoff. Motors off until the robot is stood back up.

## Ideas for later (stretch goals)

- **Encoders + outer velocity loop:** stops the robot from slowly drifting across the floor.
- **Remote control:** add a steering input that turns the left and right motors differently.
- **Kalman filter:** replace the complementary filter once the basics work.
- **Data logging:** print angle, command, and gains over Serial and plot them in Python to tune scientifically.
- **LQR control:** a model-based alternative to PID, if your team wants a challenge.

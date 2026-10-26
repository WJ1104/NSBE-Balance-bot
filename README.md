# NSBE Self-Balancing Robot: Example Software Repo

An example repo for NSBE Design Team software members. **Copy this structure, then swap in your own team's parts.** Every team's hardware is different, so the pins, IMU, and motor driver here are placeholders (MPU6050 IMU + a dual H-bridge motor driver on an Arduino-style board).


## What's in this repo

```
nsbe-balancebot-example/
├── README.md                  <- you are here
├── CONTRIBUTING.md            <- how our team shares code with Git
├── docs/
│   ├── pin_map.md             <- which pin connects to what (keep this updated!)
│   └── control_loop.md        <- block diagram + how balancing works
├── firmware/
│   ├── 00_blink/              <- Step 1: is my toolchain working?
│   ├── 01_imu_angle/          <- Step 2: read the tilt sensor
│   ├── 02_motor_test/         <- Step 3: spin the motors (wheels off the ground!)
│   └── 03_balance/            <- Step 4: the full balancing sketch
│       ├── 03_balance.ino     <- main loop + state machine
│       ├── config.h           <- pins, gains, limits (edit here first)
│       ├── imu.h              <- sensor reading + angle filter
│       ├── motors.h           <- motor driver interface
│       └── pid.h              <- PID controller
└── simulation/
    └── pendulum_sim.py        <- try PID gains on a virtual robot (no hardware needed)
```

## Step-by-step plan

| Step | Goal | How you know it worked |
|------|------|------------------------|
| 1 | Install the Arduino IDE (or PlatformIO), upload `00_blink` | The LED blinks |
| 2 | Wire the IMU, upload `01_imu_angle` | Open Serial Monitor (115200 baud). Tilt the board and the angle changes smoothly |
| 3 | Wire the motor driver, upload `02_motor_test` **with wheels off the ground** | Wheels spin forward, stop, spin backward |
| 4 | Fill in `config.h`, upload `03_balance` | Robot stays up (this takes patience!) |
| 5 | Run `simulation/pendulum_sim.py` | You see how Kp, Ki, Kd change the response |

Don't skip to Step 4. If the angle reading is wrong, no amount of PID tuning will fix it.

## How balancing works (the short version)

1. **Measure** how far the robot is leaning (IMU: accelerometer + gyroscope).
2. **Compare** it to the target (0 degrees = straight up).
3. **Push the wheels** in the direction of the lean, harder when the lean is bigger or faster. That is the PID controller.
4. Repeat about 200 times per second.

The two sensors cover each other's weaknesses. The accelerometer is noisy but doesn't drift. The gyro is smooth but drifts. A **complementary filter** blends them (see `imu.h`).

## Tuning PID (the order matters)

1. Set `Ki = 0` and `Kd = 0`. Raise **Kp** until the robot tries to balance but oscillates back and forth.
2. Raise **Kd** to damp the wobble. Stop if you hear motor buzzing or jitter.
3. Back Kp off a bit if it still shakes.
4. Add a little **Ki** only if the robot slowly leans and drifts to one side.

You can change gains live over Serial without re-uploading. See "Serial commands" below.

## Serial commands (in `03_balance`)

Open the Serial Monitor at 115200 baud, line ending set to "Newline".

| Send | Does |
|------|------|
| `p 18.5` | Set Kp to 18.5 |
| `i 0.5` | Set Ki |
| `d 0.9` | Set Kd |
| `t 1.2` | Set angle trim (degrees), if your balance point isn't exactly 0 |
| `s` | Print current gains |

## Safety checklist

- Test motors with the **wheels off the ground** first.
- The code cuts power if the robot tilts more than `TILT_CUTOFF_DEG` (default 35).
- Keep a hand near the power switch for first balance attempts.
- Never power the motors from the microcontroller's 5V pin. Use the battery through the motor driver.

## Making it yours

- Change the pins in `config.h` and update `docs/pin_map.md`.
- Different IMU? Only `imu.h` should need to change.
- Different motor driver? Only `motors.h` should need to change.
- ESP32? Works with a recent ESP32 Arduino core. You may need to adjust the PWM pins and I2C pins.

## Status

Written as a starting template. It has **not been tuned on a real robot**, so expect to adjust the gains, the IMU axis, and the motor directions for your build.

## Resources

- Search "Arduino self balancing robot PID" for walkthroughs
- Search "complementary filter IMU" for the angle math
- Search "inverted pendulum control" for the theory

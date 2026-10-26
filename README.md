# Self-Balancing Robot: example software repo

This is an example repo for the NSBE design team software people. Copy it, rename it, and change stuff to match your own robot. Your team probably picked different parts than I did here (I used an MPU6050 and a basic dual H-bridge motor driver on an Arduino Nano), so don't treat the pins or the gains as correct for your build. They're just there so you can see how it all fits together.

If you've never done this before, that's fine. Go in order. Each step has something you can check before moving on.

## What's in here

- `firmware/` has the Arduino code, one folder per step
- `docs/` has the pin map and a rough picture of the control loop
- `simulation/` has a Python script so you can play with PID gains without a robot
- `CONTRIBUTING.md` is how we use Git so we don't overwrite each other

## Order to do things in

1. **`00_blink`**: Install the Arduino IDE and upload this. If the LED blinks, your setup works.
2. **`01_imu_angle`**: Wire up the IMU and open the Serial Monitor at 115200. Tilt the board by hand. The angle should move smoothly when you do. If it goes the wrong way or the wrong axis moves, that's an axis/sign thing in the code, not a broken sensor.
3. **`02_motor_test`**: Wheels OFF the ground. Prop it up on a box. The wheels should go forward, stop, then go backward. If one spins the wrong way, flip its two pins in the code.
4. **`03_balance`**: The real thing. Edit `config.h` first (pins, motor direction, gains), then upload.
5. **`simulation/pendulum_sim.py`**: Run it any time. It's good for getting a feel for what Kp and Kd actually do.

Please don't jump straight to step 4. If the angle reading is off, tuning won't fix it and you'll waste an afternoon.

## How it works

The robot is basically an upside-down pendulum. It reads how far it's leaning, then drives the wheels toward the lean so they get back underneath it. It does that around 200 times a second.

For the angle we use the accelerometer and the gyro together. The accelerometer is jumpy but doesn't drift, and the gyro is smooth but drifts over time. A complementary filter mixes the two (it's a few lines in `imu.h`). `docs/control_loop.md` has a diagram if you want it.

## Tuning

Do these in order, one at a time:

1. Start with Ki and Kd at 0. Turn Kp up until it tries to balance but wobbles back and forth.
2. Add Kd to calm the wobble. If the motors start buzzing, you went too high.
3. Drop Kp a little if it's still shaky.
4. Only add Ki if it slowly leans to one side and creeps away.

You don't have to re-upload to change the numbers. Open the Serial Monitor (115200, line ending set to Newline) and type things like:

````
p 0.08
d 0.003
t 1.5
s
````

`p`, `i` and `d` set the gains, `t` sets a trim angle in degrees (your center of mass probably isn't at exactly zero), and `s` prints what it's currently using.

## Safety stuff

- Wheels off the ground for the first tests
- The code shuts the motors off if it tilts past 35 degrees (`TILT_CUTOFF_DEG` in `config.h`)
- Keep a hand near the power switch the first few times
- Don't power the motors from the board's 5V pin. They run off the battery through the driver

## Swapping in your own parts

Different IMU: you should only have to change `imu.h`.
Different motor driver: only `motors.h`.
Different pins: `config.h`, and then update `docs/pin_map.md` so electrical and software agree.
ESP32: should work with a recent Arduino core, but you'll need to check the PWM and I2C pins.

## Heads up

I haven't tuned this on a real robot, so the starting gains are guesses. Expect to change them, and expect to flip a sign or two before it behaves. That's normal.

## Stuff to look up

- "Arduino self balancing robot PID" for build videos and writeups
- "complementary filter IMU" if you want to understand the angle math
- "inverted pendulum control" for the theory behind all of this
````
````

# Pin Map

**Keep this in sync with the real wiring.** Electrical and software should both treat this file as the source of truth. The values below match `firmware/03_balance/config.h`, and are examples for an Arduino Nano-style board.

## IMU (MPU6050, I2C)

| IMU pin | Board pin | Notes |
|---------|-----------|-------|
| VCC | 3.3V or 5V | Check your breakout board's regulator |
| GND | GND | |
| SDA | A4 | Hardware I2C |
| SCL | A5 | Hardware I2C |
| AD0 | GND | Sets address 0x68 |

## Motor driver (dual H-bridge, direction pins + PWM)

| Signal | Board pin | Notes |
|--------|-----------|-------|
| Left PWM | D5 | Must be a PWM-capable pin |
| Left IN1 | D4 | Direction |
| Left IN2 | D7 | Direction |
| Right PWM | D6 | Must be a PWM-capable pin |
| Right IN1 | D8 | Direction |
| Right IN2 | D9 | Direction |
| Standby/Enable | 5V (or a pin) | Driver dependent |

## Power

| Item | Notes |
|------|-------|
| Battery | Voltage: ____ V. Goes to the motor driver's motor supply input |
| Board supply | Via regulator or USB. Share a common GND with the motor driver |
| Power switch | Between battery and everything else |

## Status LED

| Signal | Board pin |
|--------|-----------|
| LED | D13 (built-in) |

## Change log

| Date | Who | What changed |
|------|-----|--------------|
| | | |

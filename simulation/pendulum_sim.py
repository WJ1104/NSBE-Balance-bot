"""
Virtual self-balancing robot (inverted pendulum on a cart).

No hardware needed. Use it to build intuition for what Kp, Ki, and Kd do
BEFORE you touch the real robot.

Run:
    pip install numpy matplotlib
    python pendulum_sim.py                      # default gains
    python pendulum_sim.py --kp 40 --kd 3       # try your own
    python pendulum_sim.py --no-plot            # just print a summary

The model is simplified (point mass on a rod, wheel acceleration as the
direct control input, no motor lag or friction). The gains here are in
simulation units and will NOT transfer directly to your robot. The point is
the *behavior*: too little Kp falls over, too much Kd amplifies noise, and
so on.
"""

import argparse
import math

import numpy as np

G = 9.81  # m/s^2


def simulate(kp, ki, kd, length=0.15, theta0_deg=5.0, duration=5.0,
             dt=0.005, noise_deg=0.3, max_accel=15.0, seed=0):
    """Simulate the robot. Returns time, angle (deg), accel command arrays."""
    rng = np.random.default_rng(seed)
    n = int(duration / dt)

    theta = math.radians(theta0_deg)   # lean angle (rad), 0 = upright
    omega = 0.0                        # angular velocity (rad/s)
    integral = 0.0

    t_hist = np.zeros(n)
    th_hist = np.zeros(n)
    u_hist = np.zeros(n)

    for k in range(n):
        # Sensor with noise (like a real IMU)
        measured = theta + math.radians(rng.normal(0, noise_deg))
        measured_rate = omega + math.radians(rng.normal(0, noise_deg))

        # PID on angle error, D term uses angular rate (like the firmware)
        error = 0.0 - measured
        integral += error * dt
        u = kp * error + ki * integral - kd * measured_rate

        # Wheel acceleration is limited by the motors
        u = max(-max_accel, min(max_accel, u))

        # Physics: lean grows with gravity, wheel acceleration pushes it back.
        # Positive theta = leaning forward; accelerating forward (u > 0)
        # moves the base under the robot.
        # In this sign convention the PID output is applied as -u.
        accel = -u
        alpha = (G / length) * math.sin(theta) - (accel / length) * math.cos(theta)

        omega += alpha * dt
        theta += omega * dt

        t_hist[k] = k * dt
        th_hist[k] = math.degrees(theta)
        u_hist[k] = u

        if abs(theta) > math.radians(60):   # it fell
            return t_hist[:k + 1], th_hist[:k + 1], u_hist[:k + 1]

    return t_hist, th_hist, u_hist


def summarize(t, th):
    fell = np.abs(th).max() > 59   # simulate() stops once the lean passes 60 deg
    settle_band = 1.0  # degrees
    settled_at = None
    for i in range(len(th)):
        if np.all(np.abs(th[i:]) < settle_band):
            settled_at = t[i]
            break
    print(f"Peak lean: {np.abs(th).max():.1f} deg")
    if fell:
        print(f"Result: FELL OVER at t={t[-1]:.2f}s. Try adjusting the gains.")
    elif settled_at is None:
        print("Result: stayed up but never settled within 1 degree (oscillating?).")
    else:
        print(f"Result: balanced. Settled within 1 degree at t={settled_at:.2f}s.")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--kp", type=float, default=60.0)
    ap.add_argument("--ki", type=float, default=0.0)
    ap.add_argument("--kd", type=float, default=4.0)
    ap.add_argument("--length", type=float, default=0.15,
                    help="distance from wheel axle to center of mass (m)")
    ap.add_argument("--theta0", type=float, default=5.0, help="starting lean (deg)")
    ap.add_argument("--noise", type=float, default=0.3, help="sensor noise (deg)")
    ap.add_argument("--no-plot", action="store_true")
    ap.add_argument("--save", help="save the plot to this PNG file instead of showing it")
    args = ap.parse_args()

    t, th, u = simulate(args.kp, args.ki, args.kd, length=args.length,
                        theta0_deg=args.theta0, noise_deg=args.noise)
    summarize(t, th)

    if args.no_plot:
        return

    import matplotlib
    if args.save:
        matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(8, 5))
    ax1.plot(t, th)
    ax1.axhline(0, color="gray", lw=0.8)
    ax1.set_ylabel("Lean angle (deg)")
    ax1.set_title(f"Kp={args.kp}  Ki={args.ki}  Kd={args.kd}")
    ax2.plot(t, u, color="tab:orange")
    ax2.set_ylabel("Wheel command (sim units)")
    ax2.set_xlabel("Time (s)")
    fig.tight_layout()

    if args.save:
        fig.savefig(args.save, dpi=120)
        print(f"Saved plot to {args.save}")
    else:
        plt.show()


if __name__ == "__main__":
    main()

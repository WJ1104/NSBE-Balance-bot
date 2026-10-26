# Working Together With Git

## First-time setup

```
git clone <your-team-repo-url>
cd <repo-folder>
```

## Everyday workflow

1. **Get the latest code:** `git pull`
2. **Make a branch for what you're working on:** `git checkout -b imu-filter`
3. **Make small changes and test them** on the bench.
4. **Save a snapshot:** `git add .` then `git commit -m "Add complementary filter to imu.h"`
5. **Share it:** `git push -u origin imu-filter`, then open a Pull Request on GitHub.
6. A teammate looks it over, then merge.

## Rules of thumb

- **Commit messages say what changed**, not "stuff" or "fix".
- **Never commit directly to `main`** once the team has a working version.
- **Update `docs/pin_map.md`** any time wiring changes. Software and electrical must match.
- **Don't change gains and wiring in the same commit.** If something breaks, you want to know which one did it.
- **Test before you push.** If the robot balanced before, it should still balance after.
- **Tag good versions:** `git tag balanced-v1` so you can always go back.

## Who does what (fill this in)

| Area | Owner |
|------|-------|
| IMU / angle estimation | |
| PID / control | |
| Motors / encoders | |
| Simulation / tuning tools | |
| Docs / pin map | |

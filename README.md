
# OBC ADCS Mini-App — cFS + 42 Integration (Reference Solution)

> **Note for candidate**: This repository contains *code and engineering notes* only.  
> Per the challenge rules, the final consolidated report must be written manually by you (no AI).  
> Use `docs/REPORT_NOTES.md` as raw notes to rewrite in your own words.

## What this contains
- `adcs_app/`: a small, clean ADCS app (C++17) that can run **standalone** or be wrapped into cFS.
  - Robust quaternion math, rate estimator, PID attitude hold loop.
  - UDP I/O to talk to a simulator (42) and a ground telemetry listener.
  - Fault-injection hooks (via UDP command channel).
- `telemetry/telemetry_logger.py`: UDP listener that logs JSON telemetry to CSV and prints key values.
- `scripts/`: helper scripts to run the Docker container, launch the app, inject faults, etc.
- `docs/REPORT_NOTES.md`: bullet-point notes to help you manually author the final report.

## Quick start (standalone demo)
```bash
# Build (requires g++ and make inside the challenge container or any Linux with g++17)
make -C adcs_app

# Run the app (standalone) — sends telemetry on UDP/5005 and listens for faults on UDP/5006
./adcs_app/bin/adcs_app

# In another shell, start telemetry logger (records CSV under ./telemetry/out)
python3 telemetry/telemetry_logger.py

# Inject a fault after a few seconds (bias gyro by 0.03 rad/s, saturate wheel torque 0.02 Nm)
python3 scripts/inject_fault.py --gyro-bias 0.03 --torque-sat 0.02
```

## cFS integration (outline)
- Place `adcs_app/` under your cFS apps folder (e.g., `cfs/apps/adcs_app/`).
- Add the app to your mission's `CMakeLists.txt` (see `adcs_app/CMakeLists.txt` for target).
- Provide a cFE App Main symbol `ADCS_AppMain` (already included), and register SB pipes, TLM, CMD.
- Telemetry is also mirrored over UDP/5005 for the provided Python logger.

See `docs/REPORT_NOTES.md` for detailed steps to copy into your manual report.

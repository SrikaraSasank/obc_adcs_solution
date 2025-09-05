# OBC + ADCS Subsystem Challenge 🚀

Solution for the **On-Board Computer (OBC) & Attitude Determination and Control System (ADCS)** challenge.

---

## 📂 Repository Contents
- `adcs_app/` → C++ ADCS application (build inside Docker)
- `telemetry/` → Telemetry logger (Python)
- `scripts/` → Fault injection scripts (Python)
- `report.pdf` → Final challenge report
- `tlm_*.csv` → Telemetry logs
- `plot_results.py` → Plot graphs from telemetry CSV
- `README.md` → This file

---

## ⚙️ How to Run

### 1️⃣ ADCS App (inside Docker)
```bash
cd /work/adcs_app
make
./bin/adcs_app

import socket, json, csv, time, sys
from datetime import datetime
from pathlib import Path

PORT = 5005
OUTDIR = Path(__file__).parent / "out"
OUTDIR.mkdir(parents=True, exist_ok=True)
csv_path = OUTDIR / f"tlm_{int(time.time())}.csv"

print(f"[telemetry] Listening on UDP :{PORT} -> {csv_path}")
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", PORT))
sock.settimeout(1.0)

with open(csv_path, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["ts","step","q_ref_w","q_ref_x","q_ref_y","q_ref_z",
                "q_body_w","q_body_x","q_body_y","q_body_z",
                "w_x","w_y","w_z","bias_x","bias_y","bias_z",
                "u_x","u_y","u_z","saturate","torque_sat"])
    while True:
        try:
            data,addr = sock.recvfrom(4096)
            j = json.loads(data.decode("utf-8"))
            row = [datetime.utcnow().isoformat(), j.get("step")]
            row += j.get("q_ref", [0,0,0,0])
            row += j.get("q_body", [0,0,0,0])
            row += j.get("w_body", [0,0,0])
            row += j.get("gyro_bias", [0,0,0])
            row += j.get("torque_cmd", [0,0,0])
            row += [j.get("saturate"), j.get("torque_sat")]
            w.writerow(row); f.flush()
            if j.get("step",0) % 25 == 0:
                print(f"step={j.get('step')}, w={j.get('w_body')}, u={j.get('torque_cmd')} sat={j.get('saturate')}")
        except socket.timeout:
            pass
        except KeyboardInterrupt:
            print("\n[telemetry] Stopped.")
            sys.exit(0)

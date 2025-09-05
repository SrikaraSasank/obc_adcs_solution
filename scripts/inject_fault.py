import argparse, socket

parser = argparse.ArgumentParser()
parser.add_argument("--host", default="127.0.0.1")
parser.add_argument("--port", type=int, default=5006)
parser.add_argument("--gyro-bias", type=float, default=None, help="bias for all axes (rad/s)")
parser.add_argument("--bias", nargs=3, type=float, default=None, help="gx gy gz (rad/s)")
parser.add_argument("--torque-sat", type=float, default=None, help="set torque saturation value (Nm) and enable saturation")
parser.add_argument("--sat", type=int, choices=[0,1], default=None, help="enable(1)/disable(0) saturation")

args = parser.parse_args()
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send(msg):
    sock.sendto(msg.encode("utf-8"), (args.host, args.port))
    print("[fault] sent:", msg)

if args.gyro_bias is not None:
    send(f"BIAS {args.gyro_bias} {args.gyro_bias} {args.gyro_bias}")
if args.bias is not None:
    gx,gy,gz = args.bias
    send(f"BIAS {gx} {gy} {gz}")
if args.torque_sat is not None:
    send("SAT 1")
    send(f"SATVAL {args.torque_sat}")
if args.sat is not None:
    send(f"SAT {args.sat}")

#!/usr/bin/env python3
"""Compare complete clean byte streams with the installed sigrok UART decoder."""
import argparse
import json
from pathlib import Path
import subprocess
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from dmt99.session import sha256, write_json


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sessions", nargs="+", type=Path)
    args = parser.parse_args()
    reports = []
    success = True
    for session in args.sessions:
        data = json.loads((session / "decoded.json").read_text())
        if sha256(session / "logic.bin") != data["logic_sha256"]:
            raise ValueError("Source logic differs from decoded metadata")
        checks = []
        for direction, channel in data["channel_map"].items():
            records = [r for r in data["bytes"] if r["direction"] == direction]
            clean = all(not r["errors"] and r["value"] is not None for r in records)
            command = ["sigrok-cli", "-i", str(session / "logic.bin"), "-I",
                       f"binary:numchannels=8:samplerate={data['samplerate_hz']}",
                       "-P", f"uart:rx={channel}:baudrate=9600:data_bits=8:parity=none:stop_bits=1", "-B", "uart=rx"]
            reference = subprocess.run(command, capture_output=True)
            (session / f"sigrok_reference_{direction}.bin").write_bytes(reference.stdout)
            (session / f"sigrok_reference_{direction}.log").write_bytes(reference.stderr)
            expected = bytes(r["value"] for r in records) if clean else None
            match = expected == reference.stdout if clean and reference.returncode == 0 else None
            success = success and match is True
            checks.append({"direction": direction, "channel": channel, "command": command,
                           "returncode": reference.returncode, "python_byte_candidates": len(records),
                           "reference_bytes": len(reference.stdout), "python_all_clean": clean,
                           "exact_match": match,
                           "reason": "Clean stream comparison" if clean else "Not compared: UART error/partial candidates retained for separate review"})
        report = {"session": str(session.resolve()), "origin": data["origin"],
                  "source_logic_sha256": data["logic_sha256"], "checks": checks,
                  "note": "Decoder agreement is not analog calibration or checksum validation"}
        write_json(session / "decoder_validation.json", report)
        reports.append(report)
    print(json.dumps(reports, indent=2))
    return 0 if success else 2


if __name__ == "__main__": raise SystemExit(main())

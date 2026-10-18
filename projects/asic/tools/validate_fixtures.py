#!/usr/bin/env python3
"""Compare preserved hardware sample excerpts with the independent sigrok UART decoder."""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from dmt99.uart import decode


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--fixtures", type=Path, default=Path("tests/fixtures"))
    parser.add_argument("--output", type=Path, default=Path("evidence/all_fixture_decoder_validation.json"))
    args = parser.parse_args()
    paths = sorted(args.fixtures.glob("real_*.json"))
    if not paths:
        raise ValueError("No hardware fixtures found")
    reports = []
    with tempfile.TemporaryDirectory(prefix="dmt99-real-fixtures-") as temporary:
        source = Path(temporary) / "logic.bin"
        for path in paths:
            info = json.loads(path.read_text())
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            digest = hashlib.sha256(raw).hexdigest()
            if digest != info["raw_excerpt_sha256"]:
                raise ValueError(f"Fixture hash mismatch: {path}")
            source.write_bytes(raw)
            for direction, channel in info["channel_map"].items():
                records, _ = decode(raw, info["samplerate_hz"], channel, direction=direction)
                clean = all(not r["errors"] and r["value"] is not None for r in records)
                command = ["sigrok-cli", "-i", str(source), "-I",
                           f"binary:numchannels=8:samplerate={info['samplerate_hz']}",
                           "-P", f"uart:rx={channel}:baudrate=9600:data_bits=8:parity=none:stop_bits=1",
                           "-B", "uart=rx"]
                reference = subprocess.run(command, capture_output=True)
                match = (clean and reference.returncode == 0 and
                         reference.stdout == bytes(r["value"] for r in records))
                reports.append({"fixture": path.name, "direction": direction,
                                "status": "exact_match" if match else "not_matched",
                                "bytes": len(records), "raw_sha256": digest,
                                "python_all_clean": clean, "sigrok_returncode": reference.returncode,
                                "sigrok_stderr": reference.stderr.decode(errors="replace")})
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(reports, indent=2) + "\n")
    passed = sum(r["status"] == "exact_match" for r in reports)
    print(f"{passed}/{len(reports)} independent UART comparisons matched; {args.output}")
    return 0 if passed == len(reports) else 1


if __name__ == "__main__":
    raise SystemExit(main())

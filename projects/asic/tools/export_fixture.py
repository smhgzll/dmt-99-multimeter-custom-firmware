#!/usr/bin/env python3
"""Export an exact real-capture sample window with full lineage and expectations."""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from dmt99.protocol import packets
from dmt99.session import sha256, write_json
from dmt99.uart import decode


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("session", type=Path)
    parser.add_argument("output_prefix", type=Path)
    parser.add_argument("--start-sample", type=int, required=True)
    parser.add_argument("--end-sample", type=int, required=True)
    args = parser.parse_args()
    decoded = json.loads((args.session / "decoded.json").read_text())
    if decoded["origin"] != "hardware": raise ValueError("Requires hardware-origin capture")
    if sha256(args.session / "raw.sr") != decoded["raw_sha256"]: raise ValueError("Source .sr changed")
    if sha256(args.session / "logic.bin") != decoded["logic_sha256"]: raise ValueError("Source logic changed")
    if not 0 <= args.start_sample < args.end_sample <= decoded["sample_count"]:
        raise ValueError("Sample window outside source capture")
    gzip_path = Path(str(args.output_prefix) + ".logic.bin.gz")
    json_path = Path(str(args.output_prefix) + ".json")
    if gzip_path.exists() or json_path.exists(): raise ValueError("Refusing to overwrite a fixture")
    with (args.session / "logic.bin").open("rb") as source:
        source.seek(args.start_sample)
        raw = source.read(args.end_sample - args.start_sample)
    info = {"origin": "hardware_excerpt", "source_capture": str(args.session.resolve()),
            "source_capture_status": decoded["metadata"].get("status"),
            "source_raw_sr_sha256": decoded["raw_sha256"], "source_logic_sha256": decoded["logic_sha256"],
            "source_metadata": decoded["metadata"],
            "start_sample": args.start_sample, "end_sample_exclusive": args.end_sample,
            "samplerate_hz": decoded["samplerate_hz"], "channel_map": decoded["channel_map"],
            "raw_excerpt_sha256": hashlib.sha256(raw).hexdigest(), "expected": {},
            "expected_packets": [], "rx_checksum_verified": False,
            "cut_policy": "exact contiguous samples; boundary truncation and initial-low errors retained"}
    for direction, channel in info["channel_map"].items():
        records, _ = decode(raw, info["samplerate_hz"], channel, direction=direction)
        info["expected"][direction] = [{k:r[k] for k in ("value", "start_sample", "errors")} for r in records]
        for packet in packets(records, info["samplerate_hz"], rx_bytes=decoded["metadata"].get("rx_frame_bytes",13)):
            info["expected_packets"].append({k:packet[k] for k in ("direction", "hex", "kind", "uart_errors")})
    gzip_path.parent.mkdir(parents=True, exist_ok=True)
    gzip_path.write_bytes(gzip.compress(raw, mtime=0))
    write_json(json_path, info)
    print(json_path)


if __name__ == "__main__": main()

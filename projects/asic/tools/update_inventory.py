#!/usr/bin/env python3
"""Rebuild capture indexes without changing recordings or their analysis."""
import csv
from datetime import datetime
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from dmt99.session import sha256


def main():
    rows = []
    for path in sorted((ROOT / "captures").glob("*/metadata.json")):
        session = path.parent
        metadata = json.loads(path.read_text())
        data = json.loads((session / "decoded.json").read_text())
        analysis = json.loads((session / "analysis.json").read_text())
        if sha256(session / "raw.sr") != data["raw_sha256"]:
            raise ValueError(f"Raw archive hash mismatch: {session}")
        rows.append({"capture": session.name, "test": metadata["test"],
                     "mode": metadata.get("mode"), "status": metadata["status"],
                     "duration_s": data["duration_s"], "samplerate_hz": data["samplerate_hz"],
                     "uart_candidates": analysis["total_uart_candidates"],
                     "uart_error_candidates": analysis["uart_error_candidates"],
                     "complete_packet_candidates": sum(p["complete"] for p in data["packets"]),
                     "display_user_report": metadata.get("display_text"),
                     "component_report": metadata.get("component_actual"),
                     "raw_sha256": data["raw_sha256"]})
    if not rows:
        raise ValueError("No captures found")
    (ROOT / "evidence/capture_inventory.json").write_text(json.dumps(rows, ensure_ascii=False, indent=2) + "\n")
    with (ROOT / "evidence/capture_inventory.csv").open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=rows[0])
        writer.writeheader()
        writer.writerows(rows)
    total = sum(r["uart_candidates"] for r in rows)
    lines = ["# Physical capture index", "",
             f"Generated: {datetime.now().astimezone().isoformat()}. {len(rows)} sessions; {total} UART candidates.", "",
             "| Session | Test | Duration (s) | Status | UART error candidates |",
             "|---|---|---:|---|---:|"]
    for row in rows:
        name = row["capture"]
        base = f"../captures/{name}"
        lines.append(f"| [{name}]({base}/metadata.json) · [raw]({base}/raw.sr) · [analysis]({base}/analysis.json) | "
                     f"{row['test']} | {row['duration_s']:.6f} | {row['status']} | {row['uart_error_candidates']} |")
    lines += ["", "`captured` means the requested sample count was acquired; it does not imply",
              "every UART candidate is error-free. `partial_or_failed` means missing samples or capture failure.",
              "Every original `.sr` SHA-256 was checked when generating this index.",
              "Earlier analysis revisions remain in each session's `revisions/` directory.", ""]
    (ROOT / "docs/CAPTURE_INDEX.md").write_text("\n".join(lines))
    print(f"{len(rows)} captures; {total} UART candidates; all raw archive hashes matched")


if __name__ == "__main__":
    main()

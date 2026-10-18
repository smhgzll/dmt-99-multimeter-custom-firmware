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
    lines = ["# Gerçek kayıt indeksi", "",
             f"Oluşturuldu: {datetime.now().astimezone().isoformat()}. {len(rows)} oturum; {total} UART adayı.", "",
             "| Oturum | Test | Süre (s) | Durum | UART hata adayı |",
             "|---|---|---:|---|---:|"]
    for row in rows:
        name = row["capture"]
        base = f"../captures/{name}"
        lines.append(f"| [{name}]({base}/metadata.json) · [ham]({base}/raw.sr) · [analiz]({base}/analysis.json) | "
                     f"{row['test']} | {row['duration_s']:.6f} | {row['status']} | {row['uart_error_candidates']} |")
    lines += ["", "`captured` hedef sample sayısının alındığını belirtir; tüm UART adaylarının",
              "hatasız olduğunu söylemez. `partial_or_failed` sample eksikliği veya kayıt hatasıdır.",
              "Her ham `.sr` SHA-256 değeri indeks oluşturulurken kontrol edildi.",
              "Eski analiz revizyonları ilgili oturumun `revisions/` dizinindedir.", ""]
    (ROOT / "docs/CAPTURE_INDEX.md").write_text("\n".join(lines))
    print(f"{len(rows)} captures; {total} UART candidates; all raw archive hashes matched")


if __name__ == "__main__":
    main()

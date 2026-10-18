"""Sigrok session extraction, lossless derived exports and reproducibility."""
import configparser
import csv
import hashlib
import json
import mmap
import re
import zipfile
from pathlib import Path

from .protocol import packets
from .uart import decode


def write_json(path, value):
    Path(path).write_text(json.dumps(value, indent=2, ensure_ascii=False) + "\n")


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as source:
        for block in iter(lambda: source.read(1 << 20), b""):
            h.update(block)
    return h.hexdigest()


def rate_hz(value):
    match = re.fullmatch(r"\s*([0-9.]+)\s*([kMG]?)\s*Hz\s*", value)
    if not match:
        raise ValueError(f"Unsupported samplerate metadata: {value!r}")
    return int(float(match[1]) * {"": 1, "k": 1000, "M": 1000000, "G": 1000000000}[match[2]])


def extract(srfile, directory, channel_map=None, allow_unlisted=False):
    """Only read known stream entries; never trust arbitrary ZIP paths."""
    directory = Path(directory)
    config = configparser.ConfigParser()
    with zipfile.ZipFile(srfile) as archive:
        config.read_string(archive.read("metadata").decode())
        devices = [s for s in config.sections() if s.startswith("device ")]
        if devices != ["device 1"]:
            raise ValueError("Expected exactly one logic device; original .sr retained")
        dev = config["device 1"]
        if int(dev.get("unitsize", "0")) != 1:
            raise ValueError("Expected one byte/sample; original .sr retained")
        # Hardware bit positions remain explicit, including a documented recovery
        # from a channel sampled by FX2 but omitted from the original CLI selection.
        channel_map = channel_map or {"MCU_TX": 0, "ASIC_TX": 1}
        if set(channel_map) != {"MCU_TX", "ASIC_TX"} or len(set(channel_map.values())) != 2:
            raise ValueError("Need two distinct MCU_TX/ASIC_TX channel bits")
        unlisted = []
        for channel in channel_map.values():
            if channel not in range(8): raise ValueError("Channel bit must be 0..7")
            name = dev.get(f"probe{channel+1}")
            if name != f"D{channel}":
                if not (name is None and allow_unlisted and int(dev.get("total probes", "0")) > channel):
                    raise ValueError(f"Expected probe{channel+1}=D{channel}; no implicit remapping")
                unlisted.append(f"D{channel}")
        stream = dev.get("capturefile", "logic-1")
        chunks = [n for n in archive.namelist() if re.fullmatch(re.escape(stream) + r"-\d+", n)]
        chunks.sort(key=lambda name: int(name.rsplit("-", 1)[1]))
        if not chunks and stream in archive.namelist(): chunks = [stream]
        if not chunks:
            raise ValueError("No logic chunks in .sr; original retained")
        if chunks != [stream]:
            numbers = [int(n.rsplit("-", 1)[1]) for n in chunks]
            if numbers != list(range(1, len(numbers) + 1)):
                raise ValueError("Missing/noncontiguous logic chunks; original retained")
        raw = directory / "logic.bin"
        # Reanalysis never overwrites the original acquisition archive.
        with raw.open("wb") as target:
            for name in chunks:
                with archive.open(name) as source:
                    for block in iter(lambda: source.read(1 << 20), b""):
                        target.write(block)
        details = dict(dev)
        if unlisted: details["explicitly_recovered_unlisted_channels"] = unlisted
        return raw, rate_hz(dev["samplerate"]), details


def csv_export(path, rows, fields):
    with Path(path).open("w", newline="") as target:
        writer = csv.DictWriter(target, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow({k: json.dumps(v, ensure_ascii=False) if isinstance(v, (dict, list)) else v
                             for k, v in row.items() if k in fields})


def analyze(directory, baud=9600, gap_chars=4):
    directory = Path(directory)
    metadata = json.loads((directory / "metadata.json").read_text())
    channel_map = metadata.get("channel_map", {"MCU_TX": 0, "ASIC_TX": 1})
    raw, samplerate, srmetadata = extract(directory / "raw.sr", directory, channel_map,
                                          metadata.get("unlisted_channels_confirmed", False))
    all_bytes, all_packets, signal = [], [], {}
    with raw.open("rb") as source:
        data = mmap.mmap(source.fileno(), 0, access=mmap.ACCESS_READ) if raw.stat().st_size else b""
        try:
            for direction, channel in channel_map.items():
                records, stats = decode(data, samplerate, channel, baud, direction=direction)
                signal[direction] = stats
                all_bytes.extend(records)
                all_packets.extend(packets(records, samplerate, baud, gap_chars,
                                           rx_bytes=metadata.get("rx_frame_bytes", 13)))
        finally:
            if isinstance(data, mmap.mmap): data.close()
    all_bytes.sort(key=lambda r: (r["start_sample"], r["channel"]))
    all_packets.sort(key=lambda p: (p["start_sample"], p["direction"]))
    for index, item in enumerate(all_packets): item["timeline_id"] = index
    # Relate packets temporally without claiming request/response causality.
    last_tx = None
    for item in all_packets:
        if item["direction"] == "MCU_TX" and item["complete"]:
            last_tx = item
        elif item["direction"] == "ASIC_TX":
            item["preceding_tx"] = None if last_tx is None else {
                "timeline_id": last_tx["timeline_id"], "command_id": last_tx["command_id"],
                "delta_start_s": item["t_s"] - last_tx["t_s"],
                "delta_after_tx_end_s": (item["start_sample"] - last_tx["end_sample_exclusive"]) / samplerate,
                "causality": "unverified_temporal_association"}
    result = {"schema": 1, "origin": metadata.get("origin", "unknown"),
              "analysis_source_sha256": {p.name: sha256(p) for p in sorted(Path(__file__).parent.glob("*.py"))},
              "metadata": metadata, "samplerate_hz": samplerate, "baud": baud,
              "gap_chars": gap_chars, "channel_map": channel_map, "sample_count": raw.stat().st_size,
              "duration_s": raw.stat().st_size / samplerate, "sigrok_metadata": srmetadata,
              "signal": signal, "raw_sha256": sha256(directory / "raw.sr"),
              "logic_sha256": sha256(raw),
              "decoder_policy": "8N1 center sampling; errors preserved; no correction; start-low ambiguous",
              "bytes": all_bytes, "packets": all_packets}
    write_json(directory / "decoded.json", result)
    csv_export(directory / "bytes.csv", all_bytes,
               ["id", "channel", "direction", "start_sample", "end_sample_exclusive", "t_s",
                "value", "hex", "partial_value", "bits_lsb_first", "start_bit", "stop_bit", "errors"])
    csv_export(directory / "packets.csv", all_packets,
               ["timeline_id", "id", "direction", "t_s", "start_sample", "end_sample_exclusive",
                "kind", "complete", "hex", "byte_ids", "uart_errors", "integrity",
                "command_id", "firmware_interpretation", "preceding_tx", "boundary_reason",
                "embedded_header_offsets"])
    lines = [f"Origin: {result['origin']}; label: {metadata.get('label')}",
             f"{samplerate} Hz / {baud} 8N1; {result['duration_s']:.6f}s",
             "Timestamps relative to acquisition; host wall-clock is separate metadata.",
             "RX integrity UNKNOWN. Fields are firmware-derived until physically tested."]
    for p in all_packets:
        lines.append(f"{p['t_s']:12.6f} {p['direction']:7} {p['kind']:16} {p['hex']} "
                     f"integrity={p['integrity']['status']} uart_errors={len(p['uart_errors'])}")
    if not all_bytes: lines.append("NO UART CANDIDATES. Check wiring/power/line activity; no data invented.")
    (directory / "decoded.txt").write_text("\n".join(lines) + "\n")
    from .analysis import summarize
    write_json(directory / "analysis.json", summarize(result))
    return result

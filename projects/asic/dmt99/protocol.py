"""Firmware-derived interpretations, deliberately separate from physical proof."""
import functools
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PATTERNS = bytes.fromhex("eb 0a ad 8f 4e c7 e7 8a ef cf ee 23 65 27 61 e5 04 00 e1 ec")
SYMBOLS = "0123456789AUTOLE- CPSr"
UNIT_NAMES = {6: "V", 7: "mV", 8: "uA", 9: "mA", 10: "A", 11: "ohm",
              12: "kohm", 13: "Mohm", 15: "nF", 16: "uF", 17: "mF",
              18: "Hz", 19: "kHz", 20: "MHz", 21: "temperature_C_or_F"}


@functools.lru_cache
def catalog():
    return json.loads((ROOT / "evidence/firmware_commands.json").read_text())


def segment(value):
    value &= 0xef
    if value in PATTERNS:
        return PATTERNS.index(value)
    return 21 if value == 0x24 else 255


def unit_code(b):
    if b[8] & 2: return 7 if b[7] & 1 else 6
    if b[7] & 0x20: return 12 if b[6] & 0x40 else 13 if b[7] & 4 else 11
    if b[8] & 1: return 8 if b[6] & 0x10 else 9 if b[7] & 1 else 10
    if b[7] & 0x10: return 15 if b[6] & 0x20 else 16 if b[6] & 0x10 else 17
    if b[8] & 0x20: return 21
    if b[8] & 4: return 19 if b[6] & 0x40 else 20 if b[7] & 4 else 18
    return 1


def display(b):
    result = {"evidence": "original_firmware_derived", "hardware_validated": False}
    if b[2] == 0xfd:
        return dict(result, kind="event", event_id=b[3], correlated_ack_verified=False)
    codes = [segment((b[2+i] & 0xf0) | (b[3+i] & 15)) for i in range(4)]
    symbols = [SYMBOLS[c] if c < len(SYMBOLS) else "?" for c in codes]
    decimal = next((i for i in range(1, 4) if b[2+i] & 0x10), 0)
    text = "-" if b[2] & 0x10 else ""
    for i, symbol in enumerate(symbols, 1):
        text += symbol
        if i == decimal: text += "."
    numeric = None
    # Physical 1-ohm capture contains leading blank cells ("  1.1").
    # Keep raw text unchanged; accept only leading blanks in numeric conversion.
    numeric_text = ("-" if b[2] & 0x10 else "") + text[1 if b[2] & 0x10 else 0:].lstrip(" ")
    if re.fullmatch(r"-?\d+(?:\.\d+)?", numeric_text):
        numeric = float(numeric_text)
    code = unit_code(b)
    display_pattern = ("resistance_open_pattern" if codes == [17, 0, 14, 17] and decimal == 2 and code == 13
                       else "AUTO" if symbols == list("AUTO")
                       else "blank" if all(c == 17 for c in codes)
                       else "numeric" if numeric is not None else "other")
    return dict(result, kind="display", symbols=symbols, symbol_codes=codes,
                display_pattern=display_pattern,
                display_text=text, numeric_value=numeric,
                numeric_policy="leading_blank_cells_only_ignored; raw_text_unchanged; unknowns_rejected",
                negative=bool(b[2] & 0x10),
                decimal_after=decimal, unit_code=code,
                unit_resource_association=UNIT_NAMES.get(code, "unknown"),
                ac_dc_code=2 if b[2] & 2 else 1 if b[2] & 1 else 255,
                status_b7_bit7=bool(b[7] & 0x80), range="unknown",
                observed_rx_bytes=len(b), opaque_b9_to_b12=list(b[9:13]))


def host_ui_status(interpretation, host_mode):
    """Recovered screen.c behavior; caller must supply the known host mode.

    These are UI rendering states, not calibrated field/voltage detection.
    Generic segment text is retained, including unknown symbols.
    """
    if interpretation.get("kind") != "display":
        return None
    codes = interpretation["symbol_codes"]
    if host_mode == 0x09:
        return {"kind": "NCV_level", "value": codes.count(16),
                "evidence": "original_screen_c_mode_09", "active_input_validated": False}
    if host_mode == 0x13:
        return {"kind": "LIVE_active_render", "value": codes[0] != 14,
                "evidence": "original_screen_c_mode_13", "active_input_validated": False}
    return None


def integrity(b, direction):
    if direction == "ASIC_TX":
        return {"status": "unknown", "reason": "RX checksum existence/location/rule unverified"}
    for command in catalog()["commands"]:
        literal = bytes.fromhex(command["hex"])
        if b == literal:
            return {"status": "firmware_literal_match", "hardware_validated": False,
                    "host_association": command["host_association"],
                    "general_checksum_verified": False}
        if b[:-1] == literal[:-1]:
            return {"status": "firmware_literal_check_byte_mismatch",
                    "expected_literal_byte": literal[-1], "actual": b[-1],
                    "general_checksum_verified": False}
    return {"status": "unknown", "reason": "No exact firmware template"}


def packets(records, samplerate, baud=9600, gap_chars=4, rx_bytes=13):
    """Fixed-size header candidates; every UART record belongs to exactly one item.

    Headers inside a full candidate are not silently used for resynchronization.
    Alternative offsets are reported. Gaps split truncated candidates explicitly.
    """
    if gap_chars <= 0:
        raise ValueError("gap_chars must be positive")
    if not records: return []
    direction = records[0]["direction"]
    if rx_bytes not in (12, 13): raise ValueError("Supported RX hypotheses are 12 or 13 bytes")
    gap_samples = gap_chars * 10 * samplerate / baud
    def header_spec(i):
        forms = [((0xaa, 0x55), 10, "MCU_command")] if direction == "MCU_TX" else [
            ((0x5a, 0xa5), rx_bytes, "ASIC_display_5AA5"),
            ((0xaa, 0x55, 0xfd), 10, "ASIC_event_AA55FD")]
        for prefix, size, family in forms:
            if i + len(prefix) > len(records): continue
            if tuple(r["value"] for r in records[i:i+len(prefix)]) != prefix: continue
            if any(records[j+1]["start_sample"] - records[j]["end_sample_exclusive"] > gap_samples
                   for j in range(i, i+len(prefix)-1)): continue
            return size, family
        return None
    def is_header(i): return header_spec(i) is not None
    result = []
    i = 0
    while i < len(records):
        start = i
        spec = header_spec(i)
        framed = spec is not None
        length, family = spec if framed else (10 if direction == "MCU_TX" else rx_bytes, "unidentified")
        i += 1
        boundary = "capture_end"
        while i < len(records):
            if records[i]["start_sample"] - records[i-1]["end_sample_exclusive"] > gap_samples:
                boundary = "idle_gap"; break
            if framed and i-start >= length:
                boundary = "fixed_length"; break
            if not framed and is_header(i):
                boundary = "next_header"; break
            i += 1
        selected = records[start:i]
        values = [r["value"] for r in selected]
        complete = framed and len(values) == length and None not in values
        errors = [{"byte_id": r["id"], "errors": r["errors"]} for r in selected if r["errors"]]
        item = {"id": len(result), "direction": direction,
                "start_sample": selected[0]["start_sample"],
                "end_sample_exclusive": selected[-1]["end_sample_exclusive"],
                "t_s": selected[0]["t_s"], "byte_ids": [r["id"] for r in selected],
                "values": values, "hex": " ".join("??" if v is None else f"{v:02X}" for v in values),
                "kind": "frame_candidate" if complete else "incomplete_frame" if framed else "unframed",
                "wire_family": family,
                "complete": complete, "uart_errors": errors, "boundary_reason": boundary,
                "expected_bytes": length,
                "boundary_evidence": "header_and_explicit_length_hypothesis",
                "embedded_header_offsets": [j-start for j in range(start+2, i-1) if is_header(j)],
                "integrity": {"status": "not_evaluated_incomplete"}}
        if complete:
            b = bytes(values)
            item["integrity"] = integrity(b, direction)
            if direction == "MCU_TX": item["command_id"] = b[3]
            elif family == "ASIC_event_AA55FD":
                item["firmware_interpretation"] = {
                    "evidence": "hardware_observed_event_candidate_and_firmware_FD_dispatch",
                    "hardware_validated": False, "kind": "event", "event_id": b[3],
                    "correlated_ack_verified": False, "observed_rx_bytes": len(b)}
            else: item["firmware_interpretation"] = display(b)
        result.append(item)
    assert sum(len(p["byte_ids"]) for p in result) == len(records)
    return result

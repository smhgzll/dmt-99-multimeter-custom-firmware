#!/usr/bin/env python3
"""Audit UART templates and real RX fields against the preserved original instruction trace.

The old analysis tree is read only. Execution is confined to a Python RAM model;
UI/USART leaf calls are intercepted. No target connection or UART transmission.
"""
import argparse
import ctypes
import hashlib
import json
import math
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from dmt99.protocol import catalog, display, host_ui_status, segment


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference", type=Path, default=ROOT.parents[1])
    parser.add_argument("--output", type=Path, default=ROOT / "evidence/decompiled_audit.json")
    args = parser.parse_args()
    reference = args.reference.resolve()
    # Prevent imports from creating __pycache__ in the reference tree.
    sys.dont_write_bytecode = True
    sys.path.insert(0, str(reference / "tools"))
    from rv32_trace import TraceMachine

    sources = [reference / "analysis/instructions.json", reference / "analysis/functions.json",
               reference / "DMT-99_V2.4_20240306.bin", reference / "tools/rv32_trace.py",
               reference / "tools/rv32_subset.py", reference / "analysis/asic_protocol_commands.json"]
    sources += sorted((reference / "decompiled/clean").glob("*measurement*.c"))
    sources += [reference / "decompiled/clean" / f"{address:08x}_FUN_ram_{address:08x}.c"
                for address in (0xa7a4, 0xa74e, 0xa782, 0x9caa, 0xb56a, 0x17f92)]
    gap = reference / "analysis/ncv_gap_instructions.json"
    if gap.exists():
        sources.append(gap)
    sources = sorted(set(p for p in sources if p.exists()))
    before = {str(p): digest(p) for p in sources}
    functions = {int(f["address"], 16): f for f in json.loads((reference / "analysis/functions.json").read_text())}
    generated = {}
    contexts = []

    def transmit(machine, pointer, *unused):
        raw = bytes(machine.read(pointer + i, 1) for i in range(10))
        contexts[-1]["packets"].append(raw.hex(" "))
        generated.setdefault(raw.hex(" "), []).append(len(contexts) - 1)
        return 0

    # The normal view's three selectors plus HOLD, AUTO and legacy template.
    for event, selector in [(e, s) for e, n in ((1, 5), (3, 6), (4, 5)) for s in range(n)] + [(2, 0), (5, 0), (7, 0)]:
        machine = TraceMachine()
        machine.stub(*(int(a, 16) for a in functions[0xb5c8]["callees"]
                       if int(a, 16) > 0x6700 and int(a, 16) != 0x1e59e))
        machine.write(0x200001ee, 1, event)
        for address in (0x200001fc, 0x200001fd, 0x200001fe):
            machine.write(address, 1, selector)
        machine.hooks[0x97c4] = transmit
        contexts.append({"entry": "0x0000b5c8", "event": event, "selector_before": selector, "packets": []})
        machine.call(0xb5c8)
    for key in range(1, 5):
        machine = TraceMachine()
        machine.stub(0xcb40, 0x1a64c)
        machine.write(0x200001ee, 1, key)
        machine.hooks[0x97c4] = transmit
        contexts.append({"entry": "0x000097f8", "accepted_key": key, "packets": []})
        machine.call(0x97f8)
    machine = TraceMachine()
    machine.stub(0x1a64c)
    machine.hooks[0x97c4] = transmit
    contexts.append({"entry": "0x0000b56a", "packets": []})
    machine.call(0xb56a)

    expected_catalog = {bytes.fromhex(c["hex"]).hex(" ") for c in catalog()["commands"]}
    old_catalog = json.loads((reference / "analysis/asic_protocol_commands.json").read_text())
    assert expected_catalog == {bytes.fromhex(c["hex"]).hex(" ") for c in old_catalog["commands"]}
    if set(generated) != expected_catalog:
        raise AssertionError({"missing": sorted(expected_catalog - set(generated)),
                              "unexpected": sorted(set(generated) - expected_catalog)})
    serializer_checks = []
    with tempfile.TemporaryDirectory(prefix="dmt99-decompiled-audit-") as temporary:
        library = Path(temporary) / "asic.so"
        subprocess.run(["gcc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-shared", "-fPIC",
                        str(ROOT / "firmware/dmt99_asic.c"), "-o", str(library)], check=True, capture_output=True)
        lib = ctypes.CDLL(str(library))
        lib.dmt99_build_firmware_template.argtypes = [ctypes.c_uint8, ctypes.POINTER(ctypes.c_uint8)]
        lib.dmt99_build_firmware_template.restype = ctypes.c_bool
        for raw_hex in sorted(generated):
            raw = bytes.fromhex(raw_hex)
            out = (ctypes.c_uint8 * 10)(*([0xcc] * 10))
            accepted = lib.dmt99_build_firmware_template(raw[3], out)
            match = accepted and bytes(out) == raw if raw[3] != 0xa0 else not accepted and bytes(out) == b"\xcc" * 10
            assert match, raw_hex
            serializer_checks.append({"original_hex": raw_hex, "serializer_accepts": accepted,
                                      "result": "exact_match" if accepted else "diagnostic_A0_intentionally_not_exposed"})

    for value in range(256):
        machine = TraceMachine()
        assert machine.call(0x98aa, value) == segment(value), value

    captures, tx, unique = [], [], {}
    for path in sorted((ROOT / "captures").glob("*/decoded.json")):
        data = json.loads(path.read_text())
        captures.append(path.parent.name)
        for packet in data["packets"]:
            if not packet["complete"] or packet["uart_errors"]:
                continue
            if packet["direction"] == "MCU_TX":
                raw_hex = bytes(packet["values"]).hex(" ")
                assert raw_hex in generated, (path, raw_hex)
                tx.append({"capture": path.parent.name, "t_s": packet["t_s"], "hex": raw_hex,
                           "result": "exact_original_constructor_match"})
            elif packet.get("wire_family") == "ASIC_display_5AA5":
                raw = bytes(packet["values"])
                entry = unique.setdefault(raw, {"captures": set(), "modes": set()})
                entry["captures"].add(path.parent.name)
                if "cable_wave_icon" in path.parent.name:
                    entry["modes"].add(9)
                if "LIVE_cable_only" in path.parent.name:
                    entry["modes"].add(19)

    rx_checks = []
    for raw, context in sorted(unique.items()):
        interpreted = display(raw)
        machine = TraceMachine()
        machine.ram[0x2ac:0x2ac + len(raw)] = raw
        # RX bytes stay unchanged. Remaining oracle RAM is initialized state,
        # not an extra byte appended to a hardware packet.
        machine.call(0x997e, 0x20000800)
        symbols = list(machine.ram[0x800:0x804])
        assert symbols == interpreted["symbol_codes"], raw.hex()

        def formatted(machine, destination, format_pointer, *characters):
            for i, character in enumerate(characters[:6]):
                machine.write(destination + i, 1, character)
            machine.write(destination + 6, 1, 0)
            return 7

        machine.hooks[0x969e] = formatted
        machine.call(0xbe78, limit=2000000)
        original_text = machine.string(0x2000028c)
        original_fields = {"unit_code": machine.read(0x20000289, 1),
                           "ac_dc_code": machine.read(0x20000288, 1),
                           "status_b7_bit7": bool(machine.read(0x2000028a, 1)),
                           "negative": original_text.startswith("-"),
                           "decimal_after": original_text.index(".") - 1 if "." in original_text else 0}
        assert all(interpreted[k] == value for k, value in original_fields.items()), raw.hex()
        original_number = struct.unpack("<f", machine.read(0x20000284, 4).to_bytes(4, "little"))[0]
        number_match = None
        if interpreted["numeric_value"] is not None:
            number_match = math.isclose(original_number, interpreted["numeric_value"], rel_tol=1e-6, abs_tol=1e-9)
            assert number_match, (raw.hex(), original_number, interpreted["numeric_value"])
        row = {"hex": raw.hex(" "), "captures": sorted(context["captures"]),
               "original_symbols": symbols, "original_fields": original_fields,
               "our_raw_text": interpreted["display_text"], "our_numeric": interpreted["numeric_value"],
               "stock_parser_text_mode0": original_text, "fields_match": True,
               "stock_parser_float_mode0": original_number, "numeric_match_when_ours_numeric": number_match}
        statuses = []
        for mode in sorted(context["modes"]):
            machine.write(0x20000206, 1, mode)
            observed = []
            machine.hooks[0xfe08] = lambda m, state, *args: observed.append((9, state)) or 0
            machine.hooks[0x1014e] = lambda m, state, *args: observed.append((19, state)) or 0
            machine.call(0xa7a4)
            expected = host_ui_status(interpreted, mode)
            assert observed == [(mode, int(expected["value"]))], (mode, raw.hex(), observed)
            statuses.append({"host_mode": mode, "original_render_state": observed[0][1], "match": True})
        row["context_status_checks"] = statuses
        rx_checks.append(row)
    assert before == {str(p): digest(p) for p in sources}, "Reference source changed"
    report = {"reference": str(reference), "reference_sources_sha256": before,
              "reference_files_unchanged": True,
              "oracle_scope": "Preserved decoded instruction inventory + original ROM data in rv32_trace; leaf IO hooks, not a full MCU or peer ASIC emulator. No target I/O.",
              "constructor_contexts": contexts, "distinct_original_templates": len(generated),
              "catalog_exact_match": True, "old_catalog_exact_match": True, "serializer_checks": serializer_checks,
              "source_captures": captures, "captured_tx_checks": tx,
              "all_256_segment_codes_match": True, "distinct_real_display_checks": rx_checks,
              "intentional_differences": ["Lossless raw text; unknown symbol '?' instead of stock coercion/rendering.",
                                         "Nonnumeric/unknown frames remain None, not stock float fallback.",
                                         "No mode-dependent leading blank replacement by zero in raw text.",
                                         "12 observed wire bytes preserved, separate from stock DMA buffer13.",
                                         "Diagnostic A0 template retained as reference but not exposed by serializer."],
              "not_confirmed": ["RX checksum", "opaque tail", "ASIC analog semantics", "wire command selection for last7 captures"]}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n")
    print(f"{len(generated)} original templates; {len(tx)} real TX matches; 256 segment codes; {len(rx_checks)} distinct real display frames match fields; reference unchanged")


if __name__ == "__main__":
    main()

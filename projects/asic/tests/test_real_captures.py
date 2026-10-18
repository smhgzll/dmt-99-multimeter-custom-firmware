"""Regression vectors are exact excerpts of passive DMT99 hardware captures."""
import gzip
import hashlib
import json
from pathlib import Path
import unittest

from dmt99.analysis import checksum_hypotheses
from dmt99.protocol import host_ui_status, packets
from dmt99.uart import decode

FIXTURES = Path(__file__).parent / "fixtures"


class RealCaptureTests(unittest.TestCase):
    def test_temperature_open_wire_numeric_and_separate_user_unit(self):
        path = FIXTURES / "real_temperature_C_open_1000000hz.json"
        info = json.loads(path.read_text())
        raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
        self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
        self.assertEqual(info["source_metadata"]["display_unit"], "degC")
        records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
        self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                         info["expected"]["ASIC_TX"])
        for frame in packets(records, info["samplerate_hz"], rx_bytes=12):
            d = frame["firmware_interpretation"]
            self.assertEqual(d["display_text"], "  32")
            self.assertEqual(d["numeric_value"], 32)
            self.assertEqual(d["unit_code"], 21)
            self.assertEqual(d["unit_resource_association"], "temperature_C_or_F")

    def test_ncv_and_live_open_status_requires_host_context(self):
        cases = [("real_cable_wave_icon_idle_1000000hz", 9, " E? ", "NCV_level"),
                 ("real_live_cable_only_idle_1000000hz", 19, "L1UE", "LIVE_active_render")]
        for name, mode, text, status_kind in cases:
            path = FIXTURES / (name + ".json")
            info = json.loads(path.read_text())
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
            records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
            self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                             info["expected"]["ASIC_TX"])
            for frame in packets(records, info["samplerate_hz"], rx_bytes=12):
                d = frame["firmware_interpretation"]
                self.assertEqual(d["display_text"], text)
                self.assertIsNone(d["numeric_value"])
                self.assertIsNone(host_ui_status(d, 0x0c))
                status = host_ui_status(d, mode)
                self.assertEqual(status["kind"], status_kind)
                self.assertEqual(status["value"], 0)
                self.assertFalse(status["active_input_validated"])

    def test_current_open_modes_preserve_reported_and_wire_units(self):
        cases = [("dc_ma", "   0", "uA", 2, "mA"),
                 ("ac_ma", "   3", "uA", 1, "mA"),
                 ("dc_a", "0.000", "A", 2, "A"),
                 ("ac_a", "0.001", "A", 1, "A")]
        for mode, text, unit, code, reported_unit in cases:
            with self.subTest(mode=mode):
                path = FIXTURES / (f"real_current_{mode}_open_1000000hz.json")
                info = json.loads(path.read_text())
                raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
                self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
                self.assertEqual(info["source_metadata"]["display_unit"], reported_unit)
                records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
                self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                                 info["expected"]["ASIC_TX"])
                frames = packets(records, info["samplerate_hz"], rx_bytes=12)
                self.assertGreaterEqual(len(frames), 5)
                for frame in frames:
                    d = frame["firmware_interpretation"]
                    self.assertFalse(frame["uart_errors"])
                    self.assertEqual(d["display_text"], text)
                    self.assertEqual(d["unit_resource_association"], unit)
                    self.assertEqual(d["ac_dc_code"], code)
                    self.assertEqual(frame["integrity"]["status"], "unknown")

    def test_open_frequency_exact_samples_and_hz_units(self):
        path = FIXTURES / "real_frequency_open_1000000hz.json"
        info = json.loads(path.read_text())
        raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
        self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
        records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
        self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                         info["expected"]["ASIC_TX"])
        for packet in packets(records, info["samplerate_hz"], rx_bytes=12):
            self.assertFalse(packet["uart_errors"])
            d = packet["firmware_interpretation"]
            self.assertEqual(d["unit_resource_association"], "Hz")
            self.assertGreaterEqual(d["numeric_value"], 48.40)
            self.assertLessEqual(d["numeric_value"], 51.57)

    def test_dc_ac_open_sign_and_status_codes(self):
        for name, expected_code in [("real_dc_voltage_open_1000000hz", 2),
                                    ("real_ac_voltage_open_1000000hz", 1)]:
            path = FIXTURES / (name + ".json")
            info = json.loads(path.read_text())
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
            records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
            self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                             info["expected"]["ASIC_TX"])
            decoded = packets(records, info["samplerate_hz"], rx_bytes=12)
            self.assertGreaterEqual(len(decoded), 10)
            for p in decoded:
                d = p["firmware_interpretation"]
                self.assertFalse(p["uart_errors"])
                self.assertEqual(d["unit_resource_association"], "V")
                self.assertEqual(d["ac_dc_code"], expected_code)
                self.assertIsNotNone(d["numeric_value"])
                if expected_code == 2:
                    self.assertGreaterEqual(d["numeric_value"], -0.003)
                    self.assertLessEqual(d["numeric_value"], 0)
                else:
                    self.assertIn(d["numeric_value"], (0.272, 0.273))

    def test_capacitance_and_buzzer_open_wire_forms(self):
        cases = [("real_capacitance_open_1000000hz", "0.014", 0.014, "nF"),
                 ("real_continuity_open_1000000hz", " .0L ", None, "V")]
        for name, text, number, unit in cases:
            path = FIXTURES / (name + ".json")
            info = json.loads(path.read_text())
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
            records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
            self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                             info["expected"]["ASIC_TX"])
            decoded = packets(records, info["samplerate_hz"], rx_bytes=12)
            self.assertGreaterEqual(len(decoded), 5)
            for p in decoded:
                d = p["firmware_interpretation"]
                self.assertFalse(p["uart_errors"])
                self.assertEqual(d["display_text"], text)
                self.assertEqual(d["numeric_value"], number)
                self.assertEqual(d["unit_resource_association"], unit)
                self.assertEqual(p["integrity"]["status"], "unknown")

    def test_auto_to_resistance_command_event_sequence(self):
        path = FIXTURES / "real_mode_transition_1000000hz.json"
        info = json.loads(path.read_text())
        raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
        self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
        frames = {}
        for direction, channel in info["channel_map"].items():
            records, _ = decode(raw, info["samplerate_hz"], channel, direction=direction)
            self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                             info["expected"][direction])
            frames[direction] = packets(records, info["samplerate_hz"], rx_bytes=12)
        commands = [p["command_id"] for p in frames["MCU_TX"] if p["complete"]]
        events = [p["firmware_interpretation"]["event_id"] for p in frames["ASIC_TX"]
                  if p.get("wire_family") == "ASIC_event_AA55FD"]
        self.assertEqual(commands, [0x0c, 0x0d, 0x0b])
        self.assertEqual(events, commands)

    def test_exported_resistance_reference_windows(self):
        paths = sorted(FIXTURES.glob("real_resistance_*.json"))
        self.assertGreaterEqual(len(paths), 1)
        for path in paths:
            info = json.loads(path.read_text())
            if "expected_packets" not in info or info["source_metadata"].get("display_numeric") is None:
                continue
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
            observed = []
            numeric = []
            for direction, channel in info["channel_map"].items():
                records, _ = decode(raw, info["samplerate_hz"], channel, direction=direction)
                self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                                 info["expected"][direction])
                for packet in packets(records, info["samplerate_hz"], rx_bytes=12):
                    observed.append({k:packet[k] for k in ("direction", "hex", "kind", "uart_errors")})
                    d = packet.get("firmware_interpretation", {})
                    if d.get("kind") == "display":
                        self.assertIsNotNone(d["numeric_value"])
                        self.assertEqual(d["unit_resource_association"], info["source_metadata"]["display_unit"])
                        numeric.append(d["numeric_value"])
            self.assertEqual(observed, info["expected_packets"])
            self.assertGreaterEqual(len(numeric), 5)

    def test_physical_open_probes_not_coerced_to_zero(self):
        path = FIXTURES / "real_resistance_open_1000000hz.json"
        info = json.loads(path.read_text())
        raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
        self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
        records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
        self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                         info["expected"]["ASIC_TX"])
        decoded = packets(records, info["samplerate_hz"], rx_bytes=12)
        self.assertGreaterEqual(len(decoded), 5)
        for packet in decoded:
            d = packet["firmware_interpretation"]
            self.assertEqual(d["display_text"], " 0.L ")
            self.assertEqual(d["display_pattern"], "resistance_open_pattern")
            self.assertIsNone(d["numeric_value"])

    def test_physical_one_ohm_leading_blanks_and_decimal(self):
        path = FIXTURES / "real_resistance_1ohm_1000000hz.json"
        info = json.loads(path.read_text())
        raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
        self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
        records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
        self.assertEqual([r["value"] for r in records], info["expected_rx_values"])
        self.assertEqual([r["start_sample"] for r in records], info["expected_rx_start_samples"])
        decoded = packets(records, info["samplerate_hz"], rx_bytes=12)
        self.assertGreaterEqual(len(decoded), 5)
        for packet in decoded:
            self.assertFalse(packet["uart_errors"])
            d = packet["firmware_interpretation"]
            self.assertEqual(d["unit_resource_association"], "ohm")
            self.assertIn(d["numeric_value"], info["numeric_values_observed"])
            self.assertTrue(d["display_text"].startswith("  "))
            self.assertEqual(d["decimal_after"], 3)

    def test_startup_command_and_distinct_event_header(self):
        path = FIXTURES / "real_startup_command_event_2000000hz.json"
        info = json.loads(path.read_text())
        raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
        self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
        families = {}
        for direction, channel in info["channel_map"].items():
            records, _ = decode(raw, info["samplerate_hz"], channel, direction=direction)
            self.assertEqual([{k:r[k] for k in ("value", "start_sample", "errors")} for r in records],
                             info["expected"][direction])
            families[direction] = packets(records, info["samplerate_hz"], rx_bytes=12)
        command = next(p for p in families["MCU_TX"] if p["complete"])
        event = next(p for p in families["ASIC_TX"] if p["wire_family"] == "ASIC_event_AA55FD")
        self.assertEqual(command["hex"], info["command_hex"])
        self.assertEqual(event["hex"], info["event_hex"])
        self.assertEqual(len(event["values"]), 10)
        self.assertEqual(event["firmware_interpretation"]["event_id"], 0x14)
        self.assertFalse(event["firmware_interpretation"]["correlated_ack_verified"])

    def test_auto_idle_at_1_and_2_mhz(self):
        paths = sorted(FIXTURES.glob("real_auto_idle_*.json"))
        self.assertEqual(len(paths), 2, "Both hardware rates must be represented")
        for path in paths:
            info = json.loads(path.read_text())
            self.assertEqual(info["origin"], "hardware_excerpt")
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            self.assertEqual(hashlib.sha256(raw).hexdigest(), info["raw_excerpt_sha256"])
            rx, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
            tx, _ = decode(raw, info["samplerate_hz"], 0, direction="MCU_TX")
            self.assertEqual(tx, [])
            self.assertEqual([r["value"] for r in rx], info["expected_rx_values"])
            self.assertEqual([r["start_sample"] for r in rx], info["expected_rx_start_samples"])
            self.assertFalse(any(r["errors"] for r in rx))
            observed = packets(rx, info["samplerate_hz"], rx_bytes=12)
            self.assertGreaterEqual(len(observed), 2)
            self.assertTrue(all(p["complete"] and len(p["values"]) == 12 for p in observed))
            self.assertTrue(all(p["firmware_interpretation"]["display_text"] == "AUTO" for p in observed))
            self.assertTrue(all(p["integrity"]["status"] == "unknown" for p in observed))
            legacy = packets(rx, info["samplerate_hz"], rx_bytes=13)
            self.assertTrue(all(p["kind"] == "incomplete_frame" for p in legacy))
            self.assertEqual(sum(len(p["values"]) for p in legacy), len(rx))


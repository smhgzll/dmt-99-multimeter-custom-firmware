"""Synthetic in-memory unit vectors. Never physical evidence or capture fixtures."""
import json
import subprocess
import tempfile
import unittest
from pathlib import Path
import zipfile

from dmt99.analysis import checksum_hypotheses, compare
from dmt99.protocol import catalog, display, integrity, packets, segment
from dmt99.session import analyze, extract, write_json
from dmt99.uart import decode


def waveform(values, rate=1000000, channel=0, stop=1):
    """Independent transmitter timeline; fractional-bit positions rounded globally."""
    cells = [1] * 3
    for value in values:
        cells += [0] + [(value >> n) & 1 for n in range(8)] + [stop]
    cells += [1] * 2
    data = bytearray([3] * round(len(cells) * rate / 9600))
    for i, value in enumerate(cells):
        if not value:
            begin, end = round(i * rate / 9600), round((i+1) * rate / 9600)
            for j in range(begin, end): data[j] &= ~(1 << channel)
    return data


def records(values, direction="MCU_TX", gap_after=None):
    result = []
    shift = 0
    for i, value in enumerate(values):
        if gap_after == i: shift += 10000
        start = i * 1042 + shift
        result.append({"id": i, "direction": direction, "value": value,
                       "start_sample": start, "end_sample_exclusive": start+1042,
                       "t_s": start / 1e6, "errors": []})
    return result


class UARTTests(unittest.TestCase):
    def test_all_bytes_both_rates_channels_across_block_boundaries(self):
        for rate in (1000000, 2000000):
            for channel in (0, 1):
                data = waveform(range(256), rate, channel)
                result, stats = decode(data, rate, channel, block_size=1371)
                self.assertEqual([r["value"] for r in result], list(range(256)))
                self.assertFalse(any(r["errors"] for r in result))
                self.assertEqual(stats["samples"], len(data))
                self.assertLessEqual(abs(result[0]["start_sample"] - rate * 3 / 9600), 1)

    def test_framing_error_preserved(self):
        result, _ = decode(waveform([0xa5], stop=0), 1000000, 0)
        self.assertEqual(result[0]["value"], 0xa5)
        self.assertIn("framing_error_stop_low", result[0]["errors"])

    def test_truncated_and_glitch_and_stuck_lines(self):
        result, _ = decode(waveform([0x55])[:700], 1000000, 0)
        self.assertIsNone(result[0]["value"])
        self.assertIn("truncated_uart_frame", result[0]["errors"])
        data = bytearray([3] * 2000)
        data[100:102] = b"\x02\x02"
        result, _ = decode(data, 1000000, 0)
        self.assertIn("short_low_false_start", result[0]["errors"])
        low, stats = decode(b"\0" * 2000, 1000000, 0)
        self.assertIn("capture_started_low_ambiguous_start", low[0]["errors"])
        self.assertEqual(stats["high_samples"], 0)
        high, _ = decode(b"\3" * 2000, 1000000, 0)
        self.assertEqual(high, [])

    def test_reference_libsigrokdecode_all_values(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "unit_test_synthetic.bin"
            path.write_bytes(waveform(range(256)))
            command = ["sigrok-cli", "-i", str(path), "-I", "binary:numchannels=8:samplerate=1000000",
                       "-P", "uart:rx=0:baudrate=9600:data_bits=8:parity=none:stop_bits=1",
                       "-B", "uart=rx"]
            process = subprocess.run(command, capture_output=True, check=True)
            self.assertEqual(process.stdout, bytes(range(256)), process.stderr)


class PacketTests(unittest.TestCase):
    def test_all_19_literals_and_legacy_exception(self):
        for item in catalog()["commands"]:
            b = bytes.fromhex(item["hex"])
            self.assertEqual(integrity(b, "MCU_TX")["status"], "firmware_literal_match")
            changed = b[:-1] + bytes([b[-1] ^ 1])
            self.assertEqual(integrity(changed, "MCU_TX")["status"], "firmware_literal_check_byte_mismatch")
        legacy = bytes.fromhex("aa550500000000000011")
        self.assertNotEqual(sum(legacy[2:9]) & 255, legacy[-1])
        self.assertFalse(integrity(legacy, "MCU_TX")["general_checksum_verified"])

    def test_noise_truncation_gap_and_back_to_back_lossless(self):
        tx = bytes.fromhex("aa55050c000000000011")
        values = [0x13, 0x24] + list(tx) * 2 + list(tx[:5])
        r = records(values)
        p = packets(r, 1000000)
        self.assertEqual([x["kind"] for x in p], ["unframed", "frame_candidate", "frame_candidate", "incomplete_frame"])
        self.assertEqual([v for item in p for v in item["values"]], values)
        self.assertEqual([i for item in p for i in item["byte_ids"]], list(range(len(r))))
        split = packets(records(tx, gap_after=5), 1000000)
        self.assertEqual(split[0]["kind"], "incomplete_frame")
        self.assertEqual(split[1]["kind"], "unframed")

    def test_embedded_header_and_uart_errors_remain_visible(self):
        b = bytes.fromhex("5aa50102035aa5000000000000")
        r = records(b, "ASIC_TX")
        r[2]["errors"] = ["framing_error_stop_low"]
        p = packets(r, 1000000)[0]
        self.assertEqual(p["embedded_header_offsets"], [5])
        self.assertEqual(len(p["uart_errors"]), 1)
        self.assertEqual(p["integrity"]["status"], "unknown")
        self.assertEqual(p["values"], list(b))

    def test_unknown_value_and_partial_header_preserved(self):
        r = records([0xaa, 0x55, None, 4, 5, 6, 7, 8, 9, 10, 0xaa])
        p = packets(r, 1000000)
        self.assertEqual(sum(len(x["values"]) for x in p), len(r))
        self.assertFalse(p[0]["complete"])

    def test_hypotheses_never_promote_repetition(self):
        p = packets(records(bytes.fromhex("5aa50000000000000000000000"), "ASIC_TX"), 1000000)
        h = checksum_hypotheses(p * 30)
        self.assertEqual(h["distinct_frames"], 1)
        self.assertTrue(all(not c["confirmed"] for c in h["candidates"]))
        with self.assertRaises(ValueError): compare([{"origin": "unit_test_synthetic"}])


class SessionTests(unittest.TestCase):
    def test_unlisted_channel_recovery_is_explicit(self):
        with tempfile.TemporaryDirectory() as tmp:
            directory = Path(tmp)
            with zipfile.ZipFile(directory / "raw.sr", "w") as archive:
                archive.writestr("metadata", "[device 1]\ncapturefile=logic-1\nunitsize=1\nsamplerate=1 MHz\ntotal probes=8\nprobe1=D0\nprobe2=D1\n")
                archive.writestr("logic-1-1", b"\x03\x07\x03\x07")
            mapping = {"MCU_TX": 0, "ASIC_TX": 2}
            with self.assertRaises(ValueError): extract(directory / "raw.sr", directory, mapping)
            raw, _, details = extract(directory / "raw.sr", directory, mapping, allow_unlisted=True)
            self.assertEqual(raw.read_bytes(), b"\x03\x07\x03\x07")
            self.assertEqual(details["explicitly_recovered_unlisted_channels"], ["D2"])

    def test_actual_sigrok_archive_writer_compatibility(self):
        with tempfile.TemporaryDirectory() as tmp:
            directory = Path(tmp)
            data = waveform(bytes.fromhex("aa55050c000000000011"))
            source = directory / "unit_test_synthetic.bin"
            source.write_bytes(data)
            subprocess.run(["sigrok-cli", "-i", str(source), "-I", "binary:numchannels=8:samplerate=1000000",
                            "-C", "0=D0,1=D1", "-O", "srzip", "-o", str(directory / "raw.sr")],
                           check=True, capture_output=True)
            raw, rate, metadata = extract(directory / "raw.sr", directory)
            self.assertEqual(raw.read_bytes(), data)
            self.assertEqual(rate, 1000000)
            self.assertEqual(metadata["probe1"], "D0")

    def test_chunk_order_rate_exports_and_metadata(self):
        with tempfile.TemporaryDirectory() as tmp:
            directory = Path(tmp)
            data = waveform(bytes.fromhex("aa55050c000000000011"))
            metadata = "[global]\nsigrok version=0.5.2\n[device 1]\ncapturefile=logic-1\nunitsize=1\nsamplerate=1 MHz\nprobe1=D0\nprobe2=D1\ntotal probes=8\n"
            with zipfile.ZipFile(directory / "raw.sr", "w") as archive:
                archive.writestr("metadata", metadata)
                archive.writestr("version", "2")
                pieces = [data[i:i+1000] for i in range(0, len(data), 1000)]
                for i in reversed(range(len(pieces))): archive.writestr(f"logic-1-{i+1}", pieces[i])
            write_json(directory / "metadata.json", {"origin": "unit_test_synthetic", "label": "unit_only"})
            result = analyze(directory)
            self.assertEqual((directory / "logic.bin").read_bytes(), data)
            self.assertEqual(result["origin"], "unit_test_synthetic")
            self.assertEqual(result["packets"][0]["command_id"], 0x0c)
            for name in ("bytes.csv", "packets.csv", "decoded.json", "decoded.txt", "analysis.json"):
                self.assertGreater((directory / name).stat().st_size, 0)

    def test_reject_wrong_mapping_or_missing_chunk(self):
        with tempfile.TemporaryDirectory() as tmp:
            directory = Path(tmp)
            for extra in ("probe1=MCU_TX\nprobe2=ASIC_TX", "probe1=D0\nprobe2=D1"):
                with zipfile.ZipFile(directory / "raw.sr", "w") as archive:
                    archive.writestr("metadata", "[device 1]\ncapturefile=logic-1\nunitsize=1\nsamplerate=1 MHz\n" + extra)
                    archive.writestr("logic-1-1", b"123")
                    archive.writestr("logic-1-3", b"456")
                with self.assertRaises(ValueError): extract(directory / "raw.sr", directory)


if __name__ == "__main__": unittest.main()

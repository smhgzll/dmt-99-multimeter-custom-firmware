import ctypes as C
import gzip
import json
from pathlib import Path
import subprocess
import tempfile
import unittest

from dmt99.protocol import catalog, host_ui_status, packets
from dmt99.uart import decode

ROOT = Path(__file__).resolve().parents[1]


class RX(C.Structure):
    _fields_ = [("raw", C.c_uint8 * 13), ("length", C.c_uint8), ("kind", C.c_int),
                ("symbols", C.c_uint8 * 4), ("decimal_after", C.c_uint8),
                ("unit_code", C.c_uint8), ("ac_dc_code", C.c_uint8), ("event_id", C.c_uint8),
                ("negative", C.c_bool), ("status_b7_bit7", C.c_bool), ("numeric", C.c_bool),
                ("mantissa", C.c_int32), ("divisor", C.c_uint16), ("text", C.c_char * 8)]


class CModuleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        library = Path(cls.tmp.name) / "dmt99.so"
        subprocess.run(["gcc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-shared", "-fPIC",
                        str(ROOT / "firmware/dmt99_asic.c"), "-o", str(library)],
                       capture_output=True, check=True)
        cls.lib = C.CDLL(str(library))
        cls.lib.dmt99_parse_rx.argtypes = [C.POINTER(C.c_uint8), C.c_size_t, C.POINTER(RX)]
        cls.lib.dmt99_parse_rx.restype = C.c_bool
        cls.lib.dmt99_build_firmware_template.argtypes = [C.c_uint8, C.POINTER(C.c_uint8)]
        cls.lib.dmt99_build_firmware_template.restype = C.c_bool
        cls.lib.dmt99_resistance_ohms.argtypes = [C.POINTER(RX), C.POINTER(C.c_double)]
        cls.lib.dmt99_resistance_ohms.restype = C.c_bool
        cls.lib.dmt99_is_resistance_open_pattern.argtypes = [C.POINTER(RX)]
        cls.lib.dmt99_is_resistance_open_pattern.restype = C.c_bool
        cls.lib.dmt99_value_si.argtypes = [C.POINTER(RX), C.POINTER(C.c_double), C.POINTER(C.c_int)]
        cls.lib.dmt99_value_si.restype = C.c_bool
        cls.lib.dmt99_host_ui_status.argtypes = [C.POINTER(RX), C.c_uint8, C.POINTER(C.c_uint8)]
        cls.lib.dmt99_host_ui_status.restype = C.c_bool

    @classmethod
    def tearDownClass(cls): cls.tmp.cleanup()

    def parse(self, values):
        result = RX()
        ok = self.lib.dmt99_parse_rx((C.c_uint8 * len(values))(*values), len(values), C.byref(result))
        return ok, result

    def test_real_frame_parsing(self):
        seen = set()
        for path in (ROOT / "tests/fixtures").glob("real_*.json"):
            info = json.loads(path.read_text())
            raw = gzip.decompress(path.with_suffix(".logic.bin.gz").read_bytes())
            records, _ = decode(raw, info["samplerate_hz"], 2, direction="ASIC_TX")
            for packet in packets(records, info["samplerate_hz"], rx_bytes=12):
                if not packet["complete"]: continue
                ok, result = self.parse(packet["values"])
                self.assertTrue(ok)
                self.assertEqual(list(result.raw)[:result.length], packet["values"])
                interpreted = packet["firmware_interpretation"]
                if interpreted["kind"] == "display":
                    self.assertEqual(result.kind, 1)
                    if path.name.startswith(("real_cable_wave_icon_", "real_live_cable_only_")):
                        mode = 9 if path.name.startswith("real_cable_wave_icon_") else 19
                        status = C.c_uint8(99)
                        self.assertTrue(self.lib.dmt99_host_ui_status(C.byref(result), mode, C.byref(status)))
                        self.assertEqual(status.value, host_ui_status(interpreted, mode)["value"])
                        self.assertEqual(status.value, 0)
                        status.value = 99
                        self.assertFalse(self.lib.dmt99_host_ui_status(C.byref(result), 12, C.byref(status)))
                        self.assertEqual(status.value, 99)
                    self.assertEqual(result.text.decode(), interpreted["display_text"])
                    self.assertEqual(self.lib.dmt99_is_resistance_open_pattern(C.byref(result)),
                                     interpreted.get("display_pattern") == "resistance_open_pattern")
                    self.assertEqual(result.numeric, interpreted["numeric_value"] is not None)
                    if result.unit_code == 21:
                        si_value, si_kind = C.c_double(99), C.c_int(99)
                        self.assertFalse(self.lib.dmt99_value_si(C.byref(result), C.byref(si_value), C.byref(si_kind)))
                        self.assertEqual((si_value.value, si_kind.value), (99, 99))
                    if result.numeric:
                        self.assertAlmostEqual(result.mantissa/result.divisor, interpreted["numeric_value"])
                        if result.unit_code in (11, 12, 13):
                            ohms = C.c_double()
                            self.assertTrue(self.lib.dmt99_resistance_ohms(C.byref(result), C.byref(ohms)))
                            self.assertAlmostEqual(ohms.value, interpreted["numeric_value"] * {11:1,12:1000,13:1000000}[result.unit_code])
                        units = {6:(1,1),8:(2,1e-6),9:(2,1e-3),10:(2,1),11:(3,1),12:(3,1000),13:(3,1000000),15:(4,1e-9),18:(5,1)}
                        if result.unit_code in units:
                            quantity, factor = units[result.unit_code]
                            si_value, si_kind = C.c_double(), C.c_int()
                            self.assertTrue(self.lib.dmt99_value_si(C.byref(result), C.byref(si_value), C.byref(si_kind)))
                            expected = interpreted["numeric_value"] * factor
                            self.assertEqual(si_kind.value, quantity)
                            self.assertAlmostEqual(si_value.value, expected, delta=max(abs(expected)*1e-12,1e-24))
                    else:
                        self.assertEqual(result.divisor, 0)
                    seen.add("display12")
                else:
                    self.assertEqual(result.kind, 2)
                    self.assertEqual(result.event_id, interpreted["event_id"])
                    seen.add("event10")
        self.assertEqual(seen, {"display12", "event10"})

    def test_literal_serializer(self):
        for command in catalog()["commands"]:
            literal = bytes.fromhex(command["hex"])
            output = (C.c_uint8 * 10)(*([0xcc] * 10))
            ok = self.lib.dmt99_build_firmware_template(literal[3], output)
            if literal[3] == 0xa0:
                self.assertFalse(ok)
                self.assertEqual(bytes(output), b"\xcc" * 10)
            else:
                self.assertTrue(ok)
                self.assertEqual(bytes(output), literal)

    def test_numeric_policy_and_rejected_input_retention(self):
        # Firmware-derived synthetic segment test, not a physical -1.234V test.
        ok, result = self.parse(bytes.fromhex("5a a5 10 ba 8d 4f 0e 00 02 00 01 3f"))
        self.assertTrue(ok)
        self.assertTrue(result.numeric)
        self.assertEqual((result.mantissa, result.divisor, result.unit_code), (-1234, 1000, 6))
        self.assertEqual(result.text, b"-1.234")
        ok, result = self.parse(b"\x5a\xa5\x01")
        self.assertFalse(ok)
        self.assertEqual(list(result.raw)[:result.length], [0x5a, 0xa5, 1])
        ok, result = self.parse(bytes.fromhex("5a a5 04 00 00 1a 0a 20 00 00 01 3d"))
        self.assertTrue(ok)
        self.assertTrue(result.numeric)
        self.assertEqual((result.mantissa, result.divisor, result.unit_code), (11, 10, 11))
        self.assertEqual(result.text, b"  1.1")
        ohms = C.c_double(42)
        self.assertTrue(self.lib.dmt99_resistance_ohms(C.byref(result), C.byref(ohms)))
        self.assertAlmostEqual(ohms.value, 1.1)
        ok, result = self.parse(bytes.fromhex("5a a5 04 e0 7b 01 00 24 00 00 01 3e"))
        self.assertTrue(ok)
        self.assertTrue(self.lib.dmt99_is_resistance_open_pattern(C.byref(result)))
        ohms.value = 42
        self.assertFalse(self.lib.dmt99_resistance_ohms(C.byref(result), C.byref(ohms)))
        self.assertEqual(ohms.value, 42)

    def test_stream_fuzz_with_address_and_undefined_sanitizers(self):
        import os
        import shutil
        executable = Path(self.tmp.name) / "stream_test"
        local = ROOT / ".local/extracted/usr/lib64"
        link_flags = (["-L" + str(local), "-Wl,-rpath," + str(local)]
                      if (local / "libasan.so").exists() and (local / "libubsan.so").exists() else [])
        compiler = os.environ.get('SANITIZER_CC', 'clang' if shutil.which('clang') else 'gcc')
        subprocess.run([compiler, "-std=c11", "-Wall", "-Wextra", "-Werror", "-g",
                        "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                        *link_flags,
                        "-I" + str(ROOT / "firmware"), str(ROOT / "firmware/dmt99_asic.c"),
                        str(ROOT / "tests/stream_test.c"), "-o", str(executable)],
                       capture_output=True, check=True)
        subprocess.run([str(executable)], capture_output=True, check=True)

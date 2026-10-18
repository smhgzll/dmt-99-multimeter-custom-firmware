"""Differential checks against read-only preexisting reconstructed C, not ASIC emulation."""
import ctypes
from pathlib import Path
import random
import subprocess
import tempfile
import unittest

from dmt99.protocol import display, segment

REFERENCE = Path(__file__).resolve().parents[3] / 'reconstructed'


class Frame(ctypes.Structure):
    _fields_ = [("kind", ctypes.c_int), ("raw", ctypes.c_uint8 * 13),
                ("symbol_codes", ctypes.c_uint8 * 4), ("symbols", ctypes.c_char * 4),
                ("decimal_after", ctypes.c_uint8), ("ac_dc_code", ctypes.c_uint8),
                ("unit_code", ctypes.c_uint8), ("negative", ctypes.c_bool),
                ("high_status_bit", ctypes.c_bool), ("event", ctypes.c_uint8)]


class FirmwareReferenceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        shared = Path(cls.tmp.name) / "firmware_reference.so"
        subprocess.run(["gcc", "-shared", "-fPIC", "-std=c11", "-Wall", "-Wextra", "-Werror",
                        "-I" + str(REFERENCE / "drivers"), "-I" + str(REFERENCE / "ui"),
                        str(REFERENCE / "drivers/measurement_asic.c"), "-o", str(shared)],
                       check=True, capture_output=True)
        cls.lib = ctypes.CDLL(str(shared))
        cls.lib.measurement_segment_decode.argtypes = [ctypes.c_uint8]
        cls.lib.measurement_segment_decode.restype = ctypes.c_uint8
        cls.lib.measurement_decode_frame.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_size_t,
                                                     ctypes.POINTER(Frame)]

    @classmethod
    def tearDownClass(cls): cls.tmp.cleanup()

    def test_all_256_segment_codes(self):
        for value in range(256):
            self.assertEqual(segment(value), self.lib.measurement_segment_decode(value))

    def test_4096_frames_and_unit_priority_combinations(self):
        rng = random.Random(99)
        for _ in range(4096):
            b = bytes([0x5a, 0xa5] + [rng.randrange(256) for _ in range(11)])
            frame = Frame()
            self.lib.measurement_decode_frame((ctypes.c_uint8 * 13).from_buffer_copy(b), 13,
                                               ctypes.byref(frame))
            d = display(b)
            if b[2] == 0xfd:
                self.assertEqual(d["event_id"], frame.event)
                self.assertEqual(frame.kind, 2)
            else:
                self.assertEqual(d["symbol_codes"], list(frame.symbol_codes))
                for name in ("decimal_after", "unit_code", "ac_dc_code", "negative"):
                    self.assertEqual(d[name], getattr(frame, name))
                self.assertEqual(d["status_b7_bit7"], frame.high_status_bit)
                self.assertEqual(frame.kind, 1)


if __name__ == "__main__": unittest.main()

#!/usr/bin/env python3
"""Independent Python oracle for the C exporter, malformed captures and receiver.
All MCU memory is a private synthetic file, or the read-only original BIN.
"""
import argparse
import io
import json
import random
import struct
import subprocess
import tempfile
import unittest
import zlib
from pathlib import Path
from flash_dump_receiver import BASE, BLOCK, END, HEADER, Receiver, compare_files, receive

ROOT = Path(__file__).resolve().parents[1]
REPLAY = ROOT/'reconstructed/build/offline_export/flash-export-replay'


def packet(kind, pas, index, start, size, payload, address=None, passes=2, session=123, status=0):
    if address is None:
        address = start+index*BLOCK if kind == 1 else start+size
    b = HEADER.pack(b'D99X', 1, kind, pas, passes, session, index, address, start, size, len(payload), status, 0)+payload
    return b+struct.pack('<I', zlib.crc32(b))


def frames(data, start=BASE):
    result = []
    for pas in range(2):
        blocks = [data[i:i+BLOCK] for i in range(0, len(data), BLOCK)]
        for i, b in enumerate(blocks):
            result.append(packet(1, pas, i, start, len(data), b))
        result.append(packet(2, pas, len(blocks), start, len(data), struct.pack('<I', zlib.crc32(data))))
    return result


class ExportTests(unittest.TestCase):
    def test_c_export_against_independent_wire_oracle(self):
        rng = random.Random(0xd99)
        with tempfile.TemporaryDirectory() as t:
            path = Path(t)/'synthetic.bin'
            for n in (1, 255, 256, 257, 513, 0x6400, END-BASE):
                data = rng.randbytes(n)
                path.write_bytes(data)
                wire = subprocess.check_output([str(REPLAY), str(path), hex(BASE), hex(BASE), str(n), '123'])
                self.assertEqual(wire, b''.join(frames(data)))
                for step in (1, 31, 32, 33, 291, 4096):
                    r = Receiver(BASE, n, 123)
                    for i in range(0, len(wire), step):
                        r.feed(wire[i:i+step])
                    image, report = r.finish()
                    self.assertEqual(image, data)
                    self.assertTrue(report['identical_passes'])
                    self.assertFalse(report['hardware_origin_verified'])

    def test_original_bin_not_full_mcu_fixture(self):
        path = ROOT/'DMT-99_V2.4_20240306.bin'
        data = path.read_bytes()
        start = BASE+0x6400
        wire = subprocess.check_output([str(REPLAY), str(path), hex(start), hex(start), str(len(data)), '123'])
        image, result = receive(io.BytesIO(wire), start, len(data), 123)
        self.assertEqual(image, data)
        self.assertEqual(result['bytes'], 135096)
        self.assertEqual(wire, b''.join(frames(data, start)))
        bad = subprocess.run([str(REPLAY), str(path), hex(start), hex(BASE), '256', '123'], capture_output=True)
        self.assertNotEqual(bad.returncode, 0)
        self.assertEqual(bad.stdout, b'')  # missing prefix is never fabricated

    def test_noise_duplicates_reordering_corruption_and_missing_blocks(self):
        data = bytes(range(256))+b'last'
        f = frames(data)
        ordered = [f[1], f[0], f[0], f[2], f[4], f[3], f[5]]
        corrupt = bytearray(f[0]);corrupt[40] ^= 1
        image, result = receive(io.BytesIO(b'log\n'+bytes(corrupt)+b''.join(ordered)+b'\n'), BASE, len(data), 123)
        self.assertEqual(image, data)
        self.assertEqual(result['duplicate_frames'], 1)
        self.assertGreater(result['bad_crc_candidates'], 0)
        for wire in (b''.join(f[1:]), b''.join(f[:-1]), b''.join(f)[:-1], b'', b''.join(f)+b'D99X'):
            with self.assertRaises(ValueError):receive(io.BytesIO(wire), BASE, len(data), 123)

    def test_metadata_errors_repeats_and_end_crc(self):
        data = b'a'*257
        f = frames(data)
        invalid = [
            packet(1, 0, 0, BASE, len(data), data[:256], address=BASE+1),
            packet(1, 0, 0, BASE, len(data), data[:256], session=999),
            packet(1, 0, 0, BASE, len(data), data[:256], passes=1),
            packet(1, 0, 2, BASE, len(data), b'a'),
            packet(1, 0, 0, BASE, len(data), b'a'),
            packet(1, 0, 0, BASE, len(data), data[:256], status=1),
            packet(3, 0, 0, BASE, len(data), b'', status=2),
            packet(2, 0, 0, BASE, len(data), b'1234')]
        for frame in invalid:
            with self.assertRaises(ValueError):receive(io.BytesIO(frame+b''.join(f)), BASE, len(data), 123)
        # Every individual pass CRC can be valid while the two images disagree.
        changed = frames(b'b'*257)[3:]
        with self.assertRaisesRegex(ValueError, 'repeated reads differ'):
            receive(io.BytesIO(b''.join(f[:3]+changed)), BASE, len(data), 123)
        with self.assertRaisesRegex(ValueError, 'conflicting duplicate'):
            receive(io.BytesIO(f[0]+packet(1, 0, 0, BASE, len(data), b'b'*256)+b''.join(f)), BASE, len(data), 123)
        wrong_end = packet(2, 0, 2, BASE, len(data), b'1234')
        with self.assertRaisesRegex(ValueError, 'pass CRC mismatch'):
            receive(io.BytesIO(b''.join(f[:2]+[wrong_end]+f[3:])), BASE, len(data), 123)

    def test_boundary_policy(self):
        for start, n in ((BASE-1, 1), (END, 1), (BASE, 0), (END-1, 2), (0xfffffff0, 64)):
            with self.assertRaises(ValueError):Receiver(start, n, 123)
        image, _ = receive(io.BytesIO(b''.join(frames(b'x', END-1))), END-1, 1, 123)
        self.assertEqual(image, b'x')

    def test_host_cli_preserves_outputs_and_compares_files(self):
        with tempfile.TemporaryDirectory() as t:
            d = Path(t);cap=d/'cap';out=d/'dump';rep=d/'report.json';other=d/'other'
            data=b'original'*100;cap.write_bytes(b''.join(frames(data)))
            cmd=['python3', str(ROOT/'tools/flash_dump_receiver.py'), 'receive', str(cap), '--start', hex(BASE), '--length', str(len(data)), '--session', '123', '--output', str(out), '--report', str(rep)]
            subprocess.run(cmd, check=True, capture_output=True)
            self.assertEqual(out.read_bytes(), data)
            self.assertFalse(json.loads(rep.read_text())['hardware_origin_verified'])
            self.assertNotEqual(subprocess.run(cmd, capture_output=True).returncode, 0)
            self.assertEqual(out.read_bytes(), data)
            other.write_bytes(data);self.assertTrue(compare_files(out, other)['identical'])
            other.write_bytes(data[:-1]+b'!');self.assertFalse(compare_files(out, other)['identical'])
            self.assertEqual(compare_files(out, other)['different_bytes_in_overlap'], 1)
            cmp=['python3', str(ROOT/'tools/flash_dump_receiver.py'), 'compare', str(out), str(other)]
            self.assertEqual(subprocess.run(cmp, capture_output=True).returncode, 1)
            # Failure never publishes a dump, and mismatched known data is rejected.
            out.unlink();rep.unlink();cap.write_bytes(cap.read_bytes()[:-1])
            self.assertNotEqual(subprocess.run(cmd, capture_output=True).returncode, 0)
            self.assertFalse(out.exists())
            cap.write_bytes(b''.join(frames(data)))
            self.assertNotEqual(subprocess.run(cmd+['--known-bin',str(ROOT/'DMT-99_V2.4_20240306.bin')], capture_output=True).returncode,0)
            self.assertFalse(out.exists())
            known = ROOT/'DMT-99_V2.4_20240306.bin'
            cap.write_bytes(b''.join(frames(known.read_bytes(), BASE+0x6400)))
            good = cmd.copy()
            good[good.index('--start')+1] = hex(BASE+0x6400)
            good[good.index('--length')+1] = str(known.stat().st_size)
            subprocess.run(good+['--known-bin',str(known)], check=True, capture_output=True)
            self.assertEqual(out.read_bytes(), known.read_bytes())
            self.assertEqual(json.loads(rep.read_text())['known_application_identical_bytes'],135096)


if __name__ == '__main__':
    p = argparse.ArgumentParser();p.add_argument('--replay',type=Path);args,rest=p.parse_known_args()
    if args.replay:REPLAY=args.replay.resolve()
    unittest.main(argv=['test_flash_export']+rest)

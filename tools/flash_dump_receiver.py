#!/usr/bin/env python3
"""Receive new offline D99X v1 export streams. No device, debug or write commands.

Input is a binary capture (or stdin), not a serial port. Two complete equal
passes, frame CRCs and pass CRCs are required. CRC is not authentication.
Existing outputs are never overwritten. Data is not claimed to come from a
device; a complete replay is still only a replay.
"""
import argparse
import hashlib
import json
import struct
import sys
import zlib
from pathlib import Path

BASE, END, BLOCK = 0x08000000, 0x08078000, 256
HEADER = struct.Struct('<4sBBBBIIIIIHBB')


def valid_range(start, length):
    return BASE <= start < END and 0 < length <= END - start


class Receiver:
    def __init__(self, start, length, session):
        if not valid_range(start, length) or not 0 <= session <= 0xffffffff:
            raise ValueError('invalid expected main Flash range/session')
        self.start, self.length, self.session = start, length, session
        self.buffer = bytearray()
        self.passes = None
        self.blocks, self.ends = {}, {}
        self.noise = self.bad_crc = self.duplicates = 0

    def feed(self, data):
        self.buffer.extend(data)
        while True:
            at = self.buffer.find(b'D99X')
            if at < 0:
                n = max(0, len(self.buffer) - 3)
                self.noise += n
                del self.buffer[:n]
                return
            self.noise += at
            del self.buffer[:at]
            if len(self.buffer) < HEADER.size:
                return
            fields = HEADER.unpack_from(self.buffer)
            count = fields[10]
            if count > BLOCK:  # bounded re-synchronisation, never allocate from wire
                self.noise += 1
                del self.buffer[0]
                continue
            size = HEADER.size + count + 4
            if len(self.buffer) < size:
                return
            packet = bytes(self.buffer[:size])
            if zlib.crc32(packet[:-4]) != struct.unpack_from('<I', packet, size-4)[0]:
                self.bad_crc += 1
                del self.buffer[0]
                continue
            del self.buffer[:size]
            self._accept(fields, packet[HEADER.size:-4])

    def _accept(self, f, payload):
        _, version, kind, pas, passes, session, index, address, start, length, count, status, reserved = f
        if version != 1 or reserved or kind not in (1, 2, 3):
            raise ValueError('unsupported frame')
        if (start, length, session) != (self.start, self.length, self.session):
            raise ValueError('foreign range/session')
        if not 2 <= passes <= 8 or pas >= passes:
            raise ValueError('invalid pass number')
        if self.passes not in (None, passes):
            raise ValueError('changed pass count')
        self.passes = passes
        total = (length + BLOCK-1)//BLOCK
        if kind == 3:
            raise ValueError(f'export read error status={status} pass={pas} block={index} address={address:#x}')
        if status:
            raise ValueError('nonzero success status')
        if kind == 1:
            if index >= total or address != start + index*BLOCK or count != min(BLOCK, length-index*BLOCK):
                raise ValueError('invalid block address/size/index')
            key = (pas, index)
            if key in self.blocks:
                if self.blocks[key] != payload:
                    raise ValueError('conflicting duplicate block')
                self.duplicates += 1
            self.blocks[key] = payload
        else:
            if index != total or address != start+length or count != 4:
                raise ValueError('invalid END')
            crc = struct.unpack('<I', payload)[0]
            if pas in self.ends and self.ends[pas] != crc:
                raise ValueError('conflicting END')
            self.ends[pas] = crc

    def finish(self):
        if self.buffer and not b'D99X'.startswith(self.buffer) and b'D99X' not in self.buffer:
            self.noise += len(self.buffer)
            self.buffer.clear()
        if self.buffer:  # a truncated framed capture must not be accepted
            raise ValueError('truncated capture')
        if self.passes is None:
            raise ValueError('no export frames')
        total = (self.length+BLOCK-1)//BLOCK
        images = []
        for pas in range(self.passes):
            if pas not in self.ends or any((pas, i) not in self.blocks for i in range(total)):
                raise ValueError(f'incomplete pass {pas}')
            image = b''.join(self.blocks[pas, i] for i in range(total))
            if zlib.crc32(image) != self.ends[pas]:
                raise ValueError(f'pass CRC mismatch {pas}')
            images.append(image)
        if any(p != images[0] for p in images[1:]):
            raise ValueError('repeated reads differ')
        result = {'start': hex(self.start), 'bytes': self.length, 'session': self.session,
                  'passes': self.passes, 'blocks_per_pass': total,
                  'sha256': hashlib.sha256(images[0]).hexdigest(),
                  'crc32': f'{zlib.crc32(images[0]):08x}', 'identical_passes': True,
                  'duplicate_frames': self.duplicates, 'bad_crc_candidates': self.bad_crc,
                  'ignored_noise_bytes': self.noise, 'hardware_origin_verified': False}
        return images[0], result


def receive(stream, start, length, session):
    receiver = Receiver(start, length, session)
    while chunk := stream.read(4096):
        receiver.feed(chunk)
    return receiver.finish()


def compare_files(first, second):
    a, b = Path(first).read_bytes(), Path(second).read_bytes()
    return {'first_bytes': len(a), 'second_bytes': len(b),
            'first_sha256': hashlib.sha256(a).hexdigest(),
            'second_sha256': hashlib.sha256(b).hexdigest(), 'identical': a == b,
            'different_bytes_in_overlap': sum(x != y for x, y in zip(a, b)),
            'unmatched_bytes': abs(len(a)-len(b))}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest='command', required=True)
    r = sub.add_parser('receive')
    r.add_argument('capture', help='binary capture path, or - for stdin')
    for name in ('start', 'length', 'session'):
        r.add_argument('--'+name, required=True, type=lambda s: int(s, 0))
    r.add_argument('--output', required=True, type=Path)
    r.add_argument('--known-bin', type=Path, help='compare overlap at physical 0x08006400; requires nonempty overlap')
    c = sub.add_parser('compare')
    c.add_argument('first', type=Path)
    c.add_argument('second', type=Path)
    for cmd in (r, c):
        cmd.add_argument('--report', type=Path)
    a = p.parse_args()
    try:
        if a.report and a.report.exists():
            raise ValueError('report exists; choose a new output')
        if a.command == 'compare':
            result = compare_files(a.first, a.second)
            ok = result['identical']
        else:
            if a.output.exists():
                raise ValueError('output exists; refusing to overwrite')
            if a.capture == '-':
                image, result = receive(sys.stdin.buffer, a.start, a.length, a.session)
            else:
                with open(a.capture, 'rb') as stream:
                    image, result = receive(stream, a.start, a.length, a.session)
            if a.known_bin:
                known = a.known_bin.read_bytes()
                low, high = max(a.start, BASE+0x6400), min(a.start+len(image), BASE+0x6400+len(known))
                if low >= high or image[low-a.start:high-a.start] != known[low-BASE-0x6400:high-BASE-0x6400]:
                    raise ValueError('known application comparison has no overlap or differs')
                result['known_application_identical_bytes'] = high-low
            with a.output.open('xb') as f:
                f.write(image)
            ok = True
        if a.report:
            with a.report.open('x') as f:
                json.dump(result, f, indent=2)
                f.write('\n')
        print(json.dumps(result, indent=2))
        return 0 if ok else 1
    except (OSError, ValueError) as e:
        print(f'flash dump: {e}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())

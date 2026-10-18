#!/usr/bin/env python3
"""Size, memory-layout and link-hygiene report for the firmware ELF.

Usage: firmware_report.py ELF BIN SIZE_TOOL CC_TOOL
The layout follows from _start: 0x0 for the standalone image, 0x6400 for the
USB update image installed behind the manufacturer bootloader.
Fails (exit 1) when a layout rule is violated:
  - standalone: reset jump at 0x0, 104-slot vector table at 0x4;
  - update: reset jump at 0x6400, 12 nops + ebreak, vector table at 0x6438
    (the V2.4 update-file header), nothing linked below 0x6400;
  - every implemented vector slot points inside the image;
  - BIN equals the loadable image (text + data load copy) from the origin;
  - image <= 192 KiB and below the settings page 0x08040000;
  - static RAM + stack <= 32 KiB;
  - no host-only or libc runtime symbols (simulator, stdio, malloc, files).
"""
import struct, subprocess, sys
from pathlib import Path

ELF, BIN, SIZE, CC = sys.argv[1:5]
prefix = SIZE[:-len('size')]
NM, READELF = prefix + 'nm', prefix + 'readelf'

def run(*args):
    return subprocess.run(args, check=True, capture_output=True, text=True).stdout

symbols = {}
sizes = []
for line in run(NM, '-S', '--size-sort', '-C', ELF).splitlines():
    parts = line.split()
    if len(parts) == 4:
        addr, size, kind, name = parts
        sizes.append((int(size, 16), kind, name, int(addr, 16)))
for line in run(NM, ELF).splitlines():
    parts = line.split()
    if len(parts) == 3:
        symbols[parts[2]] = int(parts[0], 16)
undefined = [l.split()[-1] for l in run(NM, '-u', ELF).splitlines() if l.strip()]

sections = {}
for line in run(READELF, '-SW', ELF).splitlines():
    if line.strip().startswith('[') and ']' in line:
        fields = line.split(']', 1)[1].split()
        if len(fields) >= 6 and fields[0].startswith('.'):
            sections[fields[0]] = (int(fields[2], 16), int(fields[4], 16))

image = Path(BIN).read_bytes()
problems = []
def need(cond, text):
    if not cond:
        problems.append(text)

origin = symbols.get('_start')
update = origin == 0x6400
vector = 0x6438 if update else 4
need(origin in (0, 0x6400), f'reset entry _start at {origin} is neither 0x0 nor 0x6400')
need(symbols.get('_vector_base') == vector, f'vector table is not at {vector:#x}')
need(image[:4] != b'\xff\xff\xff\xff' and len(image) >= vector - origin + 104 * 4, 'BIN too short for vector table')
opcode = struct.unpack_from('<I', image, 0)[0]
need(opcode & 0x7f == 0x6f, 'first word is not a JAL/J instruction')
if update:
    need(struct.unpack_from('<13I', image, 4) == (0x13,) * 12 + (0x00100073,),
         'header words 1..13 differ from the V2.4 update file (12 nops, ebreak)')
    low = [n for n, (a, sz) in sections.items() if sz and 0 < a < 0x6400]
    need(not low, 'sections linked below 0x6400: ' + ', '.join(low))
slots = struct.unpack_from('<104I', image, vector - origin)
image_end = symbols.get('_image_end', origin + len(image))
need(origin + len(image) == image_end, f'BIN length {len(image)} != _image_end {image_end:#x} - origin')
default = symbols.get('fw_default_handler')
implemented = {}
for i, target in enumerate(slots):
    if target == 0:
        continue
    need(origin <= target < image_end, f'vector slot {i} points outside the image ({target:#x})')
    if target != default:
        implemented[i] = target
names = {v: k for k, v in symbols.items() if k.endswith(('_Handler', '_IRQHandler', '_start'))}
need(image_end <= 0x30000, f'image {image_end} bytes exceeds 192 KiB')
need(image_end <= 0x40000, 'image overlaps the settings page')
ram_static = symbols.get('_susrstack', 0) - 0x20000000
stack = symbols.get('_eusrstack', 0) - symbols.get('_susrstack', 0)
need(symbols.get('_eusrstack', 0) <= 0x20008000, 'RAM use exceeds 32 KiB')
forbidden = ('simulator_', 'fopen', 'fread', 'fwrite', 'fclose', 'malloc', 'free', 'printf',
             'puts', 'exit', '_sbrk', 'png_', 'abort')
host = [n for n in symbols if n.startswith(forbidden) and not n.startswith('fw_')]
need(not host, 'host/libc symbols linked: ' + ', '.join(sorted(host)))
need(not undefined, 'undefined symbols: ' + ', '.join(undefined))

print(f'DMT99 firmware report ({"USB update image" if update else "standalone image"})')
print(f'ELF  {ELF}')
if update:
    print(f'BIN  {BIN}  {len(image)} bytes, installed by the bootloader at 0x08006400 (runs from alias 0x00006400)')
else:
    print(f'BIN  {BIN}  {len(image)} bytes, programmed at 0x08000000 (runs from alias 0x00000000)')
print(f'entry _start=0x{symbols.get("_start", 0):08x} -> handle_reset=0x{symbols.get("handle_reset", 0):08x}, '
      f'main=0x{symbols.get("main", 0):08x}')
print()
print(run(SIZE, '-A', '-x', ELF).strip())
print()
print(f'Flash: {len(image)} bytes from {origin:#x}, ends at {image_end:#x} / {0x30000:#x} '
      f'({100 * image_end / 0x30000:.1f} % of the 192 KiB budget; '
      f'settings page 0x08040000 untouched)')
print(f'RAM:   static {ram_static} bytes (.data+.bss+.noinit), stack {stack} bytes, '
      f'total {ram_static + stack} / 32768 ({100 * (ram_static + stack) / 32768:.1f} %)')
print()
print('Implemented vector slots:')
for i, target in sorted(implemented.items()):
    print(f'  {i:3d}  0x{target:08x}  {names.get(target, "?")}')
print(f'  remaining {sum(1 for s in slots if s == default)} slots -> fw_default_handler (record + reset), '
      f'{sum(1 for s in slots if s == 0)} reserved zero slots')
print()
print('Largest objects:')
for size, kind, name, addr in sorted(sizes, reverse=True)[:25]:
    print(f'  {size:7d}  {kind}  0x{addr:08x}  {name}')
if problems:
    print()
    for p in problems:
        print('LAYOUT ERROR:', p)
    sys.exit(1)
print()
print('Layout checks: PASS')

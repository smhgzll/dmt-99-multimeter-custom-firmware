#!/usr/bin/env python3
"""Compose a standalone restore image from the manufacturer V2.4 application.

The V2.4 BIN is linked to run at 0x6400 (Code Flash 0x08006400) behind a
25 KiB prefix that is not available. Its own reset routine (0xc668) sets gp,
sp, copies .data, clears .bss, writes CORECFGR/INTSYSCR/mstatus/mtvec, calls
SystemInit and enters main, so nothing from the prefix is needed to run it.
This tool writes:

  0x08000000  j 0x6400            (4 bytes, RV32 JAL x0)
  0x08000004  0xFF fill up to 0x6400
  0x08006400  the unmodified V2.4 BIN (SHA-256 checked)

Flashing this image after RDP removal restores the original measurement
firmware. It does NOT restore the lost prefix (presumably the USB mass-storage
updater): firmware/logo updates over USB will not work. The original image is
the user's own file; this tool only rearranges it.

Usage: make_restore_image.py ORIGINAL_BIN OUTPUT_BIN
"""
import hashlib, struct, sys
from pathlib import Path

EXPECTED = '4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e'


def jal_x0(offset):
    """Encode JAL x0, offset (J-type, offset even, +-1 MiB)."""
    assert offset % 2 == 0 and -(1 << 20) <= offset < (1 << 20)
    imm = offset & 0x1fffff
    return (((imm >> 20) & 1) << 31 | ((imm >> 1) & 0x3ff) << 21 | ((imm >> 11) & 1) << 20 |
            ((imm >> 12) & 0xff) << 12 | 0x6f)


def main():
    original, output = Path(sys.argv[1]), Path(sys.argv[2])
    data = original.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED or len(data) != 135096:
        sys.exit(f'{original}: unexpected image (sha256 {digest}, {len(data)} bytes)')
    image = bytearray(b'\xff' * 0x6400)
    struct.pack_into('<I', image, 0, jal_x0(0x6400))
    image += data
    output.write_bytes(image)
    print(f'{output}: {len(image)} bytes, entry j 0x6400 (0x{jal_x0(0x6400):08x}), '
          f'sha256 {hashlib.sha256(image).hexdigest()}')


if __name__ == '__main__':
    main()

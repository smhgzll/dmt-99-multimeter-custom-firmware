"""Format rules for the USB update image (shared by make_release.py and
test_update_image.py; standard library only).

The meter's bootloader installs a raw application BIN at Code Flash 0x08006400
(alias 0x6400). DMT-99_V2.4_20240306.bin starts with a jump, 12 nops and an
ebreak, followed at +0x38 by a 104-slot vector table whose slot 0 is 0x6400.
The bootloader itself has never been read, so these rules describe the V2.4
file, not a documented bootloader contract.
"""
import struct

ORIGIN = 0x6400
VECTOR = 0x6438
CODE_LIMIT = 0x30000          # smallest standard zero-wait code size (192 KiB)
FLASH_BASE = 0x08000000


def jal_target(word, pc):
    imm = ((word >> 31 & 1) << 20 | (word >> 12 & 0xff) << 12 | (word >> 20 & 1) << 11 |
           (word >> 21 & 0x3ff) << 1)
    return pc + (imm - (1 << 21) if imm & (1 << 20) else imm)


def fw_info(image):
    """(load address, version) from the .fw_info record, or (None, None)."""
    at = image.find(b'DMT99FW1')
    if at < 0:
        return None, None
    load = struct.unpack_from('<I', image, at + 12)[0]
    return load, image[at + 20:at + 44].split(b'\0')[0].decode()


def check(image, original, version=None):
    """Return (problems, info) for an update image against the V2.4 file."""
    problems = []
    if len(image) < VECTOR - ORIGIN + 104 * 4:
        return ['image too short for the header and vector table'], {}
    words = struct.unpack_from('<14I', image)
    if words[0] & 0x7f != 0x6f:
        problems.append('first word is not a jump')
    if words[1:14] != struct.unpack_from('<13I', original, 4):
        problems.append('header words 1..13 differ from the V2.4 update file (12 nops, ebreak)')
    slots = struct.unpack_from('<104I', image, VECTOR - ORIGIN)
    if slots[0] != ORIGIN or slots[1] != 0:
        problems.append(f'vector slots 0/1 are {slots[0]:#x}/{slots[1]:#x}, V2.4 has {ORIGIN:#x}/0')
    end = ORIGIN + len(image)
    outside = [i for i, t in enumerate(slots) if t and not ORIGIN <= t < end]
    if outside:
        problems.append(f'vector slots outside the image: {outside}')
    if end > CODE_LIMIT:
        problems.append(f'image ends at {end:#x}, beyond 192 KiB')
    reset = jal_target(words[0], ORIGIN)
    if not VECTOR + 104 * 4 <= reset < end:
        problems.append(f'reset jump target {reset:#x} outside the code')
    load, text = fw_info(image)
    if load != FLASH_BASE + ORIGIN:
        problems.append(f'.fw_info load address is {load}, expected 0x08006400')
    if version is not None and text != version:
        problems.append(f'.fw_info version {text!r} != {version!r}')
    return problems, {'size': len(image), 'flash_start': FLASH_BASE + ORIGIN, 'flash_end': FLASH_BASE + end,
                      'reset': reset, 'version': text}

#!/usr/bin/env python3
"""Test fixtures for installable themes (ui/theme.h).

build_package() writes the THEME.PKG format; with_boot_file() returns a copy
of an 8 MiB flash image with one file added to the root of the BOOT volume
(volume 0), the way the bootloader's USB disk stores a copied file. Used by
test_update_image.py to run the real firmware installer in the emulator.
"""
import io
import struct
import zlib


def crc32(data):
    return zlib.crc32(data) & 0xFFFFFFFF


def build_package(name, files, palette=None):
    """files: {assets path: JPEG bytes}, palette: {rgb565: rgb565} -> THEME.PKG bytes."""
    entries = [(p.encode('ascii'), d) for p, d in sorted(files.items())]
    if palette:
        entries.append((b'THEME.PAL', b''.join(struct.pack('<HH', a, b) for a, b in sorted(palette.items()))))
    data_offset = 48 + sum(13 + len(p) for p, _ in entries)
    total = data_offset + sum(len(d) for _, d in entries) + 4
    out = bytearray(b'DMT99THM' + struct.pack('<HHI', 1, len(entries), total) +
                    name.encode('ascii')[:24].ljust(24, b'\0') + struct.pack('<II', 48, data_offset))
    offset = data_offset
    for p, d in entries:
        out += struct.pack('<IIIB', offset, len(d), crc32(d), len(p)) + p
        offset += len(d)
    for _, d in entries:
        out += d
    out += struct.pack('<I', crc32(bytes(out)))
    return bytes(out)


def solid_jpeg(width, height, rgb):
    from PIL import Image
    buf = io.BytesIO()
    Image.new('RGB', (width, height), rgb).save(buf, 'JPEG', quality=90)
    return buf.getvalue()


def with_boot_file(image, name83, payload):
    """Add NAME.EXT to the BOOT volume root (first free directory slot and clusters)."""
    img = bytearray(image)
    off = struct.unpack_from('<I', img, 446 + 8)[0] * 4096           # partition 1 start
    bps, spc, res, nfat, nroot, total, _, fatsz = struct.unpack_from('<HBHBHHBH', img, off + 11)
    fat = off + res * bps
    root = fat + nfat * fatsz * bps
    data = root + (nroot * 32 + bps - 1) // bps * bps
    clusters = (total - (data - off) // bps) // spc

    def get(c):
        v = img[fat + c + c // 2] | img[fat + c + c // 2 + 1] << 8
        return v >> 4 if c & 1 else v & 0xFFF

    def put(c, v):
        for k in range(nfat):
            p = fat + k * fatsz * bps + c + c // 2
            if c & 1:
                img[p] = (img[p] & 0x0F) | ((v << 4) & 0xF0)
                img[p + 1] = (v >> 4) & 0xFF
            else:
                img[p] = v & 0xFF
                img[p + 1] = (img[p + 1] & 0xF0) | ((v >> 8) & 0x0F)
    cluster_bytes = bps * spc
    need = (len(payload) + cluster_bytes - 1) // cluster_bytes
    # Skip cluster 2: the acquired dump's System Volume Information still points there.
    free = [c for c in range(3, clusters + 2) if get(c) == 0][:need]
    if len(free) < need:
        raise ValueError('BOOT volume full')
    for i, c in enumerate(free):
        put(c, free[i + 1] if i + 1 < len(free) else 0xFFF)
        chunk = payload[i * cluster_bytes:(i + 1) * cluster_bytes]
        p = data + (c - 2) * cluster_bytes
        img[p:p + cluster_bytes] = chunk.ljust(cluster_bytes, b'\0')
    for slot in range(root, root + nroot * 32, 32):
        if img[slot] in (0, 0xE5):
            base, _, ext = name83.upper().partition('.')
            entry = bytearray(32)
            entry[:11] = (base.ljust(8) + ext.ljust(3)).encode('ascii')
            entry[11] = 0x20
            struct.pack_into('<HI', entry, 26, free[0], len(payload))
            terminator = img[slot] == 0
            img[slot:slot + 32] = entry
            if terminator and slot + 32 < root + nroot * 32:
                img[slot + 32] = 0
            return bytes(img)
    raise ValueError('BOOT root directory full')

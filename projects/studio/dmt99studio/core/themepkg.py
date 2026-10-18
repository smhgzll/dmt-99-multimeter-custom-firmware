"""THEME.PKG: theme packages installed by the POTAT-99 firmware.

Copy the file to the meter's bootloader USB disk (hold POWER+MENU, connect
USB). At the next normal start the firmware validates it, copies the images
into a new folder themes/Tnn on the assets volume, activates the theme and
deletes the package. Installed themes are listed, selected and deleted on the
meter under About > ENTER. Original assets are never modified.

An optional entry THEME.PAL (up to 16 pairs of RGB565 from/to, firmware
0.5.0 and later) recolours what the firmware draws itself: the classic-layout
digits, menu text and borders.

Format (little endian, see ui/theme.h in the firmware project):
    0  "DMT99THM"   8 version u16 = 1   10 file count u16   12 total size u32
    16 name[24]     40 directory offset u32 (48)            44 data offset u32
    directory: {offset u32, size u32, crc32 u32, path_len u8, path}
    data, crc32 of all preceding bytes (last 4 bytes)
"""
from __future__ import annotations

import struct
import zlib
from dataclasses import dataclass

MAGIC = b'DMT99THM'
VERSION = 1
HEADER = 48
NAME_LEN = 24
MAX_FILES = 400
PATH_LIMIT = 64
PACKAGE_NAME = 'THEME.PKG'
INF_MAGIC = b'DMT99TI1'
INF2_MAGIC = b'DMT99TI2'
PALETTE_NAME = 'THEME.PAL'
PALETTE_MAX = 16
ALLOWED = set('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-./~')


class PackageError(ValueError):
    pass


def crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


def check_path(path: str) -> None:
    if not (5 <= len(path) <= PATH_LIMIT) or path[0] in '/.' or path.endswith('/'):
        raise PackageError(f'{path!r}: unsupported path length or form')
    if any(c not in ALLOWED for c in path) or '//' in path or '/.' in path:
        raise PackageError(f'{path!r}: only letters, digits and _ - . ~ / are allowed')
    if path.count('/') > 3 or not path.lower().endswith('.jpg'):
        raise PackageError(f'{path!r}: must be a .jpg at most 3 folders deep')


def palette_bytes(palette: dict[int, int]) -> bytes:
    if len(palette) > PALETTE_MAX:
        raise PackageError(f'at most {PALETTE_MAX} palette colours')
    return b''.join(struct.pack('<HH', a, b) for a, b in sorted(palette.items()))


def build(name: str, files: dict[str, bytes], palette: dict[int, int] | None = None) -> bytes:
    """files: assets-volume path ('mode/dc_v.jpg') -> JPEG bytes; palette: RGB565 -> RGB565."""
    if not files and not palette:
        raise PackageError('a theme needs at least one changed image or colour')
    if len(files) > MAX_FILES:
        raise PackageError(f'at most {MAX_FILES} files per theme')
    raw_name = name.encode('ascii', errors='replace')[:NAME_LEN]
    entries = []
    for path in sorted(files):
        check_path(path)
        data = files[path]
        if data[:3] != b'\xff\xd8\xff':
            raise PackageError(f'{path}: not a JPEG')
        entries.append((path.encode('ascii'), data))
    if palette:
        entries.append((PALETTE_NAME.encode('ascii'), palette_bytes(palette)))
    directory_size = sum(13 + len(p) for p, _ in entries)
    data_offset = HEADER + directory_size
    total = data_offset + sum(len(d) for _, d in entries) + 4
    out = bytearray()
    out += MAGIC + struct.pack('<HHI', VERSION, len(entries), total) + raw_name.ljust(NAME_LEN, b'\0')
    out += struct.pack('<II', HEADER, data_offset)
    offset = data_offset
    for p, d in entries:
        out += struct.pack('<IIIB', offset, len(d), crc32(d), len(p)) + p
        offset += len(d)
    for _, d in entries:
        out += d
    out += struct.pack('<I', crc32(bytes(out)))
    assert len(out) == total
    return bytes(out)


@dataclass
class Package:
    name: str
    files: dict[str, bytes]
    size: int
    palette: dict[int, int] = None


def parse(data: bytes) -> Package:
    if len(data) < HEADER + 4 or data[:8] != MAGIC:
        raise PackageError('not a DMT99 theme package')
    version, count, total = struct.unpack_from('<HHI', data, 8)
    if version != VERSION or total != len(data) or not 1 <= count <= MAX_FILES:
        raise PackageError('unsupported version or inconsistent size')
    if struct.unpack_from('<I', data, total - 4)[0] != crc32(data[:-4]):
        raise PackageError('package CRC mismatch')
    name = data[16:16 + NAME_LEN].split(b'\0')[0].decode('ascii', errors='replace')
    directory, data_offset = struct.unpack_from('<II', data, 40)
    files, p = {}, directory
    for _ in range(count):
        off, size, c, n = struct.unpack_from('<IIIB', data, p)
        path = data[p + 13:p + 13 + n].decode('ascii')
        p += 13 + n
        blob = data[off:off + size]
        if crc32(blob) != c or off < data_offset:
            raise PackageError(f'{path}: CRC or offset mismatch')
        files[path] = blob
    palette = {}
    if PALETTE_NAME in files:
        raw = files.pop(PALETTE_NAME)
        palette = {struct.unpack_from('<H', raw, i)[0]: struct.unpack_from('<H', raw, i + 2)[0]
                   for i in range(0, len(raw) - 3, 4)}
    return Package(name, files, total, palette)


def packable(path: str) -> bool:
    try:
        check_path(path)
        return True
    except PackageError:
        return False


def session_files(session, skipped: list | None = None) -> dict[str, bytes]:
    """Changed assets of a session that a package can carry (assets volume, supported names)."""
    files = {}
    for key, data in sorted(session.replacements.items()):
        if not key.startswith('1:/'):
            continue
        entry = session.working.resolve(key)
        path = entry.path if entry else key[3:]
        if packable(path):
            files[path] = data
        elif skipped is not None:
            skipped.append(path)
    return files


def from_session(session, name: str, skipped: list | None = None) -> bytes:
    """Package the changed assets of a ThemeSession. Names the firmware cannot open
    (non-ASCII such as 'mode/组 1.jpg') are left out and reported in `skipped`."""
    return build(name, session_files(session, skipped), getattr(session, 'palette', None) or None)


QUALITY_STEPS = (85, 80, 75, 70, 65, 60, 55, 50, 45, 40)


def invisible_replacements(session, mean_limit: float = 2.5, p999_limit: float = 25.0) -> list[str]:
    """Replacements that differ from the original only by JPEG re-encoding noise
    (for example images an earlier batch re-encoded without recolouring)."""
    import io as _io
    import numpy as np
    from PIL import Image
    keys = []
    for key, data in sorted(session.replacements.items()):
        try:
            a = np.asarray(Image.open(_io.BytesIO(data)).convert('RGB'), dtype=np.int16)
            b = np.asarray(Image.open(_io.BytesIO(session.original_bytes(key))).convert('RGB'), dtype=np.int16)
        except Exception:  # noqa: BLE001 - undecodable replacements stay
            continue
        if a.shape == b.shape:
            d = np.abs(a - b)
            if d.mean() <= mean_limit and np.percentile(d, 99.9) <= p999_limit:
                keys.append(key)
    return keys


def drop_invisible(session, keys: list[str] | None = None) -> int:
    """Restore the originals of `keys` (default: invisible_replacements) as one undo step."""
    keys = invisible_replacements(session) if keys is None else keys
    if keys:
        session.replace_many({k: session.original_bytes(k) for k in keys}, source='unchanged images dropped')
    return len(keys)


def fit_to_size(session, name: str, limit: int, progress=None) -> tuple[int, int]:
    """Re-encode the session's replacement JPEGs at decreasing quality until the
    package is at most `limit` bytes. Applies the result as one undo step and
    returns (quality, package size). Raises PackageError when even the lowest
    quality does not fit."""
    import io as _io
    from PIL import Image
    from . import jpegtools
    files = session_files(session)
    keys = {path: key for key, path in ((k, (session.working.resolve(k).path if session.working.resolve(k) else k[3:]))
                                        for k in session.replacements) if path in files}
    decoded = {path: Image.open(_io.BytesIO(data)).convert('RGB') for path, data in files.items()}
    for step, quality in enumerate(QUALITY_STEPS):
        encoded = {}
        for path, image in decoded.items():
            info = jpegtools.inspect(session.original_bytes(keys[path]))
            sampling = info.subsampling if info.subsampling in jpegtools.PIL_SUBSAMPLING else '4:2:0'
            encoded[path] = jpegtools.encode(image, quality, sampling)
        size = len(build(name, encoded, getattr(session, 'palette', None) or None))
        if progress:
            progress(step + 1, len(QUALITY_STEPS), quality, size)
        if size <= limit:
            session.replace_many({keys[p]: d for p, d in encoded.items()}, source=f'fit to {limit} bytes, quality {quality}')
            return quality, size
    raise PackageError(f'the theme does not fit in {limit // 1024} KiB even at quality {QUALITY_STEPS[-1]}; '
                       'change fewer images or split it into two themes')


def installed_themes(image) -> list[dict]:
    """Themes present in a flash image (themes/Tnn with THEME.INF)."""
    rows = []
    themes = image.assets.find('themes')
    if not themes or not themes.is_dir:
        return rows
    for e in image.assets.walk():
        parts = e.path.split('/')
        if len(parts) == 2 and parts[0].lower() == 'themes' and e.is_dir and len(parts[1]) == 3 \
                and parts[1][0] in 'Tt' and parts[1][1:].isdigit():
            info = image.assets.find(f'{e.path}/THEME.INF')
            row = {'slot': int(parts[1][1:]), 'folder': e.path, 'valid': False, 'name': '(incomplete)'}
            if info:
                inf = image.assets.read(info)
                ok1 = len(inf) == 112 and inf[:8] == INF_MAGIC and crc32(inf[:108]) == struct.unpack_from('<I', inf, 108)[0]
                ok2 = len(inf) == 176 and inf[:8] == INF2_MAGIC and crc32(inf[:172]) == struct.unpack_from('<I', inf, 172)[0]
                if ok1 or ok2:
                    count = inf[34] if ok2 else 0
                    row.update(valid=True, name=inf[8:32].split(b'\0')[0].decode('ascii', 'replace'),
                               files=struct.unpack_from('<H', inf, 32)[0], bytes=struct.unpack_from('<I', inf, 36)[0],
                               palette={struct.unpack_from('<H', inf, 108 + 4 * i)[0]:
                                        struct.unpack_from('<H', inf, 110 + 4 * i)[0] for i in range(count)})
            rows.append(row)
    return sorted(rows, key=lambda r: r['slot'])


def boot_free_bytes(image) -> int:
    """Free space on the bootloader disk (BOOT volume) of an image."""
    return image.boot.usage()['free_bytes']


def simulate_install(image_bytes: bytes, package: bytes, script: tuple[str, ...] = (),
                     timeout: float = 120) -> tuple[bytes, str, str]:
    """Run the firmware's own installer on a copy of an image.

    THEME.PKG is placed on the BOOT volume, dmt99-simulator-release boots
    with FatFs writes enabled (memory copy only) and the result is saved.
    Returns (new image, installer message, simulator stderr).
    """
    import subprocess
    import tempfile
    from pathlib import Path

    from .. import paths
    from .flashimage import FlashImage
    paths.ensure_tools()
    work = FlashImage(image_bytes)
    existing = work.boot.find(PACKAGE_NAME)
    if existing:
        raise PackageError('the image already holds a THEME.PKG on its BOOT volume')
    work.boot.create('', PACKAGE_NAME, package)
    with tempfile.TemporaryDirectory(prefix='dmt99-theme-') as tmp:
        t = Path(tmp)
        src, out = t / 'in.bin', t / 'out.bin'
        src.write_bytes(work.data)
        (t / 'run.sim').write_text('\n'.join(list(script) + [f'flash-save {out}', 'quit']) + '\n')
        r = subprocess.run([str(paths.SIMULATOR), '--flash', str(src), '--writable', '--script', str(t / 'run.sim')],
                           capture_output=True, text=True, timeout=timeout)
        if r.returncode or not out.exists():
            raise PackageError('simulator failed: ' + r.stderr.strip())
        message = next((l.split(':', 1)[1].strip() for l in r.stderr.splitlines() if l.startswith('THEME.PKG:')),
                       'no package processed')
        return out.read_bytes(), message, r.stderr

"""JPEG inspection, conversion and device-decoder validation.

The firmware decodes assets with ChaN TJpgDec R0.01b inside a 0x1c00-byte
work pool. That decoder accepts only:
  - baseline (SOF0) JPEG, 8-bit precision
  - exactly three components (Y, Cb, Cr) -- no grayscale, no CMYK
  - luma sampling 1x1, 2x1 or 2x2 with chroma 1x1 (4:4:4, 4:2:2, 4:2:0)
  - optional restart intervals
Progressive files, arithmetic coding and 12-bit files are rejected.
convert() always produces files inside these limits and validate() runs the
real firmware decoder (dmt99-jpeg-check) on the result.
"""
from __future__ import annotations

import io
import json
import struct
import subprocess
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from PIL import Image

from .. import paths

SOF_NAMES = {0xC0: 'baseline', 0xC1: 'extended sequential', 0xC2: 'progressive', 0xC3: 'lossless',
             0xC9: 'arithmetic', 0xCA: 'progressive arithmetic'}
SUBSAMPLING = {(1, 1): '4:4:4', (2, 1): '4:2:2', (2, 2): '4:2:0'}
PIL_SUBSAMPLING = {'4:4:4': 0, '4:2:2': 1, '4:2:0': 2}


@dataclass
class JpegInfo:
    size: int
    width: int = 0
    height: int = 0
    process: str = 'unknown'
    precision: int = 0
    components: int = 0
    subsampling: str = 'unknown'
    restart_interval: int = 0
    app_segments: list[str] = field(default_factory=list)
    quant_tables: dict[int, list[int]] = field(default_factory=dict)
    quality_estimate: int | None = None
    error: str | None = None

    def as_dict(self) -> dict:
        d = dict(self.__dict__)
        d.pop('quant_tables')
        return d


def inspect(data: bytes) -> JpegInfo:
    info = JpegInfo(len(data))
    if data[:2] != b'\xff\xd8':
        info.error = 'not a JPEG (no SOI marker)'
        return info
    p = 2
    try:
        while p + 4 <= len(data):
            if data[p] != 0xFF:
                info.error = f'marker expected at {p}'
                break
            marker = data[p + 1]
            if marker == 0xD8 or 0xD0 <= marker <= 0xD7 or marker == 0x01:
                p += 2
                continue
            length = struct.unpack_from('>H', data, p + 2)[0]
            seg = data[p + 4:p + 2 + length]
            if 0xE0 <= marker <= 0xEF:
                tag = seg[:5].split(b'\0')[0].decode('latin-1', errors='replace')
                info.app_segments.append(f'APP{marker - 0xE0}:{tag}')
            elif marker == 0xDB:
                q = 0
                while q < len(seg):
                    pq, tq = seg[q] >> 4, seg[q] & 15
                    n = 128 if pq else 64
                    vals = list(seg[q + 1:q + 1 + n]) if not pq else list(struct.unpack_from(f'>{64}H', seg, q + 1))
                    info.quant_tables[tq] = vals
                    q += 1 + n
            elif marker == 0xDD:
                info.restart_interval = struct.unpack_from('>H', seg, 0)[0]
            elif marker in SOF_NAMES or 0xC0 <= marker <= 0xCF and marker not in (0xC4, 0xC8, 0xCC):
                info.process = SOF_NAMES.get(marker, f'SOF{marker - 0xC0}')
                info.precision = seg[0]
                info.height, info.width = struct.unpack_from('>HH', seg, 1)
                info.components = seg[5]
                if info.components >= 1:
                    hv = seg[7]
                    info.subsampling = SUBSAMPLING.get((hv >> 4, hv & 15), f'{hv >> 4}x{hv & 15}')
                    if info.components == 3 and any(seg[7 + 3 * i] != 0x11 for i in (1, 2)):
                        info.subsampling += ' (chroma not 1x1)'
            elif marker == 0xDA:
                break
            p += 2 + length
    except (struct.error, IndexError):
        info.error = 'truncated JPEG header'
    info.quality_estimate = estimate_quality(info.quant_tables.get(0))
    return info


# libjpeg's standard luminance table (zigzag order as stored in DQT).
_STD_LUMA = [16, 11, 12, 14, 12, 10, 16, 14, 13, 14, 18, 17, 16, 19, 24, 40, 26, 24, 22, 22, 24, 49, 35, 37, 29, 40,
             58, 51, 61, 60, 57, 51, 56, 55, 64, 72, 92, 78, 64, 68, 87, 69, 55, 56, 80, 109, 81, 87, 95, 98, 103,
             104, 103, 62, 77, 113, 121, 112, 100, 120, 92, 101, 103, 99]


def estimate_quality(table: list[int] | None) -> int | None:
    """Closest libjpeg quality factor for a luminance table (approximate)."""
    if not table or len(table) != 64:
        return None
    best, best_err = None, None
    for q in range(1, 101):
        scale = 5000 / q if q < 50 else 200 - 2 * q
        ref = [min(255, max(1, (v * scale + 50) // 100)) for v in _STD_LUMA]
        err = sum(abs(a - b) for a, b in zip(ref, table))
        if best_err is None or err < best_err:
            best, best_err = q, err
    return best


def static_problems(info: JpegInfo) -> list[str]:
    """Reasons the firmware decoder would reject the file, from headers alone."""
    out = []
    if info.error:
        out.append(info.error)
    if info.process != 'baseline':
        out.append(f'{info.process} JPEG; the device decoder supports baseline only')
    if info.precision and info.precision != 8:
        out.append(f'{info.precision}-bit precision; only 8-bit is supported')
    if info.components and info.components != 3:
        out.append(f'{info.components} component(s); the device decoder needs Y/Cb/Cr colour (3)')
    if info.subsampling not in ('4:4:4', '4:2:2', '4:2:0'):
        out.append(f'sampling {info.subsampling} not supported')
    return out


@dataclass
class DeviceDecode:
    ok: bool
    prepare: int
    decode: int
    width: int
    height: int
    pool: int
    rgb565: bytes | None = None
    message: str = ''

    def image(self) -> Image.Image | None:
        """The decoded pixels as the LCD shows them (RGB565 expanded to RGB8)."""
        if not self.rgb565 or not self.width:
            return None
        px = memoryview(self.rgb565).cast('H')
        out = bytearray(self.width * self.height * 3)
        for i, v in enumerate(px):
            r, g, b = v >> 11, (v >> 5) & 63, v & 31
            out[i * 3:i * 3 + 3] = bytes(((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)))
        return Image.frombytes('RGB', (self.width, self.height), bytes(out))


JDR = ['OK', 'interrupted', 'input error', 'insufficient memory pool', 'insufficient stream buffer',
       'parameter error', 'data format error', 'unsupported format', 'unsupported JPEG standard']


def device_decode(data: bytes, want_pixels: bool = True) -> DeviceDecode:
    """Decode with the firmware's TJpgDec build (same pool size as the meter)."""
    paths.ensure_tools()
    with tempfile.TemporaryDirectory(prefix='dmt99-jpeg-') as tmp:
        src = Path(tmp) / 'in.jpg'
        src.write_bytes(data)
        args = [str(paths.JPEG_CHECK), str(src)]
        out = Path(tmp) / 'out.rgb565'
        if want_pixels:
            args.append(str(out))
        r = subprocess.run(args, capture_output=True, text=True, timeout=30)
        try:
            row = json.loads(r.stdout.strip().splitlines()[-1])
        except (IndexError, json.JSONDecodeError):
            return DeviceDecode(False, -1, -1, 0, 0, 0, None, r.stderr.strip() or 'decoder produced no result')
        ok = r.returncode == 0
        pixels = out.read_bytes() if ok and want_pixels and out.exists() else None
        msg = 'decoded by the device decoder' if ok else \
            f'device decoder: prepare={JDR[row["prepare"]] if 0 <= row["prepare"] < len(JDR) else row["prepare"]}, ' \
            f'decode={JDR[row["decode"]] if 0 <= row["decode"] < len(JDR) else row["decode"]}'
        return DeviceDecode(ok, row['prepare'], row['decode'], row['width'], row['height'], row['pool'], pixels, msg)


@dataclass
class Compatibility:
    ok: bool
    errors: list[str]
    warnings: list[str]
    info: JpegInfo
    decode: DeviceDecode | None


def validate(data: bytes, expected_size: tuple[int, int] | None = None, run_decoder: bool = True,
             max_bytes: int | None = None) -> Compatibility:
    info = inspect(data)
    errors = static_problems(info)
    warnings = []
    if info.restart_interval:
        warnings.append(f'restart interval {info.restart_interval} (supported, but the originals use none)')
    extra = [a for a in info.app_segments if not a.startswith('APP0:JFIF')]
    if extra:
        warnings.append('metadata segments ' + ', '.join(extra) + ' waste flash space')
    if expected_size and (info.width, info.height) != tuple(expected_size):
        errors.append(f'size {info.width}x{info.height} differs from the original {expected_size[0]}x{expected_size[1]}; '
                      'the firmware draws at fixed positions, so the layout would break')
    if max_bytes is not None and len(data) > max_bytes:
        errors.append(f'{len(data)} bytes exceed the free space ({max_bytes} bytes)')
    decode = None
    if run_decoder and not static_problems(info):
        decode = device_decode(data)
        if not decode.ok:
            errors.append(decode.message)
    return Compatibility(not errors, errors, warnings, info, decode)


FIT_MODES = ('stretch', 'fit', 'fill', 'center')


def prepare_image(source: Image.Image, size: tuple[int, int], mode: str = 'stretch',
                  background: tuple[int, int, int] = (0, 0, 0)) -> Image.Image:
    """Bring a source picture to the target size without changing the layout."""
    src = source.convert('RGBA') if source.mode in ('P', 'LA', 'RGBA', 'PA') else source.convert('RGB')
    w, h = size
    canvas = Image.new('RGB', size, background)
    if mode == 'stretch':
        im = src.resize(size, Image.LANCZOS)
        canvas.paste(im, (0, 0), im if im.mode == 'RGBA' else None)
        return canvas
    if mode == 'center':
        im = src
    else:
        scale = min(w / src.width, h / src.height) if mode == 'fit' else max(w / src.width, h / src.height)
        im = src.resize((max(1, round(src.width * scale)), max(1, round(src.height * scale))), Image.LANCZOS)
    x, y = (w - im.width) // 2, (h - im.height) // 2
    canvas.paste(im, (x, y), im if im.mode == 'RGBA' else None)
    return canvas


def encode(image: Image.Image, quality: int = 90, subsampling: str = '4:2:0') -> bytes:
    """Baseline, 3-component JPEG without metadata (device compatible)."""
    buf = io.BytesIO()
    image.convert('RGB').save(buf, 'JPEG', quality=int(quality), subsampling=PIL_SUBSAMPLING.get(subsampling, 2),
                              progressive=False, optimize=False, exif=b'', icc_profile=None)
    return buf.getvalue()


def convert(source: Image.Image, original: bytes | None = None, size: tuple[int, int] | None = None,
            mode: str = 'stretch', quality: int | None = None, subsampling: str | None = None,
            background: tuple[int, int, int] = (0, 0, 0)) -> bytes:
    """Produce a replacement for `original` (keeps its size and sampling)."""
    ref = inspect(original) if original else None
    if size is None:
        if not ref or not ref.width:
            raise ValueError('target size unknown')
        size = (ref.width, ref.height)
    if subsampling is None:
        subsampling = ref.subsampling if ref and ref.subsampling in PIL_SUBSAMPLING else '4:2:0'
    if quality is None:
        quality = max(75, ref.quality_estimate or 90) if ref else 90
    return encode(prepare_image(source, size, mode, background), quality, subsampling)


def load_source(path: str | Path) -> Image.Image:
    im = Image.open(path)
    im.load()
    return im

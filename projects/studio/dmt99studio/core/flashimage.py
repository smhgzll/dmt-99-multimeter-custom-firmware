"""The DMT99 external Winbond W25Q64JV image (8 MiB).

Verified layout of the acquired dump (4096-byte sectors):

    LBA 0..62      MBR and reserved area
    LBA 63..462    BOOT FAT12: the bootloader's USB disk (update BIN drop
                   zone) and the optional custom logo LOGO.jpg
    LBA 463..2047  assets FAT12 (menu/, mode/, units/, num/, ...)

A FlashImage is always a working copy in memory. The file it was loaded
from is opened read-only and is never written; save_as() refuses to write
over its source or over any file whose content is the original dump.
"""
from __future__ import annotations

import hashlib
import os
import struct
import tempfile
from dataclasses import dataclass
from pathlib import Path

from .fat12 import DirEntry, FatError, FatVolume

FLASH_SIZE = 8 * 1024 * 1024
SECTOR = 4096
SECTORS = FLASH_SIZE // SECTOR
# The user's acquired dump flashdump/dmt99_flash_1.bin.
ORIGINAL_DUMP_SHA256 = 'fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69'

REGIONS = {
    'mbr': (0, 63),
    'boot': (63, 400),
    'assets': (463, 1585),
}
# Writable over USB: the assets partition only (see DEVICE_PROTOCOL.md).
ASSET_FIRST_SECTOR, ASSET_SECTORS = REGIONS['assets']


@dataclass
class Partition:
    index: int
    bootable: bool
    type: int
    first_sector: int    # in 4096-byte units, as stored in the MBR of this device
    sectors: int

    @property
    def offset(self) -> int:
        return self.first_sector * SECTOR

    @property
    def length(self) -> int:
        return self.sectors * SECTOR


def parse_mbr(image: bytes) -> list[Partition]:
    if len(image) < 512 or image[510:512] != b'\x55\xaa':
        raise FatError('MBR signature missing')
    out = []
    for i in range(4):
        e = image[446 + 16 * i:462 + 16 * i]
        first, count = struct.unpack_from('<II', e, 8)
        if e[4] and count:
            out.append(Partition(i, e[0] == 0x80, e[4], first, count))
    return out


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


class FlashImage:
    def __init__(self, data: bytes, source: Path | None = None, label: str = ''):
        if len(data) != FLASH_SIZE:
            raise FatError(f'expected an 8 MiB image, got {len(data)} bytes')
        self.data = bytearray(data)
        self.baseline = bytes(data)          # what this working copy started from
        self.source = Path(source).resolve() if source else None
        self.source_sha256 = sha256(self.baseline)
        self.label = label or (self.source.name if self.source else 'image')
        self.partitions = parse_mbr(self.baseline)
        self.volumes: list[FatVolume] = []
        self.volume_errors: list[str] = []
        for p in self.partitions:
            try:
                self.volumes.append(FatVolume(self.data, p.offset, p.length, p.index))
            except FatError as ex:
                self.volume_errors.append(str(ex))
        self._index: dict[tuple[int, str], DirEntry] | None = None

    # ------------------------------------------------------------- loading
    @classmethod
    def load(cls, path: str | os.PathLike) -> 'FlashImage':
        p = Path(path)
        with open(p, 'rb') as f:          # read-only by construction
            data = f.read(FLASH_SIZE + 1)
        return cls(data, p)

    @property
    def is_original_dump(self) -> bool:
        return self.source_sha256 == ORIGINAL_DUMP_SHA256

    @property
    def sha256(self) -> str:
        return sha256(bytes(self.data))

    # ------------------------------------------------------------ volumes
    def volume(self, index: int) -> FatVolume:
        for v in self.volumes:
            if v.index == index:
                return v
        raise FatError(f'volume {index} not present')

    @property
    def boot(self) -> FatVolume:
        return self.volume(0)

    @property
    def assets(self) -> FatVolume:
        return self.volume(1)

    def entries(self) -> list[DirEntry]:
        return [e for v in self.volumes for e in v.walk()]

    def index(self) -> dict[tuple[int, str], DirEntry]:
        if self._index is None:
            self._index = {}
            for e in self.entries():
                self._index[(e.volume, e.path.lower())] = e
                parts = e.path.split('/')
                parts[-1] = e.short_name
                self._index.setdefault((e.volume, '/'.join(parts).lower()), e)
        return self._index

    def resolve(self, firmware_path: str) -> DirEntry | None:
        """Map a firmware path ('0:/on.jpg', 'mode/dc_v.jpg') to its entry."""
        from .screens import asset_key
        key = asset_key(firmware_path)
        return self.index().get((int(key[0]), key[3:]))

    def read_file(self, entry: DirEntry) -> bytes:
        return self.volume(entry.volume).read(entry)

    def baseline_file(self, entry: DirEntry) -> bytes | None:
        """Contents of the same path in the baseline image (None if absent)."""
        base = FlashImage(self.baseline)
        e = base.index().get((entry.volume, entry.path.lower()))
        return base.read_file(e) if e else None

    def replace_file(self, entry: DirEntry, data: bytes) -> DirEntry:
        if entry.volume != 1:
            raise FatError('only files on the assets volume (1:) can be replaced')
        result = self.volume(entry.volume).replace(entry, data)
        self._index = None
        return result

    def create_file(self, folder: str, short_name: str, data: bytes) -> DirEntry:
        result = self.assets.create(folder, short_name, data)
        self._index = None
        return result

    # ------------------------------------------------------------ analysis
    def validate(self) -> list[str]:
        issues = list(self.volume_errors)
        expected = [(63, 400), (463, 1585)]
        got = [(p.first_sector, p.sectors) for p in self.partitions]
        if got != expected:
            issues.append(f'partition table {got} differs from the DMT99 layout {expected}')
        for v in self.volumes:
            issues += v.validate()
        return issues

    def changed_sectors(self, other: bytes | None = None) -> list[int]:
        ref = self.baseline if other is None else other
        cur = self.data
        return [s for s in range(SECTORS) if cur[s * SECTOR:(s + 1) * SECTOR] != ref[s * SECTOR:(s + 1) * SECTOR]]

    def region_of(self, sector: int) -> str:
        for name, (first, count) in REGIONS.items():
            if first <= sector < first + count:
                return name
        return 'unpartitioned'

    def changed_files(self, other: bytes | None = None) -> list[dict]:
        """Files whose contents differ from the baseline (or another image)."""
        base = FlashImage(self.baseline if other is None else other)
        old = {(e.volume, e.path.lower()): e for e in base.entries() if not e.is_dir}
        rows = []
        for e in self.entries():
            if e.is_dir:
                continue
            key = (e.volume, e.path.lower())
            new_data = self.read_file(e)
            if key not in old:
                rows.append({'volume': e.volume, 'path': e.path, 'change': 'added', 'size': e.size})
                continue
            old_data = base.read_file(old.pop(key))
            if old_data != new_data:
                rows.append({'volume': e.volume, 'path': e.path, 'change': 'modified', 'size': e.size,
                             'old_size': len(old_data), 'sha256': sha256(new_data), 'old_sha256': sha256(old_data)})
        for (vol, _), e in old.items():
            rows.append({'volume': vol, 'path': e.path, 'change': 'removed', 'size': e.size})
        return rows

    def diff_report(self, other: bytes | None = None) -> dict:
        sectors = self.changed_sectors(other)
        by_region: dict[str, int] = {}
        for s in sectors:
            by_region[self.region_of(s)] = by_region.get(self.region_of(s), 0) + 1
        return {'baseline_sha256': sha256(self.baseline if other is None else other), 'image_sha256': self.sha256,
                'changed_sectors': sectors, 'changed_by_region': by_region, 'files': self.changed_files(other)}

    def binary_diff(self, other: bytes | None = None) -> bytes:
        """Compact patch: for each changed sector, 'DS' + u16 sector + 4096 bytes."""
        out = bytearray(b'DMT99SECTORPATCH\x01')
        for s in self.changed_sectors(other):
            out += b'DS' + struct.pack('<H', s) + self.data[s * SECTOR:(s + 1) * SECTOR]
        return bytes(out)

    # ------------------------------------------------------------- saving
    def save_as(self, path: str | os.PathLike) -> Path:
        target = Path(path).resolve()
        if self.source and target == self.source:
            raise PermissionError('refusing to overwrite the image this working copy was loaded from')
        if target.exists():
            with open(target, 'rb') as f:
                if sha256(f.read()) == ORIGINAL_DUMP_SHA256:
                    raise PermissionError(f'{target} holds the original dump; it is never overwritten')
        target.parent.mkdir(parents=True, exist_ok=True)
        fd, tmp = tempfile.mkstemp(dir=target.parent, prefix=target.name + '.', suffix='.tmp')
        try:
            with os.fdopen(fd, 'wb') as f:
                f.write(self.data)
            os.replace(tmp, target)
        except BaseException:
            if os.path.exists(tmp):
                os.unlink(tmp)
            raise
        return target

    def export_region(self, name: str) -> bytes:
        first, count = REGIONS[name]
        return bytes(self.data[first * SECTOR:(first + count) * SECTOR])

    def export_partition(self, index: int) -> bytes:
        p = next(p for p in self.partitions if p.index == index)
        return bytes(self.data[p.offset:p.offset + p.length])

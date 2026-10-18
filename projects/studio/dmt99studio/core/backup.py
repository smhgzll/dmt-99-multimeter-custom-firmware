"""Backups with metadata, and safe asset deployment to a connected device.

Every backup is written as <name>.bin plus <name>.json holding the address
space, address, length, timestamp, SHA-256, CRC32 and whether a device-side
CRC confirmed the bytes. Nothing in this module writes outside the assets
partition; deploy_assets refuses any other sector.
"""
from __future__ import annotations

import datetime as _dt
import hashlib
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Callable

from .. import paths
from .fat12 import FatError
from .flashimage import ASSET_FIRST_SECTOR, ASSET_SECTORS, FLASH_SIZE, SECTOR, FlashImage
from .protocol import crc32


@dataclass
class BackupRecord:
    path: Path
    meta_path: Path
    sha256: str
    crc32: int
    verified: bool | None
    meta: dict


def save_backup(kind: str, data: bytes, address: int, space: str = 'spi', source: str = '',
                verify: int | None = None, folder: Path | None = None, extra: dict | None = None) -> BackupRecord:
    folder = Path(folder or paths.BACKUPS)
    folder.mkdir(parents=True, exist_ok=True)
    stamp = _dt.datetime.now().astimezone()
    name = f'{stamp:%Y%m%d-%H%M%S}_{kind}'
    path = folder / f'{name}.bin'
    n = 1
    while path.exists():
        path = folder / f'{name}-{n}.bin'
        n += 1
    path.write_bytes(data)
    digest = hashlib.sha256(data).hexdigest()
    c = crc32(data)
    verified = None if verify is None else verify == c
    meta = {'kind': kind, 'space': space, 'address': address, 'address_hex': f'0x{address:08x}', 'length': len(data),
            'timestamp': stamp.isoformat(timespec='seconds'), 'sha256': digest, 'crc32': f'0x{c:08x}',
            'device_crc32': None if verify is None else f'0x{verify:08x}', 'verified_against_device': verified,
            'source': source, 'tool': 'DMT99 Studio', **(extra or {})}
    meta_path = path.with_suffix('.json')
    meta_path.write_text(json.dumps(meta, indent=2, ensure_ascii=False) + '\n')
    return BackupRecord(path, meta_path, digest, c, verified, meta)


def list_backups(folder: Path | None = None) -> list[dict]:
    folder = Path(folder or paths.BACKUPS)
    rows = []
    for meta in sorted(folder.glob('*.json'), reverse=True):
        try:
            m = json.loads(meta.read_text())
        except json.JSONDecodeError:
            continue
        m['file'] = str(meta.with_suffix('.bin'))
        rows.append(m)
    return rows


def _device_image(device, progress) -> tuple[bytearray, bytes]:
    """MBR + assets partition from the device in an 8 MiB frame (BOOT area left 0xFF)."""
    image = bytearray(b'\xff' * FLASH_SIZE)
    mbr = device.read_spi(0, SECTOR)
    image[:SECTOR] = mbr
    assets = device.read_partition(2, 0, ASSET_SECTORS * SECTOR, progress=progress)
    start = ASSET_FIRST_SECTOR * SECTOR
    image[start:start + len(assets)] = assets
    return image, assets


def deploy_assets(device, replacements: dict[str, bytes], progress: Callable[[int, int], None] | None = None,
                  log=None, backup_folder: Path | None = None) -> dict:
    """Apply replacements on top of the device's current assets volume and write the changed sectors."""
    image, assets = _device_image(device, progress)
    rec = save_backup('assets-before-upload', assets, ASSET_FIRST_SECTOR * SECTOR, source=device.describe(),
                      verify=device.spi_crc(ASSET_FIRST_SECTOR * SECTOR, len(assets)), folder=backup_folder,
                      extra={'mbr_sha256': hashlib.sha256(bytes(image[:SECTOR])).hexdigest()})
    if rec.verified is False:
        raise FatError('the device assets did not read back consistently; nothing was written')
    work = FlashImage(bytes(image), label='device')
    if 1 not in [v.index for v in work.volumes]:
        raise FatError('device assets volume is not readable: ' + '; '.join(work.volume_errors))
    skipped = []
    for key, data in sorted(replacements.items()):
        e = work.resolve(key)
        if e is None:
            raise FatError(f'{key} does not exist on the device')
        if work.read_file(e) == data:
            skipped.append(key)
            continue
        work.replace_file(e, data)
    problems = work.assets.validate()
    if problems:
        raise FatError('refusing to write an inconsistent assets volume: ' + '; '.join(problems))
    changed = work.changed_sectors()
    outside = [s for s in changed if not ASSET_FIRST_SECTOR <= s < ASSET_FIRST_SECTOR + ASSET_SECTORS]
    if outside:
        raise FatError(f'internal error: sectors outside the assets partition changed: {outside[:5]}')
    # Data sectors first, then FAT and directory sectors (they reference the data).
    first_data = work.assets.data_offset // SECTOR
    order = sorted(changed, key=lambda s: (s < first_data, s))
    sectors = {s: bytes(work.data[s * SECTOR:(s + 1) * SECTOR]) for s in order}
    written = device.write_sectors(dict((s, sectors[s]) for s in order), progress=progress) if sectors else []
    return {'backup': str(rec.path), 'written': written, 'skipped': skipped}


def restore_assets(device, backup_bin: Path, progress=None) -> dict:
    """Write back an assets-partition backup (only sectors that differ)."""
    data = Path(backup_bin).read_bytes()
    meta = json.loads(Path(backup_bin).with_suffix('.json').read_text())
    if meta.get('address') != ASSET_FIRST_SECTOR * SECTOR or len(data) != ASSET_SECTORS * SECTOR:
        raise ValueError('not an assets-partition backup')
    if hashlib.sha256(data).hexdigest() != meta.get('sha256'):
        raise ValueError('backup file does not match its recorded SHA-256')
    sectors = {}
    for i in range(ASSET_SECTORS):
        chunk = data[i * SECTOR:(i + 1) * SECTOR]
        if device.sector_crc(ASSET_FIRST_SECTOR + i) != crc32(chunk):
            sectors[ASSET_FIRST_SECTOR + i] = chunk
    written = device.write_sectors(sectors, progress=progress) if sectors else []
    return {'written': written}

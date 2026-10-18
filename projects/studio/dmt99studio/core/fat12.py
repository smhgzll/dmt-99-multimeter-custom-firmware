"""FAT12 volume access on an in-memory flash image.

The DMT99 Winbond image holds two FAT12 volumes with 4096-byte sectors and
one sector per cluster. This module reads both and can replace or create
files in a *working copy* (a bytearray owned by FlashImage). It never touches
a file on disk.

Write policy:
- a replaced file keeps its directory entry (short and long name, attributes,
  timestamps unless asked otherwise) and reuses its old clusters first, so an
  edit changes as few flash sectors as possible;
- both FAT copies are updated identically;
- cluster slack after the end of file is zero filled.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field

ATTR_READ_ONLY = 0x01
ATTR_HIDDEN = 0x02
ATTR_SYSTEM = 0x04
ATTR_VOLUME = 0x08
ATTR_DIRECTORY = 0x10
ATTR_ARCHIVE = 0x20
ATTR_LFN = 0x0F


class FatError(Exception):
    """Structural problem or an unsupported operation."""


def sfn_checksum(raw11: bytes) -> int:
    s = 0
    for c in raw11:
        s = (((s & 1) << 7) + (s >> 1) + c) & 0xFF
    return s


def _lfn_text(entries: list[bytes]) -> str:
    chars = []
    for e in reversed(entries):
        chars.extend(struct.unpack_from('<H', e, p)[0] for p in (1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30))
    if 0 in chars:
        chars = chars[:chars.index(0)]
    return b''.join(struct.pack('<H', c) for c in chars if c != 0xFFFF).decode('utf-16le', errors='replace')


@dataclass
class DirEntry:
    volume: int
    path: str            # '/'-separated, relative to the volume root
    name: str            # long name when valid, else short name
    short_name: str
    attributes: int
    start_cluster: int
    size: int
    entry_offset: int    # absolute byte offset of the 32-byte short entry in the image
    lfn_offsets: list[int] = field(default_factory=list)
    clusters: list[int] = field(default_factory=list)
    chain_error: str | None = None

    @property
    def is_dir(self) -> bool:
        return bool(self.attributes & ATTR_DIRECTORY)

    @property
    def folder(self) -> str:
        return self.path.rsplit('/', 1)[0] if '/' in self.path else ''


class FatVolume:
    def __init__(self, image: bytearray, offset: int, length: int, index: int):
        self.image = image
        self.offset = offset
        self.length = length
        self.index = index
        b = bytes(image[offset:offset + 512])
        if len(b) != 512 or b[510:512] != b'\x55\xaa':
            raise FatError(f'volume {index}: boot sector signature missing')
        (self.bps, self.spc, self.reserved, self.nfat, self.nroot, total16, self.media,
         self.fatsz) = struct.unpack_from('<HBHBHHBH', b, 11)
        total32 = struct.unpack_from('<I', b, 32)[0]
        self.total = total16 or total32
        self.oem = b[3:11].decode('ascii', errors='replace')
        self.label = b[43:54].decode('ascii', errors='replace').rstrip()
        self.fs_type = b[54:62].decode('ascii', errors='replace').rstrip()
        if self.bps not in (512, 1024, 2048, 4096) or not self.spc or self.spc & (self.spc - 1):
            raise FatError(f'volume {index}: unsupported sector/cluster geometry')
        if not self.reserved or not self.nfat or not self.fatsz:
            raise FatError(f'volume {index}: invalid FAT geometry')
        self.root_sectors = (self.nroot * 32 + self.bps - 1) // self.bps
        self.data_sector = self.reserved + self.nfat * self.fatsz + self.root_sectors
        if self.total <= self.data_sector or self.total * self.bps > length:
            raise FatError(f'volume {index}: volume exceeds its partition')
        self.clusters = (self.total - self.data_sector) // self.spc
        if self.clusters >= 4085:
            raise FatError(f'volume {index}: not FAT12 ({self.clusters} clusters)')
        self.cluster_bytes = self.bps * self.spc
        self.fat_offset = offset + self.reserved * self.bps
        self.fat_bytes = self.fatsz * self.bps
        if self.fat_bytes * 2 // 3 < self.clusters + 2:
            raise FatError(f'volume {index}: FAT too small')
        self.root_offset = offset + (self.reserved + self.nfat * self.fatsz) * self.bps
        self.data_offset = offset + self.data_sector * self.bps

    # ----------------------------------------------------------------- FAT
    def fat_value(self, c: int, copy: int = 0) -> int:
        if not 0 <= c < self.clusters + 2:
            raise FatError(f'cluster {c} outside FAT')
        p = self.fat_offset + copy * self.fat_bytes + c + c // 2
        v = self.image[p] | self.image[p + 1] << 8
        return v >> 4 if c & 1 else v & 0xFFF

    def _set_fat(self, c: int, value: int) -> None:
        for copy in range(self.nfat):
            p = self.fat_offset + copy * self.fat_bytes + c + c // 2
            if c & 1:
                self.image[p] = (self.image[p] & 0x0F) | ((value << 4) & 0xF0)
                self.image[p + 1] = (value >> 4) & 0xFF
            else:
                self.image[p] = value & 0xFF
                self.image[p + 1] = (self.image[p + 1] & 0xF0) | ((value >> 8) & 0x0F)

    def chain(self, start: int) -> tuple[list[int], str | None]:
        result: list[int] = []
        seen: set[int] = set()
        c = start
        if c == 0:
            return result, None
        while True:
            if c in seen:
                return result, 'cycle'
            if not 2 <= c < self.clusters + 2:
                return result, 'out of range'
            seen.add(c)
            result.append(c)
            v = self.fat_value(c)
            if v >= 0xFF8:
                return result, None
            if v == 0xFF7:
                return result, 'bad cluster'
            if v < 2:
                return result, 'free cluster in chain'
            c = v

    def cluster_offset(self, c: int) -> int:
        if not 2 <= c < self.clusters + 2:
            raise FatError(f'cluster {c} outside volume')
        return self.data_offset + (c - 2) * self.cluster_bytes

    def free_clusters(self) -> list[int]:
        return [c for c in range(2, self.clusters + 2) if self.fat_value(c) == 0]

    # ------------------------------------------------------------ directory
    def _dir_slots(self, start: int) -> list[int]:
        """Absolute offsets of all 32-byte slots of a directory (0 = root)."""
        if start == 0:
            return list(range(self.root_offset, self.root_offset + self.nroot * 32, 32))
        chain, error = self.chain(start)
        if error:
            raise FatError(f'directory chain at cluster {start}: {error}')
        return [self.cluster_offset(c) + i for c in chain for i in range(0, self.cluster_bytes, 32)]

    def _entries_in(self, start: int, parent: str) -> list[DirEntry]:
        out: list[DirEntry] = []
        lfn: list[bytes] = []
        lfn_offsets: list[int] = []
        for pos in self._dir_slots(start):
            e = bytes(self.image[pos:pos + 32])
            if e[0] == 0:
                break
            if e[0] == 0xE5:
                lfn, lfn_offsets = [], []
                continue
            if e[11] == ATTR_LFN:
                lfn.append(e)
                lfn_offsets.append(pos)
                continue
            raw = e[:11]
            if e[11] & ATTR_VOLUME:
                lfn, lfn_offsets = [], []
                continue
            disp = (b'\xe5' + raw[1:]) if raw[0] == 5 else raw
            base = disp[:8].decode('cp936', errors='replace').rstrip()
            ext = disp[8:].decode('cp936', errors='replace').rstrip()
            # NT case flags (byte 12): lower-case base / extension, as Windows shows them.
            if e[12] & 0x08:
                base = base.lower()
            if e[12] & 0x10:
                ext = ext.lower()
            short = base + ('.' + ext if ext else '')
            valid_lfn = bool(lfn) and len(lfn) == (lfn[0][0] & 0x1F) and bool(lfn[0][0] & 0x40) and all(
                x[13] == sfn_checksum(raw) for x in lfn)
            name = _lfn_text(lfn) if valid_lfn else short
            offsets = lfn_offsets if valid_lfn else []
            lfn, lfn_offsets = [], []
            if name in ('.', '..'):
                continue
            path = f'{parent}/{name}' if parent else name
            start_cluster = struct.unpack_from('<H', e, 26)[0]
            size = struct.unpack_from('<I', e, 28)[0]
            entry = DirEntry(self.index, path, name, short, e[11], start_cluster, size, pos, offsets)
            entry.clusters, entry.chain_error = self.chain(start_cluster)
            out.append(entry)
        return out

    def walk(self) -> list[DirEntry]:
        """Every file and directory, depth first, root first."""
        result: list[DirEntry] = []
        visited: set[int] = set()

        def visit(start: int, parent: str) -> None:
            for entry in self._entries_in(start, parent):
                result.append(entry)
                if entry.is_dir and entry.start_cluster and entry.start_cluster not in visited and not entry.chain_error:
                    visited.add(entry.start_cluster)
                    visit(entry.start_cluster, entry.path)
        visit(0, '')
        return result

    def find(self, path: str) -> DirEntry | None:
        want = path.strip('/').lower()
        for entry in self.walk():
            if entry.path.lower() == want or self._short_path(entry).lower() == want:
                return entry
        return None

    def _short_path(self, entry: DirEntry) -> str:
        # Firmware paths use long names; also accept 8.3 names for robustness.
        parts = entry.path.split('/')
        parts[-1] = entry.short_name
        return '/'.join(parts)

    def read(self, entry: DirEntry) -> bytes:
        if entry.chain_error:
            raise FatError(f'{entry.path}: {entry.chain_error}')
        data = b''.join(bytes(self.image[self.cluster_offset(c):self.cluster_offset(c) + self.cluster_bytes])
                        for c in entry.clusters)
        if not entry.is_dir and len(data) < entry.size:
            raise FatError(f'{entry.path}: chain shorter than file size')
        return data if entry.is_dir else data[:entry.size]

    # ---------------------------------------------------------------- write
    def _allocate(self, count: int, after: int, exclude: set[int]) -> list[int]:
        free = [c for c in self.free_clusters() if c not in exclude]
        if len(free) < count:
            raise FatError(f'volume {self.index}: needs {count} free clusters, {len(free)} available')
        # Prefer clusters following the existing chain, then the lowest free ones.
        ordered = sorted(free, key=lambda c: (c <= after, c))
        return ordered[:count]

    def _write_chain(self, clusters: list[int]) -> None:
        for i, c in enumerate(clusters):
            self._set_fat(c, clusters[i + 1] if i + 1 < len(clusters) else 0xFFF)

    def _write_data(self, clusters: list[int], data: bytes) -> None:
        for i, c in enumerate(clusters):
            chunk = data[i * self.cluster_bytes:(i + 1) * self.cluster_bytes]
            p = self.cluster_offset(c)
            self.image[p:p + self.cluster_bytes] = chunk.ljust(self.cluster_bytes, b'\0')

    def replace(self, entry: DirEntry, data: bytes, timestamp: tuple[int, int] | None = None) -> DirEntry:
        """Replace the contents of an existing file in place."""
        if entry.is_dir:
            raise FatError(f'{entry.path} is a directory')
        if entry.chain_error:
            raise FatError(f'{entry.path}: refusing to rewrite a damaged chain ({entry.chain_error})')
        need = (len(data) + self.cluster_bytes - 1) // self.cluster_bytes
        old = list(entry.clusters)
        keep = old[:need]
        for c in old[need:]:
            self._set_fat(c, 0)
        if need > len(keep):
            keep += self._allocate(need - len(keep), keep[-1] if keep else 1, set(keep))
        self._write_chain(keep)
        self._write_data(keep, data)
        p = entry.entry_offset
        struct.pack_into('<H', self.image, p + 26, keep[0] if keep else 0)
        struct.pack_into('<H', self.image, p + 20, 0)
        struct.pack_into('<I', self.image, p + 28, len(data))
        if timestamp:
            struct.pack_into('<HH', self.image, p + 22, timestamp[0], timestamp[1])
        entry.clusters, entry.start_cluster, entry.size, entry.chain_error = keep, keep[0] if keep else 0, len(data), None
        return entry

    def create(self, folder: str, short_name: str, data: bytes) -> DirEntry:
        """Create a new file with an 8.3 upper-case name in an existing folder.

        Intended for theme add-ons (new files next to the originals); the
        firmware's FatFs matches 8.3 names case-insensitively.
        """
        base, _, ext = short_name.upper().partition('.')
        if not (1 <= len(base) <= 8 and len(ext) <= 3) or any(ch in base + ext for ch in ' "*+,/:;<=>?[\\]|'):
            raise FatError(f'{short_name!r} is not a valid 8.3 name')
        raw = (base.ljust(8) + ext.ljust(3)).encode('ascii')
        if folder:
            parent = self.find(folder)
            if not parent or not parent.is_dir:
                raise FatError(f'folder {folder!r} not found')
            start = parent.start_cluster
        else:
            start = 0
        if any(e.short_name.upper() == short_name.upper() or e.name.upper() == short_name.upper()
               for e in self._entries_in(start, folder)):
            raise FatError(f'{folder}/{short_name} exists')
        slot = None
        for pos in self._dir_slots(start):
            if self.image[pos] in (0, 0xE5):
                slot = pos
                break
        if slot is None:
            raise FatError(f'no free directory slot in {folder or "root"}')
        need = (len(data) + self.cluster_bytes - 1) // self.cluster_bytes
        clusters = self._allocate(need, 1, set()) if need else []
        self._write_chain(clusters)
        self._write_data(clusters, data)
        terminator = self.image[slot] == 0
        e = bytearray(32)
        e[:11] = raw
        e[11] = ATTR_ARCHIVE
        struct.pack_into('<H', e, 26, clusters[0] if clusters else 0)
        struct.pack_into('<I', e, 28, len(data))
        self.image[slot:slot + 32] = e
        if terminator:
            nxt = slot + 32
            slots = self._dir_slots(start)
            if nxt in slots:
                self.image[nxt] = 0
        path = f'{folder}/{short_name.upper()}' if folder else short_name.upper()
        entry = DirEntry(self.index, path, short_name.upper(), short_name.upper(), ATTR_ARCHIVE,
                         clusters[0] if clusters else 0, len(data), slot, [], clusters, None)
        return entry

    # ----------------------------------------------------------- validation
    def validate(self) -> list[str]:
        issues: list[str] = []
        copies = [bytes(self.image[self.fat_offset + i * self.fat_bytes:self.fat_offset + (i + 1) * self.fat_bytes])
                  for i in range(self.nfat)]
        if any(c != copies[0] for c in copies[1:]):
            issues.append(f'volume {self.index}: FAT copies differ')
        if self.fat_value(0) & 0xFF != self.media:
            issues.append(f'volume {self.index}: FAT[0] does not carry the media byte')
        owners: dict[int, str] = {}
        try:
            entries = self.walk()
        except FatError as ex:
            return issues + [str(ex)]
        for e in entries:
            if e.chain_error:
                issues.append(f'{e.path}: {e.chain_error}')
            if not e.is_dir and len(e.clusters) * self.cluster_bytes < e.size:
                issues.append(f'{e.path}: chain shorter than size')
            if not e.is_dir and e.size and len(e.clusters) > (e.size + self.cluster_bytes - 1) // self.cluster_bytes:
                issues.append(f'{e.path}: chain longer than size')
            for c in e.clusters:
                if c in owners:
                    issues.append(f'cluster {c} cross-linked: {owners[c]} and {e.path}')
                owners[c] = e.path
        allocated = {c for c in range(2, self.clusters + 2) if self.fat_value(c) != 0}
        lost = sorted(allocated - set(owners))
        if lost:
            issues.append(f'volume {self.index}: {len(lost)} allocated clusters without an owner (lost chains)')
        return issues

    def usage(self) -> dict:
        free = len(self.free_clusters())
        return {'clusters': self.clusters, 'cluster_bytes': self.cluster_bytes, 'free_clusters': free,
                'used_bytes': (self.clusters - free) * self.cluster_bytes, 'free_bytes': free * self.cluster_bytes,
                'capacity_bytes': self.clusters * self.cluster_bytes}

    def info(self) -> dict:
        return {'index': self.index, 'offset': self.offset, 'length': self.length, 'oem': self.oem,
                'label': self.label, 'fs_type': self.fs_type, 'bytes_per_sector': self.bps,
                'sectors_per_cluster': self.spc, 'reserved_sectors': self.reserved, 'fat_copies': self.nfat,
                'fat_sectors': self.fatsz, 'root_entries': self.nroot, 'total_sectors': self.total,
                'fat_offset': self.fat_offset, 'root_offset': self.root_offset, 'data_offset': self.data_offset,
                **self.usage()}

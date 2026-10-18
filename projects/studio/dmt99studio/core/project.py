"""Theme editing session and theme project files.

The working flash image is always a pure function of
    (base image, replacements)
and is rebuilt from the base whenever an asset changes. Restoring an asset
therefore gives back exactly the original sectors, and undo/redo is a stack
of (asset, before, after) byte strings.

A theme project is a directory:
    project.json          metadata, base image hash, changed assets, layout overrides
    assets/<vol>/<path>   replacement JPEG files, same names as on the meter
"""
from __future__ import annotations

import datetime as _dt
import json
import os
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from .fat12 import DirEntry, FatError
from .flashimage import FlashImage, sha256
from . import jpegtools
from .screens import asset_key

PROJECT_FORMAT = 'dmt99-studio-theme'
PROJECT_VERSION = 1


@dataclass
class AssetRow:
    key: str             # '1:/mode/dc_v.jpg'
    volume: int
    path: str            # as stored (long name), volume relative
    size: int
    clusters: int
    is_jpeg: bool
    width: int = 0
    height: int = 0
    modified: bool = False
    used_by: list[str] = field(default_factory=list)

    @property
    def folder(self) -> str:
        return self.path.rsplit('/', 1)[0] if '/' in self.path else ''

    @property
    def name(self) -> str:
        return self.path.rsplit('/', 1)[-1]


class ThemeSession:
    def __init__(self, base: FlashImage):
        self.base = base
        self.replacements: dict[str, bytes] = {}
        self.layout_overrides: dict = {}
        self.palette: dict[int, int] = {}      # RGB565 firmware colour -> theme colour (THEME.PAL)
        self.name = 'Untitled theme'
        self.notes = ''
        self.sources: dict[str, str] = {}
        # Undo entries: list of (asset key, before, after); key '#palette' holds palette dicts.
        self._undo: list[list[tuple]] = []
        self._redo: list[list[tuple]] = []
        self.version = 0
        self.working = FlashImage(bytes(base.data), label='working copy')
        self._working_file: Path | None = None
        self._working_file_version = -1
        self._tmpdir = tempfile.TemporaryDirectory(prefix='dmt99-studio-')

    # ------------------------------------------------------------ assets
    def _entry(self, image: FlashImage, key: str) -> DirEntry:
        e = image.resolve(key)
        if e is None:
            raise FatError(f'{key} not found')
        return e

    def original_bytes(self, key: str) -> bytes:
        return self.base.read_file(self._entry(self.base, key))

    def current_bytes(self, key: str) -> bytes:
        return self.replacements.get(key) or self.original_bytes(key)

    def assets(self, usage: dict[str, list[str]] | None = None) -> list[AssetRow]:
        rows = []
        for e in self.working.entries():
            if e.is_dir:
                continue
            key = asset_key(f'{e.volume}:/{e.path}')
            head = self.working.read_file(e)[:4096] if e.size else b''
            info = jpegtools.inspect(head) if head[:2] == b'\xff\xd8' else None
            rows.append(AssetRow(key, e.volume, e.path, e.size, len(e.clusters), info is not None,
                                 info.width if info else 0, info.height if info else 0,
                                 key in self.replacements, (usage or {}).get(key, [])))
        return rows

    def free_bytes(self) -> int:
        return self.working.assets.usage()['free_bytes']

    def check(self, key: str, data: bytes, run_decoder: bool = True) -> jpegtools.Compatibility:
        original = jpegtools.inspect(self.original_bytes(key))
        expected = (original.width, original.height) if original.width else None
        current = self._entry(self.working, key)
        cluster = self.working.assets.cluster_bytes
        room = self.free_bytes() + len(current.clusters) * cluster
        return jpegtools.validate(data, expected, run_decoder, max_bytes=room)

    def _rebuild(self) -> None:
        image = FlashImage(bytes(self.base.data), label='working copy')
        for key in sorted(self.replacements):
            image.replace_file(self._entry(image, key), self.replacements[key])
        self.working = image
        self.version += 1

    def _set(self, key: str, data: bytes | None) -> None:
        if not key.startswith('1:/'):
            raise FatError('only assets on volume 1: can be replaced (the BOOT volume holds the update area)')
        old = self.replacements.get(key)
        if data is None or data == self.original_bytes(key):
            self.replacements.pop(key, None)
        else:
            self.replacements[key] = data
        try:
            self._rebuild()
        except FatError:
            if old is None:
                self.replacements.pop(key, None)
            else:
                self.replacements[key] = old
            self._rebuild()
            raise

    def replace(self, key: str, data: bytes, force: bool = False, source: str | None = None) -> jpegtools.Compatibility:
        result = self.check(key, data)
        if not result.ok and not force:
            return result
        before = self.replacements.get(key)
        self._set(key, data)
        self._undo.append([(key, before, self.replacements.get(key))])
        self._redo.clear()
        if source:
            self.sources[key] = source
        return result

    def replace_many(self, changes: dict[str, bytes], palette: dict[int, int] | None = None,
                     source: str | None = None) -> None:
        """Apply several replacements (and a palette) as one undo step."""
        step = []
        old = dict(self.replacements)
        try:
            for key, data in changes.items():
                if not key.startswith('1:/'):
                    raise FatError(f'{key}: only assets on volume 1: can be replaced')
                if data == self.original_bytes(key):
                    self.replacements.pop(key, None)
                else:
                    self.replacements[key] = data
            self._rebuild()
        except FatError:
            self.replacements = old
            self._rebuild()
            raise
        for key in changes:
            step.append((key, old.get(key), self.replacements.get(key)))
            if source:
                self.sources[key] = source
        if palette is not None and palette != self.palette:
            step.append(('#palette', dict(self.palette), dict(palette)))
            self.palette = dict(palette)
        if step:
            self._undo.append(step)
            self._redo.clear()

    def set_palette(self, palette: dict[int, int]) -> None:
        if palette != self.palette:
            self._undo.append([('#palette', dict(self.palette), dict(palette))])
            self._redo.clear()
            self.palette = dict(palette)
            self.version += 1

    def restore(self, key: str) -> None:
        before = self.replacements.get(key)
        if before is None:
            return
        self._set(key, None)
        self._undo.append([(key, before, None)])
        self._redo.clear()
        self.sources.pop(key, None)

    def restore_all(self) -> None:
        if self.replacements or self.palette:
            self.replace_many({k: self.original_bytes(k) for k in self.replacements}, palette={})

    def can_undo(self) -> bool:
        return bool(self._undo)

    def can_redo(self) -> bool:
        return bool(self._redo)

    def _apply_step(self, step: list[tuple], forward: bool) -> str:
        keys = []
        for key, before, after in step:
            value = after if forward else before
            if key == '#palette':
                self.palette = dict(value)
            elif value is None:
                self.replacements.pop(key, None)
            else:
                self.replacements[key] = value
            keys.append(key)
        self._rebuild()
        return keys[0] if len(keys) == 1 and keys[0] != '#palette' else ''

    def undo(self) -> str | None:
        if not self._undo:
            return None
        step = self._undo.pop()
        self._redo.append(step)
        return self._apply_step(step, False)

    def redo(self) -> str | None:
        if not self._redo:
            return None
        step = self._redo.pop()
        self._undo.append(step)
        return self._apply_step(step, True)

    # ------------------------------------------------------- working file
    def working_file(self) -> Path:
        """The working image on disk for the simulator (rewritten on change)."""
        if self._working_file is None:
            self._working_file = Path(self._tmpdir.name) / 'working.bin'
        if self._working_file_version != self.version:
            self._working_file.write_bytes(self.working.data)
            self._working_file_version = self.version
        return self._working_file

    def base_file(self) -> Path:
        if self.base.source and self.base.source.exists():
            return self.base.source
        p = Path(self._tmpdir.name) / 'base.bin'
        if not p.exists():
            p.write_bytes(self.base.baseline)
        return p

    # ----------------------------------------------------------- project
    def save_project(self, directory: str | os.PathLike) -> Path:
        root = Path(directory)
        root.mkdir(parents=True, exist_ok=True)
        assets = {}
        for key, data in sorted(self.replacements.items()):
            vol, rel = key.split(':/', 1)
            target = root / 'assets' / vol / rel
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
            assets[key] = {'file': str(target.relative_to(root)), 'sha256': sha256(data), 'size': len(data),
                           'original_sha256': sha256(self.original_bytes(key)),
                           'source': self.sources.get(key)}
        # Drop asset files that are no longer part of the project.
        if (root / 'assets').exists():
            keep = {str(Path(a['file'])) for a in assets.values()}
            for f in (root / 'assets').rglob('*'):
                if f.is_file() and str(f.relative_to(root)) not in keep:
                    f.unlink()
        meta = {
            'format': PROJECT_FORMAT, 'version': PROJECT_VERSION, 'name': self.name, 'notes': self.notes,
            'saved': _dt.datetime.now().astimezone().isoformat(timespec='seconds'),
            'base_image': {'label': self.base.label, 'sha256': self.base.source_sha256,
                           'path': str(self.base.source) if self.base.source else None},
            'assets': assets, 'layout_overrides': self.layout_overrides,
            'palette': {f'{k:04x}': f'{v:04x}' for k, v in sorted(self.palette.items())},
            'target': {'firmware': 'POTAT-99 0.3.x (V2.4 asset layout)', 'volume': '1: assets FAT12'},
        }
        tmp = root / 'project.json.tmp'
        tmp.write_text(json.dumps(meta, indent=2, ensure_ascii=False) + '\n')
        os.replace(tmp, root / 'project.json')
        return root / 'project.json'

    @classmethod
    def load_project(cls, directory: str | os.PathLike, base: FlashImage | None = None,
                     allow_different_base: bool = False) -> 'ThemeSession':
        root = Path(directory)
        meta = json.loads((root / 'project.json').read_text())
        if meta.get('format') != PROJECT_FORMAT:
            raise ValueError('not a DMT99 Studio theme project')
        if meta.get('version', 0) > PROJECT_VERSION:
            raise ValueError('project was saved by a newer DMT99 Studio')
        if base is None:
            path = meta['base_image'].get('path')
            if not path or not Path(path).exists():
                raise FileNotFoundError('base image not found; open it first')
            base = FlashImage.load(path)
        if base.source_sha256 != meta['base_image']['sha256'] and not allow_different_base:
            raise ValueError('the opened base image differs from the one this project was made for')
        session = cls(base)
        session.name = meta.get('name', session.name)
        session.notes = meta.get('notes', '')
        session.layout_overrides = meta.get('layout_overrides', {})
        session.palette = {int(k, 16): int(v, 16) for k, v in meta.get('palette', {}).items()}
        for key, a in meta.get('assets', {}).items():
            data = (root / a['file']).read_bytes()
            if sha256(data) != a['sha256']:
                raise ValueError(f'{a["file"]} does not match its recorded checksum')
            session.replacements[key] = data
            if a.get('source'):
                session.sources[key] = a['source']
        session._rebuild()
        return session

    def export_assets(self, directory: str | os.PathLike, only_modified: bool = True) -> list[Path]:
        out = []
        root = Path(directory)
        for row in self.assets():
            if only_modified and not row.modified:
                continue
            target = root / str(row.volume) / row.path
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(self.current_bytes(row.key))
            out.append(target)
        return out

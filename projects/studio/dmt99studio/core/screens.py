"""Screen catalog and renderer.

Screens are rendered by the firmware project's native simulator
(dmt99-simulator-release): the same C UI code, FatFs and TJpgDec that run on
the meter, fed with the working flash image. The simulator's draw trace
tells which asset or firmware primitive produced each region, which is how
the editor maps a click on the preview back to an asset.

Each catalog entry starts from a fresh boot with a settings fixture and
replays button presses, mode events and readings. Readings are synthetic
display packets (marked as such in the UI); no measurement is invented for
a connected device.
"""
from __future__ import annotations

import hashlib
import json
import subprocess
import tempfile
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path

from .. import paths
from .settings import Settings, simulator_state

WIDTH, HEIGHT = 320, 240


@dataclass
class ScreenDef:
    id: str
    title: str
    group: str
    view: int = 1
    steps: tuple[str, ...] = ()
    valid_settings: bool = True
    note: str = ''


def _reading(mode: int, text: str, unit: int) -> tuple[str, ...]:
    return (f'ack {mode:#04x}', f'reading {text} {unit}')


def _menu(*keys: int) -> tuple[str, ...]:
    out: list[str] = []
    for k in keys:
        out += [f'button {k}', 'step', 'step']
    return out


CATALOG: list[ScreenDef] = [
    ScreenDef('language', 'Language selection (first boot)', 'System', valid_settings=False),
    ScreenDef('auto', 'AUTO mode', 'Regular view', 1, ('step', 'reading 12.34 6')),
    ScreenDef('dc_v', 'DC voltage 12.34 V', 'Regular view', 1, _reading(0x0C, '12.34', 6)),
    ScreenDef('ac_v', 'AC voltage 230.1 V', 'Regular view', 1, _reading(0x0D, '230.1', 6)),
    ScreenDef('resistance', 'Resistance 1.000 kΩ', 'Regular view', 1, _reading(0x0B, '1.000', 12)),
    ScreenDef('capacitance', 'Capacitance 47.00 µF', 'Regular view', 1, _reading(0x0A, '47.00', 16)),
    ScreenDef('continuity', 'Continuity 12.00 Ω', 'Regular view', 1, _reading(0x0E, '12.00', 11)),
    ScreenDef('frequency', 'Frequency 50.00 Hz', 'Regular view', 1, _reading(0x0F, '50.00', 18)),
    ScreenDef('dc_ma', 'DC current 12.34 mA', 'Regular view', 1, _reading(0x11, '12.34', 9)),
    ScreenDef('dc_a', 'DC current 1.234 A', 'Regular view', 1, _reading(0x10, '1.234', 10)),
    ScreenDef('ac_a', 'AC current 1.234 A', 'Regular view', 1, _reading(0x15, '1.234', 10)),
    ScreenDef('temperature', 'Temperature 0025 °C', 'Regular view', 1, _reading(0x12, '0025', 21)),
    ScreenDef('hold', 'DC voltage with HOLD', 'Regular view', 1, _reading(0x0C, '12.34', 6) + ('button 2', 'ack 3')),
    ScreenDef('relative', 'DC voltage with REL', 'Regular view', 1,
              _reading(0x0C, '12.34', 6) + ('button 2 long', 'reading 13.34 6')),
    ScreenDef('recording', 'Recording graph', 'Recording view', 2,
              _reading(0x0C, '12.34', 6) + ('reading 12.40 6', 'reading 12.20 6', 'reading 12.50 6')),
    ScreenDef('recording_r', 'Recording, resistance', 'Recording view', 2, _reading(0x0B, '1.000', 12)),
    ScreenDef('monitor', 'Monitor gauge', 'Monitor view', 3, _reading(0x0C, '12.34', 6)),
    ScreenDef('monitor_threshold', 'Monitor threshold editor', 'Monitor view', 3, ('button 5', 'step')),
    ScreenDef('menu_mode', 'Menu: Mode selection', 'Menu', 1, _menu(6)),
    ScreenDef('menu_mode_open', 'Menu: Mode selection (opened)', 'Menu', 1, _menu(6, 3)),
    ScreenDef('menu_threshold', 'Menu: Threshold settings', 'Menu', 1, _menu(6, 1)),
    ScreenDef('menu_threshold_open', 'Menu: Threshold settings (editing)', 'Menu', 1, _menu(6, 1, 3)),
    ScreenDef('menu_history', 'Menu: History record', 'Menu', 1, _menu(6, 1, 1)),
    ScreenDef('menu_system', 'Menu: System settings', 'Menu', 1, _menu(6, 1, 1, 1)),
    ScreenDef('menu_system_open', 'Menu: System settings (opened)', 'Menu', 1, _menu(6, 1, 1, 1, 3)),
    ScreenDef('menu_reset', 'Menu: Reset', 'Menu', 1, _menu(6, 1, 1, 1, 1)),
    ScreenDef('menu_reset_open', 'Menu: Reset confirmation', 'Menu', 1, _menu(6, 1, 1, 1, 1, 3)),
    ScreenDef('menu_about', 'Menu: About', 'Menu', 1, _menu(6, 1, 1, 1, 1, 1)),
    ScreenDef('charging', 'Charging lock screen', 'System', 1, ('charge 1',)),
    ScreenDef('low_battery', 'Low battery shutdown', 'System', 1, ('battery 0', 'tick 30')),
]
BY_ID = {s.id: s for s in CATALOG}


@dataclass
class DrawOp:
    op: str
    x: int = 0
    y: int = 0
    w: int = 0
    h: int = 0
    path: str = ''
    text: str = ''
    color: int | None = None
    ok: bool = True
    raw: dict = field(default_factory=dict)

    @property
    def rect(self) -> tuple[int, int, int, int]:
        return self.x, self.y, self.w, self.h

    @property
    def kind(self) -> str:
        if self.op == 'jpeg':
            return 'image asset'
        if self.op in ('text_box', 'ascii', 'chinese', 'ascii_char', 'measurement_char', 'numeric'):
            return 'firmware-drawn text'
        if self.op == 'bitmap':
            return 'firmware ROM bitmap'
        return 'firmware primitive'

    def contains(self, px: int, py: int) -> bool:
        return self.w > 0 and self.h > 0 and self.x <= px < self.x + self.w and self.y <= py < self.y + self.h


def _op_from(row: dict) -> DrawOp | None:
    op = row.get('op')
    if op == 'jpeg':
        return DrawOp('jpeg', row['x'], row['y'], row['w'], row['h'], path=row['path'],
                      ok=row['open'] == 0 and row['prepare'] == 0 and row['decode'] == 0, raw=row)
    if op == 'fill':
        return DrawOp('fill', row['x'], row['y'], row['x1'] - row['x'] + 1, row['y1'] - row['y'] + 1,
                      color=row['color'], raw=row)
    if op == 'border':
        return DrawOp('border', row['x'], row['y'], row['x1'] - row['x'] + 1, row['y1'] - row['y'] + 1,
                      color=row['color'], raw=row)
    if op == 'text_box':
        return DrawOp('text_box', row['x'], row['y'], row['w'], row['h'], text=row['text'], color=row['color'], raw=row)
    if op == 'ascii':
        size = row.get('size', 16) or 16
        return DrawOp('ascii', row['x'], row['y'], len(row['text']) * size // 2, size, text=row['text'],
                      color=row['color'], raw=row)
    if op == 'chinese':
        size = row.get('size', 16) or 16
        return DrawOp('chinese', row['x'], row['y'], len(row['text']) // 2 * size, size, text=row['text'],
                      color=row['color'], raw=row)
    if op in ('ascii_char', 'measurement_char'):
        size = row.get('size', 16) or 16
        return DrawOp(op, row['x'], row['y'], size // 2 if op == 'ascii_char' else size, size,
                      text=chr(row['char']) if 32 <= row['char'] < 127 else '', color=row['color'], raw=row)
    if op == 'bitmap':
        return DrawOp('bitmap', row['x'], row['y'], 0, 0, text=f'ROM 0x{row["rom"]:x}', color=row['color'], raw=row)
    if op == 'numeric':
        return DrawOp('numeric', text=row['text'], raw=row)
    return None


@dataclass
class RenderedScreen:
    screen: ScreenDef
    layout: int
    language: int
    png: bytes
    ops: list[DrawOp]
    error: str = ''

    @property
    def visible_ops(self) -> list[DrawOp]:
        """Draw calls still (partly) visible in the final frame, back to front."""
        out: list[DrawOp] = []
        for i, op in enumerate(self.ops):
            if not op.w or not op.h:
                continue
            covered = any(o.op in ('jpeg', 'fill') and o.x <= op.x and o.y <= op.y and
                          o.x + o.w >= op.x + op.w and o.y + o.h >= op.y + op.h for o in self.ops[i + 1:])
            if not covered:
                out.append(op)
        return out

    def assets(self) -> list[str]:
        seen: list[str] = []
        for op in self.visible_ops:
            if op.op == 'jpeg' and op.path not in seen:
                seen.append(op.path)
        return seen

    def hit(self, x: int, y: int) -> DrawOp | None:
        for op in reversed(self.visible_ops):
            if op.contains(x, y) and op.op != 'fill':
                return op
        return None

    @property
    def digest(self) -> str:
        return hashlib.sha256(self.png).hexdigest()


def render(flash_file: Path, screen: ScreenDef, layout: int = 0, language: int = 1,
           simulator: Path | None = None, timeout: float = 60,
           palette: dict[int, int] | None = None) -> RenderedScreen:
    paths.ensure_tools()
    sim = Path(simulator or paths.SIMULATOR)
    settings = Settings.defaults(view=screen.view, layout=layout, language=language, valid=screen.valid_settings)
    with tempfile.TemporaryDirectory(prefix='dmt99-screen-') as tmp:
        t = Path(tmp)
        (t / 'state.bin').write_bytes(simulator_state(settings))
        shot, trace = t / 'shot.png', t / 'draws.jsonl'
        script = ['step', 'step'] + list(screen.steps) + ['step', f'snapshot {shot}', 'quit']
        (t / 'run.sim').write_text('\n'.join(script) + '\n')
        args = [str(sim), '--flash', str(flash_file), '--script', str(t / 'run.sim'),
                '--state', str(t / 'state.bin'), '--trace', str(trace)]
        if palette:
            args += ['--palette', ','.join(f'{a:04x}:{b:04x}' for a, b in sorted(palette.items()))]
        r = subprocess.run(args, capture_output=True, text=True, timeout=timeout)
        ops: list[DrawOp] = []
        if trace.exists():
            for line in trace.read_text().splitlines():
                try:
                    op = _op_from(json.loads(line))
                except (json.JSONDecodeError, KeyError):
                    continue
                if op:
                    ops.append(op)
        png = shot.read_bytes() if shot.exists() else b''
        error = '' if r.returncode == 0 and png else (r.stderr.strip() or f'simulator exit {r.returncode}')
        return RenderedScreen(screen, layout, language, png, ops, error)


def render_many(flash_file: Path, screens: list[ScreenDef], layout: int = 0, language: int = 1,
                workers: int = 8, palette: dict[int, int] | None = None) -> list[RenderedScreen]:
    with ThreadPoolExecutor(max_workers=workers) as pool:
        return list(pool.map(lambda s: render(flash_file, s, layout, language, palette=palette), screens))


def asset_key(path: str) -> str:
    """Canonical 'V:/path' form (lower case) of the file a firmware path opens.

    Volume quirk kept from V2.4: one FATFS object serves both drives, and
    resource_optional_logo remounts it on "1:" right after trying
    0:/LOGO.jpg. Every later "0:/..." access (the boot logo "0:/on.jpg")
    therefore reads volume 1. Only LOGO.jpg genuinely comes from the BOOT
    volume.
    """
    p = path.replace('\\', '/')
    if len(p) > 2 and p[1] == ':' and p[0] in '01':
        rel = p[2:].lstrip('/').lower()
        if p[0] == '0' and rel != 'logo.jpg':
            return '1:/' + rel
        return f'{p[0]}:/{rel}'
    return '1:/' + p.lstrip('/').lower()


def asset_usage(rendered: list[RenderedScreen]) -> dict[str, list[str]]:
    """Asset key ('1:/mode/dc_v.jpg') -> ids of the screens that show it."""
    use: dict[str, list[str]] = {}
    for r in rendered:
        for path in r.assets():
            key = asset_key(path)
            use.setdefault(key, [])
            if r.screen.id not in use[key]:
                use[key].append(r.screen.id)
    return use

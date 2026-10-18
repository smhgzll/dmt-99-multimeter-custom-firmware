#!/usr/bin/env python3
"""Create a palette-only example package; no manufacturer JPEG is copied."""
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'projects/studio'))
from dmt99studio.core.themepkg import build, parse

if __name__ == '__main__':
    out = ROOT / 'artifacts/themes/green-demo/THEME.PKG'
    data = build('Green demo', {}, {0xFD40: 0x07E0})
    assert parse(data).palette == {0xFD40: 0x07E0}
    if out.exists() and out.read_bytes() != data:
        raise SystemExit('existing package differs; refusing to overwrite')
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(data)
    print(f'{out.relative_to(ROOT)}: {len(data)} bytes; POTAT-99 >= 0.5.0')

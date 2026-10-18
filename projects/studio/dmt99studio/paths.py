"""Locations of the firmware project and its host tools.

DMT99 Studio reuses the existing firmware project instead of copying it:
the native simulator renders screens with the firmware's own UI code and
dmt99-jpeg-check decodes images with the firmware's TJpgDec build.
Override with DMT99_FIRMWARE_PROJECT if the project lives elsewhere.
"""
from __future__ import annotations

import os
import subprocess
from pathlib import Path

STUDIO_ROOT = Path(__file__).resolve().parents[1]
FIRMWARE_PROJECT = Path(os.environ.get('DMT99_FIRMWARE_PROJECT', STUDIO_ROOT.parents[1]))
RECONSTRUCTED = FIRMWARE_PROJECT / 'reconstructed'
BUILD = RECONSTRUCTED / 'build'
ORIGINAL_DUMP = FIRMWARE_PROJECT / 'artifacts/flash/original/dmt99_flash_1.bin'
FINAL_RELEASES = FIRMWARE_PROJECT / 'artifacts/firmware/potat-99'
ASIC_ANALYSIS = Path(os.environ.get('DMT99_ASIC_ANALYSIS', STUDIO_ROOT.parent / 'asic'))

SIMULATOR = BUILD / 'dmt99-simulator-release'
JPEG_CHECK = BUILD / 'dmt99-jpeg-check'

USER_DATA = Path(os.environ.get('DMT99_STUDIO_HOME', Path.home() / '.local/share/dmt99-studio'))
BACKUPS = USER_DATA / 'backups'
LOGS = USER_DATA / 'logs'


class ToolMissing(RuntimeError):
    pass


def ensure_tools(build: bool = True) -> None:
    """Make sure the simulator and the JPEG checker exist (builds them on demand)."""
    if SIMULATOR.exists() and JPEG_CHECK.exists():
        return
    if not build:
        raise ToolMissing(f'host tools missing; run: make -C {RECONSTRUCTED} studio-tools')
    r = subprocess.run(['make', '-C', str(RECONSTRUCTED), 'studio-tools'], capture_output=True, text=True)
    if r.returncode or not (SIMULATOR.exists() and JPEG_CHECK.exists()):
        raise ToolMissing('building the firmware host tools failed:\n' + r.stdout[-2000:] + r.stderr[-2000:])

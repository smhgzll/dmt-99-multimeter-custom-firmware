"""Live decode check with real UART captures (../dmt_99_asic_analysis/tests/fixtures).

Real 12-byte display frames from the meter's measurement IC are fed to the
simulated device (firmware controller and parser). The decoded measurement
record must match the value and unit the user read on the meter's screen
during the capture.
"""
import glob
import json
import time
from pathlib import Path

import pytest

from dmt99studio import paths
from dmt99studio.core.device import DeviceClient

FIXTURES = sorted(glob.glob(str(paths.ASIC_ANALYSIS / 'tests/fixtures/real_resistance_*.json')))
UNITS = {'ohm': 'Ω', 'kohm': 'kΩ', 'mohm': 'MΩ'}


def cases():
    out = []
    for f in FIXTURES:
        d = json.loads(Path(f).read_text())
        meta = d.get('source_metadata') or {}
        frames = [p['hex'] for p in d.get('expected_packets', []) if p.get('direction') == 'ASIC_TX'
                  and p['hex'].startswith('5A A5') and not p.get('uart_errors')]
        if meta.get('display_numeric') is None or not frames:
            continue
        distinct = list(dict.fromkeys(frames))[:12]
        out.append((Path(f).stem, distinct, meta['display_numeric'], meta['display_unit']))
    return out


@pytest.mark.skipif(not FIXTURES, reason='ASIC capture fixtures not available')
def test_firmware_decode_matches_screen_readings(original_path):
    rows = cases()
    assert rows, 'no annotated resistance fixtures'
    dev = DeviceClient.open_simulated(original_path)
    try:
        dev.sim.command('ack 0x0b')        # resistance mode, as during the captures
        checked = exact = 0
        for name, frames, value, unit in rows:
            decoded = []
            for frame in frames:
                raw = bytes.fromhex(frame) + b'\0'   # 12 bytes on the wire, 13-byte DMA buffer in firmware
                dev.sim.command('packet ' + raw.hex())
                time.sleep(0.12)
                m = dev.measurement()
                assert m['available'] and m['numeric'], (name, m)
                assert m['unit_name'] == UNITS[unit.lower()], (name, m['unit_name'], unit)
                decoded.append(m['value'])   # float32 from the firmware
            # The full captures hold the reported value (RESISTANCE_VALIDATION.md, e.g. 1 kΩ: 1.000-1.001);
            # the stored excerpts may not. Every decoded frame must be within two display steps of the
            # value the user read on the meter.
            decimals = len(str(value).split('.')[1]) if '.' in str(value) else 0
            step = 10 ** -decimals
            assert all(abs(v - value) <= 2 * step + 1e-6 for v in decoded), (name, value, decoded)
            exact += any(abs(v - value) < 1e-5 * max(1.0, abs(value)) for v in decoded)
            checked += 1
        assert checked >= 3 and exact >= checked - 1, (checked, exact)
    finally:
        dev.close()

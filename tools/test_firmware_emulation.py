#!/usr/bin/env python3
"""Level-4 check: the RV32 firmware image versus the host simulator.

Boots build/firmware/dmt99-reconstructed.bin in tools/fw_emulator.py (Unicorn
plus board models) and drives it with the same inputs as the native host
simulator (reconstructed/build/dmt99-simulator). After every step the
emulated ST7789 framebuffer must equal the simulator framebuffer pixel for
pixel, and the list of UART command packets must be identical. Further
checks cover first boot (erased settings page), settings persistence across a
power cycle, charging, low-battery shutdown, auto-off and the SPI Flash write
policy.

The simulator advances one application_step per input; the firmware loops
continuously, so the simulator script adds idle steps after every input.
This proves that the cross-compiled image, its startup, interrupt handlers,
DMA/UART path, SPI/FatFs/JPEG path and panel bus produce what the reviewed C
produces on the host. It does not prove physical electrical behaviour.

Usage: test_firmware_emulation.py [--out DIR] [--only NAME ...]
"""
import argparse, json, os, struct, subprocess, sys, tempfile
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fw_emulator import Ch32Emulator, ROOT, default_image

PATTERNS = [0xeb, 0x0a, 0xad, 0x8f, 0x4e, 0xc7, 0xe7, 0x8a, 0xef, 0xcf]
KEYS = {1: 'PC10', 2: 'PC11', 3: 'PC9', 4: 'PC12', 5: 'PC8', 6: 'PC8'}
SIMULATOR = ROOT / 'reconstructed/build/dmt99-simulator'
FLASH = ROOT / 'flashdump/dmt99_flash_1.bin'


def reading_packet(text, unit):
    """Port of simulator_reading (simulator/simulator.c)."""
    p = bytearray(13); p[0], p[1] = 0x5a, 0xa5
    negative = text.startswith('-')
    text = text.lstrip('+-')
    segs, decimal = [], 0
    for ch in text:
        if ch == '.':
            decimal = len(segs); continue
        segs.append(PATTERNS[int(ch)])
    assert len(segs) == 4
    for i, s in enumerate(segs):
        p[2 + i] |= s & 0xf0; p[3 + i] |= s & 0x0f
    if negative: p[2] |= 0x10
    if decimal: p[2 + decimal] |= 0x10
    bits = {6: [(8, 2)], 7: [(8, 2), (7, 1)], 8: [(8, 1), (6, 16)], 9: [(8, 1), (7, 1)], 10: [(8, 1)],
            11: [(7, 32)], 12: [(7, 32), (6, 64)], 13: [(7, 32), (7, 4)], 15: [(7, 16), (6, 32)],
            16: [(7, 16), (6, 16)], 17: [(7, 16)], 18: [(8, 4)], 19: [(8, 4), (6, 64)], 20: [(8, 4), (7, 4)],
            21: [(8, 32)]}[unit]
    for index, bit in bits:
        p[index] |= bit
    return bytes(p)


def segments_packet(patterns, b2_flags=0, tail=(0, 0)):
    """Raw display frame from four LCD symbol patterns (see measurement_asic.c);
    tail sets bytes 7 and 8 (unit/status bits)."""
    p = bytearray(13); p[0], p[1] = 0x5a, 0xa5
    for i, s in enumerate(patterns):
        p[2 + i] |= s & 0xf0; p[3 + i] |= s & 0x0f
    p[2] |= b2_flags
    p[7] |= tail[0]; p[8] |= tail[1]
    return bytes(p)


DASH, BLANK, L_SYM = 0x04, 0x00, 0x61


def ack_packet(mode):
    return bytes([0x5a, 0xa5, 0xfd, mode] + [0] * 9)


def settings_image(view=1, layout=0, english=1, valid=True, auto_off=0):
    h = [0x1, 0, 0, 0, 0, 0, 0, 0, 0, 0x8005, 0xdc, 0x8064, 0x64, 0, 0x64, 0x118, 0] + [0] * 111
    if valid:
        h[0] = 0
    w = h[15]
    w = (w & 0xfcff) | (view << 8)
    w = (w & 0xf3ff) | (layout << 10)
    w = (w & 0xfff8) | english
    w = (w & 0xff3f) | (auto_off << 6)
    h[15] = w
    return struct.pack('<128H', *h)


def battery_raw(percent):
    """Smallest ADC mean whose 75f0 result equals percent (float32 path)."""
    f = np.float32
    for raw in range(4096):
        mv = f(int((raw * 0x19c8) >> 12))
        if int(np.float32((f(1.0) - (f(4000.0) - mv) / f(600.0)) * f(100.0))) == percent:
            return raw
    raise ValueError(percent)


# Each scenario: settings fixture, then steps. Step forms:
#   ('button', event, long) ('packet', bytes) ('tick', seconds) ('charge', 0|1)
SCENARIOS = {
    'regular_v': dict(settings=dict(view=1), steps=[
        ('packet', ack_packet(0x0c)), ('packet', reading_packet('12.34', 6)),
        ('button', 2, False), ('packet', ack_packet(3)), ('packet', reading_packet('12.34', 6)),
        ('button', 1, False), ('packet', ack_packet(0x12)), ('button', 6, False), ('button', 3, False),
        ('button', 4, False), ('button', 6, False)]),
    'regular_units': dict(settings=dict(view=1), steps=[
        ('button', 3, False), ('packet', ack_packet(0x0d)), ('packet', reading_packet('-1.234', 7)),
        ('button', 4, False), ('packet', ack_packet(0x11)), ('packet', reading_packet('0.523', 9)),
        ('button', 4, False), ('packet', ack_packet(0x10)), ('packet', reading_packet('9.999', 10)),
        ('button', 3, False), ('packet', ack_packet(0x0b)), ('packet', reading_packet('47.00', 13)),
        ('button', 3, False), ('packet', ack_packet(0x0a)), ('packet', reading_packet('2.200', 17)),
        ('button', 1, False), ('packet', ack_packet(0x0f)), ('packet', reading_packet('50.00', 20)),
        ('button', 2, True), ('packet', reading_packet('49.00', 20))]),
    'regular_layout1': dict(settings=dict(view=1, layout=1), steps=[
        ('packet', ack_packet(0x0c)), ('packet', reading_packet('3.300', 6)), ('button', 2, False),
        ('packet', ack_packet(3)), ('button', 2, True), ('packet', reading_packet('3.310', 6))]),
    'record': dict(settings=dict(view=2), steps=[
        ('packet', ack_packet(0x0c)), ('packet', reading_packet('1.000', 6)), ('packet', reading_packet('2.000', 6)),
        ('packet', reading_packet('3.000', 6)), ('packet', reading_packet('1.500', 6)), ('button', 2, False),
        ('button', 3, False), ('packet', ack_packet(0x0d)), ('packet', reading_packet('230.0', 6)),
        ('button', 5, False), ('button', 1, False)]),
    'monitor': dict(settings=dict(view=3), steps=[
        ('packet', ack_packet(0x0c)), ('packet', reading_packet('5.000', 6)), ('packet', reading_packet('9.000', 6)),
        ('button', 3, False), ('packet', ack_packet(0x0d)), ('packet', reading_packet('1.000', 6)),
        ('button', 5, False), ('button', 2, False), ('button', 1, False)]),
    'menu_tour': dict(settings=dict(view=1), steps=[
        ('button', 6, False), ('button', 4, False), ('button', 5, False), ('button', 3, False),
        ('button', 5, False), ('button', 4, False), ('button', 4, False), ('button', 5, False),
        ('button', 3, False), ('button', 4, False), ('button', 1, False), ('button', 4, False),
        ('button', 4, False), ('button', 5, False), ('button', 4, False), ('button', 5, False),
        ('button', 1, False), ('button', 6, False)]),
    'chinese': dict(settings=dict(view=1, english=0), steps=[
        ('button', 6, False), ('button', 4, False), ('button', 4, False), ('button', 4, False),
        ('button', 5, False), ('button', 6, False), ('packet', reading_packet('0.000', 6))]),
    'menu_views': dict(settings=dict(view=1), steps=[
        ('button', 6, False), ('button', 3, False), ('button', 1, False), ('button', 5, False), ('button', 6, False),
        ('packet', ack_packet(0x0c)), ('packet', reading_packet('4.200', 6)), ('button', 6, False),
        ('button', 3, False), ('button', 1, False), ('button', 1, False), ('button', 5, False), ('button', 6, False),
        ('packet', reading_packet('4.300', 6))]),
    'menu_thresholds': dict(settings=dict(view=1), steps=[
        ('button', 6, False), ('button', 1, False), ('button', 3, False), ('button', 5, False), ('button', 3, False),
        ('button', 2, False), ('button', 2, False), ('button', 3, False), ('button', 1, False), ('button', 5, False),
        ('button', 1, False), ('button', 5, False), ('button', 4, False), ('button', 1, False), ('button', 6, False)]),
    'menu_settings': dict(settings=dict(view=1), steps=[
        ('button', 6, False), ('button', 1, False), ('button', 1, False), ('button', 1, False), ('button', 3, False),
        ('button', 1, False), ('button', 5, False), ('button', 1, False), ('button', 1, False), ('button', 1, False),
        ('button', 3, False), ('button', 3, False), ('button', 1, False), ('button', 1, False), ('button', 5, False),
        ('button', 1, False), ('button', 1, False), ('button', 5, False), ('button', 4, False), ('button', 6, False),
        ('packet', ack_packet(0x12)), ('packet', reading_packet('25.00', 21))]),
    'history': dict(settings=dict(view=2), steps=[
        ('packet', ack_packet(0x0c))] + [x for i in range(7) for x in (('packet', reading_packet(f'{i + 1}.250', 6)), ('button', 5, False))] + [
        ('button', 6, False), ('button', 1, False), ('button', 1, False), ('button', 3, False), ('button', 1, False),
        ('button', 2, False), ('button', 4, False), ('button', 6, False)]),
    'menu_reset': dict(settings=dict(view=3, layout=1), steps=[
        ('button', 6, False), ('button', 2, False), ('button', 2, False), ('button', 3, False), ('button', 5, False),
        ('button', 3, False), ('button', 2, False), ('button', 5, False), ('packet', reading_packet('1.111', 6))]),
    'ncv_live': dict(settings=dict(view=1), steps=[
        ('button', 1, False), ('packet', ack_packet(0x12)), ('button', 1, False), ('packet', ack_packet(0x0f)),
        ('button', 1, False), ('packet', ack_packet(9)),
        ('packet', segments_packet([BLANK, BLANK, BLANK, BLANK])),
        ('packet', segments_packet([DASH, BLANK, BLANK, BLANK])),
        ('packet', segments_packet([DASH, DASH, DASH, BLANK])),
        ('packet', segments_packet([DASH, DASH, DASH, DASH])),
        ('button', 1, False), ('packet', ack_packet(0x13)),
        ('packet', segments_packet([L_SYM, BLANK, BLANK, BLANK])),
        ('packet', segments_packet([DASH, DASH, DASH, DASH])),
        ('packet', segments_packet([L_SYM, BLANK, BLANK, BLANK])),
        ('button', 1, False), ('packet', ack_packet(0x12))]),
    'ncv_live_layout1': dict(settings=dict(view=1, layout=1), steps=[
        ('button', 1, False), ('button', 1, False), ('button', 1, False), ('packet', ack_packet(9)),
        ('packet', segments_packet([DASH, DASH, BLANK, BLANK])), ('button', 1, False), ('packet', ack_packet(0x13)),
        ('packet', segments_packet([DASH, DASH, DASH, DASH])), ('packet', segments_packet([L_SYM, BLANK, BLANK, BLANK]))]),
    'charging': dict(settings=dict(view=1), steps=[
        ('charge', 1), ('tick', 2), ('charge', 0), ('packet', reading_packet('1.234', 6))]),
}


def simulator_run(settings, steps, idle_steps=3, ticks=None):
    """ticks[i]: TIM3 updates the firmware saw during step i (0 = boot); they are
    replayed after the input so periodic battery refreshes line up."""
    ticks = ticks or [0] * (len(steps) + 1)
    with tempfile.TemporaryDirectory() as tmp:
        state = Path(tmp) / 'state.bin'
        state.write_bytes(b'DMTSIM\x04\x00' + settings + bytes(16))
        script, shots = [], []
        def snap(i):
            path = Path(tmp) / f'{i}.png'
            script.append(f'snapshot {path}')
            shots.append(path)
        if ticks[0]:
            script.append(f'tick {ticks[0]}')
        script += ['step'] * idle_steps
        snap(0)
        for i, step in enumerate(steps, 1):
            kind = step[0]
            if kind == 'button':
                script.append(f'button {step[1]}' + (' long' if step[2] else ''))
            elif kind == 'packet':
                script.append('packet ' + step[1].hex())
            elif kind == 'charge':
                script.append(f'charge {step[1]}')
            if ticks[i]:
                script.append(f'tick {ticks[i]}')
            script += ['step'] * idle_steps
            snap(i)
        script += ['txlog', 'status']
        (Path(tmp) / 'run.sim').write_text('\n'.join(script) + '\n')
        out = subprocess.run([str(SIMULATOR), '--flash', str(FLASH), '--script', str(Path(tmp) / 'run.sim'),
                              '--state', str(state)], check=True, capture_output=True, text=True).stdout
        from PIL import Image
        frames = [list(Image.open(p).convert('RGB').get_flattened_data()) for p in shots]
        lines = out.strip().splitlines()
        return frames, json.loads(lines[-2])['tx'], json.loads(lines[-1])


LEGACY_ZERO = 'aa550500000000000011'


def collapse(tx):
    """0xbc90/0xb5c8/0xb9e0 resend the legacy zero template on every loop pass
    while an unhandled event stays latched; the firmware loops continuously and
    the simulator once per step, so consecutive repeats count once."""
    out = []
    for packet in tx:
        if not (packet == LEGACY_ZERO and out and out[-1] == LEGACY_ZERO):
            out.append(packet)
    return out


def compare(a, b):
    q = lambda c: (c[0] >> 3, c[1] >> 2, c[2] >> 3)
    return sum(1 for x, y in zip(a, b) if q(x) != q(y))


def emulate(settings, steps, out_dir=None, name='', image=None):
    emu = Ch32Emulator(bin_path=image, settings=settings, battery_raw=battery_raw(87))
    emu.run(seconds=3.5)
    ticks = [emu.tim3_updates]
    frames = [list(emu.panel.image().get_flattened_data())]
    if out_dir:
        emu.panel.save(Path(out_dir) / f'{name}_00.png')
    for i, step in enumerate(steps, 1):
        kind = step[0]
        if kind == 'packet':
            emu.frame(step[1])
        elif kind == 'button':
            emu.press(KEYS[step[1]], hold=0.8 if (step[1] == 6 or step[2]) else 0.12)
        elif kind == 'charge':
            emu.charging = bool(step[1])
        before = emu.tim3_updates
        seconds = 1.5 + (step[1] if kind == 'tick' else 0)
        emu.run(seconds=seconds)
        ticks.append(emu.tim3_updates - before)
        frames.append(list(emu.panel.image().get_flattened_data()))
        if out_dir:
            emu.panel.save(Path(out_dir) / f'{name}_{i:02d}.png')
    return frames, [p.hex() for p in emu.tx_packets], emu, ticks


def run_scenario(name, spec, out_dir=None):
    settings = settings_image(**spec['settings'])
    emu_frames, emu_tx, emu, ticks = emulate(settings, spec['steps'], out_dir, name)
    sim_frames, sim_tx, sim_status = simulator_run(settings, spec['steps'], ticks=ticks)
    diffs = [compare(a, b) for a, b in zip(emu_frames, sim_frames)]
    same_tx = collapse(sim_tx) == collapse(emu_tx)
    ok = all(d == 0 for d in diffs) and same_tx and not emu.nor.erases and not emu.nor.programs
    print(f'{"PASS" if ok else "FAIL"} {name}: {len(diffs)} frames, differing pixels {diffs}, '
          f'tx {len(collapse(emu_tx))} {"==" if same_tx else "!="} simulator {len(collapse(sim_tx))} '
          f'(raw {len(emu_tx)}/{len(sim_tx)}), stack unused {emu.stack_unused()} B')
    if not same_tx:
        print('   simulator tx', collapse(sim_tx)[:40])
        print('   firmware  tx', collapse(emu_tx)[:40])
    return ok


def run_first_boot():
    """Erased page: language selection, English, power key -> commit; then the
    committed page boots straight into the measurement screen."""
    emu = Ch32Emulator(battery_raw=battery_raw(87))
    emu.run(seconds=1.5)
    lang = emu.panel.checksum()
    emu.press('PC12', hold=0.12); emu.run(seconds=0.8)
    emu.press('PC8', hold=0.12); emu.run(seconds=3.5)
    shown = emu.panel.checksum()
    emu.press('PC2', hold=0.12); emu.run(seconds=2.0)
    page = bytes(emu.settings)
    h = struct.unpack('<128H', page)
    ok = (h[0] == 0 and h[15] & 7 == 1 and not emu.gpio['C'] & 2 and emu.halted
          and not emu.nor.erases and not emu.nor.programs and lang != shown
          and ('erase', 0x08040000) in emu.flash_ops)
    second = Ch32Emulator(settings=page, battery_raw=battery_raw(87))
    second.run(seconds=3.5)
    ok &= second.panel.checksum() == shown and not any(op[0] == 'erase' for op in second.flash_ops)
    print(f'{"PASS" if ok else "FAIL"} first boot: language screen, commit of 0x08040000, latch released, '
          f'reboot shows the same screen without rewriting')
    if not ok:
        print(emu.text())
    return ok


def run_low_battery():
    """7780: a negative 75f0 result without charger shows the countdown and powers off."""
    emu = Ch32Emulator(settings=settings_image(view=1), battery_raw=battery_raw(87), tim3_warp=20)
    emu.run(seconds=3.5)
    emu.battery_raw = 1900   # about 3061 mV -> -56 %
    emu.run_ticks(22)
    emu.run(seconds=6)
    ok = emu.halted and not emu.gpio['C'] & 2 and any(op[0] == 'erase' for op in emu.flash_ops)
    print(f'{"PASS" if ok else "FAIL"} low battery: countdown, settings commit and power latch release')
    return ok


def run_auto_off():
    """Auto-off 5 min (setting 1): 7780 adds 20 s per 21 ticks and shuts down past 300 s."""
    emu = Ch32Emulator(settings=settings_image(view=1, auto_off=1), battery_raw=battery_raw(87), tim3_warp=50)
    emu.run(seconds=1.0)
    start = emu.tim3_updates
    emu.run_ticks(21 * 15 - 1)          # 14 x 20 s accounted: 280 s idle, not yet > 300
    early = emu.halted
    emu.run_ticks(21 * 2 + 1)
    emu.run(seconds=2)
    ok = not early and emu.halted and not emu.gpio['C'] & 2
    print(f'{"PASS" if ok else "FAIL"} auto-off 5 min: running after {21 * 15 - 1} ticks, powered off after '
          f'{emu.tim3_updates - start} ticks (320 s accounted)')
    return ok


def run_diagnostic_gate():
    """PC10+PC11 held at power-on select 8ed4's diagnostic loop in the original,
    which sends the uninterpreted A0 command. The release build must boot
    normally instead and never transmit A0."""
    emu = Ch32Emulator(settings=settings_image(view=1), battery_raw=battery_raw(87))
    emu.pressed.update({'PC10', 'PC11'})
    emu.run(seconds=3.5)
    emu.pressed.clear()
    emu.run(seconds=1.0)
    ok = not any(p[3] == 0xa0 or p[2] == 0x02 for p in emu.tx_packets) and 'run 0' in emu.text()
    print(f'{"PASS" if ok else "FAIL"} diagnostic entry gated: normal boot with PC10+PC11 held, no A0 packet')
    return ok


def run_hse_failure():
    emu = Ch32Emulator(settings=settings_image(view=1), battery_raw=battery_raw(87), hse_ok=False)
    emu.run(seconds=3.5)
    ok = 'source=HSI-fallback' in emu.text() and 'clock=144000000' in emu.text()
    print(f'{"PASS" if ok else "FAIL"} crystal failure: HSI x18 fallback keeps 144 MHz')
    return ok


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--out')
    p.add_argument('--only', nargs='*')
    p.add_argument('--image', help='firmware BIN (default: standalone build); layout read from its .fw_info')
    args = p.parse_args()
    if args.image:
        os.environ['DMT_FW_IMAGE'] = str(Path(args.image).resolve())
    print('image', default_image())
    if args.out:
        Path(args.out).mkdir(parents=True, exist_ok=True)
    ok = True
    for name, spec in SCENARIOS.items():
        if args.only and name not in args.only:
            continue
        ok &= run_scenario(name, spec, args.out)
    checks = {'first_boot': run_first_boot, 'low_battery': run_low_battery, 'auto_off': run_auto_off,
              'hse_failure': run_hse_failure, 'diagnostic_gate': run_diagnostic_gate}
    for name, check in checks.items():
        if args.only and name not in args.only:
            continue
        ok &= check()
    print('OVERALL', 'PASS' if ok else 'FAIL')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
"""Whole-firmware differential: manufacturer V2.4 image versus the standalone
reconstructed image, on the same emulated DMT99 board.

Both binaries run in tools/fw_emulator.py with the same W25Q64 dump, settings
page, battery level and inputs (keys, measurement frames, charger). After
every step the two ST7789 framebuffers are compared pixel for pixel and the
UART command packets are compared (runs of the legacy zero template that the
original resends while an event stays latched count once).

The original runs from its preserved machine code: its WCH XW compressed
loads/stores are emulated from analysis/instructions.json, the hardware
interrupt prologue (HPE) is emulated around its handlers and main is entered
in machine mode. Its missing 25 KiB prefix is never executed. Scenarios run in
parallel worker processes.

Frames are compared once the V2.4 side has settled after each input (its
drawing is slower in this emulator); the firmware then runs for the same
virtual time, so both see the same timer schedule.

Usage: test_original_differential.py [--out DIR] [--only NAME ...] [--jobs N] [--report FILE]
"""
import argparse, os, sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fw_emulator import Ch32Emulator, default_image
from test_firmware_emulation import SCENARIOS, KEYS, settings_image, battery_raw, compare, collapse

EXTRA = {
    # First boot with an erased settings page: the original's blocking 15168
    # language loop versus the cooperative selection state.
    'first_boot_language': dict(settings=None, steps=[
        ('button', 4, False), ('button', 3, False), ('button', 4, False), ('button', 5, False),
        ('packet', bytes([0x5a, 0xa5, 0xfd, 0x14] + [0] * 9))]),
}


def settle(emu, minimum, maximum=8.0, quantum=0.25, stable=3):
    """Run at least `minimum` seconds, then until the panel is unchanged for
    `stable` consecutive quanta (blocking loops that redraw identical pixels
    count as stable). Returns the elapsed virtual seconds."""
    emu.run(seconds=minimum)
    elapsed, last, same = minimum, emu.panel.checksum(), 0
    while elapsed < maximum and same < stable:
        emu.run(seconds=quantum)
        elapsed += quantum
        now = emu.panel.checksum()
        same = same + 1 if now == last else 0
        last = now
    return elapsed


def drive(original, spec, out_dir, name, durations=None):
    """durations=None: settle each step and record the time (V2.4 side);
    otherwise run each step for the recorded time (firmware side), so both
    images see the same timer schedule."""
    settings = settings_image(**spec['settings']) if spec['settings'] is not None else None
    emu = Ch32Emulator(original=original, settings=settings, battery_raw=battery_raw(87))
    record = durations is None
    durations = [] if record else list(durations)
    if record:
        durations.append(settle(emu, 3.5))
    else:
        emu.run(seconds=durations[0])
    frames = [list(emu.panel.image().get_flattened_data())]
    tag = 'orig' if original else 'ours'
    if out_dir:
        emu.panel.save(Path(out_dir) / f'{name}_{tag}_00.png')
    for i, step in enumerate(spec['steps'], 1):
        kind = step[0]
        if kind == 'packet':
            emu.frame(step[1])
        elif kind == 'button':
            emu.press(KEYS[step[1]], hold=0.8 if (step[1] == 6 or step[2]) else 0.12)
        elif kind == 'charge':
            emu.charging = bool(step[1])
        base = 1.5 + (step[1] if kind == 'tick' else 0)
        if record:
            durations.append(settle(emu, base))
        else:
            emu.run(seconds=durations[i])
        frames.append(list(emu.panel.image().get_flattened_data()))
        if out_dir:
            emu.panel.save(Path(out_dir) / f'{name}_{tag}_{i:02d}.png')
    return frames, [p.hex() for p in emu.tx_packets], bytes(emu.settings), emu.halted, durations


def run(name, spec, out_dir):
    orig = drive(True, spec, out_dir, name)
    ours = drive(False, spec, out_dir, name, durations=orig[4])
    diffs = [compare(a, b) for a, b in zip(ours[0], orig[0])]
    same_tx = collapse(ours[1]) == collapse(orig[1])
    same_page = ours[2] == orig[2]
    ok = all(d == 0 for d in diffs) and same_tx and same_page
    lines = [f'{"PASS" if ok else "FAIL"} {name}: {len(diffs)} frames, differing pixels {diffs}, '
             f'tx {len(collapse(ours[1]))} {"==" if same_tx else "!="} original {len(collapse(orig[1]))}, '
             f'settings page {"identical" if same_page else "differs"}']
    if not same_tx:
        lines.append(f'   original tx {collapse(orig[1])[:30]}')
        lines.append(f'   firmware tx {collapse(ours[1])[:30]}')
    result = {'scenario': name, 'pass': ok, 'frames': len(diffs), 'differing_pixels': diffs,
              'tx_original': collapse(orig[1]), 'tx_firmware': collapse(ours[1]), 'settings_page_identical': same_page,
              'step_seconds': orig[4]}
    return ok, '\n'.join(lines), result


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--out')
    p.add_argument('--only', nargs='*')
    p.add_argument('--jobs', type=int, default=6)
    p.add_argument('--report', help='write JSON results (e.g. analysis/original_differential.json)')
    p.add_argument('--image', help='firmware BIN (default: standalone build); layout read from its .fw_info')
    args = p.parse_args()
    if args.image:
        os.environ['DMT_FW_IMAGE'] = str(Path(args.image).resolve())
    print('image', default_image(), flush=True)
    if args.out:
        Path(args.out).mkdir(parents=True, exist_ok=True)
    scenarios = {**SCENARIOS, **EXTRA}
    names = [n for n in scenarios if not args.only or n in args.only]
    ok = True
    with ProcessPoolExecutor(max_workers=args.jobs) as pool:
        futures = {n: pool.submit(run, n, scenarios[n], args.out) for n in names}
        results = []
        for n in names:
            passed, text, result = futures[n].result()
            print(text, flush=True)
            ok &= passed
            results.append(result)
    if args.report:
        import json, hashlib
        root = Path(__file__).resolve().parents[1]
        image = default_image()
        Path(args.report).write_text(json.dumps({
            'definition': 'V2.4 image versus reconstructed firmware on the emulated board; frames compared after the '
                          'V2.4 side settles, firmware run for the same virtual time; no hardware involved',
            'firmware_image': str(image.relative_to(root)) if image.is_relative_to(root) else str(image),
            'firmware_sha256': hashlib.sha256(image.read_bytes()).hexdigest(),
            'original_sha256': hashlib.sha256((root / 'DMT-99_V2.4_20240306.bin').read_bytes()).hexdigest(),
            'pass': ok, 'scenarios': results}, indent=1) + '\n')
    print('OVERALL', 'PASS' if ok else 'FAIL')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()

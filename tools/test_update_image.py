#!/usr/bin/env python3
"""Checks for the USB update image (reconstructed/build/firmware-update).

The release BIN is meant to be installed by the meter's own bootloader
(POWER+MENU, copy the file to the USB disk) at Code Flash 0x08006400, exactly
like DMT-99_V2.4_20240306.bin. That bootloader has never been read, so this
test checks everything that can be checked without it:

  1. format: the V2.4 update-file header shape (j, 12 nops, ebreak, vector
     table at +0x38 with slot 0 = 0x6400), .fw_info load address 0x08006400
     and version, every vector inside the image, image within 0x6400..0x30000;
  2. hand-over: started with leftover bootloader interrupt state
     (Ch32Emulator bootloader_leftovers: SysTick and OTG_FS interrupts enabled
     and pending, an armed fast-vector slot) it reaches the same screen as a
     clean start without taking a stray interrupt;
  3. last measurement mode (release only): the mode in use at power-off is
     stored in the unused settings halfwords 120..127 and restored at the next
     start; the parity twin leaves them zero; no other Flash page is written;
  4. themes (release only): a THEME.PKG on the BOOT volume is installed by the
     real firmware at start-up into themes/T01 on the assets volume, activated
     (the themed artwork is drawn, the THEME.PAL palette recolours the
     firmware-drawn classic digits) and deleted; originals are unchanged; the
     parity twin ignores the package;
  5. simple screen (release only): with the image-free screen enabled in the
     settings (halfwords 110..113) the Regular view is drawn in the chosen
     colours with no JPEG; the parity twin ignores the setting;
  6. release versus parity twin: the scenarios of test_firmware_emulation.py
     (all, or --only) give identical frames and UART commands, except the About
     page (version label and the theme-manager hint), only inside its box.

The parity twin itself is compared with V2.4 and with the simulator by
test_original_differential.py and test_firmware_emulation.py (--image).
Usage: test_update_image.py [--version X.Y.Z] [--out DIR] [--jobs N] [--only SCENARIO...]
"""
import argparse, hashlib, struct, sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import update_image_format
from fw_emulator import Ch32Emulator, ROOT, IRQ_SYSTICK, IRQ_OTG_FS
from test_firmware_emulation import SCENARIOS, KEYS, emulate, settings_image, battery_raw, collapse, ack_packet, reading_packet

BUILD = ROOT / 'reconstructed/build'
RELEASE = BUILD / 'firmware-update/dmt99-update.bin'
PARITY = BUILD / 'firmware-update-parity/dmt99-update-parity.bin'
ORIGINAL = ROOT / 'DMT-99_V2.4_20240306.bin'
# About page label: V2.4 draws "DMT-99 V2.4" at x 180..263, y 163..178; the
# release label is re-centred on it and the "ENTER: display" hint sits below it
# (y 196..211). Pixels outside this box must not change.
BOX = (164, 158, 284, 214)


def check_format(path, version):
    problems, info = update_image_format.check(path.read_bytes(), ORIGINAL.read_bytes(), version)
    print(f'{"PASS" if not problems else "FAIL"} format: {path.name} {info.get("size")} bytes at '
          f'{info.get("flash_start", 0):#x}..{info.get("flash_end", 0):#x}, V2.4 header shape, '
          f'reset -> {info.get("reset", 0):#x}, version {info.get("version")}')
    for p in problems:
        print('   ', p)
    return not problems


def check_handover(path):
    runs = []
    for leftovers in (False, True):
        emu = Ch32Emulator(bin_path=path, settings=settings_image(view=1), battery_raw=battery_raw(87),
                           bootloader_leftovers=leftovers)
        emu.run(seconds=3.5)
        runs.append(emu)
    clean, dirty = runs
    stray = sorted({irq for _, irq in dirty.irq_log if irq in (IRQ_SYSTICK, IRQ_OTG_FS)})
    ok = (not clean.fault and not dirty.fault and not dirty.reset_requested and not stray
          and clean.panel.checksum() == dirty.panel.checksum() and [p.hex() for p in clean.tx_packets] ==
          [p.hex() for p in dirty.tx_packets])
    print(f'{"PASS" if ok else "FAIL"} hand-over: leftover SysTick/OTG_FS/VTF state from the bootloader is cleared, '
          f'same screen and commands as a clean start (fault {dirty.fault}, stray irqs {stray})')
    return ok


def check_last_mode():
    def session(image, last_page=None, settings=None):
        emu = Ch32Emulator(bin_path=image, settings=settings or settings_image(view=1), last_page=last_page,
                           battery_raw=battery_raw(87))
        emu.run(seconds=3.5)
        return emu
    def change_and_power_off(emu):
        for _ in range(3):
            emu.press(KEYS[1], hold=0.12)
            emu.run(seconds=1.0)
        mode = emu.tx_packets[-1][3]
        emu.frame(ack_packet(mode))
        emu.run(seconds=1.5)
        shown = emu.panel.checksum()
        emu.press('PC2', hold=0.12)
        emu.run(seconds=2.0)
        return mode, shown
    a = session(RELEASE)
    mode, shown = change_and_power_off(a)
    record = bytes(a.settings[240:256])
    b = session(RELEASE, None, bytes(a.settings))
    b.frame(ack_packet(mode))
    b.run(seconds=1.5)
    restored = b.tx_packets[0][3] == mode and b.panel.checksum() == shown and record[:4] == b'DM99' and record[5] == mode
    c = session(RELEASE, None, bytes(a.settings))       # restored mode itself survives the next power-off
    c.frame(ack_packet(mode))
    c.run(seconds=1.0)
    c.press('PC2', hold=0.12)
    c.run(seconds=2.0)
    kept = bytes(c.settings[240:256]) == record
    d = session(PARITY)
    change_and_power_off(d)
    parity = bytes(d.settings[240:256]) == bytes(16)
    other_page = all(not (0x08040100 <= op[1] < 0x08040200) for e in (a, b, c, d) for op in e.flash_ops)
    ok = a.halted and restored and kept and parity and other_page
    print(f'{"PASS" if ok else "FAIL"} last mode: mode 0x{mode:02x} saved in settings halfwords 120..127 at power-off '
          f'({record[:8].hex()}), restored on the next start ({restored}), kept over another cycle ({kept}); '
          f'parity twin leaves them zero ({parity}); no other Flash page written ({other_page})')
    return ok


def check_themes():
    import tempfile
    from theme_fixture import build_package, solid_jpeg, with_boot_file
    from flash_forensics import FAT
    dump = (ROOT / 'flashdump/dmt99_flash_1.bin').read_bytes()
    art = solid_jpeg(83, 23, (255, 0, 0))                     # mode/dc_v.jpg is 83x23
    green = 0x07E0                                            # palette: classic digits 0xfd40 -> green
    image = with_boot_file(dump, 'THEME.PKG', build_package('Emulator test', {'mode/dc_v.jpg': art}, {0xFD40: green}))
    results = {}
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / 'flash.bin'
        path.write_bytes(image)
        for label, firmware in (('release', RELEASE), ('parity', PARITY)):
            emu = Ch32Emulator(bin_path=firmware, flash_dump=path, settings=settings_image(view=1),
                               battery_raw=battery_raw(87))
            emu.run(seconds=8.0)
            emu.frame(ack_packet(0x0c))
            emu.run(seconds=1.0)
            emu.frame(reading_packet('88.88', 6))
            emu.run(seconds=1.0)
            r, g, b = emu.panel.image().getpixel((150, 40))[:3]
            digit = {emu.panel.image().getpixel((x, y))[:3] for x in range(70, 150, 2) for y in range(92, 140, 2)}
            q = lambda c: (c[0] >> 3, c[1] >> 2, c[2] >> 3)
            digits_green = any(q(c) == (0, 63, 0) for c in digit) and not any(q(c) == (31, 42, 0) for c in digit)
            parts = []
            for k in range(2):
                first, count = struct.unpack_from('<II', emu.nor.data, 446 + 16 * k + 8)
                fat = FAT(bytes(emu.nor.data), first * 4096, count * 4096, k)
                fat.walk()
                parts.append({f['path'].lower(): f for f in fat.files})
            themed = parts[1].get('themes/t01/mode/dc_v.jpg')
            results[label] = {'fault': emu.fault, 'red': r > 200 and g < 60 and b < 60, 'digits_green': digits_green,
                              'installed': bool(themed) and themed.get('sha256') == hashlib.sha256(art).hexdigest(),
                              'package_left': 'theme.pkg' in parts[0],
                              'original_kept': parts[1].get('mode/dc_v.jpg', {}).get('sha256') ==
                              hashlib.sha256(FAT_file(dump, 'mode/dc_v.jpg')).hexdigest()}
    rel, par = results['release'], results['parity']
    ok = (not rel['fault'] and rel['installed'] and rel['red'] and rel['digits_green'] and not rel['package_left']
          and rel['original_kept'] and not par['fault'] and not par['installed'] and not par['red']
          and not par['digits_green'] and par['package_left'])
    print(f'{"PASS" if ok else "FAIL"} themes: release installs THEME.PKG into themes/T01 ({rel["installed"]}), '
          f'draws it ({rel["red"]}), palette recolours the classic digits ({rel["digits_green"]}), '
          f'deletes the package ({not rel["package_left"]}), keeps the original '
          f'({rel["original_kept"]}); parity ignores it ({not par["installed"] and par["package_left"]})')
    return ok


def check_simple():
    settings = bytearray(settings_image(view=1))
    h = list(struct.unpack('<128H', settings))
    h[110] = 0xA500 | 1 | (2 << 1)          # on, large digits
    h[111] = 3 | (7 << 8)                   # background dark grey, digits lime
    h[112] = 1 | (5 << 8)                   # text white, accent orange
    h[113] = h[110] ^ h[111] ^ h[112] ^ 0x5A5A
    image = struct.pack('<128H', *h)
    results = {}
    for label, firmware in (('release', RELEASE), ('parity', PARITY)):
        emu = Ch32Emulator(bin_path=firmware, settings=image, battery_raw=battery_raw(87))
        emu.run(seconds=3.5)
        emu.frame(ack_packet(0x0c))
        emu.run(seconds=1.0)
        emu.frame(reading_packet('12.34', 6))
        emu.run(seconds=1.0)
        px = list(emu.panel.image().get_flattened_data())
        q = lambda c: (c[0] >> 3, c[1] >> 2, c[2] >> 3)
        colours = {q(c[:3]) for c in px}
        background = sum(1 for c in px if q(c[:3]) == (7, 15, 7))          # 0x39e7 dark grey
        results[label] = {'fault': emu.fault, 'background': background > 30000,
                          'lime': (16, 63, 0) in colours}                   # 0x87e0 digits
    rel, par = results['release'], results['parity']
    ok = (not rel['fault'] and rel['background'] and rel['lime'] and not par['fault']
          and not par['background'] and not par['lime'])
    print(f'{"PASS" if ok else "FAIL"} simple screen: release draws the image-free view in the chosen colours '
          f'(background {rel["background"]}, digits {rel["lime"]}); parity ignores the setting '
          f'({not par["background"] and not par["lime"]})')
    return ok


def FAT_file(image, path):
    from flash_forensics import FAT
    first, count = struct.unpack_from('<II', image, 446 + 16 + 8)
    fat = FAT(image, first * 4096, count * 4096, 1)
    fat.walk()
    e = next(f for f in fat.files if f['path'].lower() == path)
    return fat.content(e['clusters'])[:e['size']]


def outside_box(index):
    x, y = index % 320, index // 320
    return not (BOX[0] <= x < BOX[2] and BOX[1] <= y < BOX[3])


def compare_scenario(name, out):
    spec = SCENARIOS[name]
    settings = settings_image(**spec['settings'])
    a_frames, a_tx, a, _ = emulate(settings, spec['steps'], image=RELEASE)
    b_frames, b_tx, b, _ = emulate(settings, spec['steps'], image=PARITY)
    q = lambda c: (c[0] >> 3, c[1] >> 2, c[2] >> 3)
    outside, inside = [], []
    for i, (fa, fb) in enumerate(zip(a_frames, b_frames)):
        diff = [k for k, (x, y) in enumerate(zip(fa, fb)) if q(x) != q(y)]
        outside.append(sum(1 for k in diff if outside_box(k)))
        inside.append(len(diff) - outside[-1])
        if inside[-1] and out:
            from PIL import Image
            img = Image.new('RGB', (320, 240))
            img.putdata([tuple(c) for c in fa])
            img.save(Path(out) / f'about-{name}-{i:02d}.png')
    same_tx = collapse(a_tx) == collapse(b_tx)
    ok = not any(outside) and same_tx and not a.fault and not b.fault
    return name, ok, outside, inside, same_tx


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--version', help='expected .fw_info version of the release image')
    p.add_argument('--out', help='save About-page frames of the release image here')
    p.add_argument('--jobs', type=int, default=6)
    p.add_argument('--only', nargs='*', help='release-vs-parity scenarios (default: all)')
    args = p.parse_args()
    names = [n for n in SCENARIOS if not args.only or n in args.only]
    if args.out:
        Path(args.out).mkdir(parents=True, exist_ok=True)
    ok = check_format(RELEASE, args.version)
    ok &= check_format(PARITY, args.version)
    ok &= check_handover(RELEASE)
    ok &= check_last_mode()
    ok &= check_themes()
    ok &= check_simple()
    label_changed = False
    with ProcessPoolExecutor(max_workers=args.jobs) as pool:
        for name, passed, outside, inside, same_tx in pool.map(compare_scenario, names, [args.out] * len(names)):
            label_changed |= any(inside)
            print(f'{"PASS" if passed else "FAIL"} release vs parity {name}: differing pixels outside the About label '
                  f'{outside}, inside {inside}, tx {"same" if same_tx else "DIFFERENT"}')
            ok &= passed
    print(f'{"PASS" if label_changed else "FAIL"} About page shows the release version (label box differs)')
    ok &= label_changed
    print('OVERALL', 'PASS' if ok else 'FAIL')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()

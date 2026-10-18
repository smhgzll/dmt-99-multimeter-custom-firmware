#!/usr/bin/env python3
"""Publish the USB update image into final/v<version>/.

  final/v<version>/DMT-99_V<version>_<date>.bin   the file to copy to the meter
  final/v<version>/SHA256SUMS                     checksums of every file below
  final/v<version>/release.json                   version, commit, layout, hashes
  final/v<version>/debug/dmt99-update-<version>.{elf,map}   symbols for debugging

The BIN is checked against the V2.4 update-file format (update_image_format.py)
first. A published version is immutable: if final/v<version>/ already holds a
different BIN, the version has to be bumped (Semantic Versioning 2.0.0).

Usage: make_release.py --version X.Y.Z --date YYYYMMDD --git HASH
                       --build BUILD/dmt99-update --original V2.4.bin --out final
"""
import argparse, hashlib, json, re, shutil, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import update_image_format

SEMVER = re.compile(r'^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)'
                    r'(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?'
                    r'(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$')


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--version', required=True)
    p.add_argument('--date', required=True)
    p.add_argument('--git', required=True)
    p.add_argument('--build', required=True, help='path of the update build without extension')
    p.add_argument('--original', required=True)
    p.add_argument('--out', required=True)
    args = p.parse_args()
    if not SEMVER.match(args.version):
        sys.exit(f'{args.version!r} is not a Semantic Versioning 2.0.0 version')
    if not re.fullmatch(r'\d{8}', args.date):
        sys.exit(f'date {args.date!r} is not YYYYMMDD')
    build = Path(args.build)
    binary, elf, mapfile = (build.with_suffix(s) for s in ('.bin', '.elf', '.map'))
    image, original = binary.read_bytes(), Path(args.original).read_bytes()
    problems, info = update_image_format.check(image, original, args.version)
    if problems:
        sys.exit('update image rejected:\n  ' + '\n  '.join(problems))

    folder = Path(args.out) / f'v{args.version}'
    name = f'DMT-99_V{args.version}_{args.date}.bin'
    digest = hashlib.sha256(image).hexdigest()
    existing = sorted(folder.glob('DMT-99_V*.bin')) if folder.exists() else []
    if existing:
        if sha256(existing[0]) == digest:
            print(f'{folder}: v{args.version} already published with this image ({existing[0].name}), unchanged')
            return
        sys.exit(f'{folder}: v{args.version} is already published with a different image '
                 f'({existing[0].name}); bump FW_VERSION instead of replacing a release')

    (folder / 'debug').mkdir(parents=True)
    shutil.copyfile(binary, folder / name)
    shutil.copyfile(elf, folder / 'debug' / f'dmt99-update-{args.version}.elf')
    shutil.copyfile(mapfile, folder / 'debug' / f'dmt99-update-{args.version}.map')
    release = {
        'name': 'DMT99 reconstructed firmware',
        'version': args.version,
        'date': args.date,
        'source_commit': args.git,
        'file': name,
        'size': len(image),
        'sha256': digest,
        'format': 'raw application BIN for the manufacturer USB bootloader, same layout as DMT-99_V2.4_20240306.bin',
        'flash_range': [f'{info["flash_start"]:#010x}', f'{info["flash_end"] - 1:#010x}'],
        'preserved': {'bootloader': '0x08000000-0x080063ff (not part of the image, never written)',
                      'settings_page': '0x08040000-0x080400ff (V2.4 format, kept)'},
        'reset_entry': f'{info["reset"]:#x}',
        'rollback_file': {'name': 'DMT-99_V2.4_20240306.bin', 'sha256': hashlib.sha256(original).hexdigest()},
        'verified_on': 'emulated CH32V303 board only (tools/test_update_image.py, test_original_differential.py, '
                       'test_firmware_emulation.py); not yet run on a physical DMT99',
    }
    (folder / 'release.json').write_text(json.dumps(release, indent=1) + '\n')
    files = [folder / name, folder / 'release.json'] + sorted((folder / 'debug').iterdir())
    (folder / 'SHA256SUMS').write_text(''.join(f'{sha256(f)}  {f.relative_to(folder).as_posix()}\n' for f in files))
    print(f'published {folder / name}  ({len(image)} bytes, sha256 {digest})')


if __name__ == '__main__':
    main()

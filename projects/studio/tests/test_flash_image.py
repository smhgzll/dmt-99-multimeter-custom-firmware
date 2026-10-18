"""Reading the original dump, both FAT12 volumes, replacement and safety."""
import hashlib

import pytest

from dmt99studio.core.fat12 import FatError
from dmt99studio.core.flashimage import (ORIGINAL_DUMP_SHA256, SECTOR, FlashImage)


def test_original_dump_layout(image):
    assert image.is_original_dump
    assert [(p.first_sector, p.sectors) for p in image.partitions] == [(63, 400), (463, 1585)]
    assert image.boot.bps == 4096 and image.assets.bps == 4096
    assert image.boot.fs_type == 'FAT12' and image.assets.fs_type == 'FAT12'
    assert image.assets.validate() == []
    # Pre-existing state of the BOOT volume in the acquired dump (left by the
    # bootloader's USB disk); Studio reports it but never writes that volume.
    assert all(i.startswith(('System Volume', 'volume 0')) for i in image.validate())


def test_both_volumes_list_expected_files(image):
    boot = {e.path.lower() for e in image.boot.walk()}
    assert boot == {'system volume information'}
    assets = {e.path.lower() for e in image.assets.walk()}
    for folder in ('menu', 'mode', 'units', 'num', 'history', 'cell', 'time', 'motif2'):
        assert folder in assets, folder
    assert 'mode/dc_v.jpg' in assets
    jpegs = [e for e in image.assets.walk() if not e.is_dir and e.path.lower().endswith('.jpg')]
    assert len(jpegs) == 297  # count in the acquired dump
    for e in jpegs[:40]:
        assert image.read_file(e)[:2] == b'\xff\xd8'


def test_firmware_paths_resolve(image):
    assert image.resolve('mode/dc_v.jpg') is not None
    assert image.resolve('MODE/DC_V.JPG') is not None
    # The firmware's "0:/on.jpg" opens volume 1 (shared FATFS object quirk).
    assert image.resolve('0:/on.jpg').volume == 1
    assert image.resolve('0:/LOGO.jpg') is None  # optional custom logo, deleted in this dump


def test_replace_same_size_changes_only_data_sectors(image):
    e = image.resolve('mode/dc_v.jpg')
    old = image.read_file(e)
    new = bytes(b ^ 0x5A for b in old[:20]) + old[20:]
    image.replace_file(e, new)
    assert image.read_file(image.resolve('mode/dc_v.jpg')) == new
    changed = image.changed_sectors()
    assert changed and all(image.region_of(s) == 'assets' for s in changed)
    # Same length -> same clusters, FAT and directory untouched.
    data_first = image.assets.data_offset // SECTOR
    assert all(s >= data_first for s in changed)
    assert image.assets.validate() == []


def test_replace_larger_allocates_and_validates(image):
    e = image.resolve('units/v.jpg')
    clusters_before = len(e.clusters)
    big = image.read_file(e) + bytes(3 * 4096)
    image.replace_file(e, big)
    e2 = image.resolve('units/v.jpg')
    assert len(e2.clusters) == clusters_before + 3
    assert image.read_file(e2) == big
    assert image.assets.validate() == []
    report = image.diff_report()
    assert [f['path'] for f in report['files']] == [e2.path]
    assert report['changed_by_region'] == {'assets': len(report['changed_sectors'])}


def test_replace_smaller_frees_clusters(image):
    e = image.resolve('motif2/electric_frame.jpg')
    free_before = len(image.assets.free_clusters())
    image.replace_file(e, b'\xff\xd8\xff\xd9')
    assert len(image.assets.free_clusters()) == free_before + len(e.clusters) - 1 or len(e.clusters) == 1
    assert image.assets.validate() == []


def test_boot_volume_is_not_writable(image):
    entry = image.boot.walk()[0]
    with pytest.raises(FatError):
        image.replace_file(entry, b'x')


def test_create_file(image):
    image.create_file('menu', 'THEME.TXT', b'hello')
    e = image.resolve('menu/theme.txt')
    assert e and image.read_file(e) == b'hello'
    assert image.assets.validate() == []


def test_out_of_space_is_rejected(image):
    e = image.resolve('units/v.jpg')
    with pytest.raises(FatError):
        image.replace_file(e, bytes(image.assets.usage()['free_bytes'] + 2 * 4096 + len(e.clusters) * 4096))
    assert image.assets.validate() == []


def test_never_overwrites_original(image, tmp_path, original_path):
    with pytest.raises(PermissionError):
        image.save_as(original_path)
    copy = tmp_path / 'copy.bin'
    copy.write_bytes(original_path.read_bytes())
    with pytest.raises(PermissionError):
        image.save_as(copy)  # same content as the original dump
    out = image.save_as(tmp_path / 'work.bin')
    assert hashlib.sha256(out.read_bytes()).hexdigest() == ORIGINAL_DUMP_SHA256


def test_saved_modified_image_round_trips(image, tmp_path):
    e = image.resolve('num/radix.jpg')
    image.replace_file(e, image.read_file(e) + b'\0' * 10)
    out = image.save_as(tmp_path / 'mod.bin')
    again = FlashImage.load(out)
    assert again.read_file(again.resolve('num/radix.jpg')) == image.read_file(image.resolve('num/radix.jpg'))
    assert again.assets.validate() == []
    patch = image.binary_diff()
    assert patch.startswith(b'DMT99SECTORPATCH') and len(patch) == 17 + len(image.changed_sectors()) * (4 + 4096)

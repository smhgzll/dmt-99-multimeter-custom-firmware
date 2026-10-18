"""THEME.PKG: package format and the firmware's installer/manager, run in the simulator."""
import io

import pytest
from PIL import Image

from dmt99studio.core import jpegtools, themepkg
from dmt99studio.core.flashimage import FlashImage
from dmt99studio.core.project import ThemeSession


def solid(image, key, color):
    info = jpegtools.inspect(image.read_file(image.resolve(key)))
    return jpegtools.encode(Image.new('RGB', (info.width, info.height), color))


def package(image, name, colors):
    s = ThemeSession(image)
    for key, color in colors.items():
        assert s.replace(key, solid(image, key, color)).ok
    return themepkg.from_session(s, name)


def originals_untouched(original, image):
    for e in original.assets.walk():
        if e.is_dir:
            continue
        other = image.resolve('1:/' + e.path)
        if other is None or image.read_file(other) != original.read_file(e):
            return e.path
    return None


def test_package_round_trip_and_rules(image):
    data = package(image, 'Round trip', {'1:/mode/dc_v.jpg': (255, 0, 0)})
    p = themepkg.parse(data)
    assert p.name == 'Round trip' and list(p.files) == ['mode/dc_v.jpg']
    bad = bytearray(data)
    bad[len(bad) // 2] ^= 0xFF
    with pytest.raises(themepkg.PackageError):
        themepkg.parse(bytes(bad))
    for path in ('../x.jpg', '/abs.jpg', 'a/b/c/d/e.jpg', 'mode/x.png', 'mo de/x.jpg'):
        with pytest.raises(themepkg.PackageError):
            themepkg.build('x', {path: b'\xff\xd8\xff\xe0'})
    with pytest.raises(themepkg.PackageError):
        themepkg.build('x', {'mode/dc_v.jpg': b'PNG'})


def test_install_select_persist_delete(image, original_path):
    original = FlashImage.load(original_path)
    pkg1 = package(image, 'Red', {'1:/mode/dc_v.jpg': (255, 0, 0), '1:/units/v.jpg': (0, 255, 0)})
    img1, msg, _ = themepkg.simulate_install(bytes(image.data), pkg1)
    assert msg.startswith('Installed')
    im1 = FlashImage(img1)
    themes = themepkg.installed_themes(im1)
    assert [(t['slot'], t['name'], t['valid'], t['files']) for t in themes] == [(1, 'Red', True, 2)]
    assert im1.boot.find('THEME.PKG') is None            # package removed from the bootloader disk
    assert im1.assets.validate() == []
    assert originals_untouched(original, im1) is None
    assert im1.read_file(im1.resolve('themes/T01/mode/dc_v.jpg')) == themepkg.parse(pkg1).files['mode/dc_v.jpg']

    # Second theme goes to a new slot; the first one stays.
    pkg2 = package(image, 'Blue', {'1:/mode/dc_v.jpg': (0, 0, 255)})
    shot = '/tmp/claude-themes-check.png'
    img2, msg, _ = themepkg.simulate_install(img1, pkg2, script=(
        'ack 0x0c', 'reading 12.34 6', 'poweroff', 'poweron', 'ack 0x0c', 'reading 12.34 6', f'snapshot {shot}'))
    assert msg.startswith('Installed')
    im2 = FlashImage(img2)
    assert [t['name'] for t in themepkg.installed_themes(im2)] == ['Red', 'Blue']
    # After a power cycle the new theme is still active: DC V artwork is blue.
    px = Image.open(shot).convert('RGB').getpixel((150, 40))
    assert px[2] > 200 and px[0] < 60, px

    # Delete the active theme in the meter's theme manager: Menu > About > ENTER, ENTER, OK.
    img3, _, _ = themepkg.simulate_install(img2, package(image, 'Green', {'1:/units/v.jpg': (0, 200, 0)}), script=(
        'button 6', 'button 2', 'button 3', 'button 2', 'button 3', 'button 5', 'button 6'))
    im3 = FlashImage(img3)
    names = [t['name'] for t in themepkg.installed_themes(im3)]
    assert names == ['Red', 'Blue'], names                # 'Green' (active, slot 3) deleted
    assert im3.resolve('themes/T03') is None and im3.assets.validate() == []
    assert originals_untouched(original, im3) is None


def test_deleting_everything_restores_free_space(image):
    pkg = package(image, 'Tmp', {'1:/mode/dc_v.jpg': (9, 9, 9)})
    free_before = image.assets.usage()['free_clusters']
    out, msg, _ = themepkg.simulate_install(bytes(image.data), pkg, script=(
        'button 6', 'button 2', 'button 3', 'button 2', 'button 3', 'button 5', 'button 6'))
    im = FlashImage(out)
    assert msg.startswith('Installed')
    assert themepkg.installed_themes(im) == [] and im.resolve('themes') is None
    assert im.assets.usage()['free_clusters'] == free_before


def test_rejected_packages_change_nothing(image):
    good = package(image, 'Bad', {'1:/mode/dc_v.jpg': (1, 2, 3)})
    damaged = bytearray(good)
    damaged[100] ^= 0x55
    out, msg, _ = themepkg.simulate_install(bytes(image.data), bytes(damaged))
    im = FlashImage(out)
    assert 'damaged' in msg and themepkg.installed_themes(im) == [] and im.boot.find('THEME.PKG') is None
    assert im.assets.usage() == image.assets.usage()
    unknown = themepkg.build('Unknown', {'mode/not_there.jpg': solid(image, '1:/mode/dc_v.jpg', (1, 1, 1))})
    out, msg, _ = themepkg.simulate_install(bytes(image.data), unknown)
    assert 'unknown files' in msg and themepkg.installed_themes(FlashImage(out)) == []


def test_not_enough_space(image):
    full = FlashImage(bytes(image.data))
    free = full.assets.usage()['free_bytes']
    full.create_file('', 'FILLER.BIN', bytes(free - 2 * 4096))   # leave 2 clusters
    pkg = package(image, 'Big', {'1:/motif2/electric_frame.jpg': (200, 100, 0), '1:/on.jpg': (0, 0, 0)})
    out, msg, _ = themepkg.simulate_install(bytes(full.data), pkg)
    im = FlashImage(out)
    assert 'space' in msg and themepkg.installed_themes(im) == [] and im.assets.validate() == []


def test_palette_package_and_classic_digits(image):
    from dmt99studio.core import colorops
    pal = colorops.palette_for(colorops.ColorAdjust(hue_shift=60, hue_center=35, hue_width=25))
    assert pal == {0xFD40: pal[0xFD40]} and pal[0xFD40] != 0xFD40
    data = themepkg.build('Green digits', {}, pal)          # palette-only theme
    assert themepkg.parse(data).palette == pal
    shot = '/tmp/claude-themes-palette.png'
    out, msg, _ = themepkg.simulate_install(bytes(image.data), data, script=(
        'ack 0x0c', 'reading 88.88 6', f'snapshot {shot}'))
    assert msg.startswith('Installed'), msg
    t = themepkg.installed_themes(FlashImage(out))
    assert t[0]['palette'] == pal and t[0]['files'] == 0
    # Classic-layout digits (firmware ROM glyphs, normally orange 0xfd40) are now green. The first
    # digit is checked; the decimal point is a JPEG (num/radix.jpg) and is recoloured as an image.
    px = Image.open(shot).convert('RGB')
    colours = {px.getpixel((x, y)) for x in range(70, 150, 2) for y in range(92, 140, 2)}
    target = colorops.rgb565_to_rgb(pal[0xFD40])
    assert target in colours and colorops.rgb565_to_rgb(0xFD40) not in colours


def test_tilde_names_and_fit_to_disk(image):
    s = ThemeSession(image)
    for key in ('1:/history/~a.jpg', '1:/history/~v.jpg', '1:/mode/dc_v.jpg'):
        assert s.replace(key, solid(image, key, (0, 160, 255))).ok
    # The non-ASCII original ('mode/组 1.jpg') is never packable; '~' names are.
    assert themepkg.packable('history/~a.jpg') and not themepkg.packable('mode/组 1.jpg')
    data = themepkg.from_session(s, 'Tilde')
    assert set(themepkg.parse(data).files) == {'history/~a.jpg', 'history/~v.jpg', 'mode/dc_v.jpg'}
    out, msg, _ = themepkg.simulate_install(bytes(image.data), data)
    assert msg.startswith('Installed'), msg
    assert FlashImage(out).resolve('themes/T01/history/~a.jpg') is not None
    # A re-encoded but visually identical image is dropped; real changes stay.
    orig = s.original_bytes('1:/units/v.jpg')
    same = jpegtools.encode(Image.open(io.BytesIO(orig)).convert('RGB'), 95, jpegtools.inspect(orig).subsampling)
    s.replace('1:/units/v.jpg', same)
    assert themepkg.drop_invisible(s) == 1 and '1:/units/v.jpg' not in s.replacements
    # Fit to a small limit by lowering the JPEG quality (one undo step).
    big = '1:/motif2/electric_frame.jpg'
    noisy = Image.effect_noise((318, 22), 80).convert('RGB')
    s.replace(big, jpegtools.encode(noisy, 98, '4:4:4'), force=True)
    size = len(themepkg.from_session(s, 'Fit'))
    quality, fitted = themepkg.fit_to_size(s, 'Fit', size * 3 // 4)
    assert fitted <= size * 3 // 4 and quality in themepkg.QUALITY_STEPS
    s.undo()
    assert len(themepkg.from_session(s, 'Fit')) == size

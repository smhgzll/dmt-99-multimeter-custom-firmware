"""Asset compatibility, image replacement, screen preview and project round trip."""
import io

from PIL import Image

from dmt99studio.core import jpegtools, screens
from dmt99studio.core.flashimage import FlashImage
from dmt99studio.core.project import ThemeSession


def test_originals_pass_the_device_decoder(image):
    for path in ('mode/dc_v.jpg', 'motif2/electric_frame.jpg', 'on.jpg', 'units/v.jpg'):
        data = image.read_file(image.resolve(path))
        result = jpegtools.validate(data)
        assert result.ok, (path, result.errors)
        assert result.decode.width == result.info.width


def test_convert_png_to_device_jpeg(image, tmp_path):
    original = image.read_file(image.resolve('mode/dc_v.jpg'))
    info = jpegtools.inspect(original)
    src = Image.new('RGBA', (400, 100), (255, 0, 0, 255))
    out = jpegtools.convert(src, original, mode='fit')
    result = jpegtools.validate(out, (info.width, info.height))
    assert result.ok, result.errors
    assert result.info.process == 'baseline' and result.info.components == 3
    shown = result.decode.image()
    r, g, b = shown.getpixel((info.width // 2, info.height // 2))
    assert r > 200 and g < 60 and b < 60


def test_incompatible_images_are_rejected(image):
    original = image.read_file(image.resolve('mode/dc_v.jpg'))
    info = jpegtools.inspect(original)
    buf = io.BytesIO()
    Image.new('RGB', (info.width, info.height), (0, 255, 0)).save(buf, 'JPEG', progressive=True)
    r = jpegtools.validate(buf.getvalue(), (info.width, info.height))
    assert not r.ok and any('progressive' in e for e in r.errors)
    buf = io.BytesIO()
    Image.new('L', (info.width, info.height), 128).save(buf, 'JPEG')
    r = jpegtools.validate(buf.getvalue(), (info.width, info.height))
    assert not r.ok and any('component' in e for e in r.errors)
    wrong = jpegtools.encode(Image.new('RGB', (10, 10)))
    r = jpegtools.validate(wrong, (info.width, info.height))
    assert not r.ok and any('differs from the original' in e for e in r.errors)
    r = jpegtools.validate(b'not a jpeg')
    assert not r.ok


def test_screen_preview_uses_working_image(image):
    session = ThemeSession(image)
    before = screens.render(session.working_file(), screens.BY_ID['dc_v'])
    assert not before.error and before.png
    assert '1:/mode/dc_v.jpg' in {screens.asset_key(p) for p in before.assets()}
    hit = next(op for op in before.visible_ops if op.op == 'jpeg' and op.path == 'mode/dc_v.jpg')
    assert before.hit(hit.x + 2, hit.y + 2).path == 'mode/dc_v.jpg'
    info = jpegtools.inspect(session.original_bytes('1:/mode/dc_v.jpg'))
    red = jpegtools.encode(Image.new('RGB', (info.width, info.height), (255, 0, 0)))
    assert session.replace('1:/mode/dc_v.jpg', red).ok
    after = screens.render(session.working_file(), screens.BY_ID['dc_v'])
    pixel = Image.open(io.BytesIO(after.png)).convert('RGB').getpixel((hit.x + 5, hit.y + 5))
    assert pixel[0] > 200 and pixel[1] < 60
    assert after.png != before.png


def test_every_catalog_screen_renders(image):
    session = ThemeSession(image)
    rendered = screens.render_many(session.working_file(), screens.CATALOG)
    assert all(r.png and not r.error for r in rendered), [r.screen.id for r in rendered if r.error]
    usage = screens.asset_usage(rendered)
    assert '1:/motif2/electric_frame.jpg' in usage and '1:/on.jpg' not in usage  # logo is covered later
    # Every visible asset of every screen exists in the image.
    for key in usage:
        assert image.resolve(key) is not None, key


def test_undo_redo_restore(image):
    s = ThemeSession(image)
    key = '1:/units/v.jpg'
    info = jpegtools.inspect(s.original_bytes(key))
    a = jpegtools.encode(Image.new('RGB', (info.width, info.height), (255, 0, 0)))
    b = jpegtools.encode(Image.new('RGB', (info.width, info.height), (0, 0, 255)))
    s.replace(key, a)
    s.replace(key, b)
    assert s.current_bytes(key) == b
    s.undo()
    assert s.current_bytes(key) == a
    s.undo()
    assert s.current_bytes(key) == s.original_bytes(key)
    assert s.working.changed_sectors() == []
    s.redo()
    assert s.current_bytes(key) == a
    s.restore(key)
    assert s.working.data == image.data


def test_project_round_trip(image, tmp_path):
    s = ThemeSession(image)
    s.name = 'Test theme'
    for key, color in (('1:/units/v.jpg', (255, 0, 0)), ('1:/mode/dc_v.jpg', (0, 255, 0))):
        info = jpegtools.inspect(s.original_bytes(key))
        assert s.replace(key, jpegtools.encode(Image.new('RGB', (info.width, info.height), color))).ok
    s.layout_overrides = {'about_label': {'x': 10}}
    s.save_project(tmp_path / 'proj')
    again = ThemeSession.load_project(tmp_path / 'proj', FlashImage(image.baseline, image.source))
    assert again.name == 'Test theme'
    assert again.replacements == s.replacements
    assert again.working.data == s.working.data
    assert again.layout_overrides == {'about_label': {'x': 10}}
    # Removing an asset and saving again deletes its file.
    again.restore('1:/units/v.jpg')
    again.save_project(tmp_path / 'proj')
    assert not (tmp_path / 'proj/assets/1/units/v.jpg').exists()
    exported = again.export_assets(tmp_path / 'export')
    assert [p.name for p in exported] == ['dc_v.jpg']

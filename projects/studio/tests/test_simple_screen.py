"""Image-free simple screen (firmware 0.6.0), switched on in the meter's display menu (simulator)."""
import struct
import subprocess

from PIL import Image

from dmt99studio import paths
from dmt99studio.core.settings import Settings, simulator_state


def run(tmp_path, script, settings=None):
    paths.ensure_tools()
    state = tmp_path / 'state.bin'
    state.write_bytes(simulator_state(settings or Settings.defaults(view=1)))
    (tmp_path / 'run.sim').write_text('\n'.join(script + ['quit']) + '\n')
    r = subprocess.run([str(paths.SIMULATOR), '--flash', str(paths.ORIGINAL_DUMP), '--state', str(state),
                        '--script', str(tmp_path / 'run.sim')], capture_output=True, text=True, timeout=60)
    assert r.returncode == 0, r.stderr
    return state


def test_enable_in_menu_and_persist(tmp_path, original_path):
    shot1, shot2 = tmp_path / 'a.png', tmp_path / 'b.png'
    # Menu > About > ENTER opens "Display"; OK toggles "Simple screen", then down to
    # "Digits" (row 2) and OK twice: Orange -> Yellow -> Lime. MENU leaves.
    state = run(tmp_path, ['button 6', 'button 2', 'button 3', 'button 5', 'button 1', 'button 1', 'button 5', 'button 5',
                           'button 6', 'ack 0x0c', 'reading 12.34 6', f'snapshot {shot1}', 'poweroff', 'poweron',
                           'ack 0x0c', 'reading 56.78 6', f'snapshot {shot2}'])
    for shot in (shot1, shot2):
        img = Image.open(shot).convert('RGB')
        colours = {img.getpixel((x, y)) for x in range(0, 320, 2) for y in range(40, 130, 2)}
        assert (132, 255, 0) in colours, colours          # lime digits (0x87e0)
        assert (255, 170, 0) not in colours                 # no orange classic digits / artwork
    saved = Settings.from_bytes(state.read_bytes()[8:264])
    assert saved.simple_screen() == {'on': True, 'digit_size': 'Large', 'background': 'Black', 'digits': 'Lime',
                                     'text': 'White', 'accent': 'Blue'}


def test_off_by_default_is_unchanged(tmp_path, original_path):
    shot = tmp_path / 'c.png'
    run(tmp_path, ['ack 0x0c', 'reading 12.34 6', f'snapshot {shot}'])
    img = Image.open(shot).convert('RGB')
    assert img.getpixel((150, 40))[0] > 200            # the original orange "V" artwork is still drawn

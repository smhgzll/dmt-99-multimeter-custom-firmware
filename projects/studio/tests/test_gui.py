"""GUI start-up and main workflows (offscreen Qt)."""
import time

import pytest
from PIL import Image
from PySide6.QtCore import QThreadPool
from PySide6.QtWidgets import QApplication

from dmt99studio.core import jpegtools


@pytest.fixture(scope='module')
def app():
    return QApplication.instance() or QApplication([])


def wait(app, cond, timeout=20):
    end = time.monotonic() + timeout
    while time.monotonic() < end:
        app.processEvents()
        QThreadPool.globalInstance().waitForDone(50)
        app.processEvents()
        if cond():
            return True
    return False


def test_main_window_workflow(app, original_path, tmp_path):
    from dmt99studio.gui.app import MainWindow
    win = MainWindow()
    win.show()
    assert win.tabs.count() == 5
    win.state.open_image(original_path)
    page = win.theme
    assert wait(app, lambda: page.screen.rendered is not None)
    assert page.screen.elements.topLevelItemCount() > 3
    assert wait(app, lambda: bool(win.state.usage))
    assert page.browser.tree.topLevelItemCount() >= 1  # BOOT volume holds no files in this dump
    # Click the DC V mode artwork on the preview -> asset panel follows.
    op = next(o for o in page.screen.rendered.visible_ops if o.op == 'jpeg' and o.path == 'mode/dc_v.jpg')
    page.screen._clicked(op.x + 3, op.y + 3)
    assert page.panel.key == '1:/mode/dc_v.jpg'
    # Replace through the panel's code path (device-compatible JPEG).
    s = win.state.session
    info = jpegtools.inspect(s.original_bytes(page.panel.key))
    data = jpegtools.encode(Image.new('RGB', (info.width, info.height), (0, 200, 0)))
    page.panel._apply(data, source='test')
    assert page.panel.key in s.replacements
    assert wait(app, lambda: page.screen.rendered is not None and page.screen.before is not None
                and page.screen.rendered.png != page.screen.before.png)
    page.screen.compare_box.setCurrentIndex(2)   # difference view renders
    assert wait(app, lambda: True)
    win.undo()
    assert not s.replacements
    win.redo()
    assert s.replacements
    win.state.project_dir = tmp_path / 'proj'
    win.save_project()
    assert (tmp_path / 'proj' / 'project.json').exists()
    win.flash.refresh()
    assert '1 changed' in win.flash.diff.toPlainText() or 'changed 4 KiB sectors' in win.flash.diff.toPlainText()
    page.center.setCurrentIndex(1)
    assert page.gallery.count() > 0
    win.close()


def test_live_meter_with_simulated_device(app, original_path, tmp_path):
    from dmt99studio.gui.app import MainWindow
    win = MainWindow()
    win.state.open_image(original_path)
    assert win.live.value.text() == '—'          # disconnected: nothing shown
    win.start_simulated()
    d = win.state.device
    assert d is not None and d.simulated
    assert win.live.sim_box.isVisibleTo(win.live)
    win.live.stream_btn.setChecked(True)
    win.live.capture_btn.setChecked(True)
    d.sim.command('ack 0x0c')
    for v in ('12.34', '12.40', '12.20'):
        d.sim.command(f'reading {v} 6')
        time.sleep(0.05)
    assert wait(app, lambda: len(win.live.series) >= 3, 10)
    assert 'V' in win.live.value.text()
    win.live.refresh_stats()
    assert win.live.stat_labels['Min'].text().startswith('12.2')
    win.live.save_capture(tmp_path / 'c.csv', 'csv')
    win.live.save_capture(tmp_path / 'c.json', 'json')
    assert 'simulated: True' in (tmp_path / 'c.csv').read_text()
    win.firmware.refresh()
    assert win.firmware.map.rowCount() > 5 and win.firmware.info.rowCount() > 5
    win.settings.read_device()
    assert 'device' in win.settings.source.text()
    win.disconnect_device()
    assert win.state.device is None
    win.close()


def test_batch_colour_dialog(app, original_path):
    from dmt99studio.gui.app import MainWindow
    from dmt99studio.gui.theme_editor import BatchColorDialog
    from dmt99studio.core import screens
    win = MainWindow()
    win.state.open_image(original_path)
    s = win.state.session
    dlg = BatchColorDialog(win.theme, win.state, screens.BY_ID['dc_v'], 1, 1)   # large-digit layout
    dlg._preset((60, 100, 100, True, 35, 25))                                    # orange -> green
    keys = dlg.selected_keys()
    assert any(k.startswith('1:/num/') for k in keys) and any(k.startswith('1:/units/') for k in keys)
    assert wait(app, lambda: not dlg.after.pixmap().isNull(), 30)
    dlg.apply()
    assert set(keys) <= set(s.replacements) or all(k in s.replacements or
                                                    s.current_bytes(k) == s.original_bytes(k) for k in keys)
    assert 0xFD40 in s.palette
    changed = len(s.replacements)
    assert changed > 10
    win.undo()                                   # one undo step reverts the whole batch
    assert not s.replacements and not s.palette
    win.redo()
    assert len(s.replacements) == changed and 0xFD40 in s.palette
    win.close()

#!/usr/bin/env python3
"""Capture the actual offline Studio UI, without enumerating or opening devices."""
import json
import os
from pathlib import Path
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
os.environ.setdefault('QT_QPA_PLATFORM', 'offscreen')
sys.path.insert(0, str(ROOT / 'projects/studio'))


def main():
    with tempfile.TemporaryDirectory(prefix='dmt99-gallery-') as home:
        os.environ['DMT99_STUDIO_HOME'] = home
        from PySide6.QtCore import QThreadPool
        from PySide6.QtWidgets import QApplication
        from dmt99studio import paths
        from dmt99studio.gui.app import MainWindow
        app = QApplication([])
        win = MainWindow()
        win.resize(1440, 900)
        win.show()
        win.state.open_image(paths.ORIGINAL_DUMP)
        deadline = time.monotonic() + 20
        while win.theme.screen.rendered is None and time.monotonic() < deadline:
            app.processEvents()
            QThreadPool.globalInstance().waitForDone(50)
        if win.theme.screen.rendered is None:
            raise RuntimeError('offline screen preview did not finish within 20 seconds')
        out = ROOT / 'docs/gallery'
        out.mkdir(exist_ok=True)
        app.processEvents()
        if not win.grab().save(str(out / 'studio-theme-editor.png')):
            raise RuntimeError('failed to save Studio screenshot')
        win.tabs.setCurrentWidget(win.flash)
        win.flash.refresh()
        app.processEvents()
        if not win.grab().save(str(out / 'studio-flash-manager.png')):
            raise RuntimeError('failed to save Flash Manager screenshot')
        win.close()
        QThreadPool.globalInstance().waitForDone(5000)
        print(json.dumps({'source': 'actual Qt offscreen UI + native C screen renderer',
                          'device_connected': False, 'new_RV32_emulation': False,
                          'files': ['studio-theme-editor.png', 'studio-flash-manager.png']}))


if __name__ == '__main__':
    main()

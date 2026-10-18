"""DMT99 Studio main window."""
from __future__ import annotations

import sys
from pathlib import Path

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction, QKeySequence
from PySide6.QtWidgets import (QApplication, QComboBox, QDialog, QDialogButtonBox, QFileDialog, QLabel,
                               QMainWindow, QMessageBox, QPushButton, QTabWidget, QVBoxLayout)

from .. import __version__, paths
from ..core.device import DeviceClient, list_ports
from ..core.project import ThemeSession
from .common import error
from .device_settings import DeviceSettingsPage
from .firmware_backup import FirmwareBackupPage
from .flash_manager import FlashManagerPage
from .live_meter import LiveMeterPage
from .state import AppState
from .theme_editor import ThemeEditorPage


class ConnectDialog(QDialog):
    def __init__(self, parent):
        super().__init__(parent)
        self.setWindowTitle('Connect device')
        lay = QVBoxLayout(self)
        lay.addWidget(QLabel('Serial port of the POTAT-99 management link:'))
        self.ports = QComboBox()
        self.ports.setEditable(True)
        lay.addWidget(self.ports)
        refresh = QPushButton('Refresh')
        refresh.clicked.connect(self.refresh)
        lay.addWidget(refresh)
        note = QLabel('Firmware 0.3.x on the meter does not provide this link yet; use the simulated device to try '
                      'the online functions (Device ▸ Start simulated device).')
        note.setWordWrap(True)
        note.setStyleSheet('color:gray')
        lay.addWidget(note)
        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        lay.addWidget(buttons)
        self.refresh()

    def refresh(self) -> None:
        self.ports.clear()
        for p in list_ports():
            label = f'{p["device"]}  {p["description"] or ""}'
            self.ports.addItem(label, p['device'])

    def port(self) -> str:
        return self.ports.currentData() or self.ports.currentText().split()[0]


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle(f'DMT99 Studio {__version__}')
        self.resize(1500, 950)
        self.state = AppState()
        self.tabs = QTabWidget()
        self.theme = ThemeEditorPage(self.state)
        self.flash = FlashManagerPage(self.state)
        self.live = LiveMeterPage(self.state)
        self.firmware = FirmwareBackupPage(self.state)
        self.settings = DeviceSettingsPage(self.state)
        for page, title in ((self.theme, 'Theme Editor'), (self.flash, 'Flash Manager'), (self.live, 'Live Multimeter'),
                            (self.firmware, 'Firmware && Backup'), (self.settings, 'Device Settings')):
            self.tabs.addTab(page, title)
        self.setCentralWidget(self.tabs)
        self.device_label = QLabel('No device')
        self.statusBar().addPermanentWidget(self.device_label)
        self.state.status.connect(lambda t: self.statusBar().showMessage(t, 8000))
        self.state.device_changed.connect(self._device_label)
        self.state.assets_changed.connect(lambda _k: self._update_actions())
        self.state.image_opened.connect(self._update_actions)
        self._menus()
        self._update_actions()
        if paths.ORIGINAL_DUMP.exists():
            self.state.status.emit('File ▸ Open original dump loads the acquired Winbond image (read-only).')

    # -------------------------------------------------------------- menus
    def _action(self, menu, text, slot, shortcut=None):
        a = QAction(text, self)
        a.triggered.connect(slot)
        if shortcut:
            a.setShortcut(QKeySequence(shortcut))
        menu.addAction(a)
        return a

    def _menus(self) -> None:
        m = self.menuBar().addMenu('&File')
        self._action(m, 'Open original dump', self.open_original, 'Ctrl+Shift+O')
        self._action(m, 'Open flash image…', self.open_image, QKeySequence.StandardKey.Open)
        self._action(m, 'Open theme project…', self.open_project)
        m.addSeparator()
        self.save_act = self._action(m, 'Save theme project', self.save_project, QKeySequence.StandardKey.Save)
        self.save_as_act = self._action(m, 'Save theme project as…', lambda: self.save_project(True), 'Ctrl+Shift+S')
        self.export_act = self._action(m, 'Export working flash image…', self.flash.save_working)
        self.export_assets_act = self._action(m, 'Export edited assets…', self.export_assets)
        self.export_pkg_act = self._action(m, 'Export THEME.PKG for the meter…', self.export_theme_package, 'Ctrl+E')
        m.addSeparator()
        self._action(m, 'Quit', self.close, QKeySequence.StandardKey.Quit)
        e = self.menuBar().addMenu('&Edit')
        self.undo_act = self._action(e, 'Undo', self.undo, QKeySequence.StandardKey.Undo)
        self.redo_act = self._action(e, 'Redo', self.redo, QKeySequence.StandardKey.Redo)
        e.addSeparator()
        self.batch_act = self._action(e, 'Batch import from folder…', self.theme.batch_import)
        self.color_act = self._action(e, 'Batch colour adjust…', self.theme.batch_color, 'Ctrl+B')
        self.restore_all_act = self._action(e, 'Restore all original assets', self.restore_all)
        d = self.menuBar().addMenu('&Device')
        self._action(d, 'Connect…', self.connect_device)
        self._action(d, 'Start simulated device', self.start_simulated)
        self._action(d, 'Disconnect', self.disconnect_device)
        h = self.menuBar().addMenu('&Help')
        self._action(h, 'About DMT99 Studio', self.about)

    def _update_actions(self) -> None:
        s = self.state.session
        for a in (self.save_act, self.save_as_act, self.export_act, self.export_assets_act, self.batch_act,
                  self.restore_all_act, self.export_pkg_act, self.color_act):
            a.setEnabled(s is not None)
        self.undo_act.setEnabled(bool(s and s.can_undo()))
        self.redo_act.setEnabled(bool(s and s.can_redo()))
        title = f'DMT99 Studio {__version__}'
        if s:
            title += f' — {s.name} [{s.base.label}]' + (f' ({len(s.replacements)} edited)' if s.replacements else '')
        self.setWindowTitle(title)

    def _device_label(self) -> None:
        d = self.state.device
        self.device_label.setText(d.describe() if d else 'No device')

    # -------------------------------------------------------------- files
    def open_original(self) -> None:
        if not paths.ORIGINAL_DUMP.exists():
            error(self, 'Open', f'{paths.ORIGINAL_DUMP} not found')
            return
        self.state.open_image(paths.ORIGINAL_DUMP)

    def open_image(self) -> None:
        path, _ = QFileDialog.getOpenFileName(self, 'Open 8 MiB flash image', str(paths.FIRMWARE_PROJECT),
                                              'Flash images (*.bin);;All files (*)')
        if path:
            try:
                self.state.open_image(path)
            except Exception as ex:  # noqa: BLE001
                error(self, 'Open flash image', str(ex))

    def open_project(self) -> None:
        folder = QFileDialog.getExistingDirectory(self, 'Theme project folder')
        if not folder:
            return
        try:
            base = self.state.session.base if self.state.session else None
            session = ThemeSession.load_project(folder, base)
        except ValueError as ex:
            if 'differs' in str(ex) and QMessageBox.question(
                    self, 'Different base image', f'{ex}.\n\nLoad the edits on top of the open image anyway?') == \
                    QMessageBox.StandardButton.Yes:
                session = ThemeSession.load_project(folder, self.state.session.base, allow_different_base=True)
            else:
                error(self, 'Open project', str(ex))
                return
        except Exception as ex:  # noqa: BLE001
            error(self, 'Open project', str(ex))
            return
        self.state.set_session(session, Path(folder))

    def save_project(self, ask: bool = False) -> None:
        s = self.state.session
        if not s:
            return
        folder = self.state.project_dir
        if ask or folder is None:
            chosen = QFileDialog.getExistingDirectory(self, 'Folder for the theme project')
            if not chosen:
                return
            folder = Path(chosen)
        s.save_project(folder)
        self.state.project_dir = folder
        self.state.status.emit(f'Theme project saved to {folder}')

    def export_assets(self) -> None:
        s = self.state.session
        folder = QFileDialog.getExistingDirectory(self, 'Export edited assets to')
        if s and folder:
            files = s.export_assets(folder)
            self.state.status.emit(f'{len(files)} asset(s) exported to {folder}')

    def export_theme_package(self) -> None:
        from PySide6.QtWidgets import QInputDialog
        from ..core import themepkg
        s = self.state.session
        if not s or (not s.replacements and not s.palette):
            QMessageBox.information(self, 'THEME.PKG', 'Change at least one image or colour first.')
            return
        name, ok = QInputDialog.getText(self, 'Theme name', 'Name shown in the meter\'s theme list (max 24 characters):',
                                        text=s.name if s.name != 'Untitled theme' else '')
        if not ok:
            return
        name = name.strip()[:24] or 'Theme'
        free = themepkg.boot_free_bytes(s.base)
        limit = free - 8192                       # directory and cluster slack on the bootloader disk
        skipped: list[str] = []
        try:
            data = themepkg.from_session(s, name, skipped)
        except themepkg.PackageError as ex:
            error(self, 'THEME.PKG', str(ex))
            return
        if len(data) > limit:
            same = themepkg.invisible_replacements(s)
            if same and QMessageBox.question(
                    self, 'THEME.PKG too large',
                    f'The package is {len(data) // 1024} KiB; the bootloader disk has about {limit // 1024} KiB '
                    f'free.\n\n{len(same)} of the {len(s.replacements)} changed images look almost identical to the '
                    'original (they were only re-encoded). Leave them out of the theme? (One undo step.)') == \
                    QMessageBox.StandardButton.Yes:
                themepkg.drop_invisible(s, same)
                self.state.assets_changed.emit('')
                data = themepkg.from_session(s, name, [])
                self.state.status.emit(f'{len(same)} unchanged image(s) left out of the theme')
        if len(data) > limit:
            answer = QMessageBox.question(
                self, 'THEME.PKG too large',
                f'The package is {len(data) // 1024} KiB, but the meter\'s bootloader disk has about '
                f'{limit // 1024} KiB free.\n\nRe-encode the changed images at a lower JPEG quality until it fits? '
                '(One undo step; the original artwork is not affected.)\n\nNo: cancel, so you can change fewer '
                'images instead.')
            if answer != QMessageBox.StandardButton.Yes:
                return
            from PySide6.QtWidgets import QProgressDialog
            dlg = QProgressDialog('Finding a JPEG quality that fits…', None, 0, len(themepkg.QUALITY_STEPS), self)
            dlg.setWindowModality(Qt.WindowModality.WindowModal)
            dlg.show()

            def step(i, n, q, size):
                dlg.setValue(i)
                dlg.setLabelText(f'Quality {q}: {size // 1024} KiB')
                QApplication.processEvents()
            try:
                quality, size = themepkg.fit_to_size(s, name, limit, step)
            except themepkg.PackageError as ex:
                dlg.close()
                error(self, 'THEME.PKG', str(ex))
                return
            dlg.close()
            self.state.assets_changed.emit('')
            data = themepkg.from_session(s, name, [])
            self.state.status.emit(f'Images re-encoded at quality {quality}: package {size // 1024} KiB')
        path, _ = QFileDialog.getSaveFileName(self, 'Save theme package', themepkg.PACKAGE_NAME,
                                              'Theme package (THEME.PKG)')
        if not path:
            return
        Path(path).write_bytes(data)
        s.name = name
        warn = ''
        if skipped:
            warn += '\n\nLeft out (names the firmware cannot open): ' + ', '.join(skipped)
        QMessageBox.information(
            self, 'THEME.PKG saved',
            f'{Path(path).name}: "{name}", {len(s.replacements)} image(s), {len(s.palette)} firmware colour(s), '
            f'{len(data) // 1024 + 1} KiB.\n\n'
            + ('Firmware colours (classic digits, menu text) need firmware 0.5.0 or later.\n\n' if s.palette else '') +
            'Install on the meter:\n'
            '1. Switch the meter off, hold POWER+MENU and connect USB: the bootloader disk appears.\n'
            '2. Copy THEME.PKG to that disk (keep the name THEME.PKG), eject, disconnect.\n'
            '3. Switch the meter on normally: it installs the theme, activates it and deletes the package.\n\n'
            'Themes are listed under Menu ▸ About ▸ ENTER (OK: use, ENTER: delete).' + warn)

    def undo(self) -> None:
        s = self.state.session
        if s and s.can_undo():
            self.state.assets_changed.emit(s.undo() or '')

    def redo(self) -> None:
        s = self.state.session
        if s and s.can_redo():
            self.state.assets_changed.emit(s.redo() or '')

    def restore_all(self) -> None:
        s = self.state.session
        if s and (s.replacements or s.palette):
            s.restore_all()
            self.state.assets_changed.emit('')

    # ------------------------------------------------------------- device
    def connect_device(self) -> None:
        dlg = ConnectDialog(self)
        if dlg.exec() != QDialog.DialogCode.Accepted:
            return
        self.disconnect_device()
        try:
            client = DeviceClient.open_serial(dlg.port())
            client.device_info()
        except Exception as ex:  # noqa: BLE001
            try:
                client.close()
            except Exception:  # noqa: BLE001
                pass
            error(self, 'Connect', f'No answer from a POTAT-99 management link on {dlg.port()}: {ex}')
            return
        self.state.set_device(client)

    def start_simulated(self) -> None:
        self.disconnect_device()
        s = self.state.session
        flash = s.working_file() if s else paths.ORIGINAL_DUMP
        releases = sorted(paths.FINAL_RELEASES.glob('v*/*.bin'))
        try:
            client = DeviceClient.open_simulated(flash, releases[-1] if releases else None)
            client.device_info()
        except Exception as ex:  # noqa: BLE001
            error(self, 'Simulated device', str(ex))
            return
        self.state.set_device(client)
        self.state.status.emit('Simulated device started (firmware UI code + copy of the working image)')

    def disconnect_device(self) -> None:
        if self.state.device:
            try:
                self.state.device.close()
            finally:
                self.state.set_device(None)

    def about(self) -> None:
        QMessageBox.about(self, 'DMT99 Studio',
                          f'DMT99 Studio {__version__}\nTheme editor, flash manager and companion tool for the '
                          'FNIRSI DMT99 running POTAT-99 firmware.\n\nScreens are rendered by the firmware\'s own UI '
                          f'code from {paths.FIRMWARE_PROJECT}.')

    def closeEvent(self, event) -> None:  # noqa: N802
        self.disconnect_device()
        super().closeEvent(event)


def main(argv: list[str] | None = None) -> int:
    app = QApplication(argv if argv is not None else sys.argv)
    app.setApplicationName('DMT99 Studio')
    win = MainWindow()
    args = (argv or sys.argv)[1:]
    if args and Path(args[0]).exists():
        win.state.open_image(args[0])
    win.show()
    return app.exec()

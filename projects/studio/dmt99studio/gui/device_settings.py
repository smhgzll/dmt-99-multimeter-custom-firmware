"""Device Settings: decode/edit the 256-byte settings image (offline or on the device)."""
from __future__ import annotations

import json
from pathlib import Path

from PySide6.QtWidgets import (QCheckBox, QComboBox, QFileDialog, QFormLayout, QGroupBox, QHBoxLayout, QLabel,
                               QMessageBox, QPlainTextEdit, QPushButton, QVBoxLayout, QWidget)

from ..core.settings import AUTO_OFF, BACKLIGHT, LANGUAGES, LAYOUTS, VIEWS, Settings
from .state import AppState

# Fields the device accepts over SET_SETTINGS (mgmt_protocol.h).
FIELDS = {'language': 1, 'backlight': 2, 'fahrenheit': 3, 'auto_off': 4, 'view': 5, 'layout': 6}


class DeviceSettingsPage(QWidget):
    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        self.settings = Settings.defaults()
        self.loaded_from = 'defaults'
        lay = QVBoxLayout(self)
        bar = QHBoxLayout()
        self.open_btn = QPushButton('Open settings image (256 bytes)…')
        self.save_btn = QPushButton('Save settings image…')
        self.read_btn = QPushButton('Read from device')
        self.apply_btn = QPushButton('Apply changes to device')
        for b in (self.open_btn, self.save_btn, self.read_btn, self.apply_btn):
            bar.addWidget(b)
        bar.addStretch(1)
        lay.addLayout(bar)
        self.source = QLabel()
        lay.addWidget(self.source)
        box = QGroupBox('Settings')
        form = QFormLayout(box)
        self.language = QComboBox()
        self.language.addItems(LANGUAGES[:2])
        self.backlight = QComboBox()
        self.backlight.addItems(list(BACKLIGHT.values()))
        self.fahrenheit = QCheckBox('Show temperature in °F')
        self.auto_off = QComboBox()
        self.auto_off.addItems(list(AUTO_OFF.values()))
        self.view = QComboBox()
        self.view.addItems(list(VIEWS.values()))
        self.layout_box = QComboBox()
        self.layout_box.addItems(list(LAYOUTS.values()))
        form.addRow('Language', self.language)
        form.addRow('Backlight', self.backlight)
        form.addRow('', self.fahrenheit)
        form.addRow('Auto power-off', self.auto_off)
        form.addRow('View (applied at next start)', self.view)
        form.addRow('Digit layout (applied at next start)', self.layout_box)
        lay.addWidget(box)
        self.details = QPlainTextEdit(readOnly=True)
        lay.addWidget(self.details, 1)
        note = QLabel('Only the documented fields above can be changed on the device. The firmware keeps them in RAM '
                      'and stores them with its normal power-off save, exactly like a menu change. Thresholds, history '
                      'and the last-mode record are shown read-only.')
        note.setWordWrap(True)
        note.setStyleSheet('color:gray')
        lay.addWidget(note)
        self.open_btn.clicked.connect(self.open_file)
        self.save_btn.clicked.connect(self.save_file)
        self.read_btn.clicked.connect(self.read_device)
        self.apply_btn.clicked.connect(self.apply_device)
        state.device_changed.connect(self._device)
        self._device()
        self.show_settings()

    def _device(self) -> None:
        on = self.state.device is not None
        self.read_btn.setEnabled(on)
        self.apply_btn.setEnabled(on)

    def show_settings(self) -> None:
        s = self.settings
        self.language.setCurrentIndex(min(s.language, 1))
        self.backlight.setCurrentIndex(s.backlight)
        self.fahrenheit.setChecked(s.fahrenheit)
        self.auto_off.setCurrentIndex(s.auto_off)
        self.view.setCurrentIndex(max(0, s.view - 1))
        self.layout_box.setCurrentIndex(min(s.layout, 1))
        self.source.setText(f'Source: {self.loaded_from}')
        self.details.setPlainText(json.dumps(s.describe(), indent=2, ensure_ascii=False))

    def edited(self) -> Settings:
        s = Settings(list(self.settings.halfwords))
        s.language = self.language.currentIndex()
        s.backlight = self.backlight.currentIndex()
        s.fahrenheit = self.fahrenheit.isChecked()
        s.auto_off = self.auto_off.currentIndex()
        s.view = self.view.currentIndex() + 1
        s.layout = self.layout_box.currentIndex()
        return s

    def open_file(self) -> None:
        path, _ = QFileDialog.getOpenFileName(self, 'Settings image', '', 'Binary (*.bin);;All files (*)')
        if not path:
            return
        data = Path(path).read_bytes()
        if len(data) != 256:
            QMessageBox.warning(self, 'Settings', f'Expected 256 bytes, the file has {len(data)}.')
            return
        self.settings = Settings.from_bytes(data)
        self.loaded_from = path
        self.show_settings()

    def save_file(self) -> None:
        path, _ = QFileDialog.getSaveFileName(self, 'Save settings image', 'dmt99_settings.bin')
        if path:
            Path(path).write_bytes(self.edited().to_bytes())

    def read_device(self) -> None:
        d = self.state.device
        try:
            self.settings = Settings.from_bytes(d.get_settings())
            self.loaded_from = f'device: {d.describe()}'
            self.show_settings()
        except Exception as ex:  # noqa: BLE001
            QMessageBox.warning(self, 'Settings', str(ex))

    def apply_device(self) -> None:
        d = self.state.device
        new, old = self.edited(), self.settings
        changes = [(name, getattr(new, name)) for name in FIELDS if getattr(new, name) != getattr(old, name)]
        if not changes:
            QMessageBox.information(self, 'Settings', 'Nothing changed.')
            return
        try:
            for name, value in changes:
                d.set_setting(FIELDS[name], int(value))
        except Exception as ex:  # noqa: BLE001
            QMessageBox.warning(self, 'Settings', str(ex))
        self.read_device()

"""Application state shared by all Studio pages."""
from __future__ import annotations

from pathlib import Path

from PySide6.QtCore import QObject, Signal

from ..core.flashimage import FlashImage
from ..core.project import ThemeSession


class AppState(QObject):
    image_opened = Signal()          # new base image / session
    assets_changed = Signal(str)     # asset key ('' = several)
    device_changed = Signal()        # connected / disconnected
    status = Signal(str)             # status bar text

    def __init__(self):
        super().__init__()
        self.session: ThemeSession | None = None
        self.project_dir: Path | None = None
        self.device = None           # core.device.DeviceClient when connected
        self.usage: dict[str, list[str]] = {}

    def open_image(self, path: str | Path) -> None:
        self.set_session(ThemeSession(FlashImage.load(path)))

    def set_session(self, session: ThemeSession, project_dir: Path | None = None) -> None:
        self.session = session
        self.project_dir = project_dir
        self.usage = {}
        self.image_opened.emit()
        base = session.base
        note = ' (original acquired dump, kept read-only)' if base.is_original_dump else ''
        self.status.emit(f'Opened {base.label}{note}')

    def set_device(self, device) -> None:
        self.device = device
        self.device_changed.emit()

"""Small Qt helpers shared by the Studio pages."""
from __future__ import annotations

import io
import traceback
from typing import Any, Callable

from PIL import Image
from PySide6.QtCore import QObject, QRunnable, QThreadPool, Qt, Signal
from PySide6.QtGui import QImage, QPixmap
from PySide6.QtWidgets import QMessageBox, QWidget


def pil_to_qimage(im: Image.Image) -> QImage:
    rgb = im.convert('RGBA')
    data = rgb.tobytes('raw', 'RGBA')
    q = QImage(data, rgb.width, rgb.height, rgb.width * 4, QImage.Format.Format_RGBA8888)
    return q.copy()


def bytes_to_pixmap(data: bytes) -> QPixmap:
    if not data:
        return QPixmap()
    try:
        return QPixmap.fromImage(pil_to_qimage(Image.open(io.BytesIO(data))))
    except Exception:  # noqa: BLE001 - unreadable data shows as empty
        return QPixmap()


def scaled(pixmap: QPixmap, w: int, h: int) -> QPixmap:
    if pixmap.isNull():
        return pixmap
    return pixmap.scaled(w, h, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)


class _Signals(QObject):
    done = Signal(object)
    failed = Signal(str)
    progress = Signal(int, int, str)


class Task(QRunnable):
    """Run fn(progress) in the thread pool; deliver result/failure on the GUI thread."""

    def __init__(self, fn: Callable[..., Any], *args, **kwargs):
        super().__init__()
        self.fn, self.args, self.kwargs = fn, args, kwargs
        self.signals = _Signals()
        self.cancelled = False

    def run(self) -> None:
        try:
            result = self.fn(*self.args, **self.kwargs)
        except Exception as ex:  # noqa: BLE001 - reported to the user
            self.signals.failed.emit(f'{ex}\n\n{traceback.format_exc(limit=4)}')
        else:
            self.signals.done.emit(result)


_running: set[Task] = set()   # keeps tasks (and their signal objects) alive until delivery


def run_task(fn: Callable[..., Any], *args, on_done=None, on_failed=None, **kwargs) -> Task:
    task = Task(fn, *args, **kwargs)
    task.setAutoDelete(False)
    _running.add(task)
    task.signals.done.connect(lambda _r: _running.discard(task))
    task.signals.failed.connect(lambda _m: _running.discard(task))
    if on_done:
        task.signals.done.connect(on_done)
    if on_failed:
        task.signals.failed.connect(on_failed)
    QThreadPool.globalInstance().start(task)
    return task


def error(parent: QWidget | None, title: str, text: str) -> None:
    box = QMessageBox(QMessageBox.Icon.Critical, title, text.split('\n\n')[0], QMessageBox.StandardButton.Ok, parent)
    if '\n\n' in text:
        box.setDetailedText(text)
    box.exec()


def confirm(parent: QWidget | None, title: str, text: str, danger: bool = False) -> bool:
    icon = QMessageBox.Icon.Warning if danger else QMessageBox.Icon.Question
    box = QMessageBox(icon, title, text, QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.Cancel, parent)
    box.setDefaultButton(QMessageBox.StandardButton.Cancel)
    return box.exec() == QMessageBox.StandardButton.Yes


def human_size(n: int) -> str:
    for unit in ('B', 'KiB', 'MiB'):
        if n < 1024 or unit == 'MiB':
            return f'{n:.0f} {unit}' if unit == 'B' else f'{n:.1f} {unit}'
        n /= 1024
    return str(n)

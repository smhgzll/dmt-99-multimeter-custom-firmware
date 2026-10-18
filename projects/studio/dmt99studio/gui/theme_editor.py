"""Theme Editor: asset browser, screen editor and asset replacement."""
from __future__ import annotations

import io
from pathlib import Path

from PIL import Image, ImageChops
from PySide6.QtCore import QPoint, QRect, QSize, Qt, Signal
from PySide6.QtGui import QAction, QColor, QFont, QIcon, QImage, QPainter, QPen, QPixmap
from PySide6.QtWidgets import (QCheckBox, QColorDialog, QComboBox, QDialog, QDialogButtonBox, QFileDialog,
                               QFontComboBox, QFormLayout, QGroupBox, QHBoxLayout, QLabel, QLineEdit,
                               QListWidget, QListWidgetItem, QMessageBox, QPlainTextEdit, QPushButton,
                               QSpinBox, QSplitter, QTabWidget, QTreeWidget, QTreeWidgetItem, QVBoxLayout,
                               QWidget)

from ..core import jpegtools, screens, themepkg
from ..core.screens import asset_key
from .common import bytes_to_pixmap, error, human_size, pil_to_qimage, run_task, scaled
from .state import AppState

SCALE = 2
KEY_ROLE = Qt.ItemDataRole.UserRole


# --------------------------------------------------------------- browser
class AssetBrowser(QWidget):
    asset_selected = Signal(str)

    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        lay = QVBoxLayout(self)
        lay.setContentsMargins(0, 0, 0, 0)
        self.search = QLineEdit(placeholderText='Search assets (name or folder)…')
        self.batch_btn = QPushButton('Batch colour… (recolour many images at once)')
        lay.addWidget(self.batch_btn)
        self.only_modified = QCheckBox('Modified only')
        self.only_used = QCheckBox('Shown on catalog screens')
        row = QHBoxLayout()
        row.addWidget(self.only_modified)
        row.addWidget(self.only_used)
        self.tree = QTreeWidget()
        self.tree.setHeaderLabels(['Asset', 'Size', 'W×H', 'Screens'])
        self.tree.setColumnWidth(0, 210)
        self.tree.setColumnWidth(1, 70)
        self.tree.setColumnWidth(2, 70)
        self.summary = QLabel()
        self.summary.setWordWrap(True)
        lay.addWidget(self.search)
        lay.addLayout(row)
        lay.addWidget(self.tree, 1)
        lay.addWidget(self.summary)
        self.search.textChanged.connect(self.refresh)
        self.only_modified.toggled.connect(self.refresh)
        self.only_used.toggled.connect(self.refresh)
        self.tree.currentItemChanged.connect(self._current)
        self.rows = []

    def refresh(self) -> None:
        s = self.state.session
        current = self.current_key()
        self.tree.clear()
        if not s:
            self.summary.setText('Open a flash image (File ▸ Open original dump).')
            return
        self.rows = s.assets(self.state.usage)
        text = self.search.text().strip().lower()
        folders: dict[tuple[int, str], QTreeWidgetItem] = {}
        roots = {0: QTreeWidgetItem(['0: BOOT volume (read-only)']), 1: QTreeWidgetItem(['1: assets volume'])}
        shown = 0
        for row in self.rows:
            if text and text not in row.path.lower():
                continue
            if self.only_modified.isChecked() and not row.modified:
                continue
            if self.only_used.isChecked() and not row.used_by:
                continue
            parent = roots[row.volume]
            if row.folder:
                fk = (row.volume, row.folder.lower())
                if fk not in folders:
                    folders[fk] = QTreeWidgetItem(parent, [row.folder + '/'])
                parent = folders[fk]
            item = QTreeWidgetItem(parent, [row.name + (' ●' if row.modified else ''), human_size(row.size),
                                            f'{row.width}×{row.height}' if row.is_jpeg else '',
                                            ', '.join(row.used_by[:3]) + ('…' if len(row.used_by) > 3 else '')])
            item.setData(0, KEY_ROLE, row.key)
            if row.modified:
                f = item.font(0)
                f.setBold(True)
                item.setFont(0, f)
            if row.volume == 0:
                item.setForeground(0, QColor('gray'))
            shown += 1
            if row.key == current:
                self.tree.setCurrentItem(item)
        for r in roots.values():
            if r.childCount():
                self.tree.addTopLevelItem(r)
                r.setExpanded(True)
        if text or self.only_modified.isChecked() or self.only_used.isChecked():
            self.tree.expandAll()
        usage = s.working.assets.usage()
        mods = sum(1 for r in self.rows if r.modified)
        self.summary.setText(f'{shown} of {len(self.rows)} files · {mods} modified · assets volume '
                             f'{human_size(usage["used_bytes"])} used, {human_size(usage["free_bytes"])} free')

    def current_key(self) -> str | None:
        item = self.tree.currentItem()
        return item.data(0, KEY_ROLE) if item else None

    def select(self, key: str) -> None:
        def walk(item: QTreeWidgetItem):
            if item.data(0, KEY_ROLE) == key:
                return item
            for i in range(item.childCount()):
                found = walk(item.child(i))
                if found:
                    return found
            return None
        for i in range(self.tree.topLevelItemCount()):
            found = walk(self.tree.topLevelItem(i))
            if found:
                self.tree.setCurrentItem(found)
                self.tree.scrollToItem(found)
                return

    def visible_keys(self) -> list[str]:
        out = []

        def walk(item: QTreeWidgetItem):
            k = item.data(0, KEY_ROLE)
            if k:
                out.append(k)
            for i in range(item.childCount()):
                walk(item.child(i))
        for i in range(self.tree.topLevelItemCount()):
            walk(self.tree.topLevelItem(i))
        return out

    def _current(self, item, _previous) -> None:
        key = item.data(0, KEY_ROLE) if item else None
        if key:
            self.asset_selected.emit(key)


# ---------------------------------------------------------- screen view
class ScreenView(QLabel):
    clicked = Signal(int, int)

    def __init__(self):
        super().__init__()
        self.setFixedSize(320 * SCALE, 240 * SCALE)
        self.setStyleSheet('background:#111; border:1px solid #444')
        self.frame = QPixmap()
        self.rects: list[tuple[QRect, QColor]] = []
        self.selected: QRect | None = None
        self.show_rects = True

    def set_frame(self, pixmap: QPixmap) -> None:
        self.frame = pixmap
        self.update()

    def paintEvent(self, event) -> None:  # noqa: N802 - Qt API
        p = QPainter(self)
        p.fillRect(self.rect(), QColor('#111'))
        if not self.frame.isNull():
            p.drawPixmap(QRect(0, 0, 320 * SCALE, 240 * SCALE), self.frame)
        if self.show_rects:
            for r, color in self.rects:
                p.setPen(QPen(color, 1, Qt.PenStyle.DashLine))
                p.drawRect(QRect(r.x() * SCALE, r.y() * SCALE, r.width() * SCALE - 1, r.height() * SCALE - 1))
        if self.selected:
            r = self.selected
            p.setPen(QPen(QColor('#ffeb3b'), 2))
            p.drawRect(QRect(r.x() * SCALE, r.y() * SCALE, r.width() * SCALE - 1, r.height() * SCALE - 1))
        p.end()

    def mousePressEvent(self, event) -> None:  # noqa: N802
        pos = event.position().toPoint()
        self.clicked.emit(pos.x() // SCALE, pos.y() // SCALE)


KIND_COLORS = {'image asset': QColor('#4fc3f7'), 'firmware-drawn text': QColor('#ff8a65'),
               'firmware ROM bitmap': QColor('#ba68c8'), 'firmware primitive': QColor('#81c784')}


class ScreenEditor(QWidget):
    asset_selected = Signal(str)

    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        self.rendered: screens.RenderedScreen | None = None
        self.before: screens.RenderedScreen | None = None
        self._pending = False
        lay = QVBoxLayout(self)
        bar = QHBoxLayout()
        self.screen_box = QComboBox()
        group = None
        for s in screens.CATALOG:
            if s.group != group:
                group = s.group
                self.screen_box.addItem(f'— {group} —')
                self.screen_box.model().item(self.screen_box.count() - 1).setEnabled(False)
            self.screen_box.addItem(s.title, s.id)
        self.screen_box.setCurrentIndex(self.screen_box.findData('dc_v'))
        self.layout_box = QComboBox()
        self.layout_box.addItems(['Classic digits', 'Large digits'])
        self.lang_box = QComboBox()
        self.lang_box.addItem('English', 1)
        self.lang_box.addItem('Chinese', 0)
        self.compare_box = QComboBox()
        self.compare_box.addItems(['After (working copy)', 'Before (base image)', 'Difference'])
        self.rects_box = QCheckBox('Element outlines')
        self.rects_box.setChecked(True)
        for w in (QLabel('Screen'), self.screen_box, self.layout_box, self.lang_box, self.compare_box, self.rects_box):
            bar.addWidget(w)
        bar.addStretch(1)
        lay.addLayout(bar)
        self.view = ScreenView()
        lay.addWidget(self.view, 0, Qt.AlignmentFlag.AlignHCenter)
        self.note = QLabel('Preview rendered by the firmware\'s own UI code (native simulator) from the working image. '
                           'Readings on these screens are synthetic display packets.')
        self.note.setWordWrap(True)
        self.note.setStyleSheet('color:gray')
        lay.addWidget(self.note)
        self.elements = QTreeWidget()
        self.elements.setHeaderLabels(['Element', 'Origin', 'x', 'y', 'w', 'h'])
        self.elements.setColumnWidth(0, 260)
        self.elements.setColumnWidth(1, 150)
        for c in range(2, 6):
            self.elements.setColumnWidth(c, 45)
        lay.addWidget(self.elements, 1)
        legend = QLabel('<span style="color:#4fc3f7">■</span> image asset (replaceable JPEG; text inside is baked into '
                        'the picture) &nbsp; <span style="color:#ff8a65">■</span> firmware-drawn text (font ROM; '
                        'position/colour fixed in firmware) &nbsp; <span style="color:#ba68c8">■</span> ROM bitmap')
        legend.setWordWrap(True)
        lay.addWidget(legend)
        for box in (self.screen_box, self.layout_box, self.lang_box, self.compare_box):
            box.currentIndexChanged.connect(self.refresh)
        self.rects_box.toggled.connect(self._toggle_rects)
        self.view.clicked.connect(self._clicked)
        self.elements.currentItemChanged.connect(self._element)

    def _toggle_rects(self, on: bool) -> None:
        self.view.show_rects = on
        self.view.update()

    def screen_def(self) -> screens.ScreenDef:
        return screens.BY_ID[self.screen_box.currentData() or 'dc_v']

    def refresh(self) -> None:
        s = self.state.session
        if not s:
            return
        screen, layout, lang = self.screen_def(), self.layout_box.currentIndex(), self.lang_box.currentData()
        working, base = s.working_file(), s.base_file()

        palette = dict(s.palette)

        def job():
            after = screens.render(working, screen, layout, lang, palette=palette)
            before = screens.render(base, screen, layout, lang) if (s.replacements or palette) else after
            return after, before
        run_task(job, on_done=self._show, on_failed=lambda m: error(self, 'Screen preview', m))

    def _show(self, result) -> None:
        self.rendered, self.before = result
        r = self.rendered
        if r.error:
            self.note.setText(f'Simulator error: {r.error}')
        mode = self.compare_box.currentIndex()
        if mode == 0:
            pix = bytes_to_pixmap(r.png)
        elif mode == 1:
            pix = bytes_to_pixmap(self.before.png)
        else:
            a = Image.open(io.BytesIO(r.png)).convert('RGB')
            b = Image.open(io.BytesIO(self.before.png)).convert('RGB')
            diff = ImageChops.difference(a, b)
            mask = diff.convert('L').point(lambda v: 255 if v else 0)
            out = Image.blend(b, Image.new('RGB', b.size, (0, 0, 0)), 0.6)
            out.paste(Image.new('RGB', b.size, (255, 40, 40)), (0, 0), mask)
            pix = QPixmap.fromImage(pil_to_qimage(out))
        self.view.set_frame(pix)
        ops = r.visible_ops
        self.view.rects = [(QRect(o.x, o.y, o.w, o.h), KIND_COLORS.get(o.kind, QColor('white')))
                           for o in ops if o.op != 'fill']
        self.view.selected = None
        self.elements.clear()
        modified = set(self.state.session.replacements) if self.state.session else set()
        for o in reversed(ops):
            if o.op == 'fill':
                continue
            if o.op == 'jpeg':
                key = asset_key(o.path)
                name = o.path + (' ●' if key in modified else '') + ('' if o.ok else '  ⚠ failed to decode')
            elif o.op == 'chinese':
                name = 'Chinese glyphs (font ROM)'
            elif o.text:
                name = repr(o.text)
            else:
                name = o.op
            item = QTreeWidgetItem([name, o.kind, str(o.x), str(o.y), str(o.w), str(o.h)])
            item.setData(0, KEY_ROLE, o)
            item.setForeground(1, KIND_COLORS.get(o.kind, QColor('white')))
            self.elements.addTopLevelItem(item)
        self.view.update()

    def _clicked(self, x: int, y: int) -> None:
        if not self.rendered:
            return
        op = self.rendered.hit(x, y)
        if not op:
            return
        for i in range(self.elements.topLevelItemCount()):
            item = self.elements.topLevelItem(i)
            if item.data(0, KEY_ROLE) is op:
                self.elements.setCurrentItem(item)
                return

    def _element(self, item, _prev) -> None:
        if not item:
            return
        op = item.data(0, KEY_ROLE)
        self.view.selected = QRect(op.x, op.y, op.w, op.h)
        self.view.update()
        if op.op == 'jpeg':
            self.asset_selected.emit(asset_key(op.path))


# --------------------------------------------------------------- gallery
class Gallery(QListWidget):
    asset_selected = Signal(str)

    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        self.setViewMode(QListWidget.ViewMode.IconMode)
        self.setIconSize(QSize(120, 90))
        self.setGridSize(QSize(150, 125))
        self.setResizeMode(QListWidget.ResizeMode.Adjust)
        self.setUniformItemSizes(True)
        self.currentItemChanged.connect(lambda item, _p: item and self.asset_selected.emit(item.data(KEY_ROLE)))
        self._cache: dict[tuple[str, int], QIcon] = {}

    def show_keys(self, keys: list[str]) -> None:
        s = self.state.session
        self.clear()
        if not s:
            return
        for key in keys[:600]:
            data = s.current_bytes(key)
            ck = (key, hash(data))
            icon = self._cache.get(ck)
            if icon is None:
                pix = scaled(bytes_to_pixmap(data), 120, 90)
                icon = QIcon(pix)
                self._cache[ck] = icon
            item = QListWidgetItem(icon, key.split('/')[-1] + (' ●' if key in s.replacements else ''))
            item.setData(KEY_ROLE, key)
            item.setToolTip(key)
            self.addItem(item)


# ------------------------------------------------------------ text tool
class TextOverlayDialog(QDialog):
    """Repaint baked-in text: cover an area of the original image and draw new text."""

    def __init__(self, parent, original: Image.Image):
        super().__init__(parent)
        self.setWindowTitle('Edit baked-in text')
        self.original = original.convert('RGB')
        self.result_image: Image.Image | None = None
        self.color = QColor('white')
        self.background = QColor(*self.original.getpixel((0, 0)))
        form = QFormLayout()
        self.text = QLineEdit('Text')
        self.font_box = QFontComboBox()
        self.size = QSpinBox(minimum=4, maximum=200, value=max(8, self.original.height * 2 // 3))
        self.bold = QCheckBox('Bold')
        self.x = QSpinBox(minimum=-1000, maximum=1000, value=0)
        self.y = QSpinBox(minimum=-1000, maximum=1000, value=0)
        self.align = QComboBox()
        self.align.addItems(['Centre', 'Left', 'Right'])
        self.cover = QCheckBox('Cover original text with background colour')
        self.cover.setChecked(True)
        self.color_btn = QPushButton('Text colour…')
        self.bg_btn = QPushButton('Background colour…')
        form.addRow('Text', self.text)
        form.addRow('Font', self.font_box)
        form.addRow('Size (px)', self.size)
        form.addRow('', self.bold)
        form.addRow('Alignment', self.align)
        form.addRow('Offset x / y', self._pair(self.x, self.y))
        form.addRow('', self.cover)
        form.addRow(self.color_btn, self.bg_btn)
        self.preview = QLabel()
        self.preview.setMinimumSize(self.original.width * 3, self.original.height * 3)
        lay = QVBoxLayout(self)
        lay.addLayout(form)
        lay.addWidget(QLabel('Preview (3×, before JPEG encoding):'))
        lay.addWidget(self.preview)
        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        lay.addWidget(buttons)
        for w in (self.text.textChanged, self.font_box.currentFontChanged, self.size.valueChanged, self.bold.toggled,
                  self.x.valueChanged, self.y.valueChanged, self.align.currentIndexChanged, self.cover.toggled):
            w.connect(self.render)
        self.color_btn.clicked.connect(lambda: self._pick('color'))
        self.bg_btn.clicked.connect(lambda: self._pick('background'))
        self.render()

    @staticmethod
    def _pair(a, b) -> QWidget:
        w = QWidget()
        h = QHBoxLayout(w)
        h.setContentsMargins(0, 0, 0, 0)
        h.addWidget(a)
        h.addWidget(b)
        return w

    def _pick(self, which: str) -> None:
        c = QColorDialog.getColor(getattr(self, which), self)
        if c.isValid():
            setattr(self, which, c)
            self.render()

    def render(self) -> None:
        w, h = self.original.size
        img = pil_to_qimage(self.original).convertToFormat(QImage.Format.Format_RGB32)
        p = QPainter(img)
        if self.cover.isChecked():
            p.fillRect(0, 0, w, h, self.background)
        f = QFont(self.font_box.currentFont())
        f.setPixelSize(self.size.value())
        f.setBold(self.bold.isChecked())
        p.setFont(f)
        p.setPen(self.color)
        p.setRenderHint(QPainter.RenderHint.TextAntialiasing)
        flags = {0: Qt.AlignmentFlag.AlignHCenter, 1: Qt.AlignmentFlag.AlignLeft, 2: Qt.AlignmentFlag.AlignRight}
        p.drawText(QRect(self.x.value(), self.y.value(), w, h),
                   flags[self.align.currentIndex()] | Qt.AlignmentFlag.AlignVCenter, self.text.text())
        p.end()
        buf = img.convertToFormat(QImage.Format.Format_RGBA8888)
        data = bytes(buf.constBits())[:w * h * 4]
        self.result_image = Image.frombytes('RGBA', (w, h), data).convert('RGB')
        self.preview.setPixmap(QPixmap.fromImage(img).scaled(w * 3, h * 3))


# ----------------------------------------------------------- asset panel
class AssetPanel(QWidget):
    changed = Signal(str)

    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        self.key: str | None = None
        lay = QVBoxLayout(self)
        self.title = QLabel('No asset selected')
        self.title.setStyleSheet('font-weight:bold; font-size:14px')
        self.title.setWordWrap(True)
        lay.addWidget(self.title)
        pics = QHBoxLayout()
        self.orig_pic, self.cur_pic = QLabel(), QLabel()
        for label, pic in (('Original', self.orig_pic), ('Working copy (as the device decodes it)', self.cur_pic)):
            box = QGroupBox(label)
            v = QVBoxLayout(box)
            pic.setMinimumSize(150, 110)
            pic.setAlignment(Qt.AlignmentFlag.AlignCenter)
            v.addWidget(pic)
            pics.addWidget(box)
        lay.addLayout(pics)
        self.meta = QPlainTextEdit(readOnly=True)
        self.meta.setMaximumHeight(170)
        lay.addWidget(self.meta)
        opts = QGroupBox('Import options')
        form = QFormLayout(opts)
        self.fit = QComboBox()
        self.fit.addItems(['stretch', 'fit', 'fill', 'center'])
        self.quality = QSpinBox(minimum=30, maximum=100, value=90)
        self.keep_sampling = QCheckBox('Keep the original chroma sampling')
        self.keep_sampling.setChecked(True)
        form.addRow('Resize to original size', self.fit)
        form.addRow('JPEG quality', self.quality)
        form.addRow('', self.keep_sampling)
        lay.addWidget(opts)
        self.replace_btn = QPushButton('Replace from file…')
        self.text_btn = QPushButton('Edit baked-in text…')
        self.restore_btn = QPushButton('Restore original')
        self.export_btn = QPushButton('Export current…')
        self.export_orig_btn = QPushButton('Export original…')
        grid = QHBoxLayout()
        grid.addWidget(self.replace_btn)
        grid.addWidget(self.text_btn)
        lay.addLayout(grid)
        grid2 = QHBoxLayout()
        grid2.addWidget(self.restore_btn)
        grid2.addWidget(self.export_btn)
        grid2.addWidget(self.export_orig_btn)
        lay.addLayout(grid2)
        self.layout_note = QLabel('Position, size, font and colour of firmware-drawn elements are fixed in the '
                                  'current firmware; see FIRMWARE_CHANGES.md for the planned layout table.')
        self.layout_note.setWordWrap(True)
        self.layout_note.setStyleSheet('color:gray')
        lay.addWidget(self.layout_note)
        lay.addStretch(1)
        self.replace_btn.clicked.connect(self.replace_from_file)
        self.text_btn.clicked.connect(self.edit_text)
        self.restore_btn.clicked.connect(self.restore)
        self.export_btn.clicked.connect(lambda: self.export(False))
        self.export_orig_btn.clicked.connect(lambda: self.export(True))
        self.show_asset(None)

    def show_asset(self, key: str | None) -> None:
        self.key = key
        s = self.state.session
        enabled = bool(s and key)
        writable = enabled and key.startswith('1:/')
        for b in (self.replace_btn, self.text_btn):
            b.setEnabled(bool(writable))
        self.export_btn.setEnabled(enabled)
        self.export_orig_btn.setEnabled(enabled)
        self.restore_btn.setEnabled(bool(enabled and key in s.replacements))
        if not enabled:
            self.title.setText('No asset selected')
            self.orig_pic.clear()
            self.cur_pic.clear()
            self.meta.setPlainText('')
            return
        orig, cur = s.original_bytes(key), s.current_bytes(key)
        self.title.setText(key + ('  (modified)' if key in s.replacements else ''))
        self.orig_pic.setPixmap(scaled(bytes_to_pixmap(orig), 300, 200))
        info = jpegtools.inspect(cur)
        if cur[:2] == b'\xff\xd8':
            dec = jpegtools.device_decode(cur) if not jpegtools.static_problems(info) else None
            img = dec.image() if dec and dec.ok else None
            self.cur_pic.setPixmap(scaled(QPixmap.fromImage(pil_to_qimage(img)), 300, 200) if img
                                   else scaled(bytes_to_pixmap(cur), 300, 200))
            device = dec.message if dec else 'not decodable by the device: ' + '; '.join(jpegtools.static_problems(info))
        else:
            self.cur_pic.setText('(not an image)')
            device = 'not a JPEG'
        e = s.working.resolve(key)
        lines = [f'Bytes: {len(cur)} (original {len(orig)})', f'Clusters: {len(e.clusters) if e else "?"} × 4 KiB',
                 f'Image: {info.width}×{info.height} {info.process}, {info.components} comp., {info.subsampling}, '
                 f'quality ≈ {info.quality_estimate}', f'Device decoder: {device}',
                 f'Screens: {", ".join(self.state.usage.get(key, [])) or "not seen on catalog screens"}']
        if key.startswith('0:/'):
            lines.append('BOOT volume: read-only in Studio (bootloader update area).')
        if key == '1:/on.jpg':
            lines.append('Boot logo. The firmware draws it as "0:/on.jpg" (volume quirk: it reads this file). '
                         'An optional 0:/LOGO.jpg on the BOOT disk overrides it.')
        if s.sources.get(key):
            lines.append(f'Source: {s.sources[key]}')
        self.meta.setPlainText('\n'.join(lines))

    def _apply(self, data: bytes, source: str | None = None) -> None:
        s = self.state.session
        result = s.check(self.key, data)
        if not result.ok:
            text = '\n'.join(result.errors)
            box = QMessageBox(QMessageBox.Icon.Warning, 'Incompatible image',
                              f'The image would not work on the device:\n\n{text}', QMessageBox.StandardButton.Ok, self)
            box.exec()
            return
        s.replace(self.key, data, source=source)
        self.changed.emit(self.key)

    def _convert(self, image: Image.Image) -> bytes:
        orig = self.state.session.original_bytes(self.key)
        sampling = None if self.keep_sampling.isChecked() else '4:4:4'
        return jpegtools.convert(image, orig, mode=self.fit.currentText(), quality=self.quality.value(),
                                 subsampling=sampling)

    def replace_from_file(self) -> None:
        path, _ = QFileDialog.getOpenFileName(self, 'Replacement image', '',
                                              'Images (*.png *.jpg *.jpeg *.bmp *.gif *.webp *.tif *.tiff)')
        if not path:
            return
        try:
            raw = Path(path).read_bytes()
            info = jpegtools.inspect(raw)
            orig = jpegtools.inspect(self.state.session.original_bytes(self.key))
            if raw[:2] == b'\xff\xd8' and not jpegtools.static_problems(info) and \
                    (info.width, info.height) == (orig.width, orig.height):
                data = raw  # already a device-compatible JPEG of the right size: keep its bytes
            else:
                data = self._convert(jpegtools.load_source(path))
            self._apply(data, source=path)
        except Exception as ex:  # noqa: BLE001
            error(self, 'Replace asset', str(ex))

    def edit_text(self) -> None:
        s = self.state.session
        base = Image.open(io.BytesIO(s.current_bytes(self.key)))
        dlg = TextOverlayDialog(self, base)
        if dlg.exec() == QDialog.DialogCode.Accepted and dlg.result_image is not None:
            self._apply(self._convert(dlg.result_image), source='text overlay')

    def restore(self) -> None:
        self.state.session.restore(self.key)
        self.changed.emit(self.key)

    def export(self, original: bool) -> None:
        s = self.state.session
        name = self.key.split('/')[-1]
        path, _ = QFileDialog.getSaveFileName(self, 'Export asset', name)
        if path:
            Path(path).write_bytes(s.original_bytes(self.key) if original else s.current_bytes(self.key))


# ----------------------------------------------------------------- page
class ThemeEditorPage(QWidget):
    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        split = QSplitter()
        self.browser = AssetBrowser(state)
        self.center = QTabWidget()
        self.screen = ScreenEditor(state)
        self.gallery = Gallery(state)
        self.center.addTab(self.screen, 'Screen editor')
        self.center.addTab(self.gallery, 'Gallery')
        self.panel = AssetPanel(state)
        split.addWidget(self.browser)
        split.addWidget(self.center)
        split.addWidget(self.panel)
        split.setSizes([330, 700, 380])
        lay = QVBoxLayout(self)
        lay.setContentsMargins(4, 4, 4, 4)
        lay.addWidget(split)
        self.browser.asset_selected.connect(self.select)
        self.browser.batch_btn.clicked.connect(self.batch_color)
        self.screen.asset_selected.connect(self.select_and_reveal)
        self.gallery.asset_selected.connect(self.select_and_reveal)
        self.panel.changed.connect(self.on_changed)
        self.center.currentChanged.connect(lambda i: i == 1 and self.gallery.show_keys(self.browser.visible_keys()))
        self.browser.search.textChanged.connect(self._gallery_follow)
        state.image_opened.connect(self.on_opened)
        state.assets_changed.connect(self._external_change)

    def _gallery_follow(self) -> None:
        if self.center.currentIndex() == 1:
            self.gallery.show_keys(self.browser.visible_keys())

    def select(self, key: str) -> None:
        self.panel.show_asset(key)

    def select_and_reveal(self, key: str) -> None:
        self.browser.select(key)
        self.panel.show_asset(key)

    def on_opened(self) -> None:
        self.browser.refresh()
        self.panel.show_asset(None)
        self.screen.refresh()
        self.compute_usage()

    def compute_usage(self) -> None:
        s = self.state.session
        if not s:
            return
        path = s.working_file()

        def job():
            rendered = []
            for layout in (0, 1):
                for lang in (1, 0):
                    rendered += screens.render_many(path, screens.CATALOG, layout, lang)
            return screens.asset_usage(rendered)

        def done(usage):
            self.state.usage = usage
            self.browser.refresh()
            self.state.status.emit(f'Screen map: {len(usage)} assets appear on the catalog screens '
                                   '(both layouts, English and Chinese)')
        run_task(job, on_done=done)

    def on_changed(self, key: str) -> None:
        self.state.assets_changed.emit(key)

    def _external_change(self, key: str) -> None:
        self.browser.refresh()
        if key:
            self.panel.show_asset(key if self.panel.key in (None, key) else self.panel.key)
        else:
            self.panel.show_asset(self.panel.key)
        self.screen.refresh()
        if self.center.currentIndex() == 1:
            self.gallery.show_keys(self.browser.visible_keys())

    def batch_color(self) -> None:
        if not self.state.session:
            return
        dlg = BatchColorDialog(self, self.state, self.screen.screen_def(), self.screen.layout_box.currentIndex(),
                               self.screen.lang_box.currentData())
        dlg.resize(1100, 720)
        dlg.exec()

    # batch import: a folder that mirrors the asset tree (e.g. an export, edited elsewhere)
    def batch_import(self) -> None:
        s = self.state.session
        if not s:
            return
        folder = QFileDialog.getExistingDirectory(self, 'Folder mirroring the asset tree (e.g. mode/dc_v.png)')
        if not folder:
            return
        root = Path(folder)
        rows = {r.path.lower().rsplit('.', 1)[0]: r for r in s.assets() if r.volume == 1 and r.is_jpeg}
        report, ok = [], 0
        for f in sorted(root.rglob('*')):
            if not f.is_file() or f.suffix.lower() not in ('.png', '.jpg', '.jpeg', '.bmp', '.webp', '.gif'):
                continue
            rel = f.relative_to(root).as_posix()
            if rel.split('/')[0] in ('0', '1'):
                rel = rel.split('/', 1)[1] if '/' in rel else rel
            row = rows.get(rel.lower().rsplit('.', 1)[0])
            if not row:
                report.append(f'skipped {rel}: no asset with that name')
                continue
            try:
                raw = f.read_bytes()
                info = jpegtools.inspect(raw)
                if raw[:2] == b'\xff\xd8' and not jpegtools.static_problems(info) and \
                        (info.width, info.height) == (row.width, row.height):
                    data = raw
                else:
                    data = jpegtools.convert(jpegtools.load_source(f), s.original_bytes(row.key))
                result = s.replace(row.key, data, source=str(f))
                if result.ok:
                    ok += 1
                else:
                    report.append(f'rejected {rel}: ' + '; '.join(result.errors))
            except Exception as ex:  # noqa: BLE001
                report.append(f'failed {rel}: {ex}')
        self.state.assets_changed.emit('')
        QMessageBox.information(self, 'Batch import', f'{ok} asset(s) replaced.\n\n' + '\n'.join(report[:40]))


# ------------------------------------------------------------ batch colour
class BatchColorDialog(QDialog):
    """One set of colour controls for many assets and the firmware-drawn colours."""

    def __init__(self, parent, state: AppState, screen_def, layout: int, language: int):
        from PySide6.QtCore import QTimer
        from PySide6.QtWidgets import QSlider, QDoubleSpinBox
        from ..core import colorops
        super().__init__(parent)
        self.setWindowTitle('Batch colour adjust')
        self.state, self.screen_def, self.layout, self.language = state, screen_def, layout, language
        self.colorops = colorops
        self._job = 0
        lay = QHBoxLayout(self)
        left = QVBoxLayout()
        lay.addLayout(left, 1)
        scope = QGroupBox('Images to recolour')
        sv = QVBoxLayout(scope)
        self.groups = QListWidget()
        for name in colorops.GROUPS:
            item = QListWidgetItem(name)
            item.setFlags(item.flags() | Qt.ItemFlag.ItemIsUserCheckable)
            item.setCheckState(Qt.CheckState.Checked if name.startswith(('Large digits', 'Units')) else Qt.CheckState.Unchecked)
            self.groups.addItem(item)
        sv.addWidget(self.groups)
        self.scope_count = QLabel()
        sv.addWidget(self.scope_count)
        self.from_original = QCheckBox('Start from the original artwork (recommended)')
        self.from_original.setChecked(True)
        self.from_original.setToolTip('Unchecked: recolour the current working images, including manual edits.')
        sv.addWidget(self.from_original)
        qrow = QHBoxLayout()
        qrow.addWidget(QLabel('JPEG quality'))
        self.quality = QSpinBox(minimum=40, maximum=100, value=85)
        self.quality.setToolTip('Lower quality makes smaller packages (the bootloader disk holds about 1.5 MB).')
        qrow.addWidget(self.quality)
        qrow.addStretch(1)
        sv.addLayout(qrow)
        left.addWidget(scope)
        ctl = QGroupBox('Colour')
        form = QFormLayout(ctl)

        def slider(lo, hi, val, suffix=''):
            s = QSlider(Qt.Orientation.Horizontal)
            s.setRange(lo, hi)
            s.setValue(val)
            box = QSpinBox(minimum=lo, maximum=hi, value=val, suffix=suffix)
            s.valueChanged.connect(box.setValue)
            box.valueChanged.connect(s.setValue)
            w = QWidget()
            h = QHBoxLayout(w)
            h.setContentsMargins(0, 0, 0, 0)
            h.addWidget(s, 1)
            h.addWidget(box)
            return box, w
        self.hue, w = slider(-180, 180, 0, '°')
        form.addRow('Hue shift', w)
        self.sat, w = slider(0, 200, 100, ' %')
        form.addRow('Saturation', w)
        self.bri, w = slider(20, 200, 100, ' %')
        form.addRow('Brightness', w)
        self.range_on = QCheckBox('Only colours near this hue')
        self.center, w = slider(0, 359, 35, '°')
        form.addRow(self.range_on, w)
        self.width, w = slider(5, 90, 25, '°')
        form.addRow('Hue range ±', w)
        self.min_sat, w = slider(0, 60, 15, ' %')
        form.addRow('Leave greys below saturation', w)
        presets = QHBoxLayout()
        for label, values in (('Orange → green', (60, 100, 100, True, 35, 25)),
                              ('Orange → cyan', (150, 100, 100, True, 35, 25)),
                              ('Orange → red', (-30, 110, 100, True, 35, 25)),
                              ('Everything → purple', (-90, 100, 100, False, 35, 25)),
                              ('Reset', (0, 100, 100, False, 35, 25))):
            b = QPushButton(label)
            b.clicked.connect(lambda _c=False, v=values: self._preset(v))
            presets.addWidget(b)
        form.addRow(presets)
        left.addWidget(ctl)
        fw = QGroupBox('Colours the firmware draws itself (theme palette, firmware 0.5.0+)')
        fv = QVBoxLayout(fw)
        self.fw_on = QCheckBox('Apply the same change (classic-layout digits, menu text, borders)')
        self.fw_on.setChecked(True)
        fv.addWidget(self.fw_on)
        self.fw_table = QTreeWidget()
        self.fw_table.setHeaderLabels(['Colour', 'New', 'Used for'])
        self.fw_table.setRootIsDecorated(False)
        self.fw_table.setMaximumHeight(120)
        fv.addWidget(self.fw_table)
        left.addWidget(fw)
        right = QVBoxLayout()
        lay.addLayout(right, 1)
        right.addWidget(QLabel(f'Preview: {screen_def.title}'))
        self.before, self.after = QLabel(), QLabel()
        for lab, title in ((self.before, 'Before'), (self.after, 'After')):
            lab.setFixedSize(320, 240)
            lab.setStyleSheet('background:#111')
            right.addWidget(QLabel(title))
            right.addWidget(lab)
        self.note = QLabel('')
        self.note.setWordWrap(True)
        right.addWidget(self.note)
        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Apply | QDialogButtonBox.StandardButton.Cancel)
        buttons.button(QDialogButtonBox.StandardButton.Apply).setText('Apply to selected images')
        buttons.button(QDialogButtonBox.StandardButton.Apply).clicked.connect(self.apply)
        buttons.rejected.connect(self.reject)
        right.addWidget(buttons)
        self.timer = QTimer(self)
        self.timer.setSingleShot(True)
        self.timer.timeout.connect(self.preview)
        for box in (self.hue, self.sat, self.bri, self.center, self.width, self.min_sat):
            box.valueChanged.connect(lambda _v: self.timer.start(250))
        for cb in (self.range_on, self.fw_on, self.from_original):
            cb.toggled.connect(lambda _v: self.timer.start(250))
        self.groups.itemChanged.connect(lambda _i: (self._count(), self.timer.start(250)))
        self._count()
        self.before.setPixmap(QPixmap())
        self.timer.start(10)

    def _preset(self, v) -> None:
        self.hue.setValue(v[0])
        self.sat.setValue(v[1])
        self.bri.setValue(v[2])
        self.range_on.setChecked(v[3])
        self.center.setValue(v[4])
        self.width.setValue(v[5])

    def adjust(self):
        return self.colorops.ColorAdjust(self.hue.value(), self.sat.value() / 100, self.bri.value() / 100,
                                         self.center.value() if self.range_on.isChecked() else None,
                                         self.width.value(), self.min_sat.value() / 100)

    def selected_keys(self) -> list[str]:
        prefixes = []
        for i in range(self.groups.count()):
            item = self.groups.item(i)
            if item.checkState() == Qt.CheckState.Checked:
                prefixes += list(self.colorops.GROUPS[item.text()])
        keys = []
        for row in self.state.session.assets():
            if row.volume != 1 or not row.is_jpeg or row.path.lower().startswith('themes/'):
                continue
            if not themepkg.packable(row.path):     # e.g. 'mode/组 1.jpg': the firmware cannot open it
                continue
            p = row.path.lower()
            if any((pre == '' and '/' not in p) or (pre and p.startswith(pre)) for pre in prefixes):
                keys.append(row.key)
        return keys

    def _count(self) -> None:
        self.scope_count.setText(f'{len(self.selected_keys())} images selected')

    def _palette(self, adj) -> dict[int, int]:
        if not self.fw_on.isChecked():
            return {}
        return self.colorops.palette_for(adj)

    def _recolor(self, key: str, adj) -> bytes | None:
        """Recoloured JPEG, or None when the change is invisible (image left out of the theme)."""
        import numpy as np
        s = self.state.session
        base = s.original_bytes(key) if self.from_original.isChecked() else s.current_bytes(key)
        src = Image.open(io.BytesIO(base)).convert('RGB')
        image = self.colorops.adjust_image(src, adj)
        diff = np.abs(np.asarray(image, dtype=np.int16) - np.asarray(src, dtype=np.int16))
        if diff.max() <= 6 and diff.mean() <= 0.5:
            return None
        return jpegtools.convert(image, s.original_bytes(key), quality=self.quality.value())

    def _fill_table(self, palette: dict[int, int]) -> None:
        self.fw_table.clear()
        for c, use in self.colorops.FIRMWARE_COLORS.items():
            new = palette.get(c, c)
            item = QTreeWidgetItem([f'#{c:04x}', f'#{new:04x}' if new != c else 'unchanged', use])
            for col, value in ((0, c), (1, new)):
                item.setBackground(col, QColor(*self.colorops.rgb565_to_rgb(value)))
                r, g, b = self.colorops.rgb565_to_rgb(value)
                item.setForeground(col, QColor('black' if r * 0.3 + g * 0.59 + b * 0.11 > 128 else 'white'))
            self.fw_table.addTopLevelItem(item)

    def preview(self) -> None:
        from ..core.flashimage import FlashImage
        s = self.state.session
        adj = self.adjust()
        palette = self._palette(adj)
        self._fill_table(palette)
        selected = set(self.selected_keys())
        self._job += 1
        job_id = self._job
        working = bytes(s.working.data)
        screen, layout, language = self.screen_def, self.layout, self.language
        base_palette = dict(s.palette)

        def job():
            import tempfile
            before = screens.render(s.working_file(), screen, layout, language, palette=base_palette)
            visible = [screens.asset_key(p) for p in before.assets()]
            image = FlashImage(working)
            for key in visible:
                if key in selected and not adj.identity:
                    data = self._recolor(key, adj)
                    if data is not None:
                        image.replace_file(image.resolve(key), data)
            merged = dict(base_palette)
            merged.update(palette)
            with tempfile.TemporaryDirectory() as tmp:
                path = Path(tmp) / 'preview.bin'
                path.write_bytes(image.data)
                after = screens.render(path, screen, layout, language, palette=merged)
            return job_id, before.png, after.png

        def done(result):
            jid, before, after = result
            if jid != self._job:
                return
            self.before.setPixmap(bytes_to_pixmap(before))
            self.after.setPixmap(bytes_to_pixmap(after))
            self.note.setText('Only the images on this screen are recoloured in the preview; Apply recolours all '
                              f'{len(selected)} selected images.')
        run_task(job, on_done=done, on_failed=lambda m: self.note.setText(m.split('\n')[0]))

    def apply(self) -> None:
        from PySide6.QtWidgets import QProgressDialog
        s = self.state.session
        adj = self.adjust()
        keys = self.selected_keys()
        palette = dict(s.palette)
        if self.fw_on.isChecked():
            for c in self.colorops.FIRMWARE_COLORS:
                palette.pop(c, None)
            palette.update(self._palette(adj))
        if adj.identity and palette == s.palette and self.from_original.isChecked():
            changes = {k: s.original_bytes(k) for k in keys if k in s.replacements}
        else:
            progress = QProgressDialog('Recolouring images…', 'Cancel', 0, len(keys), self)
            progress.setWindowModality(Qt.WindowModality.WindowModal)
            changes = {}
            for i, key in enumerate(keys):
                if progress.wasCanceled():
                    return
                progress.setValue(i)
                QApplication_process()
                data = None if adj.identity else self._recolor(key, adj)
                if data is not None:
                    changes[key] = data
                elif self.from_original.isChecked() and key in s.replacements:
                    changes[key] = s.original_bytes(key)   # unchanged by this batch: back to the original
            progress.setValue(len(keys))
        try:
            s.replace_many(changes, palette=palette, source='batch colour ' + str(adj.as_dict()))
        except Exception as ex:  # noqa: BLE001
            error(self, 'Batch colour', str(ex))
            return
        self.state.assets_changed.emit('')
        self.accept()


def QApplication_process() -> None:  # noqa: N802 - keeps the progress dialog responsive
    from PySide6.QtWidgets import QApplication
    QApplication.processEvents()

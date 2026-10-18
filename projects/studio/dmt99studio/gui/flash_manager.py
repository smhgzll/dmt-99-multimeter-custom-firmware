"""Flash Manager: offline image inspection and online Winbond access."""
from __future__ import annotations

import datetime as _dt
import json
from pathlib import Path

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (QFileDialog, QGroupBox, QHBoxLayout, QLabel, QMessageBox, QPlainTextEdit,
                               QProgressBar, QPushButton, QSplitter, QTableWidget, QTableWidgetItem, QTabWidget,
                               QTreeWidget, QTreeWidgetItem, QVBoxLayout, QWidget)

from ..core import backup
from ..core.flashimage import ASSET_FIRST_SECTOR, ASSET_SECTORS, FLASH_SIZE, FlashImage
from .common import confirm, error, human_size, run_task
from .state import AppState

ENTRY_ROLE = Qt.ItemDataRole.UserRole


class FlashManagerPage(QWidget):
    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        self.live: FlashImage | None = None
        lay = QVBoxLayout(self)
        tabs = QTabWidget()
        lay.addWidget(tabs)
        tabs.addTab(self._offline_tab(), 'Offline image')
        tabs.addTab(self._online_tab(), 'Connected device')
        state.image_opened.connect(self.refresh)
        state.assets_changed.connect(lambda _k: self.refresh_diff())
        state.device_changed.connect(self._device_changed)
        self._device_changed()

    # ------------------------------------------------------------ offline
    def _offline_tab(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        bar = QHBoxLayout()
        self.export_file_btn = QPushButton('Export selected file…')
        self.export_part_btn = QPushButton('Export partition…')
        self.save_copy_btn = QPushButton('Save working image as…')
        self.diff_btn = QPushButton('Export diff report + sector patch…')
        for b in (self.export_file_btn, self.export_part_btn, self.save_copy_btn, self.diff_btn):
            bar.addWidget(b)
        bar.addStretch(1)
        lay.addLayout(bar)
        split = QSplitter(Qt.Orientation.Horizontal)
        left = QWidget()
        lv = QVBoxLayout(left)
        lv.setContentsMargins(0, 0, 0, 0)
        self.parts = QTableWidget(0, 6)
        self.parts.setHorizontalHeaderLabels(['#', 'Type', 'First LBA', 'Sectors', 'Bytes', 'Role'])
        self.parts.setMaximumHeight(130)
        lv.addWidget(QLabel('MBR partition table (4096-byte sectors)'))
        lv.addWidget(self.parts)
        self.files = QTreeWidget()
        self.files.setHeaderLabels(['Path', 'Size', 'Start cluster', 'Clusters', 'Flash offset'])
        self.files.setColumnWidth(0, 300)
        lv.addWidget(self.files, 1)
        split.addWidget(left)
        right = QWidget()
        rv = QVBoxLayout(right)
        rv.setContentsMargins(0, 0, 0, 0)
        self.volinfo = QPlainTextEdit(readOnly=True)
        self.validation = QPlainTextEdit(readOnly=True)
        self.diff = QPlainTextEdit(readOnly=True)
        for title, widget in (('Volumes', self.volinfo), ('Validation', self.validation),
                              ('Working copy vs base image', self.diff)):
            box = QGroupBox(title)
            v = QVBoxLayout(box)
            v.addWidget(widget)
            rv.addWidget(box)
        split.addWidget(right)
        split.setSizes([650, 500])
        lay.addWidget(split, 1)
        self.export_file_btn.clicked.connect(self.export_file)
        self.export_part_btn.clicked.connect(self.export_partition)
        self.save_copy_btn.clicked.connect(self.save_working)
        self.diff_btn.clicked.connect(self.export_diff)
        return w

    def refresh(self) -> None:
        s = self.state.session
        self.parts.setRowCount(0)
        self.files.clear()
        if not s:
            return
        img = s.working
        roles = {63: 'BOOT FAT12 — bootloader USB disk, optional LOGO.jpg (read-only here)',
                 463: 'Assets FAT12 — UI images (editable)'}
        for p in img.partitions:
            r = self.parts.rowCount()
            self.parts.insertRow(r)
            for c, v in enumerate((p.index, f'0x{p.type:02x}', p.first_sector, p.sectors, human_size(p.length),
                                   roles.get(p.first_sector, ''))):
                self.parts.setItem(r, c, QTableWidgetItem(str(v)))
        self.parts.resizeColumnsToContents()
        for v in img.volumes:
            root = QTreeWidgetItem([f'Volume {v.index}: {"BOOT" if v.index == 0 else "assets"} '
                                    f'({v.fs_type}, label "{v.label}")'])
            self.files.addTopLevelItem(root)
            folders = {'': root}
            for e in v.walk():
                parent = folders.get(e.folder.lower(), root)
                off = f'0x{v.cluster_offset(e.clusters[0]):06x}' if e.clusters else ''
                item = QTreeWidgetItem(parent, [e.name + ('/' if e.is_dir else ''), '' if e.is_dir else str(e.size),
                                                str(e.start_cluster), str(len(e.clusters)), off])
                item.setData(0, ENTRY_ROLE, e)
                if e.chain_error:
                    item.setForeground(0, Qt.GlobalColor.red)
                    item.setToolTip(0, e.chain_error)
                if e.is_dir:
                    folders[e.path.lower()] = item
            root.setExpanded(True)
        lines = []
        for v in img.volumes:
            i = v.info()
            lines.append(f'Volume {v.index} at 0x{i["offset"]:06x}: {i["bytes_per_sector"]} B/sector, '
                         f'{i["sectors_per_cluster"]} sector/cluster, {i["fat_copies"]} FATs × {i["fat_sectors"]} sector, '
                         f'{i["root_entries"]} root entries, {i["clusters"]} clusters, '
                         f'{human_size(i["used_bytes"])} used / {human_size(i["free_bytes"])} free')
        from ..core import themepkg
        themes = themepkg.installed_themes(img)
        lines.append('Installed themes: ' + (', '.join(f'T{t["slot"]:02d} {t["name"]}' + ('' if t['valid'] else ' (incomplete)')
                                                     for t in themes) or 'none'))
        pending = img.boot.find(themepkg.PACKAGE_NAME)
        if pending:
            lines.append(f'Pending THEME.PKG on the BOOT volume: {pending.size} bytes (installed at the next start)')
        lines.append(f'Base: {s.base.label}  sha256 {s.base.source_sha256[:16]}…'
                     + ('  = acquired original dump' if s.base.is_original_dump else ''))
        self.volinfo.setPlainText('\n'.join(lines))
        issues = img.validate()
        boot_issues = [i for i in issues if i.startswith(('System Volume', 'volume 0'))]
        other = [i for i in issues if i not in boot_issues]
        text = 'Assets volume: OK\n' if not other else 'Problems:\n' + '\n'.join(other) + '\n'
        if boot_issues:
            text += '\nBOOT volume (pre-existing, not written by Studio):\n' + '\n'.join(boot_issues)
        self.validation.setPlainText(text)
        self.refresh_diff()

    def refresh_diff(self) -> None:
        s = self.state.session
        if not s:
            return
        rep = s.working.diff_report()
        lines = [f'{len(rep["changed_sectors"])} changed 4 KiB sectors {rep["changed_by_region"]}']
        for f in rep['files']:
            lines.append(f'  {f["change"]:8} {f["volume"]}:/{f["path"]}  {f.get("old_size", "")}→{f["size"]} bytes')
        if rep['changed_sectors']:
            lines.append('Sectors: ' + ', '.join(str(x) for x in rep['changed_sectors'][:200]))
        self.diff.setPlainText('\n'.join(lines))

    def _selected_entry(self):
        item = self.files.currentItem()
        return item.data(0, ENTRY_ROLE) if item else None

    def export_file(self) -> None:
        e = self._selected_entry()
        if not e or e.is_dir:
            QMessageBox.information(self, 'Export', 'Select a file in the tree first.')
            return
        path, _ = QFileDialog.getSaveFileName(self, 'Export file', e.name)
        if path:
            Path(path).write_bytes(self.state.session.working.read_file(e))

    def export_partition(self) -> None:
        s = self.state.session
        if not s:
            return
        for name in ('boot', 'assets'):
            path, _ = QFileDialog.getSaveFileName(self, f'Export {name} partition', f'dmt99_{name}.img')
            if path:
                Path(path).write_bytes(s.working.export_region(name))

    def save_working(self) -> None:
        s = self.state.session
        if not s:
            return
        path, _ = QFileDialog.getSaveFileName(self, 'Save working image', 'dmt99_flash_working.bin', 'Images (*.bin)')
        if not path:
            return
        try:
            s.working.save_as(path)
            self.state.status.emit(f'Working image saved to {path}')
        except PermissionError as ex:
            error(self, 'Save working image', str(ex))

    def export_diff(self) -> None:
        s = self.state.session
        if not s:
            return
        folder = QFileDialog.getExistingDirectory(self, 'Folder for diff report and patch')
        if not folder:
            return
        rep = s.working.diff_report()
        Path(folder, 'diff_report.json').write_text(json.dumps(rep, indent=2) + '\n')
        Path(folder, 'sectors.patch').write_bytes(s.working.binary_diff())
        self.state.status.emit(f'Diff report and sector patch written to {folder}')

    # ------------------------------------------------------------- online
    def _online_tab(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        self.dev_label = QLabel()
        self.dev_label.setWordWrap(True)
        lay.addWidget(self.dev_label)
        bar = QHBoxLayout()
        self.read_btn = QPushButton('Read full 8 MiB flash → backup')
        self.compare_btn = QPushButton('Compare device with working image')
        self.upload_btn = QPushButton('Upload edited assets (assets partition only)…')
        for b in (self.read_btn, self.compare_btn, self.upload_btn):
            bar.addWidget(b)
        bar.addStretch(1)
        lay.addLayout(bar)
        self.progress = QProgressBar()
        lay.addWidget(self.progress)
        self.online_log = QPlainTextEdit(readOnly=True)
        lay.addWidget(self.online_log, 1)
        note = QLabel(f'Writes are limited to sectors {ASSET_FIRST_SECTOR}..{ASSET_FIRST_SECTOR + ASSET_SECTORS - 1} '
                      '(assets FAT12). The MBR and the BOOT volume are never written. The meter shows a maintenance '
                      'screen and stops reading assets while the PC writes; every sector is verified by CRC '
                      'after programming and a backup of the device state is taken first.')
        note.setWordWrap(True)
        note.setStyleSheet('color:gray')
        lay.addWidget(note)
        self.read_btn.clicked.connect(self.read_device)
        self.compare_btn.clicked.connect(self.compare_device)
        self.upload_btn.clicked.connect(self.upload)
        return w

    def _device_changed(self) -> None:
        d = self.state.device
        on = d is not None
        for b in (self.read_btn, self.compare_btn, self.upload_btn):
            b.setEnabled(on)
        self.dev_label.setText(f'Connected: {d.describe()}' if on else
                               'No device connected. Use Device ▸ Connect (USB management link of POTAT-99 '
                               'firmware). Offline functions work without a device.')

    def log(self, text: str) -> None:
        self.online_log.appendPlainText(f'{_dt.datetime.now():%H:%M:%S} {text}')

    def _progress(self, done: int, total: int) -> None:
        self.progress.setMaximum(max(total, 1))
        self.progress.setValue(done)

    def read_device(self) -> None:
        d = self.state.device
        self.log('Reading 8 MiB from the device…')

        def job():
            data = d.read_spi(0, FLASH_SIZE, progress=lambda a, b: self._emit_progress(a, b))
            return backup.save_backup('winbond-full', data, 0, source=d.describe(), verify=d.spi_crc(0, FLASH_SIZE))
        run_task(job, on_done=self._read_done, on_failed=lambda m: (self.log('read failed'), error(self, 'Read', m)))

    def _emit_progress(self, a: int, b: int) -> None:
        # Called from the worker thread: hop to the GUI thread.
        from PySide6.QtCore import QMetaObject, Q_ARG
        QMetaObject.invokeMethod(self.progress, 'setMaximum', Qt.ConnectionType.QueuedConnection, Q_ARG(int, b))
        QMetaObject.invokeMethod(self.progress, 'setValue', Qt.ConnectionType.QueuedConnection, Q_ARG(int, a))

    def _read_done(self, rec) -> None:
        self.log(f'Backup saved: {rec.path} sha256 {rec.sha256[:16]}… verified={rec.verified}')
        try:
            self.live = FlashImage.load(rec.path)
        except Exception as ex:  # noqa: BLE001
            self.log(f'Device image is not a valid DMT99 layout: {ex}')

    def compare_device(self) -> None:
        s = self.state.session
        d = self.state.device
        if not s:
            return
        self.log('Comparing sector CRCs (device vs working image)…')

        def job():
            return d.compare_sectors(s.working.data, progress=self._emit_progress)
        run_task(job, on_done=self._compare_done, on_failed=lambda m: error(self, 'Compare', m))

    def _compare_done(self, differing: list[int]) -> None:
        s = self.state.session
        by: dict[str, list[int]] = {}
        for sec in differing:
            by.setdefault(s.working.region_of(sec), []).append(sec)
        self.log(f'{len(differing)} sectors differ: ' + ', '.join(f'{k}: {len(v)}' for k, v in by.items()))
        if differing:
            self.log('First differing sectors: ' + ', '.join(map(str, differing[:60])))

    def upload(self) -> None:
        s = self.state.session
        d = self.state.device
        if not s or not s.replacements:
            QMessageBox.information(self, 'Upload', 'No edited assets to upload.')
            return
        if not confirm(self, 'Upload assets',
                       'This writes the edited assets to the meter\'s Winbond flash (assets partition only).\n\n'
                       '1. The current device contents are read and saved as a backup.\n'
                       '2. Your edits are applied on top of the DEVICE contents (not the old dump), so files the '
                       'meter changed (history) are kept.\n'
                       '3. Only changed sectors inside the assets partition are erased, programmed and verified.\n\n'
                       'Do not unplug or switch off the meter until it finishes. Continue?', danger=True):
            return
        self.log('Upload: reading device assets partition for backup and merge…')
        replacements = dict(s.replacements)

        def job():
            return backup.deploy_assets(d, replacements, progress=self._emit_progress, log=None)
        run_task(job, on_done=self._upload_done, on_failed=lambda m: (self.log('upload failed'), error(self, 'Upload', m)))

    def _upload_done(self, result: dict) -> None:
        self.log(f'Backup before writing: {result["backup"]}')
        self.log(f'Wrote and verified {len(result["written"])} sectors: {result["written"][:40]}')
        if result.get('skipped'):
            self.log('Unchanged on device: ' + ', '.join(result['skipped']))
        QMessageBox.information(self, 'Upload', f'{len(result["written"])} sectors written and verified. '
                                'The meter reloads its assets now.')


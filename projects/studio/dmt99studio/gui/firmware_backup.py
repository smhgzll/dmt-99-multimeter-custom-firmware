"""Firmware & Backup: device identity, memory map, backups, release BINs."""
from __future__ import annotations

import hashlib
import json
import shutil
import struct
from pathlib import Path

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (QFileDialog, QGroupBox, QHBoxLayout, QLabel, QMessageBox, QPlainTextEdit,
                               QPushButton, QSplitter, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget)

from .. import paths
from ..core import backup
from ..core.flashimage import ASSET_FIRST_SECTOR, ASSET_SECTORS, FLASH_SIZE, SECTOR
from ..core.protocol import ProtocolError, Status, crc32
from .common import confirm, error, human_size, run_task
from .state import AppState

V24_SHA256 = None  # computed from the file in the firmware project


def release_rows() -> list[dict]:
    rows = []
    for rel in sorted(paths.FINAL_RELEASES.glob('v*/release.json'), reverse=True):
        try:
            meta = json.loads(rel.read_text())
        except json.JSONDecodeError:
            continue
        bins = sorted(rel.parent.glob('*.bin'))
        if not bins:
            continue
        data = bins[0].read_bytes()
        rows.append({'version': meta.get('version', rel.parent.name[1:]), 'file': str(bins[0]), 'size': len(data),
                     'sha256': hashlib.sha256(data).hexdigest(), 'crc32': crc32(data), 'kind': 'POTAT-99 release'})
    v24 = paths.FIRMWARE_PROJECT / 'DMT-99_V2.4_20240306.bin'
    if v24.exists():
        data = v24.read_bytes()
        rows.append({'version': 'V2.4 (FNIRSI original)', 'file': str(v24), 'size': len(data),
                     'sha256': hashlib.sha256(data).hexdigest(), 'crc32': crc32(data), 'kind': 'rollback'})
    return rows


class FirmwareBackupPage(QWidget):
    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        lay = QVBoxLayout(self)
        split = QSplitter(Qt.Orientation.Horizontal)
        left = QWidget()
        lv = QVBoxLayout(left)
        lv.setContentsMargins(0, 0, 0, 0)
        self.info = QTableWidget(0, 2)
        self.info.setHorizontalHeaderLabels(['Property', 'Value'])
        self.info.horizontalHeader().setStretchLastSection(True)
        self.map = QTableWidget(0, 6)
        self.map.setHorizontalHeaderLabels(['Space', 'Region', 'Start', 'Length', 'Access', 'Notes'])
        self.map.horizontalHeader().setStretchLastSection(True)
        for title, w in (('Connected device', self.info), ('Memory map (reported by the device)', self.map)):
            box = QGroupBox(title)
            v = QVBoxLayout(box)
            v.addWidget(w)
            lv.addWidget(box)
        split.addWidget(left)
        right = QWidget()
        rv = QVBoxLayout(right)
        rv.setContentsMargins(0, 0, 0, 0)
        ops = QGroupBox('Backups (address, length, timestamp, SHA-256 and device CRC are stored with each file)')
        ov = QVBoxLayout(ops)
        self.buttons = {}
        for key, label in (('full', 'Entire Winbond (8 MiB)'), ('boot', 'BOOT FAT12 partition'),
                           ('assets', 'Assets FAT12 partition'), ('app', 'Running firmware application area'),
                           ('internal', 'Accessible internal Flash (probe 4 KiB blocks)'),
                           ('bootloader', 'Boot region (vendor bootloader, if readable)'),
                           ('settings', 'Settings page (256 bytes)'), ('config', 'Export configuration (JSON)')):
            b = QPushButton(label)
            b.clicked.connect(lambda _c=False, k=key: self.backup(k))
            ov.addWidget(b)
            self.buttons[key] = b
        self.theme_btn = QPushButton('Theme assets (edited files of the open project)')
        self.theme_btn.clicked.connect(self.backup_theme)
        ov.addWidget(self.theme_btn)
        rv.addWidget(ops)
        rel = QGroupBox('Firmware files (install with the meter\'s own USB bootloader: hold POWER+MENU, connect USB, '
                        'copy the BIN to the disk that appears)')
        relv = QVBoxLayout(rel)
        self.releases = QTableWidget(0, 5)
        self.releases.setHorizontalHeaderLabels(['Version', 'Kind', 'Size', 'SHA-256', 'Matches device'])
        self.releases.horizontalHeader().setStretchLastSection(True)
        relv.addWidget(self.releases)
        rb = QHBoxLayout()
        self.copy_btn = QPushButton('Copy selected BIN to a folder / the bootloader disk…')
        self.copy_btn.clicked.connect(self.copy_release)
        rb.addWidget(self.copy_btn)
        rb.addStretch(1)
        relv.addLayout(rb)
        rv.addWidget(rel)
        self.log = QPlainTextEdit(readOnly=True)
        rv.addWidget(self.log, 1)
        split.addWidget(right)
        split.setSizes([600, 600])
        lay.addWidget(split)
        state.device_changed.connect(self.refresh)
        self.refresh()

    def refresh(self) -> None:
        d = self.state.device
        for b in self.buttons.values():
            b.setEnabled(d is not None)
        self.info.setRowCount(0)
        self.map.setRowCount(0)
        rows = release_rows()
        self.releases.setRowCount(0)
        device_crc = None
        if d:
            try:
                info = d.device_info()
                device_crc = info.get('image_crc32')
                for k, v in info.items():
                    if k in ('capabilities',):
                        continue
                    if isinstance(v, int) and k in ('load_address', 'jedec', 'chip_id', 'image_crc32'):
                        v = f'0x{v:08x}'
                    self._row(self.info, [k, str(v)])
                notes = {'vendor-bootloader': 'FNIRSI USB bootloader; never written',
                         'settings-page': 'written only by the firmware at power-off',
                         'assets-fat12': 'the only region Studio may write',
                         'code-flash-window': 'reads beyond the nominal size are not guaranteed to be real flash'}
                for r in d.memory_map():
                    access = ('read' if r['readable'] else 'no access') + (' + write' if r['writable'] else '') + \
                             (' (fault-guarded)' if r['guarded'] else '')
                    self._row(self.map, [r['space'], r['name'], f'0x{r["start"]:08x}', human_size(r['length']),
                                         access, notes.get(r['name'], '')])
            except Exception as ex:  # noqa: BLE001
                self.log.appendPlainText(f'device info failed: {ex}')
        for r in rows:
            match = '' if device_crc is None else ('yes (CRC32)' if r['crc32'] == device_crc else 'no')
            self._row(self.releases, [r['version'], r['kind'], human_size(r['size']), r['sha256'][:16] + '…', match],
                      data=r)
        self.releases.resizeColumnsToContents()

    @staticmethod
    def _row(table: QTableWidget, values: list[str], data=None) -> None:
        r = table.rowCount()
        table.insertRow(r)
        for c, v in enumerate(values):
            item = QTableWidgetItem(v)
            if data is not None and c == 0:
                item.setData(Qt.ItemDataRole.UserRole, data)
            table.setItem(r, c, item)

    def _done(self, rec) -> None:
        if isinstance(rec, list):
            for r in rec:
                self._done(r)
            return
        if isinstance(rec, str):
            self.log.appendPlainText(rec)
            return
        v = {True: 'verified against device CRC', False: 'CRC MISMATCH', None: 'no device CRC available'}[rec.verified]
        self.log.appendPlainText(f'{rec.meta["kind"]}: {rec.path.name} {rec.meta["length"]} bytes '
                                 f'sha256 {rec.sha256[:16]}… {v}')

    def backup(self, kind: str) -> None:
        d = self.state.device
        if not d:
            return
        src = d.describe()

        def job():
            if kind == 'full':
                data = d.read_spi(0, FLASH_SIZE)
                return backup.save_backup('winbond-full', data, 0, source=src, verify=d.spi_crc(0, FLASH_SIZE))
            if kind in ('boot', 'assets'):
                index, first, count = (1, 63, 400) if kind == 'boot' else (2, ASSET_FIRST_SECTOR, ASSET_SECTORS)
                data = d.read_partition(index, 0, count * SECTOR)
                return backup.save_backup(f'{kind}-fat12', data, first * SECTOR, source=src,
                                          verify=d.spi_crc(first * SECTOR, count * SECTOR))
            if kind == 'app':
                info = d.device_info()
                size, base = info.get('image_size', 0), info.get('load_address', 0x08006400)
                if not size:
                    return 'device did not report an application size'
                data = d.read_internal(base, size)
                ok = crc32(data) == info.get('image_crc32')
                return backup.save_backup('firmware-application', data, base, space='mcu', source=src,
                                          verify=info.get('image_crc32'),
                                          extra={'matches_reported_crc': ok})
            if kind == 'internal':
                ranges: list[tuple[int, bytearray]] = []
                faulted = 0
                for base in range(0x08000000, 0x08080000, 0x1000):
                    try:
                        block = d.read_internal(base, 0x1000)
                    except ProtocolError as ex:
                        if ex.status in (Status.FAULT, Status.RANGE):
                            faulted += 1
                            continue
                        raise
                    if ranges and ranges[-1][0] + len(ranges[-1][1]) == base:
                        ranges[-1][1].extend(block)
                    else:
                        ranges.append((base, bytearray(block)))
                out = [backup.save_backup('internal-flash', bytes(data), base, space='mcu', source=src,
                                          extra={'note': 'contiguous readable range, read by the running firmware'})
                       for base, data in ranges]
                return out + [f'internal Flash: {len(ranges)} readable range(s), {faulted} unreadable 4 KiB '
                              'block(s) skipped (never filled in)']
            if kind == 'bootloader':
                try:
                    data = d.read_boot_region(0, 0x6400)
                except ProtocolError as ex:
                    return f'boot region not readable from the running firmware: {ex}. Nothing was saved.'
                return backup.save_backup('vendor-bootloader', data, 0x08000000, space='mcu', source=src,
                                          extra={'note': 'bytes as read by the running firmware'})
            if kind == 'settings':
                data = d.get_settings()
                return backup.save_backup('settings', data, 0x08040000, space='mcu-ram-image', source=src,
                                          extra={'note': 'RAM image (what the firmware stores at power-off)'})
            if kind == 'config':
                cfg = d.export_configuration()
                data = json.dumps(cfg, indent=2).encode()
                return backup.save_backup('configuration', data, 0, space='json', source=src)
            return None
        self.log.appendPlainText(f'{kind}: started…')
        run_task(job, on_done=self._done, on_failed=lambda m: error(self, 'Backup', m))

    def backup_theme(self) -> None:
        s = self.state.session
        if not s or not s.replacements:
            QMessageBox.information(self, 'Theme assets', 'Open a project with edited assets first.')
            return
        folder = QFileDialog.getExistingDirectory(self, 'Folder for the edited assets')
        if folder:
            files = s.export_assets(folder)
            self.log.appendPlainText(f'{len(files)} edited asset(s) exported to {folder}')

    def copy_release(self) -> None:
        item = self.releases.item(self.releases.currentRow(), 0) if self.releases.currentRow() >= 0 else None
        r = item.data(Qt.ItemDataRole.UserRole) if item else None
        if not r:
            QMessageBox.information(self, 'Copy BIN', 'Select a firmware file first.')
            return
        folder = QFileDialog.getExistingDirectory(self, 'Destination (e.g. the mounted bootloader USB disk)')
        if not folder:
            return
        if not confirm(self, 'Copy firmware file',
                       f'Copy {Path(r["file"]).name} to {folder}?\n\nIf this is the meter\'s bootloader disk, the '
                       'meter installs it when the copy finishes. Studio itself never flashes firmware. Keep the '
                       'V2.4 file at hand for rollback.'):
            return
        target = Path(folder) / Path(r['file']).name
        shutil.copyfile(r['file'], target)
        ok = hashlib.sha256(target.read_bytes()).hexdigest() == r['sha256']
        self.log.appendPlainText(f'copied {target} ({"checksum ok" if ok else "CHECKSUM MISMATCH"})')


_ = struct

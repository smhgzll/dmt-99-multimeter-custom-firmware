"""Live Multimeter: decoded readings from the device, graph, statistics, export."""
from __future__ import annotations

import csv
import json
import math
import time
from collections import deque
from pathlib import Path

from PySide6.QtCore import QPointF, QRectF, Qt, QTimer, Signal
from PySide6.QtGui import QColor, QFont, QPainter, QPainterPath, QPen
from PySide6.QtWidgets import (QCheckBox, QFileDialog, QGridLayout, QGroupBox, QHBoxLayout, QLabel, QLineEdit,
                               QMessageBox, QPushButton, QSpinBox, QVBoxLayout, QWidget)

from .state import AppState


class Graph(QWidget):
    def __init__(self):
        super().__init__()
        self.setMinimumHeight(260)
        self.samples: list[tuple[float, float]] = []
        self.unit = ''
        self.window_s = 60.0

    def set_samples(self, samples, unit: str) -> None:
        self.samples = samples
        self.unit = unit
        self.update()

    def paintEvent(self, event) -> None:  # noqa: N802
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(60, 10, -10, -28)
        p.fillRect(self.rect(), QColor('#151515'))
        p.setPen(QPen(QColor('#444'), 1))
        p.drawRect(r)
        if not self.samples:
            p.setPen(QColor('#888'))
            p.drawText(r, Qt.AlignmentFlag.AlignCenter, 'No readings')
            p.end()
            return
        t_end = self.samples[-1][0]
        t0 = t_end - self.window_s
        pts = [(t, v) for t, v in self.samples if t >= t0]
        vs = [v for _, v in pts]
        lo, hi = min(vs), max(vs)
        if hi - lo < 1e-9:
            lo, hi = lo - 1, hi + 1
        pad = (hi - lo) * 0.08
        lo, hi = lo - pad, hi + pad
        p.setFont(QFont('monospace', 8))
        for i in range(5):
            y = r.bottom() - i * r.height() / 4
            v = lo + i * (hi - lo) / 4
            p.setPen(QPen(QColor('#2a2a2a'), 1))
            p.drawLine(QPointF(r.left(), y), QPointF(r.right(), y))
            p.setPen(QColor('#aaa'))
            p.drawText(QRectF(0, y - 8, 56, 16), Qt.AlignmentFlag.AlignRight, f'{v:.4g}')
        p.drawText(QRectF(r.left(), r.bottom() + 6, r.width(), 16), Qt.AlignmentFlag.AlignLeft,
                   f'-{self.window_s:.0f} s')
        p.drawText(QRectF(r.left(), r.bottom() + 6, r.width(), 16), Qt.AlignmentFlag.AlignRight, f'now  [{self.unit}]')
        path = QPainterPath()
        for i, (t, v) in enumerate(pts):
            x = r.left() + (t - t0) / self.window_s * r.width()
            y = r.bottom() - (v - lo) / (hi - lo) * r.height()
            path.moveTo(x, y) if i == 0 else path.lineTo(x, y)
        p.setPen(QPen(QColor('#ffb300'), 2))
        p.drawPath(path)
        p.end()


class LiveMeterPage(QWidget):
    reading = Signal(dict)

    def __init__(self, state: AppState):
        super().__init__()
        self.state = state
        self.capture: list[dict] = []
        self.capturing = False
        self.series: deque[tuple[float, float]] = deque(maxlen=20000)
        self.series_unit = ''
        self.event_times: deque[float] = deque(maxlen=50)
        self.last: dict = {}
        lay = QVBoxLayout(self)
        self.banner = QLabel()
        self.banner.setWordWrap(True)
        lay.addWidget(self.banner)
        top = QHBoxLayout()
        big = QGroupBox('Reading')
        bv = QVBoxLayout(big)
        self.value = QLabel('—')
        self.value.setStyleSheet('font-size:56px; font-family:monospace; color:#ffb300')
        self.value.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.detail = QLabel('')
        self.detail.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.detail.setStyleSheet('font-size:15px')
        bv.addWidget(self.value)
        bv.addWidget(self.detail)
        top.addWidget(big, 2)
        stats = QGroupBox('Statistics (current unit)')
        g = QGridLayout(stats)
        self.stat_labels = {}
        for i, name in enumerate(('Min', 'Max', 'Average', 'Samples', 'Update rate', 'Frame age')):
            g.addWidget(QLabel(name), i, 0)
            lab = QLabel('—')
            lab.setStyleSheet('font-family:monospace')
            g.addWidget(lab, i, 1)
            self.stat_labels[name] = lab
        top.addWidget(stats, 1)
        lay.addLayout(top)
        self.graph = Graph()
        lay.addWidget(self.graph, 1)
        bar = QHBoxLayout()
        self.stream_btn = QPushButton('Start live stream')
        self.stream_btn.setCheckable(True)
        self.capture_btn = QPushButton('Start capture')
        self.capture_btn.setCheckable(True)
        self.clear_btn = QPushButton('Clear')
        self.csv_btn = QPushButton('Export CSV…')
        self.json_btn = QPushButton('Export JSON…')
        self.window = QSpinBox(minimum=5, maximum=3600, value=60, suffix=' s window')
        for w in (self.stream_btn, self.capture_btn, self.clear_btn, self.csv_btn, self.json_btn, self.window):
            bar.addWidget(w)
        bar.addStretch(1)
        lay.addLayout(bar)
        self.sim_box = QGroupBox('Simulated device input (synthetic packets, not measurements)')
        sh = QHBoxLayout(self.sim_box)
        self.sim_line = QLineEdit('reading 12.34 6')
        self.sim_send = QPushButton('Send to simulator')
        self.sim_demo = QCheckBox('Synthetic sweep')
        sh.addWidget(QLabel('Command'))
        sh.addWidget(self.sim_line, 1)
        sh.addWidget(self.sim_send)
        sh.addWidget(self.sim_demo)
        lay.addWidget(self.sim_box)
        self.reading.connect(self.on_reading)
        self.stream_btn.toggled.connect(self.toggle_stream)
        self.capture_btn.toggled.connect(self.toggle_capture)
        self.clear_btn.clicked.connect(self.clear)
        self.csv_btn.clicked.connect(lambda: self.export('csv'))
        self.json_btn.clicked.connect(lambda: self.export('json'))
        self.window.valueChanged.connect(lambda v: setattr(self.graph, 'window_s', float(v)))
        self.sim_send.clicked.connect(lambda: self._sim(self.sim_line.text()))
        self.demo_timer = QTimer(self)
        self.demo_timer.timeout.connect(self._demo_step)
        self.sim_demo.toggled.connect(lambda on: self.demo_timer.start(200) if on else self.demo_timer.stop())
        self.demo_phase = 0.0
        self.tick = QTimer(self)
        self.tick.timeout.connect(self.refresh_stats)
        self.tick.start(500)
        state.device_changed.connect(self.on_device)
        self.on_device()

    # ------------------------------------------------------------ device
    def on_device(self) -> None:
        d = self.state.device
        self.stream_btn.setEnabled(d is not None)
        self.sim_box.setVisible(bool(d and d.simulated))
        if d is None:
            self.stream_btn.setChecked(False)
            self.banner.setText('<b>No device connected — no readings are shown.</b> Connect a meter running firmware '
                                'with the management link, or start the simulated device (Device menu).')
            self.value.setText('—')
            self.detail.setText('')
        else:
            sim = ' <b style="color:#e57373">SIMULATION: values come from injected packets, not a meter.</b>' \
                if d.simulated else ''
            self.banner.setText(f'Connected: {d.describe()}.{sim}<br><span style="color:gray">Safety: the meter\'s '
                                'input is not known to be isolated from USB ground. Do not measure mains or circuits '
                                'at another potential while the USB cable is connected. The firmware keeps FNIRSI\'s '
                                'charging lock (no measurement while the charging screen is shown).</span>')
            d.event_callback = lambda m: self.reading.emit(m)

    def toggle_stream(self, on: bool) -> None:
        d = self.state.device
        if not d:
            return
        try:
            if on:
                d.start_stream(0)
                self.stream_btn.setText('Stop live stream')
            else:
                d.stop_stream()
                self.stream_btn.setText('Start live stream')
        except Exception as ex:  # noqa: BLE001
            QMessageBox.warning(self, 'Live stream', str(ex))
            self.stream_btn.setChecked(False)

    def _sim(self, line: str) -> None:
        d = self.state.device
        if d and d.simulated and line.strip():
            d.sim.command(line)

    def _demo_step(self) -> None:
        self.demo_phase += 0.15
        v = 12.0 + 0.8 * math.sin(self.demo_phase)
        self._sim(f'reading {v:05.2f} 6')

    # ----------------------------------------------------------- readings
    def on_reading(self, m: dict) -> None:
        now = time.monotonic()
        self.last = m
        if not m.get('available'):
            run = m.get('run_state_name', 'not available')
            why = ('the meter shows the charging screen (measurement blocked while charging)'
                   if m.get('charging') or run == 'charging screen' else f'device state: {run}')
            self.value.setText('—')
            self.detail.setText(f'No reading: {why}')
            return
        if m.get('event_frame'):
            self.detail.setText(f'{m["mode_name"]} (mode event received; waiting for a display frame)')
            return
        self.event_times.append(now)
        flags = [f for f, on in (('HOLD', m['hold']), ('REL', m['relative']), (m['ac_dc'], bool(m['ac_dc'])),
                                 ('OL', m['overload'])) if on]
        text = m['text'].strip() or '----'
        self.value.setText(f'{text} {m["unit_name"]}')
        self.detail.setText(f'{m["mode_name"]}   {"  ".join(flags)}   battery {m["battery"]} %')
        if m['numeric'] and not m['overload']:
            if m['unit_name'] != self.series_unit:
                self.series.clear()
                self.series_unit = m['unit_name']
            self.series.append((now, m['value']))
            self.graph.set_samples(list(self.series), self.series_unit)
        if self.capturing:
            row = dict(m)
            row['host_time'] = time.time()
            self.capture.append(row)

    def refresh_stats(self) -> None:
        vals = [v for _, v in self.series]
        lab = self.stat_labels
        if vals:
            u = self.series_unit
            lab['Min'].setText(f'{min(vals):.6g} {u}')
            lab['Max'].setText(f'{max(vals):.6g} {u}')
            lab['Average'].setText(f'{sum(vals) / len(vals):.6g} {u}')
        else:
            for k in ('Min', 'Max', 'Average'):
                lab[k].setText('—')
        lab['Samples'].setText(f'{len(vals)} shown, {len(self.capture)} captured')
        t = list(self.event_times)
        rate = (len(t) - 1) / (t[-1] - t[0]) if len(t) > 2 and t[-1] > t[0] else 0
        lab['Update rate'].setText(f'{rate:.1f} Hz' if rate else '—')
        lab['Frame age'].setText(f'{self.last.get("frame_age_ms", 0)} ms' if self.last.get('available') else '—')

    def toggle_capture(self, on: bool) -> None:
        self.capturing = on
        self.capture_btn.setText('Stop capture' if on else 'Start capture')
        if on:
            self.capture = []

    def clear(self) -> None:
        self.series.clear()
        self.capture = []
        self.graph.set_samples([], self.series_unit)

    def export(self, kind: str) -> None:
        if not self.capture:
            QMessageBox.information(self, 'Export', 'Nothing captured yet (use Start capture).')
            return
        path, _ = QFileDialog.getSaveFileName(self, 'Export capture', f'dmt99_capture.{kind}')
        if not path:
            return
        self.save_capture(Path(path), kind)

    def save_capture(self, path: Path, kind: str) -> None:
        d = self.state.device
        meta = {'device': d.describe() if d else None, 'simulated': bool(d and d.simulated),
                'exported': time.strftime('%Y-%m-%dT%H:%M:%S%z')}
        if kind == 'json':
            path.write_text(json.dumps({'meta': meta, 'samples': self.capture}, indent=1, ensure_ascii=False))
            return
        cols = ['host_time', 'frame_seq', 'mode', 'mode_name', 'text', 'value', 'unit_name', 'numeric', 'ac_dc',
                'hold', 'relative', 'overload', 'battery', 'frame']
        with open(path, 'w', newline='') as f:
            f.write(f'# DMT99 Studio capture, device: {meta["device"]}, simulated: {meta["simulated"]}\n')
            w = csv.DictWriter(f, fieldnames=cols, extrasaction='ignore')
            w.writeheader()
            for row in self.capture:
                w.writerow(row)

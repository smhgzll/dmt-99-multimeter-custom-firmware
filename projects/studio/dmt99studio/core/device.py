"""Device client for the management protocol.

Transports:
  SerialTransport     a serial port (/dev/ttyACM*, or a pseudo terminal)
  SimulatedDevice     starts dmt99-simulator --device-pty: the firmware's UI
                      code and a copy of a flash image, clearly labelled as a
                      simulation; readings only come from injected packets.

The meter firmware has no transport for this protocol yet (see
FIRMWARE_CHANGES.md), so a real meter will not answer until one exists.
"""
from __future__ import annotations

import json
import queue
import struct
import subprocess
import threading
import time
from pathlib import Path
from typing import Callable

from .. import paths
from .protocol import (ASSET_FIRST_SECTOR, ASSET_SECTORS, PROGRAM_MAX, READ_MAX, Cmd, Decoder, Frame,
                       ProtocolError, Status, crc32, decode_measurement, decode_memory_map, decode_tlv,
                       encode_request)

SECTOR = 4096
Progress = Callable[[int, int], None] | None


class LinkError(Exception):
    pass


class SerialTransport:
    def __init__(self, port: str, timeout: float = 0.05):
        import serial  # pyserial
        self.port = port
        self.ser = serial.Serial(port, 115200, timeout=timeout, write_timeout=5)

    def write(self, data: bytes) -> None:
        self.ser.write(data)

    def read(self) -> bytes:
        return self.ser.read(max(1, self.ser.in_waiting))

    def close(self) -> None:
        self.ser.close()


class SimulatedDevice:
    """A dmt99-simulator process serving the protocol on a pseudo terminal."""

    def __init__(self, flash: Path, mcu_image: Path | None = None):
        paths.ensure_tools()
        args = [str(paths.SIMULATOR), '--flash', str(flash), '--device-pty']
        if mcu_image:
            args += ['--mcu-image', str(mcu_image)]
        self.proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                     text=True, bufsize=1)
        line = self.proc.stdout.readline()
        try:
            self.pty = json.loads(line)['device_pty']
        except (json.JSONDecodeError, KeyError):
            self.proc.kill()
            raise LinkError('simulated device did not start: ' + (line or self.proc.stderr.read()))

    def command(self, line: str) -> None:
        """Inject a simulator command (reading, ack, button, charge ...)."""
        self.proc.stdin.write(line.strip() + '\n')
        self.proc.stdin.flush()

    def close(self) -> None:
        try:
            self.command('quit')
            self.proc.wait(timeout=3)
        except (BrokenPipeError, OSError, subprocess.TimeoutExpired):
            self.proc.kill()


class DeviceClient:
    def __init__(self, transport, label: str = '', simulated: bool = False):
        self.t = transport
        self.label = label
        self.simulated = simulated
        self.decoder = Decoder()
        self.seq = 0
        self.lock = threading.Lock()
        self.responses: queue.Queue[Frame] = queue.Queue()
        self.events: queue.Queue[Frame] = queue.Queue()
        self.event_callback: Callable[[dict], None] | None = None
        self.running = True
        self.reader = threading.Thread(target=self._read_loop, daemon=True)
        self.reader.start()
        self.info: dict = {}

    @classmethod
    def open_serial(cls, port: str) -> 'DeviceClient':
        return cls(SerialTransport(port), label=port)

    @classmethod
    def open_simulated(cls, flash: Path, mcu_image: Path | None = None) -> 'DeviceClient':
        sim = SimulatedDevice(flash, mcu_image)
        client = cls(SerialTransport(sim.pty), label=f'simulated device ({sim.pty})', simulated=True)
        client.sim = sim
        return client

    def close(self) -> None:
        self.running = False
        try:
            self.t.close()
        finally:
            if self.simulated:
                self.sim.close()

    def describe(self) -> str:
        i = self.info
        what = f'{i.get("product", "?")} {i.get("version", "")}'.strip()
        return f'{what} via {self.label}' + (' — SIMULATION, not a meter' if self.simulated else '')

    # ----------------------------------------------------------- framing
    def _read_loop(self) -> None:
        while self.running:
            try:
                data = self.t.read()
            except Exception:  # noqa: BLE001 - port closed/unplugged
                if self.running:
                    self.responses.put(Frame(-1, -1, -1, b''))
                return
            if not data:
                continue
            for f in self.decoder.feed(data):
                if f.cmd == Cmd.MEASUREMENT_EVENT:
                    self.events.put(f)
                    if self.event_callback:
                        m = decode_measurement(f.payload)
                        m['available'] = f.status == Status.OK
                        self.event_callback(m)
                else:
                    self.responses.put(f)

    def request(self, cmd: int, payload: bytes = b'', timeout: float = 3.0) -> bytes:
        with self.lock:
            self.seq = (self.seq + 1) & 0xFFFF
            seq = self.seq
            self.t.write(encode_request(cmd, seq, payload))
            deadline = time.monotonic() + timeout
            while True:
                left = deadline - time.monotonic()
                if left <= 0:
                    raise LinkError(f'{Cmd(cmd).name}: no response within {timeout:.1f} s')
                try:
                    f = self.responses.get(timeout=left)
                except queue.Empty:
                    continue
                if f.cmd == -1:
                    raise LinkError('link closed')
                if f.seq != seq or f.cmd != cmd:
                    continue  # stale response from an earlier timed-out request
                if f.status != Status.OK:
                    raise ProtocolError(f.status, cmd)
                return f.payload

    # --------------------------------------------------------- identity
    def ping(self, payload: bytes = b'DMT99') -> bytes:
        return self.request(Cmd.PING, payload)

    def device_info(self) -> dict:
        self.info = decode_tlv(self.request(Cmd.GET_DEVICE_INFO))
        return self.info

    def firmware_version(self) -> dict:
        return decode_tlv(self.request(Cmd.GET_FIRMWARE_VERSION))

    def memory_map(self) -> list[dict]:
        return decode_memory_map(self.request(Cmd.GET_MEMORY_MAP))

    def capabilities(self) -> dict:
        p = self.request(Cmd.GET_CAPABILITIES)
        caps, max_read, max_req, max_prog, proto = struct.unpack_from('<IHHHB', p)
        return {'bits': caps, 'max_read': max_read, 'max_request': max_req, 'max_program': max_prog, 'protocol': proto}

    def spi_id(self) -> dict:
        jedec, cap = struct.unpack('<II', self.request(Cmd.GET_SPI_FLASH_ID))
        return {'jedec': jedec, 'manufacturer': jedec >> 16, 'capacity': cap}

    # -------------------------------------------------------------- SPI
    def read_spi(self, address: int, length: int, progress: Progress = None) -> bytes:
        out = bytearray()
        while len(out) < length:
            n = min(READ_MAX, length - len(out))
            chunk = self.request(Cmd.READ_SPI_FLASH, struct.pack('<IH', address + len(out), n), timeout=5)
            if len(chunk) != n:
                raise LinkError(f'short read at 0x{address + len(out):06x}')
            out += chunk
            if progress:
                progress(len(out), length)
        return bytes(out)

    def read_partition(self, index: int, offset: int, length: int, progress: Progress = None) -> bytes:
        out = bytearray()
        while len(out) < length:
            n = min(READ_MAX, length - len(out))
            out += self.request(Cmd.READ_SPI_PARTITION, struct.pack('<BIH', index, offset + len(out), n), timeout=5)
            if progress:
                progress(len(out), length)
        return bytes(out)

    def spi_crc(self, address: int, length: int) -> int:
        """Device-side CRC32 of a range (done in 64 KiB steps to keep the meter responsive)."""
        crc = 0
        done = 0
        while done < length:
            n = min(0x10000, length - done)
            part, plen = struct.unpack('<II', self.request(Cmd.VERIFY_SPI_DATA, struct.pack('<II', address + done, n),
                                                           timeout=10))
            if plen != n:
                raise LinkError('verify length mismatch')
            crc = _crc32_combine(crc, part, n)
            done += n
        return crc

    def sector_crc(self, sector: int) -> int:
        crc, _ = struct.unpack('<II', self.request(Cmd.VERIFY_SPI_DATA, struct.pack('<II', sector * SECTOR, SECTOR)))
        return crc

    def compare_sectors(self, image: bytes, first: int = 0, count: int | None = None,
                        progress: Progress = None) -> list[int]:
        count = (len(image) // SECTOR - first) if count is None else count
        differing = []
        for i, s in enumerate(range(first, first + count)):
            if self.sector_crc(s) != crc32(bytes(image[s * SECTOR:(s + 1) * SECTOR])):
                differing.append(s)
            if progress:
                progress(i + 1, count)
        return differing

    def write_sectors(self, sectors: dict[int, bytes], progress: Progress = None) -> list[int]:
        """Erase, program and CRC-verify whole 4 KiB sectors of the assets partition."""
        for s, data in sectors.items():
            if not ASSET_FIRST_SECTOR <= s < ASSET_FIRST_SECTOR + ASSET_SECTORS or len(data) != SECTOR:
                raise ValueError(f'sector {s} is outside the assets partition or not 4096 bytes')
        token = self._begin()
        written = []
        try:
            for i, (s, data) in enumerate(sorted(sectors.items())):
                self._write_op(2, token, struct.pack('<H', s))
                for off in range(0, SECTOR, PROGRAM_MAX):
                    page = data[off:off + PROGRAM_MAX]
                    if page != b'\xff' * len(page):
                        self._write_op(3, token, struct.pack('<I', s * SECTOR + off) + page)
                if self.sector_crc(s) != crc32(data):
                    raise ProtocolError(Status.VERIFY, Cmd.WRITE_SPI_ASSET_REGION, f'sector {s}')
                written.append(s)
                if progress:
                    progress(i + 1, len(sectors))
        finally:
            try:
                self._write_op(4, token, b'')
            except (LinkError, ProtocolError):
                pass
        return written

    def _begin(self) -> int:
        p = self.request(Cmd.WRITE_SPI_ASSET_REGION, b'\x01ASET')
        token, first, count = struct.unpack('<IHH', p)
        if (first, count) != (ASSET_FIRST_SECTOR, ASSET_SECTORS):
            self._write_op(4, token, b'')
            raise LinkError(f'device reports assets partition {first}+{count}, expected '
                            f'{ASSET_FIRST_SECTOR}+{ASSET_SECTORS}; refusing to write')
        return token

    def _write_op(self, op: int, token: int, rest: bytes) -> bytes:
        return self.request(Cmd.WRITE_SPI_ASSET_REGION, bytes([op]) + struct.pack('<I', token) + rest, timeout=5)

    # ------------------------------------------------------- measurement
    def measurement(self) -> dict:
        try:
            m = decode_measurement(self.request(Cmd.GET_CURRENT_MEASUREMENT))
            m['available'] = True
        except ProtocolError as ex:
            if ex.status != Status.UNAVAILABLE:
                raise
            m = {'available': False}
        return m

    def start_stream(self, interval_ms: int = 0) -> None:
        self.request(Cmd.START_MEASUREMENT_STREAM, struct.pack('<H', interval_ms))

    def stop_stream(self) -> None:
        self.request(Cmd.STOP_MEASUREMENT_STREAM)

    # ---------------------------------------------------------- settings
    def get_settings(self) -> bytes:
        return self.request(Cmd.GET_SETTINGS)

    def set_setting(self, field: int, value: int) -> None:
        self.request(Cmd.SET_SETTINGS, struct.pack('<BH', field, value))

    # ------------------------------------------------------ MCU memory
    def read_internal(self, address: int, length: int, progress: Progress = None) -> bytes:
        out = bytearray()
        while len(out) < length:
            n = min(READ_MAX, length - len(out))
            out += self.request(Cmd.READ_ACCESSIBLE_INTERNAL_FLASH, struct.pack('<IH', address + len(out), n))
            if progress:
                progress(len(out), length)
        return bytes(out)

    def read_boot_region(self, region: int, length: int, progress: Progress = None) -> bytes:
        out = bytearray()
        while len(out) < length:
            n = min(READ_MAX, length - len(out))
            out += self.request(Cmd.READ_ACCESSIBLE_BOOT_REGION, struct.pack('<BIH', region, len(out), n))
            if progress:
                progress(len(out), length)
        return bytes(out)

    def export_configuration(self) -> dict:
        p = self.request(Cmd.EXPORT_CONFIGURATION)
        out: dict = {}
        i = 0
        while i + 3 <= len(p):
            sid, n = p[i], struct.unpack_from('<H', p, i + 1)[0]
            data = p[i + 3:i + 3 + n]
            i += 3 + n
            if sid == 1:
                out['device'] = decode_tlv(data)
            elif sid == 2:
                out['settings_hex'] = data.hex()
            elif sid == 3:
                out['option_bytes_hex'] = data.hex()
            elif sid == 4:
                out['esig_hex'] = data.hex()
            elif sid == 5:
                out['memory_map'] = decode_memory_map(data)
        return out


def _crc32_combine(crc1: int, crc2: int, len2: int) -> int:
    """zlib crc32_combine (CRC of A+B from crc(A), crc(B), len(B))."""
    if len2 == 0:
        return crc1

    def gf2_times(mat, vec):
        s = 0
        i = 0
        while vec:
            if vec & 1:
                s ^= mat[i]
            vec >>= 1
            i += 1
        return s

    def gf2_square(mat):
        return [gf2_times(mat, mat[n]) for n in range(32)]
    odd = [0xEDB88320] + [1 << n for n in range(31)]
    even = gf2_square(odd)
    odd = gf2_square(even)
    while True:
        even = gf2_square(odd)
        if len2 & 1:
            crc1 = gf2_times(even, crc1)
        len2 >>= 1
        if not len2:
            break
        odd = gf2_square(even)
        if len2 & 1:
            crc1 = gf2_times(odd, crc1)
        len2 >>= 1
        if not len2:
            break
    return crc1 ^ crc2


def list_ports() -> list[dict]:
    try:
        from serial.tools import list_ports as lp
    except ImportError:
        return []
    rows = []
    for p in lp.comports():
        rows.append({'device': p.device, 'description': p.description, 'vid': p.vid, 'pid': p.pid,
                     'product': p.product, 'serial': p.serial_number})
    return rows

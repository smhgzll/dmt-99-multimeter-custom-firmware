"""Frame codec for the POTAT-99 management protocol v1 (DEVICE_PROTOCOL.md)."""
from __future__ import annotations

import struct
import zlib
from dataclasses import dataclass
from enum import IntEnum

VERSION = 1
REQ_MAGIC = b'\xd9\x9a'
RSP_MAGIC = b'\xd9\x9b'
REQUEST_MAX = 512
READ_MAX = 4096
PROGRAM_MAX = 256
RESPONSE_MAX = READ_MAX + 1024
ASSET_FIRST_SECTOR, ASSET_SECTORS = 463, 1585


class Cmd(IntEnum):
    PING = 0x00
    GET_DEVICE_INFO = 0x01
    GET_FIRMWARE_VERSION = 0x02
    GET_MEMORY_MAP = 0x03
    GET_CAPABILITIES = 0x04
    GET_SPI_FLASH_ID = 0x10
    READ_SPI_FLASH = 0x11
    READ_SPI_PARTITION = 0x12
    VERIFY_SPI_DATA = 0x13
    WRITE_SPI_ASSET_REGION = 0x14
    GET_CURRENT_MEASUREMENT = 0x20
    START_MEASUREMENT_STREAM = 0x21
    STOP_MEASUREMENT_STREAM = 0x22
    MEASUREMENT_EVENT = 0x23
    GET_SETTINGS = 0x30
    SET_SETTINGS = 0x31
    READ_ACCESSIBLE_INTERNAL_FLASH = 0x40
    READ_ACCESSIBLE_BOOT_REGION = 0x41
    EXPORT_CONFIGURATION = 0x42


class Status(IntEnum):
    OK = 0
    FRAME = 1
    COMMAND = 2
    ARGUMENT = 3
    RANGE = 4
    DENIED = 5
    BUSY = 6
    IO = 7
    VERIFY = 8
    UNAVAILABLE = 9
    FAULT = 10
    SESSION = 11


STATUS_TEXT = {
    Status.OK: 'ok', Status.FRAME: 'frame error (CRC/length/version)', Status.COMMAND: 'unknown command',
    Status.ARGUMENT: 'malformed arguments', Status.RANGE: 'outside the allowed range',
    Status.DENIED: 'not allowed', Status.BUSY: 'busy (write session open)', Status.IO: 'flash I/O error',
    Status.VERIFY: 'verify failed', Status.UNAVAILABLE: 'not available now', Status.FAULT: 'region not readable',
    Status.SESSION: 'no valid write session',
}


class ProtocolError(Exception):
    def __init__(self, status: int, command: int, detail: str = ''):
        self.status = status
        self.command = command
        name = Cmd(command).name if command in Cmd._value2member_map_ else f'0x{command:02x}'
        text = STATUS_TEXT.get(status, f'status {status}')
        super().__init__(f'{name}: {text}{" - " + detail if detail else ""}')


def crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


def encode_request(cmd: int, seq: int, payload: bytes = b'') -> bytes:
    if len(payload) > REQUEST_MAX:
        raise ValueError('request payload too large')
    head = REQ_MAGIC + struct.pack('<BBHH', VERSION, cmd, seq & 0xFFFF, len(payload))
    body = head + payload
    return body + struct.pack('<I', crc32(body))


def encode_response(cmd: int, seq: int, status: int, payload: bytes = b'') -> bytes:
    """Used by tests and the protocol fixtures (device side layout)."""
    head = RSP_MAGIC + struct.pack('<BBHBH', VERSION, cmd, seq & 0xFFFF, status, len(payload))
    body = head + payload
    return body + struct.pack('<I', crc32(body))


@dataclass
class Frame:
    cmd: int
    seq: int
    status: int
    payload: bytes


class Decoder:
    """Incremental response decoder with resynchronisation on bad frames."""

    def __init__(self):
        self.buf = bytearray()
        self.bad_frames = 0

    def feed(self, data: bytes) -> list[Frame]:
        self.buf += data
        out: list[Frame] = []
        while True:
            i = self.buf.find(RSP_MAGIC)
            if i < 0:
                del self.buf[:max(0, len(self.buf) - 1)]
                return out
            if i:
                del self.buf[:i]
            if len(self.buf) < 9:
                return out
            version, cmd, seq, status, length = struct.unpack_from('<BBHBH', self.buf, 2)
            if version != VERSION or length > RESPONSE_MAX:
                self.bad_frames += 1
                del self.buf[:2]
                continue
            total = 9 + length + 4
            if len(self.buf) < total:
                return out
            body = bytes(self.buf[:9 + length])
            (crc,) = struct.unpack_from('<I', self.buf, 9 + length)
            if crc != crc32(body):
                self.bad_frames += 1
                del self.buf[:2]
                continue
            out.append(Frame(cmd, seq, status, body[9:]))
            del self.buf[:total]


# ---------------------------------------------------------------- payloads
INFO_TAGS = {1: ('product', 's'), 2: ('version', 's'), 3: ('git', 's'), 4: ('build_date', 's'),
             5: ('load_address', 'u'), 6: ('image_size', 'u'), 7: ('image_crc32', 'u'), 8: ('chip_id', 'u'),
             9: ('uid', 'b'), 10: ('flash_kb', 'u'), 11: ('jedec', 'u'), 12: ('spi_capacity', 'u'),
             13: ('protocol', 'u'), 14: ('capabilities', 'u'), 15: ('max_read', 'u'), 16: ('max_request', 'u'),
             17: ('run_state', 'u'), 18: ('clock_source', 'u'), 19: ('uptime_ms', 'u'), 20: ('charging', 'u'),
             21: ('battery', 'u'), 22: ('rdp_protected', 'u')}
CAPABILITIES = ['spi_read', 'spi_write_assets', 'measurement', 'stream', 'settings_get', 'settings_set',
                'internal_read', 'boot_read', 'export']
RUN_STATES = ['running', 'menu', 'editing threshold', 'language selection', 'history browser', 'charging screen',
              'stopped', 'diagnostic']


def decode_tlv(data: bytes) -> dict:
    out: dict = {}
    p = 0
    while p + 2 <= len(data):
        tag, n = data[p], data[p + 1]
        v = data[p + 2:p + 2 + n]
        p += 2 + n
        name, kind = INFO_TAGS.get(tag, (f'tag{tag}', 'b'))
        if kind == 's':
            out[name] = v.decode('utf-8', errors='replace')
        elif kind == 'u':
            out[name] = int.from_bytes(v, 'little')
        else:
            out[name] = v.hex()
    if 'capabilities' in out:
        out['capability_names'] = [n for i, n in enumerate(CAPABILITIES) if out['capabilities'] >> i & 1]
    if 'run_state' in out and out['run_state'] < len(RUN_STATES):
        out['run_state_name'] = RUN_STATES[out['run_state']]
    return out


def decode_memory_map(data: bytes) -> list[dict]:
    rows, p = [], 0
    while p + 11 <= len(data):
        space, flags, start, length, n = struct.unpack_from('<BBIIB', data, p)
        name = data[p + 11:p + 11 + n].decode('ascii', errors='replace')
        p += 11 + n
        rows.append({'space': 'mcu' if space == 0 else 'spi', 'name': name, 'start': start, 'length': length,
                     'readable': bool(flags & 1), 'writable': bool(flags & 2), 'guarded': bool(flags & 4)})
    return rows


UNITS = {6: 'V', 7: 'mV', 8: 'µA', 9: 'mA', 10: 'A', 11: 'Ω', 12: 'kΩ', 13: 'MΩ', 15: 'nF', 16: 'µF', 17: 'mF',
         18: 'Hz', 19: 'kHz', 20: 'MHz', 21: '°C'}


def decode_measurement(data: bytes) -> dict:
    if len(data) < 50:
        return {'available': False}
    seq, age = struct.unpack_from('<II', data, 0)
    (run, view, layout, mode, unit, numeric, hold, relative, ac_dc, status_high, charging, battery) = data[8:20]
    value, rel_value = struct.unpack_from('<ff', data, 20)
    text = data[28:36].split(b'\0')[0].decode('latin-1')
    from .settings import MODE_NAMES
    return {'frame_seq': seq, 'frame_age_ms': age, 'run_state': run,
            'run_state_name': RUN_STATES[run] if run < len(RUN_STATES) else str(run),
            'view': view, 'layout': layout, 'mode': mode, 'mode_name': MODE_NAMES.get(mode, f'0x{mode:02x}'),
            'unit': unit, 'unit_name': UNITS.get(unit, ''), 'numeric': bool(numeric), 'hold': bool(hold),
            'relative': bool(relative), 'ac_dc': {1: 'AC', 2: 'DC'}.get(ac_dc, ''), 'status_high': status_high,
            'charging': bool(charging), 'battery': battery, 'value': value, 'relative_value': rel_value,
            'text': text, 'overload': not numeric and 'L' in text.upper(), 'frame': data[36:49].hex(),
            # The newest frame may be a mode event (xx xx FD ...), not a display reading.
            'event_frame': data[38] == 0xFD}

"""The 256-byte settings page (internal Flash 0x08040000, 128 halfwords).

Layout recovered from V2.4 and kept by the custom firmware:
  h[0]        0 = valid; anything else -> defaults and language selection
  h[9..14]    monitor thresholds (raw)
  h[15]       packed word: bits 0-2 language, 3-4 backlight, 5 Fahrenheit,
              6-7 auto-off, 8-9 view (1 regular, 2 recording, 3 monitor),
              10-11 layout (0 classic, 1 large digits)
  h[16]       history record count
  h[17..76]   30 history records x 2 halfwords
  h[110..113] simple screen (on, digit size, colour indexes) -- 0.6.0 and later
  h[118..119] active theme slot and its check -- 0.4.0 and later
  h[120..127] custom last-mode record ('DM99', CRC32) -- 0.3.1 and later
"""
from __future__ import annotations

import binascii
import struct
from dataclasses import dataclass

LANGUAGES = ['Chinese', 'English', 'German', 'Japanese', 'Portuguese', 'Russian']
VIEWS = {1: 'Regular', 2: 'Recording', 3: 'Monitor'}
LAYOUTS = {0: 'Classic digits', 1: 'Large digits'}
AUTO_OFF = {0: 'Never', 1: '5 min', 2: '15 min', 3: '30 min'}
BACKLIGHT = {0: 'Level 0 (compare 0)', 1: 'Level 1 (compare 50)', 2: 'Level 2 (compare 70)', 3: 'Level 3 (compare 0)'}
SIMPLE_COLORS = ['Black', 'White', 'Grey', 'Dark grey', 'Red', 'Orange', 'Yellow', 'Lime', 'Green', 'Dark green',
                 'Cyan', 'Blue', 'Navy', 'Purple', 'Pink']
DEFAULTS = [0x1, 0, 0, 0, 0, 0, 0, 0, 0, 0x8005, 0xdc, 0x8064, 0x64, 0, 0x64, 0x118, 0] + [0] * 111

# Host mode IDs (AA 55 05 <id> commands, FD events) and the mode artwork the
# firmware draws for them (ui/screen.c names table: cap, r, dc_v, ac_v, ol, hz,
# dc_a, dc_a(-), temp, -, -, ac_a, ac_a(-)).
MODE_NAMES = {
    0x09: 'NCV', 0x0A: 'Capacitance', 0x0B: 'Resistance', 0x0C: 'DC voltage', 0x0D: 'AC voltage',
    0x0E: 'Continuity', 0x0F: 'Frequency', 0x10: 'DC current (A)', 0x11: 'DC current (mA)',
    0x12: 'Temperature', 0x13: 'LIVE wire', 0x14: 'AUTO', 0x15: 'AC current (A)', 0x16: 'AC current (mA)',
}


@dataclass
class Settings:
    halfwords: list[int]

    @classmethod
    def from_bytes(cls, data: bytes) -> 'Settings':
        if len(data) != 256:
            raise ValueError('settings image must be 256 bytes')
        return cls(list(struct.unpack('<128H', data)))

    @classmethod
    def defaults(cls, view=1, layout=0, language=1, valid=True, auto_off=0) -> 'Settings':
        s = cls(list(DEFAULTS))
        if valid:
            s.halfwords[0] = 0
        s.view, s.layout, s.language, s.auto_off = view, layout, language, auto_off
        return s

    def to_bytes(self) -> bytes:
        return struct.pack('<128H', *self.halfwords)

    def _get(self, shift, mask):
        return (self.halfwords[15] >> shift) & mask

    def _set(self, shift, mask, value):
        if not 0 <= value <= mask:
            raise ValueError(f'value {value} out of range')
        self.halfwords[15] = (self.halfwords[15] & ~(mask << shift) & 0xFFFF) | (value << shift)

    valid = property(lambda s: s.halfwords[0] == 0)
    language = property(lambda s: s._get(0, 7), lambda s, v: s._set(0, 7, v))
    backlight = property(lambda s: s._get(3, 3), lambda s, v: s._set(3, 3, v))
    fahrenheit = property(lambda s: bool(s._get(5, 1)), lambda s, v: s._set(5, 1, int(bool(v))))
    auto_off = property(lambda s: s._get(6, 3), lambda s, v: s._set(6, 3, v))
    view = property(lambda s: s._get(8, 3), lambda s, v: s._set(8, 3, v))
    layout = property(lambda s: s._get(10, 3), lambda s, v: s._set(10, 3, v))
    history_count = property(lambda s: s.halfwords[16])

    def last_mode(self) -> dict | None:
        b = struct.pack('<8H', *self.halfwords[120:128])
        if b[:5] != b'DM99\x01':
            return None
        crc = struct.unpack_from('<I', b, 12)[0]
        if crc != binascii.crc32(b[:12]):
            return {'valid': False}
        return {'valid': True, 'mode': b[5], 'view': b[6], 'name': MODE_NAMES.get(b[5], f'0x{b[5]:02x}')}

    def simple_screen(self) -> dict | None:
        h = self.halfwords
        a, b, d = h[110], h[111], h[112]
        if (a & 0xFF00) != 0xA500 or h[113] != (a ^ b ^ d ^ 0x5A5A):
            return None
        names = SIMPLE_COLORS
        pick = lambda i: names[i] if i < len(names) else i
        return {'on': bool(a & 1), 'digit_size': ['Small', 'Medium', 'Large', '?'][(a >> 1) & 3],
                'background': pick(b & 0xFF), 'digits': pick(b >> 8), 'text': pick(d & 0xFF), 'accent': pick(d >> 8)}

    def describe(self) -> dict:
        return {
            'valid': self.valid,
            'language': LANGUAGES[self.language] if self.language < len(LANGUAGES) else self.language,
            'view': VIEWS.get(self.view, self.view), 'layout': LAYOUTS.get(self.layout, self.layout),
            'backlight': BACKLIGHT.get(self.backlight), 'temperature_unit': '°F' if self.fahrenheit else '°C',
            'auto_off': AUTO_OFF.get(self.auto_off), 'history_records': self.history_count,
            'thresholds_raw': [f'0x{v:04x}' for v in self.halfwords[9:15]],
            'last_mode': self.last_mode(), 'simple_screen': self.simple_screen(),
            'packed_word': f'0x{self.halfwords[15]:04x}',
        }


def simulator_state(settings: Settings, last_record: bytes = bytes(16)) -> bytes:
    """State container understood by dmt99-simulator --state (format 4)."""
    return b'DMTSIM\x04\x00' + settings.to_bytes() + last_record

"""Batch colour adjustment for theme assets and firmware-drawn colours.

One ColorAdjust (hue shift, saturation and brightness factors, an optional
hue range, a minimum saturation so greys/black/white stay put) is applied to:
  - JPEG assets (re-encoded with the original's size and sampling), and
  - the RGB565 colours the firmware draws itself (classic digits, menu text,
    borders), which become the theme palette (THEME.PAL).
"""
from __future__ import annotations

import colorsys
from dataclasses import dataclass, field

import numpy as np
from PIL import Image

# RGB565 colours the firmware draws itself (from the draw traces of all
# catalog screens and ui/numeric_view.c).
FIRMWARE_COLORS = {
    0xFD40: 'orange: classic-layout digits, minus sign, selected menu text',
    0xFFFF: 'white: menu labels, values, borders',
    0x1C9F: 'blue: menu highlight and selected options',
    0x4228: 'grey: recording-view borders',
}

# Asset groups offered in the batch dialog (assets-volume folder prefixes).
GROUPS = {
    'Large digits (num/)': ('num/',),
    'Mode artwork (mode/)': ('mode/',),
    'Units (units/)': ('units/',),
    'Frames and decorations (motif2/)': ('motif2/',),
    'Menu pictures (menu/)': ('menu/',),
    'History, battery, time (history/, cell/, time/)': ('history/', 'cell/', 'time/'),
    'Root pictures (logo, NCV, gImage, ...)': ('',),
}


@dataclass
class ColorAdjust:
    hue_shift: float = 0.0        # degrees, -180..180
    saturation: float = 1.0       # factor
    brightness: float = 1.0       # factor
    hue_center: float | None = None   # degrees; None = all hues
    hue_width: float = 60.0       # +- degrees around hue_center
    min_saturation: float = 0.15  # pixels below this saturation are left alone
    extra: dict = field(default_factory=dict)

    @property
    def identity(self) -> bool:
        return self.hue_shift == 0 and self.saturation == 1 and self.brightness == 1

    def as_dict(self) -> dict:
        return {'hue_shift': self.hue_shift, 'saturation': self.saturation, 'brightness': self.brightness,
                'hue_center': self.hue_center, 'hue_width': self.hue_width, 'min_saturation': self.min_saturation}


def _weights(h: np.ndarray, s: np.ndarray, adj: ColorAdjust) -> np.ndarray:
    """0..1 strength per pixel: saturation gate and soft hue-range window."""
    w = np.clip((s - adj.min_saturation) / max(0.05, adj.min_saturation * 0.5 + 0.05), 0, 1)
    if adj.hue_center is not None:
        d = np.abs(((h * 360.0 - adj.hue_center) + 180.0) % 360.0 - 180.0)
        edge = max(adj.hue_width, 1.0)
        w = w * np.clip((edge + 15.0 - d) / 15.0, 0, 1)
    return w


def _rgb_to_hsv(rgb: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    mx = np.max(rgb, axis=-1)
    mn = np.min(rgb, axis=-1)
    d = mx - mn
    h = np.zeros_like(mx)
    nz = d > 1e-9
    rc = np.where(nz, (mx - r) / np.where(nz, d, 1), 0)
    gc = np.where(nz, (mx - g) / np.where(nz, d, 1), 0)
    bc = np.where(nz, (mx - b) / np.where(nz, d, 1), 0)
    h = np.where(r == mx, bc - gc, np.where(g == mx, 2.0 + rc - bc, 4.0 + gc - rc))
    h = np.where(nz, (h / 6.0) % 1.0, 0)
    s = np.where(mx > 1e-9, d / np.where(mx > 1e-9, mx, 1), 0)
    return h, s, mx


def _hsv_to_rgb(h: np.ndarray, s: np.ndarray, v: np.ndarray) -> np.ndarray:
    i = np.floor(h * 6.0).astype(int) % 6
    f = h * 6.0 - np.floor(h * 6.0)
    p, q, t = v * (1 - s), v * (1 - s * f), v * (1 - s * (1 - f))
    choices = [(v, t, p), (q, v, p), (p, v, t), (p, q, v), (t, p, v), (v, p, q)]
    out = np.zeros(h.shape + (3,))
    for k, (a, b, c) in enumerate(choices):
        m = i == k
        out[..., 0] = np.where(m, a, out[..., 0])
        out[..., 1] = np.where(m, b, out[..., 1])
        out[..., 2] = np.where(m, c, out[..., 2])
    return out


def adjust_array(rgb: np.ndarray, adj: ColorAdjust) -> np.ndarray:
    """rgb: float array 0..1 (..., 3)."""
    h, s, v = _rgb_to_hsv(rgb)
    w = _weights(h, s, adj)
    h2 = (h + adj.hue_shift / 360.0) % 1.0
    s2 = np.clip(s * adj.saturation, 0, 1)
    v2 = np.clip(v * adj.brightness, 0, 1)
    new = _hsv_to_rgb(h2, s2, v2)
    return rgb * (1 - w[..., None]) + new * w[..., None]


def adjust_image(image: Image.Image, adj: ColorAdjust) -> Image.Image:
    rgb = np.asarray(image.convert('RGB'), dtype=np.float64) / 255.0
    out = adjust_array(rgb, adj)
    return Image.fromarray(np.clip(out * 255.0 + 0.5, 0, 255).astype(np.uint8), 'RGB')


def rgb565_to_rgb(c: int) -> tuple[int, int, int]:
    r, g, b = c >> 11, (c >> 5) & 63, c & 31
    return (r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)


def rgb_to_rgb565(r: int, g: int, b: int) -> int:
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def adjust_rgb565(c: int, adj: ColorAdjust) -> int:
    rgb = np.array([[rgb565_to_rgb(c)]], dtype=np.float64) / 255.0
    out = adjust_array(rgb, adj)[0, 0]
    return rgb_to_rgb565(*[int(min(255, max(0, round(x * 255)))) for x in out])


def palette_for(adj: ColorAdjust, colors=FIRMWARE_COLORS) -> dict[int, int]:
    """Firmware colour -> adjusted colour, only for colours that change."""
    out = {}
    for c in colors:
        n = adjust_rgb565(c, adj)
        if n != c:
            out[c] = n
    return out


def hue_of(rgb: tuple[int, int, int]) -> float:
    h, _, _ = colorsys.rgb_to_hsv(*(x / 255 for x in rgb))
    return h * 360.0

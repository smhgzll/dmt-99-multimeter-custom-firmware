# Test results

Run on 2026-10-18 on Nobara Linux. Environment: Python 3.12.15, PySide6 6.12.0, Pillow 13.0.0,
pyserial 3.5. Qt ran offscreen.

```
QT_QPA_PLATFORM=offscreen .venv/bin/python -m pytest -q
41 passed in 22 s
```

| File | Tests | Covers |
|---|---|---|
| `test_flash_image.py` | 11 | See below |
| `test_assets_and_project.py` | 7 | See below |
| `test_device_protocol.py` | 10 | See below |
| `test_real_captures.py` | 1 | See below |
| `test_gui.py` | 3 | See below |
| `test_themes.py` | 7 | See below |
| `test_simple_screen.py` | 2 | See below |

**`test_flash_image.py`**
- Reading the original dump; MBR and both FAT12 volumes.
- Firmware path resolution, including the logo volume quirk.
- Same-size, larger and smaller replacement, with sector-level change checks.
- BOOT volume refused; file creation; out-of-space refusal.
- The original dump is never overwritten; save and reload round trip; sector patch format.

**`test_assets_and_project.py`**
- Originals pass the device decoder.
- PNG with alpha converts to a device JPEG, checked by decoded colour.
- Progressive, grayscale, wrong-size and non-JPEG files are rejected.
- A screen preview reflects a replaced asset, and click-to-asset hit-testing works.
- All 30 catalog screens render, and every visible asset exists.
- Undo, redo and restore give back the original bytes.
- Theme project round trip, including deletion of removed assets and export.

**`test_device_protocol.py`**
- Codec resync and CRC.
- Identity and memory map.
- Reads equal the image (flash, partition, CRC, sector compare).
- Corrupt frame recovery; range and command errors.
- Write rules, all refused: MBR, BOOT, beyond the chip, program without erase, wrong token, second
  session, page crossing, other sector, closed session.
- Sector write and verify, with the original image file unchanged.
- End-to-end asset deployment: backup, merge, validate, write, read back, a no-op second run, restore.
- Measurement unavailable before data and while charging; stream events.
- Settings get and set with validation.
- Internal reads return only real data (FAULT otherwise); configuration export.

**`test_real_captures.py`**
- Five real resistance captures from the meter's measurement IC are decoded by the firmware parser in the
  simulated device.
- 4 of 5 match the value the user read on the screen exactly.
- 1 kΩ: the stored excerpt holds 1.000 kΩ frames, while the user read 1.001 kΩ. The full capture contains
  both values.
- All frames are within two display steps, and the units match.

**`test_gui.py`**
- Main window starts and opens the dump.
- Screen map builds; clicking the preview selects the asset.
- Replace, preview, difference view, undo/redo, project save, diff view, gallery.
- Live meter against the simulated device: nothing shown while disconnected; stream; statistics; CSV/JSON
  export with the simulation flag; firmware and settings pages.
- Batch colour dialog: the orange → green preset, a live preview, and Apply recolouring the large digits
  and units plus the `#fd40` palette entry. One undo reverts the batch and redo brings it back.

**`test_themes.py`** (THEME.PKG, run with the firmware's own installer in the simulator)
- Package round trip; bad paths and non-JPEG files are refused.
- Install into `themes/T01`; the package is deleted from the BOOT volume; originals stay byte-identical;
  the assets volume validates.
- A second theme goes to T02 and stays active over power-off and power-on (checked on the screen).
- Deleting the active theme in the meter's theme menu (Menu ▸ About ▸ ENTER, ENTER, OK) removes its folder.
- Deleting the last theme brings the free clusters back to the original count and removes `themes/`.
- Damaged packages and packages with unknown files change nothing and are deleted.
- Not enough space: refused, the volume stays consistent.
- Palette-only theme (`THEME.PAL`): installed with an empty image list. The classic-layout digits, which
  are firmware ROM glyphs, are drawn in the new colour and no longer orange.

**`test_simple_screen.py`** (firmware 0.6.0, simulator)
- Switched on in the meter's Display menu (Menu ▸ About ▸ ENTER, OK). The digit colour is changed with OK.
- The Regular view shows the configured colours and no original artwork, and stays that way over a power
  cycle.
- The settings are stored in halfwords 110..113. With the screen off, the original view is unchanged.

## Firmware project

| Check | Result |
|---|---|
| `make -C reconstructed all check simulator firmware-all` (before changes) | pass |
| `make -C reconstructed check` (after) | pass: host tests, last-mode profile tests, 7 simulator integration tests |
| `make -C reconstructed studio-tools` | builds `dmt99-simulator`, `dmt99-simulator-release`, `dmt99-jpeg-check` |
| `make -C reconstructed all check studio-tools firmware-all` (0.4.0) | pass; STACK PASS for every image |
| `tools/test_update_image.py --version 0.6.0` (all 15 scenarios + themes + simple screen) | **OVERALL PASS** |
| `tools/test_update_image.py --version 0.4.0` (all 15 scenarios) | **OVERALL PASS**; details below |
| Parity image vs the previous build | identical except the version string and the git hash |
| Release 0.4.0 size | 134704 B (81.5 % of 192 KiB) |
| Stack bound | 5486 B of 6144 B (658 B margin) |
| Static RAM | `.bss` +96 B; about 1.3 KB free before the stack |

`test_update_image.py --version 0.4.0` results:
- Format: both images pass.
- Bootloader hand-over: pass.
- Last mode: pass.
- **Themes:** the real release image, running in the RV32 emulator with a flash that holds `THEME.PKG`:
  - installs it into `themes/T01`;
  - draws the themed artwork;
  - deletes the package and keeps the original file.

  The parity image ignores the package.
- Release vs parity, 15 scenarios: identical frames and UART commands outside the About box. The About
  page differs, as intended (version label and theme hint).

## Not verified

- **Real hardware.** Nothing here touched a real meter. The online functions were exercised against the
  simulated device, because the firmware has no transport for the protocol.
- **Visual rendering on the panel.** Previews are the simulator's framebuffer of the same UI code. Panel
  colour and timing on the device were not compared.
- **JPEG decoder resources.** The device decoder check runs the same TJpgDec source and pool size on the
  host, not on the MCU. Decoding time and stack use on the MCU were not measured for new images.

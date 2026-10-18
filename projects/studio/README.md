# DMT99 Studio

This application lives under `projects/studio/` in the consolidated repository.
It locates firmware and ASIC sources within the same repository; old sibling
folders are unnecessary. Current guides: [setup/build](../../docs/BUILDING.md),
[themes](../../docs/THEMES.md), [Windows installation](../../docs/UPDATING.md).
Real USB management/live-data transport is unfinished; the simulated device
is not a physical meter.

DMT99 Studio is a desktop tool for the FNIRSI DMT99 running POTAT-99 firmware. It is a theme editor, a
flash manager and a companion app. It works on the existing firmware project
(`../../reconstructed` in this monorepo) and does not copy it:

- **Screen previews.** The firmware's own UI code renders them, using the project's native simulator.
- **Image checks.** Images are checked with the firmware's own JPEG decoder build: TJpgDec R0.01b with the
  same 7 KiB pool as the meter.
- **Device protocol.** The protocol logic is C code in the firmware project. Here it is served by a
  simulated device.

## Start

```bash
cd projects/studio
./dmt99-studio                 # creates .venv on first start, then opens the GUI
./dmt99-studio /path/to/flash.bin
```

The launcher installs PySide6, Pillow and pyserial into `.venv`. The simulator and JPEG checker are built
from the firmware project on first use; you can also build them yourself with
`make -C ../../reconstructed studio-tools` from this folder.

To start, choose **File ▸ Open original dump**. It loads `../../artifacts/flash/original/dmt99_flash_1.bin`,
which is never written.

## No images at all: the simple screen (firmware 0.6.0)

You can use the meter's own settings instead of editing images. Open **Menu ▸ About ▸ ENTER ▸ Display**:

1. Switch **Simple screen** on.
2. Pick the background, digit, text and accent colours (15 colours) and the digit size (Small, Medium,
   Large).

The Regular view becomes a plain screen in those colours. No Studio and no THEME.PKG are needed.

## Putting a theme on the meter (firmware 0.4.0 or later)

1. Edit images in the **Theme Editor**, then choose **File ▸ Export THEME.PKG for the meter** and give the
   theme a name.
2. Switch the meter off, hold **POWER + MENU** and connect USB. Copy `THEME.PKG` to the disk that appears,
   eject it and disconnect.
3. Switch the meter on. It installs and verifies the theme, activates it and deletes the package.
4. **Menu ▸ About ▸ ENTER** lists the themes:
   - OK: use the selected theme (or "Original").
   - ENTER: delete it (asks first).
   - BACK/MENU: leave.

Every theme is stored in its own folder (`themes/Tnn`) and contains only the changed images. The original
assets are never modified. Deleting a theme removes all of its files.

**Batch colour** (button above the asset tree, or Edit ▸ Batch colour adjust, Ctrl+B) recolours whole groups
of images (large digits, mode artwork, units, frames, menu pictures, …) from one set of controls, with a live
before/after preview of the current screen. The controls are:
- hue shift, saturation and brightness;
- optionally, only colours near a chosen hue, with greys left alone.

The same change can be applied to the colours the firmware draws itself (classic-layout digits, menu text,
borders). Those go into the theme as a palette, which needs firmware 0.5.0. One Undo reverts the whole batch.

## What works

| Section | Offline (no meter) | Online |
|---|---|---|
| **Theme Editor** | Asset tree, search, gallery, previews, metadata; 30 firmware-rendered screens in both layouts and both languages, with a click-to-asset map; replace from PNG/JPEG/…; baked-in text tool; device-decoder check; undo/redo; before/after/difference view; batch import; restore; export; theme projects; **THEME.PKG export** | — |
| **Flash Manager** | MBR, both FAT12 volumes, file tree with cluster offsets, validation, export of files and partitions, working-copy save, diff report and sector patch | Full 8 MiB backup, sector compare, asset upload limited to the assets partition with backup, merge, verify and restore |
| **Live Multimeter** | — | Live reading, mode, unit, AC/DC, HOLD/REL/OL, graph, min/max/average, update rate, capture, CSV/JSON export |
| **Firmware & Backup** | Release list (`final/`, V2.4 rollback) with SHA-256; copy a BIN to the bootloader disk | Device identity, memory map, backups with metadata (Winbond, partitions, application area, internal Flash probe, boot region, settings, configuration) |
| **Device Settings** | Decode and edit 256-byte settings images | Read, and apply documented fields |

## What is not done

- **Online functions on the real meter (USB link).** These are not available yet. The meter firmware has no transport
  for the management protocol (`FIRMWARE_CHANGES.md`). Online pages work against the simulated device
  (**Device ▸ Start simulated device**), which is labelled as a simulation everywhere.
- **Moving or recolouring firmware-drawn elements.** Elements drawn by firmware code (menu texts, digits in
  the classic layout) cannot be moved, resized or recoloured. Their positions are fixed in firmware; the
  editor marks them as firmware-drawn. Anything inside a JPEG can be changed, including baked-in text.
- **Theme limits.** A theme replaces images and, with firmware 0.5.0, recolours firmware-drawn colours.
  Positions, fonts and text are fixed. One package must fit the free space of the bootloader disk (about
  1.5 MB).
- **Live readings and online pages without a USB link.** These need the firmware USB link, which the user
  is adding.

## Documents

- [THEME_EDITOR.md](THEME_EDITOR.md): screen map, asset rules, projects
- [FLASH_MANAGER.md](FLASH_MANAGER.md): image layout, safety rules, upload procedure
- [DEVICE_PROTOCOL.md](DEVICE_PROTOCOL.md): framing, commands, simulated device
- [FIRMWARE_CHANGES.md](FIRMWARE_CHANGES.md): what changed in the firmware project, what is reused
- [TEST_RESULTS.md](TEST_RESULTS.md): test runs and their limits

## Tests

```bash
QT_QPA_PLATFORM=offscreen .venv/bin/python -m pytest -q
```

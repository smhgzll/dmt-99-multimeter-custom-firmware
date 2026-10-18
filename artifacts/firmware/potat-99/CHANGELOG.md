# Changelog

All notable changes to the DMT99 reconstructed firmware.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/);
versions follow [Semantic Versioning 2.0.0](https://semver.org/).

## [0.6.0] — 2026-10-18

### Added
- **Simple screen.** An image-free measurement screen for the Regular view,
  drawn only with fills, the ROM font and the classic digit glyphs:
  - top: mode name and battery;
  - centre: large value;
  - below: the unit (with Ω, µ and ° glyphs);
  - bottom: AC/DC, HOLD, REL and AUTO tags.

  NCV shows level bars and LIVE shows a LIVE indicator. Only changed digits
  are redrawn.
- **On-meter settings.** Menu → About → ENTER now opens **Display**:
  - Simple screen On/Off;
  - background, digit, text and accent colour (15 colours);
  - digit size (Small 1.0×, Medium 1.3×, Large 1.55×).

  OK steps to the next value and ENTER to the previous one. The values are
  stored in settings halfwords 110..113 with the normal power-off save. The
  theme list follows below the settings on the same screen, and the About hint
  now reads `ENTER: display`.

### Notes
- The simple screen replaces only the Regular view. Recording, Monitor and
  the menus are unchanged. It is off by default.

## [0.5.1] — 2026-10-18

### Fixed
- Theme packages may contain the original file names with `~`
  (`history/~a.jpg`, `~v.jpg`, `~ma.jpg.jpg`, ...). 0.4.0/0.5.0 rejected such
  packages with "Package contains unknown files".

## [0.5.0] — 2026-10-18

### Added
- **Theme colour palette.** A `THEME.PKG` can carry `THEME.PAL`: up to 16
  RGB565 colour pairs that recolour what the firmware draws itself:
  - classic-layout digits and the minus sign (orange `#fd40`);
  - menu text and highlights (white, blue, orange);
  - borders.

  The palette is stored in `THEME.INF` (format v2) and applied while the
  theme is active. JPEG artwork is not affected by it. A theme may consist of
  only a palette.
- **Older themes.** Themes installed by 0.4.0 (`THEME.INF` v1, no palette)
  keep working.

### Not verified
- Not yet run on a physical DMT99 (RV32 emulator with the release image, and
  the simulator).

## [0.4.0] — 2026-10-18

### Added
- **Installable themes.** DMT99 Studio exports a `THEME.PKG`. Copy it to the
  bootloader's USB disk (POWER+MENU, the same disk used for firmware updates).
  At the next normal start the firmware does the following:
  - checks the package: CRC, the format, that every file replaces an existing
    original `.jpg`, and that there is enough free space;
  - copies the images into a new folder `themes/Tnn` on the assets volume and
    reads every file back to verify it;
  - activates the new theme and deletes the package from the disk.

  A rejected package is deleted too, with a message on screen. Several themes
  can be installed, and the original assets are never modified.
- **Theme manager:** Menu → About → ENTER lists the original theme and every
  installed theme, with its size and the free space.
  - OK uses the selected theme.
  - ENTER deletes it, after an OK/BACK confirmation. Its folder is removed
    completely, and the last deletion also removes the empty `themes` folder.
  - The original theme cannot be deleted.
- **Persistence.** The selected theme is remembered in settings halfwords
  118/119, stored with the settings at power-off. A factory reset returns to the
  original theme and keeps the installed ones.
- The About page shows the hint `ENTER: themes`.

### Changed
- While a theme is active, every asset is looked up in the theme folder first
  (gated by a 512-bit Bloom filter from `THEME.INF`); otherwise the original is
  drawn. With no theme active, drawing is unchanged.

### Not verified
- Not yet run on a physical DMT99. The installer was exercised on the RV32
  emulator with the real release image and in the simulator.
- **Unknown bootloader behaviour with non-firmware files.** It is not known
  whether the bootloader accepts a non-firmware file on its disk. FNIRSI's own
  `LOGO.jpg` feature suggests it does. If the meter does not start normally
  after copying `THEME.PKG`, enter the update mode again and copy the firmware
  BIN.

## [0.3.1] — 2026-10-18

### Fixed
- Last measurement mode was not restored on the meter (reported after the
  first hardware test of 0.3.0). The record now lives in the unused tail of the
  V2.4 settings page (halfwords 120..127), which is written with the settings
  at power-off, instead of the separate page 0x08040100 that 0.3.0 only used
  when it read as erased. The mode in use at power-off is saved directly,
  without waiting for the measurement IC's acknowledgement.

## [0.3.0] — 2026-10-18

### Added
- The last measurement mode is remembered: the mode acknowledged by the
  measurement IC is written at power-off (with the settings) to internal Flash
  0x08040100, the page after the V2.4 settings page, and selected again at the
  next start. The page is only used if it is erased or already holds this
  record; foreign content is never overwritten. V2.4 ignores the page.
- About page shows `POTAT-99 V0.3.0`.

### Not verified
- Not yet run on a physical DMT99 (emulator only).

## [0.2.0] — 2026-10-18

First published release, as a USB update file for the meter's own bootloader.

### Added
- USB update image: raw application for 0x08006400 with the V2.4 update-file
  header (jump, 12 nops, ebreak, vector table at +0x38). The bootloader is kept.
- About page shows `DMT-99 V0.2.0` instead of `DMT-99 V2.4`.
- Start-up clears interrupt state the bootloader may leave (PFIC enables,
  pending bits, fast-vector slots, SysTick) before initialising.
- `make release`, which checks the image format and publishes `final/vX.Y.Z/`
  with checksums and a release record.

### Behaviour
- Feature parity target: the V2.4 application (all views, units, themes,
  NCV/LIVE, menus, threshold editor, history, settings, reset, charging,
  first boot, low battery, auto-off). Identical to V2.4 on the emulated board in
  16 whole-image scenarios. Internal deviations are listed in
  `docs/FIRMWARE_RELEASE.md`.

### Not verified
- Not yet run on a physical DMT99. The bootloader's file-name rules and checks
  are unknown.

## 0.1.0-rc1 — 2026-10-17 (internal, not published)

Standalone image for 0x08000000 that would replace the bootloader (needs RDP
removal). Superseded by the USB update format; still built as
`reconstructed/build/firmware/dmt99-reconstructed.bin`.

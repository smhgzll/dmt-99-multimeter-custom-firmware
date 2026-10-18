# DMT99 reconstructed firmware — releases

Each `vX.Y.Z/` folder is one published release
([Semantic Versioning 2.0.0](https://semver.org/)). Published folders are
never changed.

| Version | File to copy | Status |
|---|---|---|
| [0.6.0](v0.6.0/) | `v0.6.0/DMT-99_V0.6.0_20261018.bin` | image-free **simple screen**; colours and digit size set on the meter (Menu → About → ENTER); emulator-verified, **not yet run on a physical meter** |
| [0.5.1](v0.5.1/) | `v0.5.1/DMT-99_V0.5.1_20261018.bin` | accepts theme files with `~` in their names (history icons); otherwise as 0.5.0; emulator-verified, **not yet run on a physical meter** |
| [0.5.0](v0.5.0/) | `v0.5.0/DMT-99_V0.5.0_20261018.bin` | theme colour palette (classic digits, menu text) on top of 0.4.0; emulator-verified, **not yet run on a physical meter** |
| [0.4.0](v0.4.0/) | `v0.4.0/DMT-99_V0.4.0_20261018.bin` | installable themes (`THEME.PKG`) and a theme manager (About → ENTER); emulator-verified, **not yet run on a physical meter** |
| [0.3.1](v0.3.1/) | `v0.3.1/DMT-99_V0.3.1_20261018.bin` | fixes last-mode memory on the meter; About shows `POTAT-99 V0.3.1`; runs on the meter |
| [0.3.0](v0.3.0/) | `v0.3.0/DMT-99_V0.3.0_20261018.bin` | remembers the last measurement mode, About shows `POTAT-99 V0.3.0`; emulator-verified, **not yet run on a physical meter** |
| [0.2.0](v0.2.0/) | `v0.2.0/DMT-99_V0.2.0_20261018.bin` | first release; emulator-verified, **not yet run on a physical meter** |

Changes per version: [CHANGELOG.md](CHANGELOG.md). Technical details:
[docs/FIRMWARE_RELEASE.md](../docs/FIRMWARE_RELEASE.md).

## What this file is

`DMT-99_V<version>_<date>.bin` is a USB update file in the same format as the
manufacturer's `DMT-99_V2.4_20240306.bin`: a raw application for Code Flash
0x08006400 with the same header and vector-table position. The meter's own
bootloader installs it. The bootloader (0x08000000–0x080063FF) is not part of
the file and is not modified, so you can always go back to V2.4 the same way.

Functionally it is the V2.4 application rebuilt from reconstructed C sources.
The visible difference is the About page label, `DMT-99 V0.2.0` instead of
`DMT-99 V2.4`, so you can see which firmware is installed. The other
deliberate differences are internal robustness changes, listed in
[docs/FIRMWARE_RELEASE.md](../docs/FIRMWARE_RELEASE.md#deliberate-differences).

## Before you update

- Put the W25Q64JV flash chip and the panel back. The USB disk the
  bootloader shows is presumably the FAT12 "BOOT" volume on the W25Q64JV, and
  the firmware needs its resources. Check that the meter works normally with
  V2.4.
- Keep `DMT-99_V2.4_20240306.bin` (in the repository root, SHA-256
  `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`) for rollback.
- Use a charged battery so the update is not interrupted.
- Check the file: `cd final/v0.6.0 && sha256sum -c SHA256SUMS`.

## Update

1. Switch the meter off.
2. Enter the update mode as in FNIRSI's update procedure: hold **POWER + MENU**,
   then connect the USB cable. A FAT12 disk appears on the computer.
3. Copy **only** the BIN of the newest version (for example
   `DMT-99_V0.6.0_20261018.bin` from `final/v0.6.0/`) to that disk.
   Do not copy the `debug/` folder or other files.
4. Finish exactly as the FNIRSI procedure says (eject the disk, unplug, restart).
5. Check the result. The splash screen appears, then the view that was active
   before; your settings are kept, because the settings page format is the same.
   **Menu → About shows `POTAT-99 V0.6.0`** (0.2.0 shows `DMT-99 V0.2.0`).

## Simple screen (0.6.0 and later): no images needed

**Menu → About → ENTER** opens **Display**:

1. Select **Simple screen** and press OK to switch it on.
2. Choose the background, digit, text and accent colours and the digit size.
   OK goes to the next value, ENTER to the previous one.
3. Press MENU or BACK to leave.

The Regular view then becomes a plain screen in those colours. Switch it off
the same way. The settings are kept over power-off.

## Themes (0.4.0 and later; colour palettes need 0.5.0)

1. In DMT99 Studio, edit images and choose **File ▸ Export THEME.PKG for the meter**.
2. Switch the meter off, hold **POWER + MENU**, connect USB, and copy
   `THEME.PKG` to the disk, keeping that name. Eject the disk and disconnect.
3. Switch the meter on normally. It shows "THEME.PKG found", installs and
   verifies the theme, activates it and deletes the package.
4. **Menu → About → ENTER** opens the Display screen; the themes are listed below the simple-screen settings:
   - OK uses a theme (including "Original").
   - ENTER deletes the selected theme, after an OK/BACK confirmation.
   - BACK or MENU leaves the list.

Themes live in `themes/Tnn` on the assets volume next to the untouched original
images. Studio's **Batch colour** tool recolours many images at once and adds
the same change for the firmware-drawn colours (classic digits, menu text) as
a palette; the palette needs firmware 0.5.0. The bootloader disk has about 1.5 MB free, so one package must be
smaller than that. The assets volume has about 3 MB free for all installed
themes together.

## Rollback

Repeat the update with `DMT-99_V2.4_20240306.bin`. The About page then shows
`DMT-99 V2.4` again.

## Known unknowns

The bootloader has never been read (the MCU is read-protected), so these points
are not known:

- whether it requires a specific file name (the release keeps the V2.4 naming
  pattern `DMT-99_V<version>_<date>.bin`);
- whether it checks a checksum, signature or version number;
- which flash pages it erases, and what it does after an interrupted copy.

If it rejects the file, the installed firmware should stay unchanged. If the
meter does not start after the update, enter the update mode again and
install the V2.4 file. Please note what happened (messages, file left on the
disk, screen content). If you have a 3.3 V USB-UART, connect it to PA2 (MCU
pin 16, 115200 8N1): the firmware prints a boot banner and any crash record there.

## Rebuilding

```bash
make -C reconstructed release                    # builds and publishes final/v$(FW_VERSION)/
make -C reconstructed release FW_VERSION=0.2.1   # next version
```

`release.json` records the source commit. Rebuilding that commit with
`FW_GIT=<commit>` reproduces the published BIN.

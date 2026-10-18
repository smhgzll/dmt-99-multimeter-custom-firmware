# Firmware project changes for DMT99 Studio

## Release 0.6.0: simple screen

Published as `final/v0.6.0/DMT-99_V0.6.0_20261018.bin` (SHA-256 `e7381d64…0ed0`, tag `v0.6.0`). `test_update_image.py --version 0.6.0`: OVERALL PASS.

| File | Change |
|---|---|
| `ui/simple_screen.c/.h` | New image-free Regular view and its settings. Colour table, digit glyphs scaled 1.0 / 1.3 / 1.55, custom Ω, µ and ° glyphs, redraw of changed characters only. Settings live in halfwords 110..113. |
| `application/application.c/.h` | When the simple screen is on and the view is Regular, the base, reading, event, screen-restore and battery paths draw the simple screen. Everything else is unchanged. |
| `ui/theme_menu.c`, `ui/theme_ui.h` | About → ENTER opens "Display": the simple-screen settings rows, then the theme list. `dmt_theme_io` gains `simple_get`/`simple_set`. |
| `firmware/main.c`, `simulator/simulator.c` | Simple-screen get/set operations. |
| `tools/test_update_image.py` | Emulator check: the real release image draws the simple screen in the configured colours; the parity image ignores the setting. |

| Release image | 0.5.1 | 0.6.0 |
|---|---|---|
| Image size | 135280 B | 141868 B (85.2 % of 192 KiB) |
| Stack margin | 594 B | 594 B |
| `.bss` | — | +12 B |
| Parity image | — | unchanged |

## Release 0.5.0: theme colour palette

Published as `final/v0.5.0/DMT-99_V0.5.0_20261018.bin` (SHA-256 `3c91119a…0d74`, tag `v0.5.0`). `test_update_image.py --version 0.5.0`: OVERALL PASS (15 scenarios).


| File | Change |
|---|---|
| `ui/theme.c/.h` | Optional `THEME.PAL` package entry (≤16 RGB565 pairs). `THEME.INF` v2 (176 bytes) stores the palette; v1 from 0.4.0 is still read. `theme_color()`. A palette-only theme is allowed. |
| `firmware/main.c` | The canvas pixel and fill callbacks pass colours through the active palette. JPEG output takes the display path and is not recoloured. |
| `simulator/*` | Separate JPEG canvas, recoloured primitive canvas, and `--palette RRRR:RRRR,...` for Studio previews. |
| `tools/test_update_image.py` | The emulator theme check also verifies that the classic digits are recoloured by the palette. |

| Release image | 0.4.0 | 0.5.0 |
|---|---|---|
| Image size | 134704 B | 135272 B (81.8 %) |
| Stack margin | 658 B | 594 B |
| `.bss` | — | +64 B |
| Parity image | — | unchanged (version string and git hash only) |

## Release 0.4.0: installable themes (firmware change)

Published as `final/v0.4.0/DMT-99_V0.4.0_20261018.bin` (SHA-256 `a2bb50f6…782c`, commit `cbc1145`, tag `v0.4.0`).

| File | Change |
|---|---|
| `ui/theme.c/.h` | New. Package validation and installation (CRC, path, JPEG and space checks; write and read back), slot list, info, activation, recursive removal, Bloom-filter overlay lookup, slot stored in settings halfwords 118/119. |
| `ui/theme_boot.c` | New. Start-up: find `0:/THEME.PKG`, read it raw through the BOOT volume's FAT12 chain, install with a progress screen, delete the package, activate the selected slot. |
| `ui/theme_menu.c` | New. Theme manager screen (firmware font only): list, use, delete with confirmation, free space. |
| `ui/theme_ui.h` | New. Host interface (`dmt_theme_io`) and start-up parameters. |
| `firmware/main.c`, `firmware/isr.c` | `image()` first tries the active theme. `theme_boot` runs before `application_init`. Theme operations are provided to the application. The TIM3 emergency power cut waits while a theme is written or deleted. |
| `application/application.c/.h` | Run state `DMT_THEME_MANAGER`. ENTER on the About page opens the manager. Leaving goes through the normal menu exit. Factory reset selects the original theme. |
| `ui/status_view.c` | About page hint `ENTER: themes`. |
| `storage/filesystem.c/.h` | `filesystem_raw_read()` for the package reader. |
| `include/config.h` | `DMT_THEMES` (release builds only). |
| `tools/stack_report.py`, `tools/test_update_image.py`, `tools/theme_fixture.py` | Callback levels for the new indirect calls; an emulator check that the real release image installs a package; About box enlarged for the hint. |

The parity image (V2.4 behaviour) is unchanged: identical to the previous build except the git hash.

| Release image | 0.3.1 | 0.4.0 |
|---|---|---|
| Image size | 124720 B | 134704 B (81.5 % of 192 KiB) |
| Stack bound | 1362 B margin | 658 B margin |
| Static RAM | unchanged | +96 B in `.bss`, about 1.3 KB still free before the stack |

## Host tools (DMT99 Studio)

All additions in this section are host-only and live in `../dmt_99_analysis/reconstructed`.

## Added (host only)

| File | Purpose |
|---|---|
| `simulator/simulator.c/.h` | Optional JSON-lines draw trace. It records every JPEG draw with decoded size and decoder results, and every semantic text/bitmap/fill call of the view layer. It also adds a packet counter. Rendering is unchanged. |
| `simulator/main.c` | New options `--trace FILE`, `--device-pty [--mcu-image BIN]` and commands `trace PATH\|off`, `marker NAME`. Unbuffered stdin in device mode. |
| `simulator/jpeg_check.c` | `dmt99-jpeg-check`: decodes one JPEG with the firmware's TJpgDec build and the same 0x1c00 pool; optionally dumps RGB565. |
| `simulator/device_link.c/.h` | Simulated device: serves the management protocol on a pseudo terminal. Data sources: the in-memory flash copy, the controller state fed by packets, and the application BIN plus settings page. Everything else answers FAULT. |
| `application/mgmt_protocol.c/.h` | Transport-independent protocol logic (DEVICE_PROTOCOL.md). It is linked into the simulator only and is not part of the firmware build. |
| `Makefile` | Target `studio-tools` builds the simulator pair and `dmt99-jpeg-check`. Simulator sources gain `device_link.c` and `mgmt_protocol.c`. |

## Reused unchanged

| Firmware part | How Studio uses it |
|---|---|
| UI renderer: `ui/screen.c`, menu, settings, monitor and graph scenes, `ui/resources.c` | Screen previews and the asset map |
| FatFs R0.14b with the V2.4 volume mapping | Loading assets in previews, so the volume quirk is reproduced exactly |
| TJpgDec R0.01b | Device compatibility checks and the device-accurate preview |
| Controller and parser (`application/control.c`, `measurement/*`) | Measurement records come from the firmware's decoded state; there is no second parser |
| Settings code (`storage/settings.c`) | Settings fields in the simulated device |
| SPI flash driver API (`drivers/spi_flash.c`) | Intended backend for a future firmware transport: erase sector, page program, read |

## Not implemented (deliberately skipped in this session)

- **Firmware transport for the management protocol (USB).** The work was stopped and, at the user's
  request, not resumed. Consequences:
  - firmware 0.3.x does not enumerate as a USB device and does not answer DMT99 Studio;
  - every online function was tested against the simulated device only.
- **Fault-guarded internal Flash reader in firmware.** Not written. `READ_ACCESSIBLE_*` are only answered
  by the simulator, which returns FAULT for anything it has no real data for.
- **Configurable layout, labels and colours in the firmware UI.** Not started. Themes replace images
  (asset-path overlay); firmware-drawn text and positions stay fixed. Theme projects store
  `layout_overrides`, but nothing reads them yet.
- **The USB management link in firmware.** Not done here; the user will add it.

## Findings recorded while doing this work

1. **0.3.0 last-mode failure, root cause from the real UART captures.** The captures in
   `../dmt_99_asic_analysis/docs/ASIC_UART_SPEC.md` show that the measurement IC's mode event starts
   `AA 55 FD`. The 0.3.0 save path required `5A A5 FD` (`application.c`, on the ACK frame), so it never
   matched on hardware. 0.3.1 saves at power-off, which is why it works. The ACK-triggered save is dead
   code on hardware and should either check only byte 2 (as `control_event` does) or be removed.
2. **Mode switch timing.** The same captures show the FD event about 165 ms after the mode command. The
   longer restore delay the user observed at power-on is therefore not the IC's command latency alone; it
   needs a UART capture at boot to explain.
3. **Boot logo quirk.** The firmware draws `"0:/on.jpg"`, but one FATFS object serves both drives and is
   remounted on `1:` right after `0:/LOGO.jpg` is tried. The logo therefore comes from `1:/ON.JPG`.
   `0:/LOGO.jpg` on the BOOT disk is FNIRSI's custom logo override.
4. **Charging lock.** The firmware enters the charging screen whenever PA8 is high. While that screen is
   shown it does not measure, and the screen says "Do not use while charging". Any live-measurement feature
   over USB has to respect this, or make an explicit, documented decision to change it. Whether PA8 means
   "USB power present" or "charging in progress" (CHRG pin) is not known.

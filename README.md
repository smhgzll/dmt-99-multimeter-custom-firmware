# POTAT-99 · FNIRSI DMT99 research and firmware workspace

> **Use at your own risk.** This independent, experimental project is not
> affiliated with, endorsed by, or supported by FNIRSI. Incorrect firmware or
> installation can disable the device, erase data, or produce incorrect
> measurements. To the extent permitted by law, no warranty is provided and
> the contributors accept no liability for resulting damage.
> [Read the full safety and licensing notice](docs/LEGAL_AND_SAFETY.md).

This repository brings together reverse engineering of the DMT99 V2.4
application, the **POTAT-99** C firmware, a desktop theme editor, a small DOOM
port, passive measurement-ASIC UART analysis, and ESP32 RVSWD diagnostics.
The aim is traceable source reconstruction and reproducible builds backed
by the original evidence.

**Start here:** [Build](docs/BUILDING.md) · [Install a BIN / roll back](docs/UPDATING.md)
· [Install themes](docs/THEMES.md) · [Gallery](docs/gallery/README.md)
· [Research and evidence](docs/RESEARCH.md) · [Documentation index](docs/README.md)

## Projects and status

| Component | Source | Status |
|---|---|---|
| POTAT-99 firmware | [reconstructed/](reconstructed/README.md) | Latest preserved release **0.6.0**; original screens, menus, measurement link, history, themes and simple screen; offline/emulator evidence |
| DOOM | [projects/doom/](projects/doom/README.md) | **0.4.0**; small E1M1 raycaster, six weapons, animation, buzzer, god mode and smooth movement; not the complete classic DOOM engine |
| DMT99 Studio | [projects/studio/](projects/studio/README.md) | Theme editor, Flash Manager and firmware previews work; the real-device management/live-data transport is **unfinished** |
| ASIC UART | [projects/asic/](projects/asic/README.md) | 25 physical captures, 24 regression excerpts and an independent C parser/serializer; analog validation is limited |
| ESP32 RVSWD | [projects/rvswd/](projects/rvswd/README.md) | Classic ESP32 diagnostics; RDP is active; **zero bytes** recovered from original MCU Flash; the repeated SRAM sample is only **4 bytes** |

Workspace consolidation is not a new firmware release or hardware
validation. Existing releases are preserved. **STOCK_COMPATIBLE** and
**CUSTOM_DMT99** remain separate; POTAT-99 release features are additionally
enabled through `DMT_RELEASE_FEATURES`. These projects share a repository,
not a single firmware image. The multimeter does not function as a meter
while running the DOOM firmware.

## Gallery

Screens come from the emulator/native C renderer and the actual Studio UI.
They are not device photographs or evidence of physical performance.

| POTAT-99: regular measurement | POTAT-99: recording view |
|---|---|
| ![Regular display](docs/firmware_screens/02-regular-dc-volts.png) | ![Recording graph](docs/firmware_screens/05-record-graph.png) |

| DOOM: gameplay | DOOM: options |
|---|---|
| ![DOOM gameplay](projects/doom/docs/screenshots/v0.4.0/game.png) | ![DOOM options](projects/doom/docs/screenshots/v0.4.0/sound-off.png) |

![Studio theme editor — actual offline Qt interface](docs/gallery/studio-theme-editor.png)

[More screenshots and their provenance](docs/gallery/README.md).

## Device and memory

| Component | Observed interface / build configuration |
|---|---|
| MCU | **WCH CH32V303RCT6**, RISC-V RV32IMAC, soft-float |
| Clock | 8 MHz HSE ×18 PLL = **144 MHz** |
| RAM | New images link within **32 KiB**; option bytes and RDP are not changed |
| Display | **320×240**, GPIOB 16-bit parallel data; PC3/4/5/13 control |
| External Flash | **Winbond W25Q64, 8 MiB**, SPI1: PA4 CS, PA5/6/7 |
| Buttons / power | PC2 and PC8…PC12; PC1 power latch, PA8 charge detection |
| Buzzer / backlight | PA1 TIM2 CH2 / PC6 TIM8 CH1 |
| Measurement ASIC link | USART1 PA9 TX / PA10 RX, **9600 8N1**; SD7502/related SDIC identity remains a hypothesis |
| Debug output | USART2 PA2, **115200 8N1**; the original debug path is TX-oriented |

| Address / sector | Meaning |
|---|---|
| `0x08000000–0x080063FF` | Original 25 KiB bootloader region; no dump is available |
| `0x08006400` | USB application BIN origin; file offset 0 maps here |
| `0x08006438` | Vector table; execution alias `0x6438` |
| `0x08030000` | Current application linker upper bound; 167 KiB available after `0x6400` |
| `0x08040000` | V2.4's observed 256-byte settings/history page; the physical Flash extent still needs verification |
| External sector 0 | MBR; sector units are **4096 bytes** |
| External sectors 63…462 | FAT12 BOOT, USB transfer volume; 400 sectors = 1.5625 MiB |
| External sectors 463…2047 | FAT12 assets; JPEG/font/theme resources |

The roughly 1.5 MiB of USB disk space is not additional MCU application
Flash or RAM. Evidence for the pin assignments and memory layout:
[hardware mapping](docs/HARDWARE_MAPPING.md),
[image format and memory layout](docs/FIRMWARE_RELEASE.md).

## Research process

1. **V2.4 application:** mapped the 135,096-byte BIN to alias `0x6400`,
   decompiled 439 functions, decoded WCH-specific instructions and
   reconstructed address-traceable C implementations.
2. **External Flash:** examined MBR/FAT12 structures, resources and historical
   residue in an 8 MiB dump. Inventoried 299 live files / 297 JPEGs and
   qualified ChaN FatFs and TJpgDec counterparts against binary evidence.
3. **Behavior comparisons:** compared original commands, menu/cache behavior,
   screens, timers and packets with native C tests and RV32 emulation.
4. **Physical UART captures:** passive D0/D2 recordings established that the
   DMA buffer size is not the wire packet length: MCU TX is 10 bytes,
   observed measurement bursts are 12 bytes, and some ASIC event candidates
   are 10 bytes. RX checksums and analog calibration remain unresolved.
5. **MCU debugging:** the connection is two-wire **RVSWD** on PA14/PA13.
   Successful debug diagnostics do not establish complete Flash readout or
   safe programming. Flash recovery under RDP has not succeeded.

[Research overview and links to evidence](docs/RESEARCH.md).
Earlier Phase 3/4 counts describe historical scopes. Different coverage
categories are not added together to claim hardware equivalence or 100% completion.

## Quick build

Linux/WSL builds require GNU make, a C compiler and Python. MCU builds need
RISC-V GCC with RV32IMAC/ILP32 support. The ESP32 adapter separately requires
**ESP-IDF 5.5.5**. [Complete setup, tests and build-to-BIN instructions](docs/BUILDING.md).

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements-dev.txt

make firmware       # reconstructed/build/firmware-update/dmt99-update.bin
make studio-tools   # Studio native renderer and JPEG checker
make asic           # projects/asic/build/libdmt99_asic_rv32.a (not a firmware image)

# DOOM needs separately licensed local game data; the data is excluded from Git.
python3 projects/doom/tools/fetch_wad.py
make doom           # projects/doom/build/dmt99-doom.bin

projects/studio/dmt99-studio
```

Source and small regression excerpts are selected for Git. The manufacturer
BIN, 8 MiB dump, complete captures and DOOM game data remain in the local
[`artifacts/` archive](artifacts/README.md) and are not automatically published.
In a clean clone, place missing inputs in the locations listed in the
[build guide](docs/BUILDING.md#input-files).
The [SHA-256 catalog](artifacts/catalog.json) identifies the archived files.

## BIN installation and rollback

**Use Windows 10/11 for now. The user reported successful BIN copying through
a Windows 11 VM with USB device passthrough.** This does not guarantee every
VM or USB controller will work. FNIRSI's
[current download instructions](https://www.fnirsi.com/pages/manuals-firmwares)
specify Windows 10/11 and warn against macOS. Linux installation is not a
supported route here; Linux/WSL can be used for building.

1. Charge the battery; disconnect probes and debug/analyzer wiring.
2. With the meter off, enter the USB update disk using **POWER + MENU**.
3. Verify SHA-256, then copy **only the intended USB application BIN**.
4. Wait for copying to finish, safely eject, disconnect according to the
   manufacturer's instructions, and check normal startup and the version.
5. To return from DOOM to multimeter operation, use the same process with a
   known-working POTAT-99 application BIN; use the V2.4 BIN for the original application.

**Do not substitute an ESP32 BIN, 8 MiB dump, ELF, standalone image or theme
package for the application BIN.** The image omits the bootloader region;
the actual bootloader erase/check behavior is unavailable, so recovery after
an interruption is not guaranteed.
[Full installation instructions and recovery limits](docs/UPDATING.md).

## Theme installation

For POTAT-99 **≥0.4.0**, edit images in Studio and use **Export THEME.PKG**.
Firmware-drawn color palettes require **≥0.5.0**; the image-free simple screen
requires **≥0.6.0**. Copy `THEME.PKG`, without renaming it, to the Windows USB
update disk. At the next normal boot, the application validates the package,
installs it under `themes/Tnn`, activates it and removes the package.
Use **Menu → About → ENTER → Display** to select or delete a theme.

Original images are preserved; the active theme is searched first.
Installation writes to the assets partition, so keep a Flash backup.
[Preparing themes, size limits and undoing changes](docs/THEMES.md).

## Limitations, risks and licensing

- Custom firmware does not certify measurement accuracy, calibration or
  safety ratings. Do not use it for critical, mains or safety-related measurements.
- USB, ESP32 and logic analyzer connections can create a ground path into
  the measurement circuit. Do not measure energized/mains circuits with debug wiring attached.
- **Removing RDP can cause mass erase:** the original bootloader and settings
  may be lost permanently. Normal USB installation does not remove RDP or change option bytes.
- Bootloader code, the actual USB MSC/update algorithm and the complete
  internal Flash layout are unavailable. No speculative bootloader has been added.
- Studio's real USB management transport is unfinished. Simulated-device
  readings are not physical measurements.
- Code, manufacturer firmware/images, the WCH SDK and DOOM game data have
  separate rights. Consolidation does not place every file under one open-source license.
  [License scope](LICENSE.md) · [Third-party notices](THIRD_PARTY_NOTICES.md).

## References

- [FNIRSI DMT-99 product page](https://www.fnirsi.com/products/dmt-99)
  and [manufacturer manuals/firmware](https://www.fnirsi.com/pages/manuals-firmwares).
- [WCH CH32V303](https://www.wch-ic.com/products/CH32V303.html),
  [official CH32V307 SDK](https://github.com/openwch/ch32v307);
  local versions/hashes are recorded under `references/` and `reconstructed/vendor/`.
- [ChaN FatFs](https://elm-chan.org/fsw/ff/) and [TJpgDec](https://elm-chan.org/fsw/tjpgd/).
- [Artfical/doom-avr](https://github.com/Artfical/doom-avr/) — the DOOM port's upstream,
  pinned at `f584566ec2e18041725e1de5668c6cb1f764ab58`.
- [sigrok](https://sigrok.org/), [Unicorn](https://www.unicorn-engine.org/),
  [Ghidra](https://github.com/NationalSecurityAgency/ghidra).
- [Nicolai Electronics RVSWD](https://github.com/Nicolai-Electronics/esp32-component-rvswd),
  [wch-protocols](https://github.com/ch32-riscv-ug/wch-protocols),
  [Espressif ESP-IDF](https://github.com/espressif/esp-idf).

Consolidation checks, the status of the old folders and publication exclusions:
[WORKSPACE_MIGRATION.md](docs/WORKSPACE_MIGRATION.md).

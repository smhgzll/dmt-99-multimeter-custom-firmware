# Building and testing

Run commands from the repository root unless shown otherwise. Build on
Linux/WSL; **copy USB update files to the device using Windows 10/11**.
None of the build/test commands below programs hardware or transmits to the measurement ASIC.

## Input files

Files with uncertain redistribution rights are excluded from Git. Place
your own backups at these locations; do not overwrite an existing acquisition:

| File | Location | SHA-256 |
|---|---|---|
| Manufacturer V2.4 application | `artifacts/firmware/original/DMT-99_V2.4_20240306.bin` | `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e` |
| W25Q64 dump, 8 MiB | `artifacts/flash/original/dmt99_flash_1.bin` | `fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69` |
| DOOM shareware 1.9 WAD | `artifacts/game-data/doom1.wad` | `1d7d43be501e67d927e415e0b8f3e29c3bf33075e859721816f652a526cac771` |

The original root BIN, `flashdump/`, `final/`, `extracted_flash/` and project
capture paths are **relative compatibility links** into `artifacts/`.
A Linux/WSL Git clone must preserve these links. Do not build with a Windows
Git configuration that substitutes text files for symlinks; use WSL.

Create missing directories with
`mkdir -p artifacts/firmware/original artifacts/flash/original`.
The dump is needed for simulator/Studio resource previews. POTAT-99 C builds
use the recovered MCU ROM tables in `ui/*rom*`. The V2.4 BIN is needed for
image-format and original-code comparisons.

## Tools

- GNU make, GCC/Clang, Python ≥3.10; a supported Qt/Python combination for the GUI.
- RISC-V GCC with `rv32imac_zicsr_zifencei`, `ilp32`, soft-float and
  the required `libgcc`/`libm` multilib. The recorded builds use
  ESP-IDF's riscv32-esp-elf GCC 14.2.0 package.
- `tools/find_riscv_toolchain.py` searches PATH and the current user's ESP
  tools installation. Override with `CROSS=/path/to/riscv32-esp-elf-` if
  necessary; not every RISC-V compiler includes the required multilib.
- Python dependencies:

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements-dev.txt
```

Pinned WCH/FatFs/TJpgDec sources are vendored. ESP-IDF and Python environments
are not copied into the repository. The diagnostic adapter separately needs
**ESP-IDF 5.5.5** and its ESP32/Xtensa toolchain.

## POTAT-99: USB application BIN

```bash
make -C reconstructed -j4 firmware-update
# reconstructed/build/firmware-update/dmt99-update.bin
# ELF/map/listing/size outputs are alongside it; build checks include memory and stack.

make -C reconstructed studio-tools
make -C reconstructed CONFIG=STOCK_COMPATIBLE check
make -C reconstructed CONFIG=CUSTOM_DMT99 check
```

`firmware-update` selects the **0x08006400** application layout and enables
POTAT release features. `CONFIG` selects the stock/custom analysis profile;
release features use the separate `DMT_RELEASE_FEATURES` flag.
Existing releases are immutable. Example for a new version:

```bash
make -C reconstructed release FW_VERSION=0.6.1
# artifacts/firmware/potat-99/v0.6.1/DMT-99_V0.6.1_<date>.bin
```

`make release` refuses to overwrite a version with a different BIN.
To reproduce the 0.6.0 source label, use `FW_VERSION=0.6.0 FW_GIT=61bbf59`;
the same compiler and flags are also required. Consolidation did not change
the 0.6.0 BIN. The root repository has a new Git history; earlier source
milestones remain in the local `.history/` archive and are not automatically
imported into public GitHub history.

`firmware-all`, `firmware` and standalone/restore targets remain for
historical analysis. They are not USB update images: the `0x0` layout risks
losing the bootloader. Normal installation uses only `firmware-update`.

## DOOM

```bash
python3 projects/doom/tools/fetch_wad.py  # separately licensed shareware input
make -C projects/doom -j4 all
# projects/doom/build/dmt99-doom.bin
make -C projects/doom test
make -C projects/doom verify
.venv/bin/python projects/doom/tools/simulator.py
```

Required upstream sources are vendored at a pinned revision; submodule
initialization is unnecessary. WAD and `generated/` are excluded from Git.
Game-data redistribution terms are separate from the AGPL code license.
The desktop simulator does not play the target buzzer sounds.

Before publishing a changed BIN, run the full RV32 test with
`make -C projects/doom emulate EMU_PYTHON=<repo>/.venv/bin/python`.
Then `make -C projects/doom package` checks that **validation belongs to the
current BIN hash** and writes `artifacts/firmware/doom/v<version>/`.
Emulation can take time; it was not rerun during consolidation, which reused
existing screenshots. If source changes, bump `src/version.h`; do not overwrite a release.

## Studio and theme editor

```bash
make studio-tools
projects/studio/dmt99-studio
# The launcher creates projects/studio/.venv when necessary.
# Alternatively, use the shared environment:
cd projects/studio
../../.venv/bin/python -m dmt99studio
QT_QPA_PLATFORM=offscreen ../../.venv/bin/python -m pytest -q
```

Studio locates the firmware sources and `artifacts/flash/original/` dump
within this repository; the old sibling project folders are unnecessary.
`DMT99_FIRMWARE_PROJECT` and `DMT99_ASIC_ANALYSIS` optionally override locations.
Studio does not generate an MCU BIN: it reuses the firmware build and exports
`THEME.PKG`, theme projects or working Flash copies. [Theme guide](THEMES.md).

## ASIC UART / independent C driver

```bash
cd projects/asic
python3 -m unittest discover -s tests -v
make -C firmware
# build/libdmt99_asic_rv32.a
python3 tools/dmt99.py --help
```

This is a **static library**, not a standalone flashable BIN.
See `firmware/INTEGRATION.md` for application binding. Physical passive
captures also need sigrok-cli, analyzer firmware and verified wiring;
`capture`/`probe` are not build/test steps. Do not replace the default
12-byte observed RX profile with the historical 13-byte DMA-buffer size.

The sanitizer test uses Clang when available, otherwise GCC; override with
`SANITIZER_CC`. GCC needs installed `libasan`/`libubsan` runtimes.
Hosts running under ptrace may require `ASAN_OPTIONS=detect_leaks=0`;
address and UB checks remain enabled.

## ESP32 RVSWD diagnostic adapter

This target is for **classic ESP32**, not the earlier ESP32-S3 plan.
After activating ESP-IDF 5.5.5:

```bash
cd projects/rvswd
CC=cc bash tests/run.sh
idf.py -B build-default -D IDF_TARGET=esp32 \
  -D SDKCONFIG=build-default/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' build
# build-default/dmt99_rvswd_diag.bin
# bootloader/bootloader.bin and partition_table/partition-table.bin also target ESP32.
```

Lab/activation/halt/progbuf gates are disabled in the default build.
Archived `*-lab` images are not normal diagnostic releases.
**Never copy the ESP32 BIN to the DMT99 USB disk.** Programming the ESP32 is
a separate hardware operation; this guide stops at building.
[Wiring and diagnostic requirements](../projects/rvswd/DEBUG_CAPABILITIES.md),
[RDP results](../projects/rvswd/FLASH_RECOVERY_REPORT.md).

## Quick verification

```bash
ASAN_OPTIONS=detect_leaks=0 make test
make audit
```

Studio pytest and the ESP-IDF build are separate steps. Tests needing complete
UART captures, a dump or WAD require those inputs; missing-input results
must not be presented as hardware success.
Recorded consolidation results: [WORKSPACE_MIGRATION.md](WORKSPACE_MIGRATION.md).

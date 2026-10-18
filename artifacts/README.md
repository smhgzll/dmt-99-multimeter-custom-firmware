# Local outputs and acquired-data archive

Images and outputs from the five projects are collected here.
**These files are not interchangeable.** Check the catalog's
`flashable_via_meter_usb` classification and the installation guide;
a `.bin` extension alone does not identify the intended target.

| Directory | Content / target |
|---|---|
| `firmware/original/` | Unmodified manufacturer V2.4 USB/MCU application |
| `firmware/potat-99/v*/` | Preserved POTAT-99 USB application releases; `debug/` remains local |
| `firmware/doom/v*/` | Preserved DOOM USB application releases; include game data |
| `firmware/esp32/<old-build>/` | ESP32 adapter images and its bootloader/partition files; **never install on the DMT99**; lab builds are classified separately |
| `firmware/development/` | Historical standalone/readonly/bringup/parity/restore outputs; not normal USB installation releases |
| `flash/original/` | 8 MiB W25Q64 acquisition; **not an MCU dump** |
| `flash/extracted/` | FAT12 files, carved data and volume copies; derived data rather than new acquisitions |
| `captures/asic-captures/` | 25 physical UART sessions, sigrok/logic files, metadata and derived reports |
| `captures/rvswd-captures/`, `captures/rvswd-recovery/` | Debug evidence, failed Flash reads and the 4-byte SRAM sample |
| `themes/` | Theme projects and THEME.PKG outputs; `green-demo` is a palette-only example |
| `game-data/` | Separately licensed DOOM WAD and shareware archive |
| `research-logs/`, `local-environment/` | Historical logs and private host/USB inventory; excluded from publication |
| `validation/previous-build/` | ELF/host libraries and reports retained before cleanup; local inspection only |

`catalog.json` lists local files by size, SHA-256 and purpose. Published
BIN/dump/data contents have not been changed. Former paths such as `final/`,
`flashdump/` and `dist/` are relative compatibility links. Old release
READMEs/reports are retained as historical records; current instructions are in
[UPDATING](../docs/UPDATING.md), [THEMES](../docs/THEMES.md) and
[BUILDING](../docs/BUILDING.md).

The single [8 MiB external Flash dump](flash/original/dmt99_flash_1.bin) and
its [checksum](flash/original/SHA256SUMS) are included for Studio and simulator
resource previews. It is byte-for-byte original acquisition data, not an MCU
Flash backup or a USB update BIN. Its firmware/images retain their existing
third-party rights; inclusion is not a grant of redistribution permission.

Other large/private inputs, firmware BINs, full captures, extracted data and
WAD files remain excluded from Git. The catalog and selected release metadata
are tracked, but a clone is not a complete archive backup.
[Place missing inputs here](../docs/BUILDING.md#input-files).

Do not overwrite an existing release version. New intermediate outputs remain
in each project's `build/` directory; POTAT-99 `make release` and DOOM
`make package` publish verified versions here. Choose this directory when
exporting themes. Review privacy and licensing before publishing newly added data.

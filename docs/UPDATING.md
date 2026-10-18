# USB BIN installation, rollback and limitations

Installation changes the device's program memory.
[Read the risks](LEGAL_AND_SAFETY.md). The bootloader has not been recovered,
so recovery after interruption and acceptance on every board revision are
not guaranteed. Removing RDP or erasing the MCU is not a normal installation step.

## Which file?

| Intended use | Correct file / location |
|---|---|
| Multimeter: POTAT-99 | `artifacts/firmware/potat-99/v0.6.0/DMT-99_V0.6.0_20261018.bin`, or a newer verified USB application release |
| DOOM | `artifacts/firmware/doom/v0.4.0/DMT-99_Doom_V0.4.0_20261018.bin` |
| Return to the manufacturer application | `artifacts/firmware/original/DMT-99_V2.4_20240306.bin` |
| Theme | `THEME.PKG`; not a BIN, [separate process](THEMES.md) |

These are local archive paths; binaries are not automatically included in a
Git clone. Obtain a file with known provenance and hash rather than selecting
an arbitrary BIN. Check its release `release.json`, `SHA256SUMS` and the
[catalog](../artifacts/catalog.json). Windows PowerShell:

```powershell
Get-FileHash .\DMT-99_V0.6.0_20261018.bin -Algorithm SHA256
```

Preserved POTAT-99 0.6.0 SHA-256:
`e7381d64205047d3e43102e690acc0be5663532424f6103665579fd576c50ed0`.
DOOM 0.4.0:
`2622d1bc35b678d74234178b79c922b0523e00d8e490df89d79b1dc253ee5d61`.

**Do not use:** standalone/restore images under `firmware/development/`,
ESP32 adapter BINs, the 8 MiB W25Q64 dump, ELF/map/HEX files or `debug/`
outputs. An external Flash dump is not an MCU update image.

## Windows 10/11 and virtual machines

FNIRSI's [official update page](https://www.fnirsi.com/pages/manuals-firmwares)
specifies Windows 10/11 and warns against updating from macOS.
This project therefore **requires Windows for installation for now**;
copying to BOOT directly from Linux is not a supported route.

The user reported **successful BIN copying through a Windows 11 VM**.
Pass the entire USB device through to the Windows guest, and do not mount
the disk simultaneously on the host. A VM shared folder is not USB device
passthrough. This report does not establish the bootloader's complete
validation/erase behavior or guarantee every VM configuration.

## Installation

1. Charge the battery. Disconnect probes, ESP32 and logic analyzer wiring.
   The original external Flash, panel and bootloader must remain installed.
2. Keep a known-working POTAT-99/V2.4 BIN on the computer for rollback.
   An external Flash backup is not an MCU bootloader backup.
3. With the device off, use **POWER + MENU** to enter USB update mode and
   connect it to Windows. Follow the manufacturer's procedure.
4. **Do not format the disk** or alter its partition table. Copy only the
   chosen application BIN; do not leave multiple firmware files or a theme
   package in the same installation attempt. Avoid unnecessary renaming.
5. Wait for Windows to finish copying, safely eject and complete the
   manufacturer's disconnect/restart procedure. Do not interrupt power.
6. For POTAT-99, check the About version, display/buttons/power behavior and
   then measurements against a safe, low-energy reference. DOOM does not measure.

## Rollback

To remove DOOM, copy a known-working POTAT-99 application BIN using the
**same Windows POWER+MENU process**. Use the V2.4 BIN for the original application.
This route requires a functioning bootloader and volumes; it is not a
guaranteed recovery from every error or interruption.

If a file is rejected or startup fails, formatting the disk, removing RDP,
mass erase or installing a `0x0` standalone image is not an automatic remedy.
Record the error and file state, and check the known-good BIN/hash and USB
procedure. If the original bootloader is lost, this project has no verified
original bootloader restore image.

## What the address checks establish

USB application BINs are checked for **file offset 0 → `0x08006400`**
and vectors at `+0x38`. The first 25 KiB is absent from the file.
This establishes the intended link layout, not which pages the unread
bootloader actually erases. Application settings-page behavior has been
examined separately; a full internal Flash dump is unavailable.

[Engineering details](FIRMWARE_RELEASE.md) · [Dump feasibility](FIRMWARE_DUMP_FEASIBILITY.md).

# FNIRSI DMT99 V2.4 — reconstruction and replacement firmware

This repository turns the analysed V2.4 manufacturer application into a
**CH32V303RCT6 firmware** built only from maintained C, the pinned WCH SDK
drivers, FatFs R0.14b and TJpgDec R0.01b.

**Release: [`final/`](final/README.md)** — `final/v0.2.0/DMT-99_V0.2.0_<date>.bin`
is a USB update file in the same format as the manufacturer's
`DMT-99_V2.4_20240306.bin` (application at 0x08006400, same header and vector
table position). The meter's own bootloader installs it (POWER+MENU, copy the
file to the USB disk) and stays untouched, so V2.4 can be reinstalled the same
way. The About page shows `DMT-99 V0.2.0`.

```bash
make -C reconstructed release          # build, check and publish final/v$(FW_VERSION)/
make -C reconstructed firmware-all     # all images (update, parity twin, standalone variants, restore)
```

The firmware uses the original panel, keys, measurement-IC UART, settings page
(settings are kept across the update) and W25Q64JV resources unchanged, and
fits ≤ 192 KiB code and ≤ 32 KiB SRAM. A standalone image for 0x08000000 is
also built for the case that the bootloader cannot be used; installing that
one needs RDP removal (mass erase) and explicit approval.

**Status: emulator-verified, not hardware-verified.** The bootloader has never
been read, so its file-name rules and checks are unknown. Read
[final/README.md](final/README.md) and
[FIRMWARE_RELEASE.md](docs/FIRMWARE_RELEASE.md) before installing.

Evidence ([FIRMWARE_VERIFICATION.md](docs/FIRMWARE_VERIFICATION.md)):

- Start-up configuration registers, SystemInit sequence and the 76 889 panel
  initialisation words are identical to the V2.4 machine code.
- The update image (parity build), executed in a CH32V303 board emulator from
  0x6400 like V2.4, produces pixel-identical screens and identical UART commands
  to the host simulator in 15 scenarios, and to the **V2.4 image itself run on
  the same emulated board** in 16 scenarios (all views, units, themes, NCV/LIVE,
  menus, editors, history, settings, reset, charging, first boot). The release
  image differs from the parity build only inside the About label, and boots
  identically when the bootloader leaves interrupt state behind.
- Every one of the 439 V2.4 functions has a disposition in
  [firmware_coverage.json](analysis/firmware_coverage.json); see the
  [feature matrix](docs/FEATURE_MATRIX.md). USB updates remain the job of the
  manufacturer bootloader (the V2.4 application has no USB code either).

Host tools remain: the C simulator (`make -C reconstructed simulator`,
`python3 tools/dmt99_simulator.py`), the differential suites under `tools/` and
the emulators (`tools/fw_emulator.py`, `tools/requirements-emulation.txt`).

Background documents: [hardware mapping](docs/HARDWARE_MAPPING.md),
[ASIC protocol](analysis/measurement_asic_protocol.md),
[ASIC control](docs/ASIC_CONTROL_REPORT.md),
[flash forensics](docs/FLASH_DUMP_ANALYSIS.md),
[dump/update feasibility](docs/FIRMWARE_DUMP_FEASIBILITY.md),
[UI recovery](docs/UI_RECOVERY.md), [simulator](docs/OFFLINE_SIMULATOR.md),
[status](docs/RECONSTRUCTION_STATUS.md), [next steps](docs/NEXT_STEPS.md).

The original BIN (SHA-256 `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`),
the flash dump and the 885 baseline evidence files are unchanged. Local Git
history lives in `.history/git` (`tools/milestone log`).

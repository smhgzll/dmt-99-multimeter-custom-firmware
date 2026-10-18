# Research and evidence overview

This page connects the five projects' findings. Raw captures, hypotheses,
simulation and physical-device validation remain separate evidence categories.

## V2.4 to C source

The 135,096-byte V2.4 file maps to physical application origin
`0x08006400`, execution alias `0x6400`. The first 25 KiB is absent.
439 functions were decompiled. Later stages use separate native/SDK/library/
partial/replaced categories. Phase 3/4 counts are historical reports, not
additional coverage to add to the current totals.

- [Architecture](FIRMWARE_ARCHITECTURE.md), [feature matrix](FEATURE_MATRIX.md),
  [firmware verification](FIRMWARE_VERIFICATION.md).
- `analysis/firmware_coverage.json`: per-address dispositions, keeping
  native/library/deviation/partial/replaced/default categories separate.
- `decompiled/`, `disassembly/`, `analysis/instructions.json`: original-code
  evidence; no new full disassembly or decompilation was run during consolidation.
- [Phase 4 continuation](PHASE4_CONTINUATION_REPORT.md),
  [ASIC control report](ASIC_CONTROL_REPORT.md): stage results and limits.

## 8 MiB Winbond Flash

The single available complete external Flash acquisition is
`artifacts/flash/original/dmt99_flash_1.bin`, SHA-256
`fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69`.
No independently acquired second file is available; repeat-read equality is not claimed.

The 4 KiB-sector MBR and two FAT12 volumes yielded 299 live files and
297 JPEGs. Physical extents and hashes are retained for extracted files.
A historical RV32 application body and PC file residue also exist; they
are not identified as the bootloader or current calibration storage.

[Flash analysis](FLASH_DUMP_ANALYSIS.md),
[version comparison](FIRMWARE_VERSION_COMPARISON.md),
[resource architecture](RESOURCE_LOADING_ARCHITECTURE.md),
[upstream matching](LIBRARY_SOURCE_RECOVERY.md).
`artifacts/flash/extracted/` is a derived archive, not another acquisition.

## ASIC: passive UART / logic analyzer

Verified channel wiring: **D0 → MCU PA9 TX**, **D2 → MCU PA10 RX**, shared GND.
Both directions decode as **9600 8N1**. Do not connect analyzer VCC.
USART1 is dedicated to the measurement ASIC. Similarity to published
SD7502/related SDIC protocols is not definitive chip identification.

**Important distinction:** the original firmware DMA buffer is 13 bytes;
observed physical measurement bursts are **12 bytes**.
MCU→ASIC literal commands are **10 bytes**; ASIC event candidates beginning
`AA 55 FD ...` were also observed as **10 bytes**.
This does not establish a unified parser covering every mode.

25 physical sessions and 24 compressed regression excerpts are preserved.
They cover open-input DC/AC current, NCV/LIVE, frequency, temperature without
an external probe, and reported resistance references. Capacitance, voltage
sources and all calibration ranges have not been physically validated.
Checksum hypotheses, ACK causality and unknown command meanings are not invented.

- [Wire specification](../projects/asic/docs/ASIC_UART_SPEC.md)
- [Hardware findings](../projects/asic/docs/HARDWARE_FINDINGS.md)
- [Resistance validation](../projects/asic/docs/RESISTANCE_VALIDATION.md)
- [Capture index](../projects/asic/docs/CAPTURE_INDEX.md)
- [Independent C interface](../projects/asic/firmware/INTEGRATION.md)

Complete captures are local under `artifacts/captures/asic-captures/`;
regression excerpts are under `projects/asic/tests/fixtures/`.
Sample indices and error information are preserved. Host paths in public
metadata were converted to relative representations.

## MCU debug / the connection sometimes called JTAG

The verified adapter is **classic ESP32**: GPIO25 SWCLK → PA14,
GPIO26 SWDIO → PA13, UART0 GPIO1/3 → CP2102 at 115200 8N1.
The target protocol is RVSWD, not ARM SWD or conventional four-wire JTAG.
ESP32-S3 was an earlier plan, not the validated adapter.

Latest physical evidence: RDP is active; running/halted Abstract Access
Memory and Program Buffer Flash/alias reads did not succeed.
**Zero bytes** of original MCU Flash were recovered.
One SRAM word at `0x20000000` returned `00 80 02 02` in two reads;
this is **4 bytes of SRAM**, not a bootloader dump.
`cmderr=3/4` alone does not establish the complete cause.

The default build is diagnostic, with separate lab/activation/halt gates
disabled. The historical programmer reference's presence does not mean the
active build exposes Flash programming/unlock commands.
No physical target commands were issued during consolidation.

[Latest recovery report](../projects/rvswd/FLASH_RECOVERY_REPORT.md),
[diagnostics and wiring evidence](../projects/rvswd/DEBUG_CAPABILITIES.md),
[application export feasibility](FIRMWARE_DUMP_FEASIBILITY.md).
Complete sessions remain local under `artifacts/captures/rvswd-*`.

## Consolidation validation

This reorganization did not change addresses, measurement policy or the
menu state machine. Changes concern project locations, build paths,
release directories and documentation. Full emulator scenarios were not
rerun; existing screenshots were reused, with short native/offscreen Studio captures.

[Recorded build/test results and hash checks](WORKSPACE_MIGRATION.md).

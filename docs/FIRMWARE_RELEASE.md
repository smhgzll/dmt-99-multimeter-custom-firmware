# POTAT-99 USB application image and release layout

The latest preserved release is **0.6.0**. This page describes image
engineering; [BUILDING](BUILDING.md), [UPDATING](UPDATING.md) and
[THEMES](THEMES.md) provide current procedures. Existing version folders
are immutable. Emulator tests do not guarantee the physical updater's erase behavior.

## Inputs and output

Manufacturer `DMT-99_V2.4_20240306.bin` is a raw application:
135,096 bytes, SHA-256
`4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
File offset `0` maps to physical `0x08006400` / execution alias `0x6400`.

`make -C reconstructed firmware-update` selects that application layout.
`tools/update_image_format.py` checks header/vector/layout rules and the
version record. It does not invent a manufacturer signature/CRC scheme.
Bootloader acceptance requires separate hardware evidence.

| Region | Location / bound |
|---|---|
| Missing bootloader | `0x08000000–0x080063FF`; not included in the USB application BIN |
| Header | `0x08006400`: reset jump, 12×nop, ebreak |
| Vectors | `0x08006438`: 104 slots; mtvec alias `0x6438` |
| Code/data load region | Starts at alias `0x6400`, ends below `0x30000`; 167 KiB capacity |
| SRAM | `0x20000000–0x20007FFF`; 32 KiB total, 6144-byte stack reserve |
| V2.4 settings/history access | `0x08040000`, 128 halfwords; the complete physical internal Flash layout remains unverified |

Linker sources: `reconstructed/firmware/dmt99-app.ld`, shared
`dmt99-sections.ld`, and `startup.S`. RDP/option bytes are not written.
Excluding the bootloader from the application file does not independently
prove its actual erase range. No guarantee that the bootloader cannot be erased is made.

## Releases and profiles

`make release FW_VERSION=<new-version>` publishes the validated BIN under
`artifacts/firmware/potat-99/v<version>/`.
`release.json` records version, commit, addresses and hashes.
`SHA256SUMS` identifies local release files, including debug outputs.
A different image cannot overwrite a published version.

| Target/profile | Purpose |
|---|---|
| `firmware-update` | Application at `0x6400`, POTAT release features enabled |
| `firmware-update-parity` | V2.4 label/stock behavior comparisons; not the normal user release |
| `CONFIG=STOCK_COMPATIBLE` | Stock analysis/test profile |
| `CONFIG=CUSTOM_DMT99` | Separate last-mode test profile with an injected backend boundary |
| `DMT_RELEASE_FEATURES` | Enables release last-mode, themes/palette and simple-screen features |
| `firmware`, readonly/bringup/restore standalone | Historical `0x0` analysis images; not normal USB installation files |

A standalone image replaces the bootloader. Opening that route on the
protected MCU risks RDP removal/mass erase and is not recommended as an
ordinary installation or rollback method. The normal guide includes no unprotect/erase step.

## Application behavior and persistence

The firmware uses the original USART1 measurement link, menus, HOLD/REL,
three measurement views, graphs/history, settings, power/charging flow
and resource volumes. The native controller represents blocking loops
through cooperative states; cycle equivalence is not claimed.
Original menu cache/erase behavior and commands have separate differential tests.

Release differences include the About label, reset cleanup of leftover
IRQ/SysTick state, deferred long ISR work, HSI fallback, UART RX snapshots,
compare-before-write NOR and fault records; last-mode/theme/simple-screen
features are optional. The unknown two-key A0 diagnostic command is disabled
during normal startup. See source and [verification](FIRMWARE_VERIFICATION.md).

POTAT-99 0.6.0 settings extensions: halfwords 110…113 for simple screen,
118/119 for active theme, 120…127 for last mode.
These are defined extensions within the observed original 256-byte image,
not confirmation of the complete physical MCU layout.
DOOM does not write this page; its options remain in RAM.

External Flash volumes use 4096-byte sectors: BOOT 63+400, assets 463+1585.
The original shared FatFs mount-object order is preserved.
V2.4 MBR/label writes are skipped when compare-before-write finds identical
content. Theme install/delete writes assets while preserving original images.
Missing/invalid CP936 tables remain a fail-closed filename boundary;
a generic replacement table is not silently installed.

## Validation and open questions

Builds check memory/layout/stack bounds. Native, original-machine-code
and board-emulator evidence is classified in
[FIRMWARE_VERIFICATION](FIRMWARE_VERIFICATION.md) and [FEATURE_MATRIX](FEATURE_MATRIX.md).

Physical UART observations are documented in the
[ASIC wire specification](../projects/asic/docs/ASIC_UART_SPEC.md).
DMA buffer size is not generalized to wire frame length.
Measurement accuracy, analog calibration, panel/interrupt timing, every
command's side effects and physical validation of every release remain incomplete.

Bootloader filename/version/signature checks, erase range, interruption
behavior and the actual USB MSC implementation are unavailable.
The successful Windows VM copy observation is separately recorded in [UPDATING](UPDATING.md).

Returning to V2.4 or an earlier POTAT-99 uses the same Windows USB application
process when the bootloader and volumes still work; recovery is not guaranteed.
[Risk notice](LEGAL_AND_SAFETY.md).

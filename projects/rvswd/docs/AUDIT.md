> Historical report. Current implementation, hardware evidence and safe test commands: [DEBUG_CAPABILITIES.md](../DEBUG_CAPABILITIES.md). Protocol 3 has no dump/reset command and disables state-changing experiments by default.

# Compatibility and safety audit

**Current hardware correction:** esptool physically identified classic
ESP32-D0WD-V3 rev 3.1, not ESP32-S3. The project now targets `esp32`, UART0 GPIO1/3,
and RVSWD GPIO25/26 to avoid possible PSRAM use of GPIO16/17. The source audit below
retains the original S3 investigation as historical context; use
[HARDWARE_SESSION.md](HARDWARE_SESSION.md) for current wiring and commands. The
wire transport is unchanged and CH32V303 hardware operation remains unvalidated.

Audit date: 2026-10-17. Physical target access: none. The exact ESP32-S3 N16R16 board
model/revision was not supplied. No CH32V303 compatibility claim is made.

## References examined

| Reference | Evidence and consequence |
| --- | --- |
| [Nicolai esp32-component-rvswd](https://github.com/Nicolai-Electronics/esp32-component-rvswd) | Local reference HEAD `889892e94885652077c50698c2222a4dd57582a1`. README limits testing/support to CH32V203. `src/rvswd.c` implements a 52-bit short frame, 7-bit address MSB first, read opcode 0, even header/data parity, fixed auxiliary fields, 100-clock synchronization and open-drain SWDIO. GPIO driver calls are portable in principle to S3, but target support is not established by compiling. |
| [wch-protocols link-to-target](https://github.com/ch32-riscv-ug/wch-protocols/blob/main/protocols/link-to-target.ja.md) | Reviewed public main via web, not a pinned local checkout. Identifies CH32V20x/V30x as two-wire RVSWD targets; short-frame captures support 52 bits plus terminal clock. Documents distinct long frames and unresolved electrical limits. V203 idle is LOW clock/HIGH data; target-specific idle and sampling behavior matter. Its verified captures are not measurements of this DMT99. Do not translate ARM SWD ACKs or the long-format status field into short-format auxiliary bits. |
| [ch32fun minichlink](https://github.com/cnlohr/ch32fun/blob/master/minichlink/minichlink.c) | Reviewed public master via web. `DefaultDetermineChipType` reads DMI 0x7f, recognizes bits 31:20 = 0x303 with the V20x/V30x marker, but also halts, writes debug state, executes code and reads additional memory to identify a full part. This diagnostic takes only the direct register hint and does not call or port that setup path. |
| [ch32fun chip table](https://github.com/cnlohr/ch32fun/blob/master/minichlink/chips.c) | Lists CH32V303 and its Flash base, but its family maximum is not evidence of an RCT6 capacity. No capacity is inferred from that table. The user's requested 256 KiB range is reserved for a later verified acquisition step. |
| [cnlohr ESP32-S2 wire implementation](https://github.com/cnlohr/esp32s2-cookbook/blob/master/ch32v003programmer/main/ch32v003_swio.h) | Supplemental upstream programmer used by the minichlink ecosystem. Short reads receive auxiliary triplets and send control pairs 00/10. It samples LOW, unlike Nicolai's HIGH sampling. Its automatic SWIO/RVSWD setup writes debug configuration/DMCONTROL and is excluded. Direct GPIO/assembly timing is not copied to S3; this project uses IDF GPIO and microsecond delays, with selectable sampling for explicit trace comparison. |
| [ESP-IDF v5.5.5 GPIO documentation](https://docs.espressif.com/projects/esp-idf/en/v5.5.5/esp32s3/api-reference/peripherals/gpio.html) | S3 GPIO16/17 support I/O. Flash/PSRAM and boot restrictions do not themselves reserve 16/17, but board connections still need verification. `gpio_reset_pin` enables a pull-up, so this project explicitly uses input mode with pulls disabled. Defaults before app_main cannot be assumed. |
| [Official DevKitC-1 pin table](https://documentation.espressif.com/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) | Confirms an example board exposes IO16/17. This table cannot verify an unspecified N16R16 board. |

Shell GitHub cloning failed because DNS/network access is restricted; the web
tool supplied the remote source inspection. Remote branch contents above were
reviewed as of the audit date, without pretending to have a pinned commit or
complete capture checkout. The local Nicolai reference was left unchanged.

## Findings in the Nicolai code

The programmer example calls `ch32v20x_program`. That path resets/halts, unlocks
Flash, erases/programs and resumes. Its option-byte reader also resets and halts.
Neither is suitable for this diagnostic. The high-level memory-read helper writes
CPU/debug registers and runs a load instruction; it is not a plain register read.
Some failures are ignored, write transactions always return success, and one halt
failure returns boolean false in an enum where zero is success. The bit routines
lack configurable setup/sample delays and the read path uses a signed `1 << 31`.
This project uses unsigned shifts, explicit timing, deadlines and propagated
errors in a separate implementation. [Source](https://github.com/Nicolai-Electronics/esp32-component-rvswd/blob/889892e94885652077c50698c2222a4dd57582a1/src/rvswd_ch32v20x.c).

## Implemented policy

Only DMI addresses `0x7f`, `0x10`, `0x11`, `0x12`, `0x16`, `0x7c` can be read. These
provide the vendor ID hint, DMCONTROL, DMSTATUS, HARTINFO, ABSTRACTCS and CPBR.
DATA0/1 and program-buffer registers are excluded: reads of such registers can
trigger autoexecution in some debug configurations. There is no raw host-selected
register command. The transport always emits opcode 0.

The identification loop requires an expected family hint, active DM, standard
Debug 0.13 version 2, authenticated/nonbusy status, no unavailable/nonexistent
hart indication and no busy abstract command. Identity, HARTINFO, CPBR and low
status flags must agree across samples. These conservative checks may reject a
real chip with variant status behavior; raw evidence is preserved for review.
They are not proof of read-protection state or Flash capacity.

No DMI-write implementation is linked or declared. No memory write/read, CPU
register manipulation, halt/reset/resume, abstract-command execution, arbitrary
program-buffer operation, unlock, erase, option-byte write or programming path
exists in the diagnostic application. ESP-IDF's own SPI Flash functions in the
ELF concern the ESP32's boot/runtime, not the WCH target. The reference programmer
is not an IDF dependency; main sources and component dependencies are explicit.

Unlike common attach implementations, this build does not even set `dmactive`.
If the target needs a debug activation/configuration write before responding,
the probe will report failure. It will not silently loosen the policy. This is
an intentional limit of the identification-first build, not evidence the target
is protected or unsupported.

## Compatibility conclusion

There is a credible protocol basis for testing two-wire short-frame RVSWD on a
CH32V303, and the S3 can compile the GPIO implementation under IDF v5.5.5. Neither
fact establishes working communication, acceptable low-speed limits, correct
board wiring, a safe activation procedure, or permitted Flash access on this meter.
Hardware identification and waveform evidence are required before the memory
stage can be designed, reviewed and enabled in a later revision.

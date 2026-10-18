# DMT99 CH32V303RCT6 debug capabilities

## Latest physical-device result

| Capability | Classification | Evidence |
|---|---|---|
| DMI / DM activation | Supported | 8/8 stable, DMCONTROL=1 |
| Halt | Supported | DMSTATUS=382 observed |
| Resume / running state | Supported | Three running samples after observed halt; final independent8/8 c82 |
| Observed resumeack bits | Unknown | WCH sampled value0; no ACK observation claimed |
| Program Buffer load + SRAM | Supported, bounded sample | 20000000→02028000, two independent reads |
| Halted Flash/prefix/alias load | Blocked, exact cause unproven | 08006400/08000000/00000000/00006400:cmderr3 |
| GPR/Program Buffer/DATA restoration | Supported for these experiments | Readback and unchangedPC checks passed |
| Normal-mode SRAM helper reading Flash | Unknown | Scratch RAM/trap/CPU restoration unverified; not executed |

**New original Flash data: zero bytes.**
SRAM files/hash, experiment table and logs:
[FLASH_RECOVERY_REPORT.md](FLASH_RECOVERY_REPORT.md).
Final DMSTATUS=c82, DMCONTROL=1, ABSTRACTCS=08000002; CPU running,
pins released. No target Flash/option-byte/RDP/WRP programming or erase.

The following sections preserve earlier stages. Their unknown/not-yet-tried
statements must not override the latest table.

> A later authorized experiment observed halt(DMSTATUS382).
> Halted Access Memory at08006400 returned cmderr3, without data.
> Running state followed resume, but resumeack was unobserved.
> Earlier cleanup uncertainty is preserved alongside an independent8/8 running check.
> [Halted session](recovery/20261017-original-halt-memory-03/session.json),
> [final state](recovery/20261017-post-cycle03-identify.json),
> [current recovery report](FLASH_RECOVERY_REPORT.md).
> The ESP32 reporting stack overflow was corrected.
> A separately gated/default-off Program Buffer candidate uses only load/ebreak.

> Earlier2026-10-17 research: “Flash dump blocked” originally meant the
> legacy reader stopped at its RDP precheck, not that Flash returned zeros
> or an alternative read had been attempted.
> SBCS=0 in the latest capability capture.
> At22:59 Turkey time an activation+lab image, with halt/resume disabled,
> was installed on the ESP32. DMCONTROL1 activation succeeded.
> Running Access Memory returned cmderr4 without Flash data.
> [Activation](recovery/20261017-active-lab-activation-01.json),
> [running experiment](recovery/20261017-original-running-memory-01/session.json).
> Existing captures were unchanged.

| Earlier running-stage capability | Classification | Evidence |
|---|---|---|
| DMI / explicit activation | Supported | 8/8 stable, DMCONTROL1readback |
| Running Access Memory02200000 | Blocked, RDP cause unproven | First08006400, ABSTRACTCS08000402,cmderr4 |
| Halted Access Memory/protected Flash | Unknown at that stage | Not yet halted, no Flash data |
| DATA restoration/pin release | Supported in that attempt | DATA0/1readback, DMSTATUSc82,state_uncertainfalse |

The DM remained active with sticky cmderr4; CPU stayed running.
The historical diagnostic-development stage below used offline mocks and
ESP32 builds without new physical target sessions, flashing, patching, halt
or reset. Mock success is not CH32V303 hardware validation.
Analysis sources were used read-only.

## Existing session evidence

A later user-reported summary gave chip30310524, DMSTATUS00000c82,
8/8stable, running/authenticated and DMCONTROL0:
debug_module_inactive. The message supplied no filename/hash, and captures
were not altered. This motivated separately controlled activation.

[captures/dmt99-20261017-031509-dump.json](captures/dmt99-20261017-031509-dump.json)
is an earlier2026-10-17 00:15:09UTC session, unchanged, SHA-256
`eb4dc1e6dadeb3554c9f64efabe87e5627b9dccc34f5f3bb89d393c3366d511c`.

| Field | Recorded value | Interpretation |
|---|---|---|
| ChipID DMI7f | 30310524 | Family clue, not complete package/capacity identification |
| DMCONTROL10 | 00000001 | dmactive1,hart0,readable reset fields0 |
| DMSTATUS11 | 00000c82 | version2,authenticated1,authbusy0,running1,halted0 |
| HARTINFO12 | 00312380 | Raw value retained |
| ABSTRACTCS16 | 08000002 | busy0,cmderr0,datacount2,progbufsize8 |
| CPBR7c | 00000000 | Successful read; zero proves no further capability |
| Identity samples | 8,resultok | 41identity-stage trace entries |
| Earlier OBR | 03fffefe | RDPRTbit1 set,RDP active |
| Earlier result | read_protection_enabled | Both passes0bytes; flash_kib0 means capacity unread |
| Earlier cleanup report | restored/resumed/pins_releasedtrue | Legacy report, not validation of the newer resumeack check |

Both .bin.partial files are zero-length and are not valid dumps.
No data was appended. A later read_protection:unknown field means not
remeasured in that session, not invalidation of the earlier OBR evidence.

An earlier [diagnose capture](captures/diagnose-20261017-025841.json)
has CPBR parity_error; a [later one](captures/test-20261017-030513-diagnose.json)
has chip-ID no_response after three samples.
One good session does not guarantee later transport health; these errors were not labeled RDP.

## Historical capability classification

| Capability | Classification | Evidence/limit at that stage |
|---|---|---|
| DMI identity/DMCONTROL/DMSTATUS | Supported | Eight consistent samples |
| CPU halt | Supported, historical evidence | Legacy OBR/restore flow; no independent halt trace then |
| CPU resume | Supported, historical report | resumedtrue; new resumeack checks then untested |
| x8–x11 read/preservation | Inferred from old flow | OBR preparation/restore succeeded; values absent from capture |
| FullPC/x0–x31snapshot | Unknown | New abstract-register path tested only with mocks |
| SRAM snapshot | Unknown | No SRAM sample then; SBA not yet queried |
| GPIO/USART/SPI snapshot | Unknown | Safe profiles tested only with mocks |
| Flash dump | Blocked | RDP evidence/zero-byte result; command removed |
| Flash/options/RDP/WRP/calibration writes | Blocked by policy | No host command/target-memory-write path |
| Reset | Blocked by policy | No command; reset-containing DMCONTROL values rejected |

## Diagnostic interface

Firmware identity: protocol3,build read-only-diagnostic.
tools/diag.py refuses the legacy flash-reader image.
info must report flash_dump/flash_write/target_memory_write/reset_commandfalse.

| Command | Operation | CPU state |
|---|---|---|
| info | Adapter information, no target signals | Unchanged |
| diagnose/identify | Existing five DMI queries; CPBR optional/separate | No activation or state change |
| capabilities | Stable five-query preflight; activeDM ABSTRACTAUTO/SBCS/final status | Default mask0: **zero DMI writes** |
| capabilities --registers | DPC/x0–x31 only on an already halted hart | Running returns hart_running_no_implicit_halt |
| capabilities --sram/--gpio/--usart/--spi | Explicit fixed snapshots when SBA supports them | No halt/resume/reset |
| halt-cycle | Explicit halt/return-to-running experiment | Disabled by default |
| activate-dm | Separate explicit DM activation | Disabled by default |

info memory_readtrue/dmi_writetrue indicate optional software capabilities,
not a completed target read or hardware grant. Actual results are in snapshots.

Each result has supported/blocked/unknown, reason and failure_class.
Supported means that route succeeded in that session.
Blocked requires concrete authentication/protection/unsupported-protocol evidence.
Untested access, clock-off, running state, transport errors or unexplained
access failures remain unknown.
unsupported_debug_capability blocks this SBA route, not all debug methods
or necessarily SRAM access under RDP.

### Registers and snapshot bounds

--registers uses RV32 Access Register transfer1/write0/postexec0/postincrement0.
PC is halted DPC0x7b1, not a live running-PC sample.
Before each command, halted state and autoexec-off are checked.
CPU registers are not written.
DATA0 is saved, restored and read back.
UnsupportedPC retains stickycmderr and stops without GPR/progbuf fallback.
pc_valid/gpr_valid_mask distinguish missing data from actual zero values.

Memory snapshots require SBCSversion1,32-bit addresses and the needed width.
Automatic read/increment are disabled.
Each allowlisted address triggers one bounded bus read.
Maximum48snapshot values:

| Profile | Addresses | Width/additional gate |
|---|---|---|
| SRAM | 20000000–2000003c,16words/64bytes | Fixed32-bit window |
| RCC masks | 40021018/4002101c | Read before selected peripherals; never written |
| GPIOA/B/C | Bases40010800/40010c00/40011000,+0/+4/+8/+c | CFGLR/CFGHR/INDR/OUTDR; APB2clock enabled |
| USART1/2/3 | Bases40013800/40004400/40004800,+8/+c/+10/+14 | BRR/CTLR1/2/3; correspondingAPBclock |
| SPI1/2/3 | Bases40013000/40003800/40003c00,+0/+4 | CTLR1/2;16-bitSBA support and clock required |

Excluded: GPIO BSHR/BCR/LCKR, USART STATR/DATAR, SPI STATR/DATAR,
FIFOs/data-consuming/read-to-clear/lock-sequence registers.
Disabled clocks are not enabled; blocks are skipped.
Supported applies only to read, clocked blocks; use RCC/address records to identify skips.
Running snapshots are non-atomic.
They do not drive GPIO, transmit UART, initiate SPI or send external-Flash commands.

Snapshots require **debug-register writes** to COMMAND/DATA0 or SBCS/SBADDRESS0,
not target SRAM/MMIO/Flash writes. SBA data is never written.
Configuration restoration is read back, but address/data latches may retain
the last result. Error bits are not cleared.
Read-only refers to target memory/CPU registers, not immutable debug-module latches.

### Explicit activation and halt/resume

CONFIG_DIAG_ALLOW_DM_ACTIVATION and CONFIG_DIAG_ALLOW_HALT_RESUME are
independent default-n gates.
Activation additionally requires --allow-dm-activation and token
EXPLICIT_DM_ACTIVATION.
Halt/resume requires its own build gate, --allow-halt-resume and
EXPLICIT_HALT_RESUME. Activation does not authorize halt.
Both remain disabled in a normal build.

activate-dm first runs stable preflight.
Already-active DM gets reads only.
Inactive DM may receive exactly one DMCONTROL=00000001 write.
No retry or blind cleanup write follows failure.
DMCONTROL/DMSTATUS readback checks authentication and unchanged running/halted state.
Reports separate dmactive_write_attempted,dmcontrol_readback_valid,
dmstatus_readback_valid,dmactive_set,activation_state_uncertain,pins_released.
Lost readback may leave uncertain state: do not automatically retry.
Successful activation intentionally leaves the DM active.

A subsequent separate mask0 capabilities reads ABSTRACTAUTO18/SBCS38 with no writes.
Only advertised SBA enables the fixed snapshot profiles above.
--registers never implicitly halts.
Flash/options/RDP/WRP/calibration addresses are excluded.
This historical diagnostic path added no target-memory/register store or progbuf execution.

When separately preparing an activation-enabled adapter, disconnect DMT99
signals before building/flashing; enable activation only and keep halt disabled.
The following are operator-controlled examples, not automatically executed tests.
Replace port and3300 with actual values:

```sh
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-info-NEW.json info
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-preflight-NEW.json diagnose --voltage-mv 3300 --board-pins-checked --wiring-checked
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-activate-NEW.json activate-dm --voltage-mv 3300 --board-pins-checked --wiring-checked --allow-dm-activation
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-capabilities-NEW.json capabilities --voltage-mv 3300 --board-pins-checked --wiring-checked
```

Inspect each JSON before continuing.
info must show activationtrue/haltfalse and verified ESP32/GPIO.
Preflight expects30310524,authenticated/running,DMCONTROL0.
Activation expects dmactive_settrue,validreadbacks,DMCONTROL1,
running/authenticated and pins_releasedtrue.
Stop on missing/unexpected results rather than retry.
Only clean mask0 results and advertised SBA justify a separately selected snapshot:

```sh
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-sram-NEW.json capabilities --voltage-mv 3300 --board-pins-checked --wiring-checked --sram
```

Do not add halt-cycle/reset/dump, hidden activation after identify or
automatic target tests to this activation-only sequence.

The separately gated halt-cycle requires stable,active,authenticated,running
state. It does not take ownership of or resume an initially halted CPU.
Ownership is recorded before halt request because a partial write may reach
the target. Historical generic halt verification requires any/allhalted;
resume requires any/allrunning **and** any/allresumeack.
Write-only request readback is not ACK; readable selection/reset/active
fields are also checked.

On halt timeout, cleanup clears haltreq and performs bounded resume only
with trustworthy transport and idle abstract engine.
DATA0 restoration is required after register reads.
Parity/GPIO/line/frame errors do not trigger blind writes/retries.
operation_result and cleanup_result are separate.
target_state_uncertain means running cannot be proved; Hi-Z alone is not resume.
No reset,DM cycling,unprotect or progbuf fallback.
Physical disconnection prevents unconditional software recovery guarantees.

### Errors and deadlines

| Result/class | Meaning |
|---|---|
| read_protection_enabled/rdp_blocked | EarlierOBR evidence; not synthesized from generic bus errors |
| authentication_required/authentication_blocked | DMSTATUS gate, not synonymous with RDP |
| parity_error/no_response/line_held_low/gpio_error | Connection failure, not RDP evidence |
| frame_timeout | One synchronization/DMI frame exceeded100ms |
| poll_timeout | Halt/resume/abstract/SBA budget expired; sberror1 is bus timeout |
| abstract_command_error/system_bus_error | Rawcmderr/sberror retained; exception alone does not proveRDP |
| unsupported_debug_capability | MissingSBA/version/width or cmderr2; no fallback |
| disabled_by_policy | State-changing gate closed |
| cleanup_unconfirmed | Separate cleanup/final-state evidence incomplete |

Poll budgets are200ms plus an in-flight frame/scheduler delay.
No unbounded loops/retries.
Half-period250µs,turnaround50µs,eight identity samples.
RTOS yield between frames; UART trace output only after signal release.
Trace capacity1024, explicit overflow/host failure.
Host response deadline60s.
Only passive hello may retry; host timeout does not prove adapter cleanup.

## Historical offline validation

Initial tests passed two legacy C mocks and11Python tests using CC=cc;
inherited gcc-14 was unavailable. Flash cases were synthetic.
The new default suite replaced the legacy Flash-reader mock with diagnostics:
twoC executables/17Python tests initially, later18Python tests/twoC groups.

- Transport:4519GPIO failure positions still release pins.
- Diagnostics:3418GPIO,338partial halt/resume-write and542DMI-parity faults.
- 33register reads,48safe snapshot values,running/halted starts,
  autoexec/clock/width gates, sticky errors/timeouts.
- Halt-timeout cleanup, uncertain resume timeout, no resuming externally halted
  CPUs and release failures.
- Actual CLI/pyserial with syntheticPTY: masks,legacy-image rejection,
  independent gates/tokens and immutable captures.
- Activation: one inactiveDM write,readbacks,zero writes ifalready active,release errors.
- Exclusion of Flash/options/calibration/FIFO/lock addresses and rejection of
  target-memory/progbuf/register/error-clear/reset writes in that diagnostic path.

```bash
cd projects/rvswd
CC=cc bash tests/run.sh
ASAN_OPTIONS=detect_leaks=0 CC=clang SANITIZE=1 bash tests/run.sh
```

GCC sanitizer runtimes were missing; Clang ASan/UBSan passed.
Only LeakSanitizer was disabled under ptrace.
These tests never accessed hardware.

ESP-IDF5.5.5 built build-diagnostic ELF/BIN with both state-change gates off.
Legacy source exists only under RV_LEGACY_READER_OFFLINE_TEST,
not normal linking/UART routing. libmain.a lacked
rv_dump_flash,read_program,write_gpr,rv_crc32.
tests/test_reader.c is not in the default runner; the programmer reference remains excluded.

A later reconfigure/ninja-j2 run passed with18Python/twoC groups.
The adapter BIN was177584bytes,SHA-256
`8e30a26889a3adedfc0100a59e381dbc31d495c214f91965a75aab248ea5cd7c`.
It was not installed/run during that development stage.

Activation-only build preparation, with target signals disconnected:

```bash
cd projects/rvswd
. ${IDF_PATH}/export.sh
idf.py -B build-diagnostic -D IDF_TARGET=esp32 -D SDKCONFIG=build-diagnostic/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' menuconfig
grep -E 'CONFIG_DIAG_ALLOW_(DM_ACTIVATION|HALT_RESUME)' build-diagnostic/sdkconfig
idf.py -B build-diagnostic -D IDF_TARGET=esp32 -D SDKCONFIG=build-diagnostic/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' reconfigure
ninja -C build-diagnostic -j 2
```

Expect activation=y and halt/resume unset.
Only if separately proceeding, flash the **ESP32 adapter** while DMT99 remains disconnected:

```bash
idf.py -B build-diagnostic -D SDKCONFIG=build-diagnostic/sdkconfig -p /dev/ttyUSB0 -b 115200 flash
```

## Physical wiring and diagnostic-only sequence

These examples were not executed during the diagnostic-development stage.
Use protocol3; if the adapter still runs flash-reader, update only the
disconnected ESP32, never route new commands to the legacy image.
No CH32 firmware patch or programming step is involved.

1. Disconnect all measurement circuits/USB charging; switch off while wiring.
   For testing, power DMT99 from its battery and ESP32 from USB.
2. CommonGND→header1,GPIO25→330Ω→header2/PA14/SWCLK,
   GPIO26→330Ω→header3/PA13/SWDIO. Keep verified wiring; GPIO16/17 unused.
3. Target-side pull-ups to target3.3V are required on both signals
   (project wiring4.7kΩ each). Open-drain, internalpullups off.
   Never connect ESP32 3V3/5V/VIN to header4.
4. With both boards on, connect ground before signals. Measure header4–1
   externally:3000–3450mV. Verify25/26/short wiring; close serial monitors.
5. Disconnect target signals before adapter reset/flashing or powering down
   either board. Host avoids intentional DTR/RTS reset, but board/driver
   open behavior still needs physical verification.

Default offline adapter build:

```bash
cd projects/rvswd
export PYTHONDONTWRITEBYTECODE=1
. ${IDF_PATH}/export.sh
idf.py -B build-diagnostic -D IDF_TARGET=esp32 -D SDKCONFIG=build-diagnostic/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' build
```

Both activation/halt gates must be unset.
Historical builds used reconfigure then ninja-j2; idf.py build works in the
same SDK environment. Do not mistake old build-esp32 outputs for current diagnostics.
If separately updating the adapter with target signals disconnected:

```bash
export ADAPTER_PORT=/dev/ttyUSB0
idf.py -B build-diagnostic -D SDKCONFIG=build-diagnostic/sdkconfig -p "$ADAPTER_PORT" -b 115200 flash
```

This targets ESP32 only and was not executed in that stage.
Console115200 8N1 usesGPIO1TX/GPIO3RX, separate from RVSWD.

With signals disconnected, read adapter info:

```bash
export ADAPTER_PORT=/dev/ttyUSB0
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-info-001.json info
```

Expected, not newly measured: protocol3,buildread-only-diagnostic,
activationfalse,haltfalse,board_verified/initializedtrue,gpio_clk25,gpio_dio26.

After wiring/fresh voltage checks, the first target operation should be **DMI reads only**:

```bash
read -r -p 'Target header4–1 voltage(mV): ' TARGET_MV
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-status-001.json capabilities --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
```

Only after stable resultok/pins_releasedtrue, with DM already active, select
bounded read-only snapshots. Each needs a new filename/current voltage;
never automatically continue after failure:

```bash
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-sram-001.json capabilities --sram --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-peripherals-001.json capabilities --gpio --usart --spi --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-registers-001.json capabilities --registers --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
```

Registers on a running hart correctly return hart_running_no_implicit_halt.
Do not halt merely to obtain them in this sequence.
MissingSBA returns unsupported_debug_capability before snapshot writes.
InactiveDM stops with debug_module_inactive; activation is a separately
selected procedure, not an implicit continuation.
After protection/parity/timeout, do not proceed with reset/unprotect/options
or the legacy dump command. help/info need no target access.

## Primary references

- [RISC-V External Debug0.13.2](https://riscv.org/wp-content/uploads/2024/12/riscv-debug-release.pdf):
  DMSTATUS,write-only requests,AccessRegister,ABSTRACTAUTO,SBA.
  The standard does not prove WCH supports optional commands.
- [WCH reference manual](https://www.wch-ic.com/downloads/CH32FV2x_V3xRM_PDF.html):
  local manufacturer-authoredv2.5 PDF under references/wch-dump-feasibility/,
  GPIO10.3.1,USART18.10,SPI20.3.7/20.4,OBR/RDPRT.
  SPI status reads may clear flags and are excluded.
- Pinned ch32v30x.h cross-checks register bases/offsets/RCC clocks.
  [Export feasibility](../../docs/FIRMWARE_DUMP_FEASIBILITY.md) supplied RDP/map context.

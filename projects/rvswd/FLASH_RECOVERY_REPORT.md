# DMT99 original internal Flash recovery investigation

**Latest result: zero original Flash bytes; one4-byte SRAM word matched
across two independent reads.** No successful Flash recovery or RDP bypass.
Final independent check:8/8stable,DMCONTROL00000001,DMSTATUS00000c82,
ABSTRACTCS08000002,pinsreleased,CPU running.
[Final status](recovery/20261017-final-after-word-tests.json).

## Completed physical experiments and raw data

| Mechanism/address | Actual result | Restoration |
|---|---|---|
| Running AccessMemory08006400 | cmderr4,no data | DATA restored,CPU running |
| Halted AccessMemory08006400 | cmderr3,no data | DATA restored; earlierACK-check uncertainty below |
| ProgramBuffer lw/ebreak08006400 | cmderr3,no data | GPR/buffer/DATA/running verified |
| ProgramBuffer directprefix08000000 | cmderr3,no data | Full cleanup,PC unchanged |
| ProgramBuffer bootalias00000000 | cmderr3,no data | Full cleanup,PC unchanged |
| ProgramBuffer appalias00006400 | cmderr3,no data | Full cleanup,PC unchanged |
| ProgramBuffer SRAM20000000 | **02028000**,two equal reads | Full cleanup,PC unchanged |

Each profile used a separate halt/read/restore/resume cycle.
A failed load was not retried in the same cycle.
The next profile began only after verified restoration.
General DMI capability scans were not repeated; each cycle used its
eight-sample safety preflight.

Evidence:
[application](recovery/20261017-original-progbuf-01/session.json),
[prefix](recovery/20261017-word-prefix-01/session.json),
[boot alias](recovery/20261017-word-boot-alias-01/session.json),
[application alias](recovery/20261017-word-app-alias-01/session.json),
[SRAM](recovery/20261017-word-sram-01/session.json).
Sessions retain raw UART,DMI traces,actual commands/addresses,errors and cleanup.
The [combined hash/trace audit](recovery/20261017-word-profiles-validation/hardware-results.json)
also confirms21older captures unchanged.
Incomplete timeout/crash traces are not complete-operation evidence.

SRAM files are **not Flash dumps**:

| Address | Size | Little-endian bytes | Files |
|---|---:|---|---|
| 0x20000000 | 4bytes/pass | `00 80 02 02` | [pass1](recovery/20261017-word-sram-01/original-sram-pass1.unverified.bin), [pass2](recovery/20261017-word-sram-01/original-sram-pass2.unverified.bin) |

Both SHA-256:
`47954a621ff103a08d6249925cb355e9c1fb8676c111128631d454b31f3cf1ba`.
Files match each other and the DMI-word byte sequence.
The .unverified name remains because no independently known SRAM reference exists;
repeat equality is nevertheless a physical observation.

This positive control shows the same lw/ebreak/register-transfer path can
return data. Flash failure cannot be explained solely as a general UART or
Program Buffer failure. Flash/alias exceptions are **consistent with activeRDP**,
but cmderr3 is not a dedicated RDP cause code.

Alias candidates came from WCH CH32FV2x/V3x RMv2.5§1.3:
boot selection maps Flash/system memory at zero.
BOOT pins,reset and mapping registers were not changed.
No alias data was obtained, so its mapped source was not assumed.

Default and build-recovery-progbuf-lab builds passed, with C mock/fault
checks and **34Python tests**.
The Program Buffer core separately passed ASan/UBSan plus33host tests;
final word profiles passed C/34host.
[Manifest](recovery/20261017-word-profiles-validation/manifest.json),
[tests](recovery/20261017-word-profiles-validation/word-profiles-tests.log),
[sanitizers](recovery/20261017-progbuf-validation/progbuf-sanitize.log).
The last physically installed ESP32 image has experiment gates enabled,
but executes no target command automatically.
Default builds keep lab/activation/halt disabled.

## Current technical boundary and next research

Direct Flash,missingprefix,aliases and two debug mechanisms were physically
tested. SBA is not advertised. Reviewed ROM ISP sources show no generic
Code Flash readout command. Autoexec/repeated attempts do not independently
grant access to the same denied load, so these were not pursued.

A distinct next question is **normal CPU execution of SRAM code outside
debug mode under RDP**. It remains unverified on the original device.
Scratch RAM free of DMA,trap/interrupt behavior and full CPU-state restoration
must first be established.
Reconstruction/offline ELF,map and linker outputs do not prove free scratch
memory in the running original application.
No code was written into original SRAM and no unknown Flash function was called.
The blocker is unverified preservation/restoration, not an assumed lack of further user permission.

The fixed-word interface below already exercised the four profiles above;
repeating the same error is not new evidence:

```sh
python3 tools/recovery.py original-progbuf-word --word-profile prefix \
  --port /dev/ttyUSB0 --output recovery/NEW-UNUSED-DIRECTORY \
  --voltage-mv 3000 --board-pins-and-wiring-checked \
  --allow-original-halt-read-resume --allow-original-progbuf-load
```

Other profiles: boot-alias,app-alias,sram.
Supply the actual fresh voltage, not a copied example.
The host accepts no arbitrary address/code.
The old SRAM decision no_larger_read_authorized was a fixed scope label,
not a revocation of the user's later broader authorization.
It was corrected in the host to
bounded_original_samples_checked_no_automatic_expansion without altering the capture.

## 2026-10-17 authorized halt and alternative-read experiments

The user's instruction to try other routes while preserving Flash was
treated as authorization for temporary halt/resume and restored debug/CPU
register changes. No Flash/options/RDP/WRP write,erase,reset or unlock was added.
Normal-build experimental gates remained closed.

lab-memory-halt-cycle performs preflight, optional idle ABSTRACTCS.cmderrW1C
00000700,halt,fixedAccessMemory reads,DATA restoration,resume,pin release in
one adapter operation. Host disconnect does not cancel firmware cleanup.
Unreliable transport/abstractbusy prevents blind resume.
Old captures remain unchanged.

- The first new attempt produced no response; the user then unplugged/replugged.
  Completion of target operations is not claimed.
- The second [raw UART](recovery/20261017-original-halt-memory-02/uart.raw.bin)
  showed **ESP32 main-task stack overflow**, not a targetRDP error.
  Large local reports consumed the serial-output stack.
  Reports moved to single-console-task static storage; raw UART capture and
  immediate adapter-reboot detection were added.
  [Fix/reflash manifest](recovery/20261017-halt-cycle-stackfix/manifest.json).
- The [third attempt](recovery/20261017-original-halt-memory-03/session.json)
  observed actual halt(DMSTATUS00000382).
  COMMAND02200000 at08006400 returned ABSTRACTCS08000302/cmderr3.
  DATA0/1 were restored; no Flash data/prefix attempt.
  After resume, eight reads showed DMSTATUS00000c82,DMCONTROL1,pinsreleased.
  Missing resumeack still caused cleanup_unconfirmed/poll_timeout in that
  version; the record remains unchanged.
- An [independent final check](recovery/20261017-post-cycle03-identify.json)
  verified8/8stable/running,DMCONTROL1,ABSTRACTCS08000302,pinsreleased.
  Thus independent evidence establishes the final running state.

WCH V4v1.3 table7-4 describes self-clearing resumereq;table7-5 defines
resumeack, without proving it must be observed at these sample instants.
Later explicit memory cycles require our halt to have been observed,
followed by three authenticated/running/no-reset samples and finalDMCONTROL1.
resume_ack_observed remains separate; no falseACK claim.
The generic diagnostic halt-cycle retains its oldACK requirement.

cmderr3 denotes exception, not proof of RDP causality.
Runningcmderr4 versus haltedcmderr3 is now physically observed.
[RISC-V Debug0.13.2§3.12.7](https://riscv.org/wp-content/uploads/2024/12/riscv-debug-release.pdf#page=37)
defines idlecmderrW1C and error codes.

The separately prepared lab-progbuf-halt-cycle uses only
lw x9,0(x8)(00042483) and ebreak(00100073), with no target-memory stores.
It saves x8/x9,DATA0,two progbuf words andPC, uses fixed addresses and verifies
buffer readback before execution.
Registers/buffer are restored and checked; changedPC prevents resume.
Sticky errors are cleared only to permit restoration, never to retry failed loads.
Separate default-off CONFIG_DIAG_PROGBUF_LAB and an explicit token are required.
[Source/build/test manifest](recovery/20261017-progbuf-validation/manifest.json).

## Earlier running experiment

> 2026-10-17 22:59 Turkey time: DM activation succeeded.
> The first running-MCU AccessMemory command returned cmderr4.
> **No Flash bytes; no prefix attempt.**
> CPU remained running,DATA0/1 restored,pinsreleased,no retry.

At that stage original Flash recovery remained0bytes; the first0x6400 bytes
were unavailable. ESP32 BINs below are adapter firmware, not DMT99 dumps.

### Activation-enabled adapter and original-MCU session

The user authorized ESP32 updating and a bounded running read.
build-recovery-active-lab enabled activation/lab,not halt.
Application179968bytes,SHA-256
`5e9d8d30129c497c39b7a55b9f46ae895936d2775d19bfe356ae8643512eaf89`.
[Upload log](recovery/esp32-active-lab-upload-20261017T195734Z/upload.log)
shows three segment hash checks/exit0.
RTS reset affected only the ESP32.
Fresh user-measured target voltage was3000mV.

[Activation](recovery/20261017-active-lab-activation-01.json):
8/8stable,chip30310524,oneDMCONTROL10=1write,readback1/c82,
running/authenticated,pinsreleased. No halt/resume/reset request.

[Original session](recovery/20261017-original-running-memory-01/session.json):
65complete DMI traces.
DATA1=08006400 written/read back; oneCOMMAND02200000.
ABSTRACTCS changed08000002→08000402,busy0/cmderr4.
No Flash-result DATA0 read; both passes empty,no .bin.
Original DATA0/1zero restored/read back.
FinalDMSTATUSc82,DMCONTROL1,data_restored/final_valid/pins_releasedtrue,
target_state_uncertainfalse.
Stickycmderr and activeDM retained, so complete debug-state rollback is not claimed.
No RDP/options/Flash/CPU-register writes.

This is a target command error rather than transport/timeout.
It agrees with the documented hart-state error.
The running command was blocked; halted/RDP interaction was then unknown.
Host reference/stale checks on empty samples do not mean actual zero/stale
Flash data was returned.

The host additionally records original-lab --allow-original-running-read
authorization, without pretending the target is a spare.
Legacy wire name lab-memory-running-spare selects the same running engine.
28Python tests passed after this interface addition.
[Evidence manifest](recovery/20261017-original-running-memory-01/evidence-manifest.json).

At that earlier stage the next proposed test was running/halted AccessMemory
on a known-content unprotected same-model spare.
Original halted work required separately authorized,verified restoration;
the installed image then had halt disabled.
Later experiments at the top supersede this historical boundary.

## Correcting the earlier readout interpretation

Physical captures/dm-capabilities-1792233896.json has SHA-256
`9dc61b7eebdbf04855b9c2996ba692a536497d34ee68224005392224dcfbb3e8`.
Eight stable samples show chip30310524,DMCONTROL1,DMSTATUSc82,
ABSTRACTCS08000002,ABSTRACTAUTO0,SBCS0,pinsreleased.
It advertises no standardSBA and says nothing conclusive about AccessMemory.
Authenticated does not mean RDP is disabled.

The old read_protection_enabled result originates in
[main/rvswd_ro.c](main/rvswd_ro.c),transfer_flash:
after OBR,obr&2 returns **before reading Flash**.
OBR03fffefe/RDPRT1/bytes[0,0] agree with that.
Zero-length partial files show **no data acquired**, not zero-filled Flash.
That old session did not establish rejection/zero-return of a particular
Flash load. GPR/progbuf-assisted OBR access is not Flash-read success.
The legacy reader is excluded from normal/new experimental linking.

Reference V2.4:
`../../DMT-99_V2.4_20240306.bin`,135096bytes,base08006400,SHA-256
`4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
It is an update application, not prefix/full-original backup.
Captures/analysis were unchanged during the investigation; session JSONs
retain hash inventories.

## Candidate access paths reviewed

The table records the earlier research stage; later physical results are above.

| Route | Source/finding | Decision then |
|---|---|---|
| StandardSBA | ActualSBCS0; snapshot code requires advertisement | No repeated capability scan; unavailable current route |
| AccessRegister,type0 | GPR/CSR/FPR transfers,not generic memory read | Existing snapshots retained |
| AccessMemory,type2 | WCH documentation internally inconsistent | Running attemptcmderr4; halted/RDP then unknown |
| ProgramBuffer | Legacy/minichlink paths modifyGPR and execute load/store | Required separate authorization; not yet run at that stage |
| QuickAccess | Temporary halt/progbuf/resume; WCH says unsupported | Not nonintrusive; not added |
| WCH-Link memory command | wlink requires haltedMCU | ProbeAPI,not a new directly transferable ESP32DMI read |
| Memoryaliases | Boot-dependent mapping,not an independentRDP exemption | No address sweep |
| ROMISP | wchisp distinguishes CodeFlash program/verify from config/EEPROM reads | No proof EEPROMread can read CodeFlash; boot change needed |
| Runningoriginalapp | Constants/settings reads and USART2TX exist,but no general exporter | CPUdata access is not debugger permission; no ASIC/UART commands sent |

Sources:
[minichlink DefaultReadWord](https://github.com/cnlohr/ch32fun/blob/master/minichlink/minichlink.c),
[wlink read_memory](https://github.com/ch32-rs/wlink/blob/main/src/operations.rs),
[wchisp protocol](https://github.com/ch32-rs/wchisp/blob/main/src/protocol.rs),
[EEPROM code](https://github.com/ch32-rs/wchisp/blob/main/src/flashing.rs).
No minichlink setup/unlock flow was executed/imported.
Probe-protocol success does not prove correct protected Flash bytes.
Verify compares supplied data; it is not general readout.
No undocumentedISP command or comparison-based guessing sweep was performed.

WCH FLASH_GetReadOutProtectionStatus readsOBR,not Flash export.
[Reference manual](https://www.wch-ic.com/downloads/CH32FV2x_V3xRM_PDF.html),
localv2.5 at references/wch-dump-feasibility/:
§§32.2.2/32.4/32.5.1/32.6.4 distinguish normal reads,RDP and destructive unprotect.
It does not give every debug-command/execution-context permission.
Generic exceptions/bus errors therefore are not labeled dedicatedRDP denial.

The local originalBIN,135036-byte SPI-extracted candidate and prior reports
were reviewed. The SPI candidate starts at0x6400 and supplies no prefix;
x86PE residue is not MCUbootloader.
[FNIRSI downloads](https://www.fnirsi.com/pages/manuals-firmwares) and
[product page](https://www.fnirsi.com/products/dmt-99) yielded no verifiable
full-MCU/prefix package in accessible content.
This does not claim no such file exists anywhere.

## AccessMemory and halted-state requirements

[RISC-V Debug0.13.2§3.6.1.3](https://riscv.org/wp-content/uploads/2019/03/riscv-debug-release.pdf#page=24)
makes AccessMemory optional.
Implementations may support halted/running access; running support implies
halted support. **The standard does not universally require halt.**
For DXLEN32,address isDATA1,read wordDATA0.
COMMAND02200000 selects type2,32-bit physical read,write/postincrement/
target-specific fields0.

[Official QingKeV4 manual record](https://www.wch.cn/downloads/QingKeV4_Processor_Manual_PDF.html)
and its reviewed manufacturer-authored distributor copy,v1.3§7.1,
describe AccessRegister/AccessMemory in the overview.
Table7-8 marks type2 unsupported,while7-9 details type2 fields.
Support **cannot be settled without measuring this silicon revision**.
Table7-7cmderr4 denotes a non-suspended/unavailable hart; that alone does
not prove every memory read requires halt.
Table7-9postexecbit18 remains0 in the experiment.

The first lab-memory halted requirement was an **implementation safety choice**,
not a proven CH32V303 requirement.
A separate lab-memory-running-spare path tested the distinction, initially
for an unprotected spare, later once on the authorized original withcmderr4.
Both use the selected existing hart state; neither implicitly halts/resumes,
retries or falls back to another command.

## Implemented bounded experiment

[main/rvswd_memory_lab.c](main/rvswd_memory_lab.c) reuses transport.
CONFIG_DIAG_ABSTRACT_MEMORY_LAB defaults off independently of activation/halt.
Default firmware rejects lab access even with a token.
The first lab image had activation/halt disabled; active-lab separately enabled activation.

- Fixed application08006400–0800640f then prefix08000000–0800000f,
  two complete passes,at most16abstract commands.
- Stable preflight,active/authenticatedDM,selected existing hart state,
  ABSTRACTAUTO0,idle/no-errorABSTRACTCS and twoDATAregisters required.
- Perword:DATA1addresswrite/readback,COMMAND02200000,boundedbusy,DATA0read.
  These are debug-register writes,not targetFlash/SRAM/MMIO/CPU stores or progbuf execution.
- DATA0/1 saved/restored/read back only with reliable connection,unchanged
  state,autoexecoff/idle. Stickyerrors retained; command history cannot be
  restored,so completeDMrollback is not claimed.
- Frame100ms,abstractpoll200ms; final in-flight frame may exceed poll budget.
  Fixed operation count; UART response after pins release.
  Host response deadline45s.
- No blind cleanup writes after connection loss.
  Busy/state/readback/cleanup failures retain raw status,word count and
  uncertainty. Pin release is attempted on every valid invocation exit.
- No aliases/options/settings/calibration/ROM/extended ranges in this
  original AccessMemory allowlist. 08040000 is not silently appended to a
  presumed256KiB dump;480KiB/zero-wait partitioning needs device confirmation.

Rawcmderr retained:2unsupported,4hartstate,3exception,5buserror.
The latter two do not establishRDP causality.
Transport/timeouts remain separate.
resultok means transfer completed; recovery_verified remainsfalse.

[tools/recovery.py](tools/recovery.py) is the host controller:

- audit: hashes/decisions only,no serial access.
- connect: passive modem lines,boundedhello,firmware/pin/interlock checks;
  no repeated capability scan or original-target lab command.
- spare-lab: explicit unprotected-spare confirmation,currenthartstate,
  freshvoltage,wiring and known16-byteprefix required; sends exactly one
  selected experiment and stores complete response/trace in a new session.
- Checks cover returned addresses/order/bounds,two passes,application
  reference,prefixcanary,constant/stale and alias similarities.
  Partial raw output is .unverified.bin with addresses/lengths/pass/hashes.
  Even passing a spare does not establish original-DMT99 recovery.

Real prefixes can be repetitive; the constant-data check rejects unsuitable
lab positive controls rather than proving such original content impossible.
Equal hashes alone do not authenticate source or address.

## Earlier offline validation and first connection

C transport/default/lab diagnostics passed.
Synthetic running/halted peers inject parity at each successfulDMIread and
connection failure at eachGPIOwrite.
Coverage includescmderr1–7,busytimeout,autoexec,wrong/changinghart,
repeatmismatch,inactiveDM,missingDATA,initialstickyerror,voltage/wiring/
build/runtime gates and pin-release failures.

**26Python tests passed** with actual host/serial-peer tests of both profiles,
hello-only access,raw/hash/address outputs,reference/prefix mismatches,
missing/stale data,invalidaddress/word bounds,incompatiblefirmware,
missingpermissions and no-retry.
ClangASan/UBSan C/host suites also passed,leakcheckingoff underptrace.
All these are offline/synthetic.

ESP-IDF5.5.5/classicESP32 built default and lab images; esptool image_info
verified checksums/validation hashes.
[Manifest](recovery/20261017-validation/manifest.json),
[tests](recovery/20261017-validation/tests-final.log),
[sanitizers](recovery/20261017-validation/sanitize.log),
[host](recovery/20261017-validation/host-final.log).
Manifest retains source/ELF/BIN/config/capture hashes.

| Build | Gates | BIN bytes | SHA-256 |
|---|---|---:|---|
| build-recovery-default | lab/activation/halt off | 178048 | `14c1745a3099f7a8673dacd6a975bcf6843aa744ee706f6697a2633d28bdac84` |
| build-recovery-lab | lab only | 179968 | `8e06d06f7a5368e62169ca63214fee76407378f8e8327de53bd755aa308cadc2` |

The [first sandbox serial attempt](recovery/20261017-session-01/session.json)
failedENOENT. sysfs/CP2102(10c4:ea60) existed,but sandbox/dev was a differenttmpfs.
Authorized host inspection found ttyUSB0 character188:0,mode0666.
This was **sandbox device visibility**,not a demonstrated driver/RDP/physicalUSB failure.

The [passive actual adapter session](recovery/20261017-host-session-01/session.json)
then succeeded:protocol3,GPIO25/26,initialized/verifiedtrue,activationtrue,
haltfalse. The installed image was older diagnostics without lab fields.
Only hello was sent; **no new DMI/memory experiment**.
TargetRDP/Flash/state and captures remained unchanged at that stage.

## Initial spare experiment proposal — historical

Before original-device experiments,the proposal was a known-content,
unprotected same-modelCH32V303 rather than the working original.
DMT99 signals must be disconnected before installing experimental adapter
firmware. No automatic upload was part of the proposal.
Without an actualspare/canary,that prerequisite was missing; it did not
authorize substituting the original device.

The first spare candidate tests already-running access.
cmderr4 stops without implicit halt.
Halted comparison is a separate prepared session.
Protected-original success remains independent of spare success.

The following reviewable interface was not executed in that initial stage.
SPARE_MV must be measured; the prefixfile is the spare's known16bytes and
the application reference's first16bytes must match08006400.
These commands do not program test data into a MCU:

```sh
python3 tools/recovery.py spare-lab --port /dev/ttyUSB0 \
  --output recovery/NEW-SPARE-RUNNING \
  --unprotected-spare-confirmed --hart-state running \
  --board-pins-and-wiring-checked --voltage-mv "$SPARE_MV" \
  --reference /path/to/known-spare-application.bin \
  --prefix-reference /path/to/known-spare-prefix-16.bin
```

The later original experiments advanced beyond this plan:
type2running returnedcmderr4,halted/progbuf/alias reads failed as documented above.
Earlier statements that halt/progbuf had not been authorized or executed
describe their historical stage,not the final result.
Normal firmware exposes no experimental access.
No real prefix disassembly or complete-recovery hash was fabricated.

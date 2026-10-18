# DMT99 firmware update and protected Flash export feasibility

17 October2026. Target: user-reported CH32V303RCT6 with active RDP.
**GO for an offline prototype; NO-GO for installing code on the working
original device or claiming a lossless complete dump.**
RDP was not removed, no debug/serial/USB target was opened, and no firmware
BIN, payload, patch or flashable image was produced.

Reading from normal user code executing in Flash appears conditionally
plausible. No verified way was found to start that code **without changing
original contents**. Updater erase bounds and rollback behavior are unknown.
This is neither successful RDP bypass nor a deployable device procedure.

## Evidence scope

Existing decompilation/disassembly of439 functions, call graph, USART/Flash/
MMIO inventory, native persistence, USB and SPI findings were examined.
Ghidra analysis was not regenerated.
[firmware_update_evidence.json](../analysis/firmware_update_evidence.json)
retains the narrow inspection's original addresses, BIN bytes and call sites.

Manufacturer-authored WCH CH32FV2x_V3xRM v2.5 and CH32V307DS0 v3.9 were reviewed.
Official [reference-manual](https://www.wch.cn/downloads/CH32FV2x_V3xRM_PDF.html)
and [datasheet](https://www.wch.cn/downloads/CH32V307DS0_PDF.html) pages were accessible,
but direct PDF download returned a retry error.
PDFs came from accessible mirrors, not directly from the manufacturer server.
URLs, hashes, API metadata and selected page text are in
[references/wch-dump-feasibility](../references/wch-dump-feasibility/).
The pinned WCH Flash driver was also checked.
Forum/commercial unlock claims were not treated as access guarantees.

## BIN format and address map

DMT-99_V2.4_20240306.bin: **135096 /0x20FB8 bytes**,
SHA-256 `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
It is a direct RV32 application image, not an ARM vector table or a truncated
complete MCU dump. Intended mapping for file offset f:

```
link/normal execution address =0x00006400+f
canonical Code Flash address =0x08006400+f
```

| Region | Canonical physical addresses, inclusive | BIN offset | Evidence |
|---|---|---|---|
| Missing pre-application area | 08000000–080063FF | absent | 25KiB; contents/function unavailable |
| Entry/padding | 08006400–08006437 | 00000–00037 | 6f608026: j0000C668, followed by NOP/EBREAK |
| Vectors/padding | 08006438–080065FF | 00038–001FF | Low-alias handler addresses |
| Present text | 08006600–0801E81F | 00200–1841F | Preserved instruction inventory |
| Constants | 0801E820–080271CF | 18420–20DCF | Fonts/tables/strings |
| Initialized SRAM load copy | 080271D0–080273B7 | 20DD0–20FB7 | Reset copy loop,488bytes |
| Code Flash after BIN | 080273B8–08077FFF | absent | Manufacturer map, not read from this device |
| Settings/history access | 08040000–080400FF | absent | D928/D87C |
| Default SRAM | 20000000–2000FFFF | Reset copies data | Stack20010000, BSS end200054D8 |
| WCH system boot memory | 1FFF8000–1FFFEFFF | absent | RM table32-1; **not the missing first25KiB** |
| User option bytes | 1FFFF800–1FFFF87F | absent | RM table32-1, outside prototype scope |

Datasheet table2-1-1/notes distinguish RC's **480KiB total Code Flash**
from the default256KiB zero-wait region/64KiB SRAM.
This corrects the earlier interpretation that08040000 must be invalid
because it lies beyond nominal256KiB.
Actual partition options and data there remain unread.
[Datasheet pages5–6](https://ch32-riscv-ug.github.io/CH32V307/datasheet_en/CH32V20x_30xDS0.PDF#page=5).

Reset C668–C6F2 on the low alias establishes gp200009B0, sp20010000,
data source271D0, destination20000000, end200001E8, mtvec643B and mepc908A.
**Physical and execution addresses are not interchangeable**:
the same AUIPC sequence at0800C668 produces gp280009B0.
Blind upper-alias jumps or writing this BIN at zero are not installation plans.
The file has not been compared byte-for-byte with the installed device.

### Header, CRC, signature and version

A valid first instruction leaves no separate pre-code container header.
The file ends exactly at the initialized-data extent copied by reset;
no additional trailer follows. The last four bytes are
`50 01 00 20`, a SRAM-address value rather than
standard CRC32 of preceding bytes.
Whole-file CRC32 **8EF0046F** is an inventory value only, not an established updater checksum.

The constant DMT-99 V2.4 at link24BA0 is rendered by143E0.
This is a displayed version, not evidence of update-file validation.
No application flow for update size/version/CRC/signature checking was found.
A control field in initialized data, external whole-file checks or filename
rules remain possible. A few CRC attempts do not prove no signature or arbitrary-payload acceptance.

## Flash writes and updater findings

| Original entry / call site | Behavior / destination |
|---|---|
| C320, C340→D87C | Shutdown: display/buzzer/backlight, settings commit, PC1low, infinite loop |
| D928, D93C→D908 | Reads128halfwords08040000→20000020 |
| D87C, D8AC→CA52 | One fast-erase request for08040000 |
| CA52 storesCA62/CA64/CA6C | CTLR40022010 page-erase20000; ADDR40022014=address&FFFFFF00; START40 |
| D87C, D8F8→C9EE | Up to128halfwords08040000…080400FE while status remains4 |
| C9EE storeCA14 | Sets PG,16-bit destination store, waits, clears PG |
| C982/CA26, C99C/CA44 | Standard/fast unlock/lock, not option-byte/RDP removal |
| DD4E→FB48("0:BOOT") | SPI FatFs directory creation, not MCU bootloader entry |
| C5F2/C5F4/C644 | USB vectors target default traps, not MSC handlers |

The only direct caller of present ProgramHalfWord/ErasePage_Fast/unlock/lock
routines is D87C, called from shutdown C320.
No application-region erase/program, USB MSC handler, reset-flag bootloader
handoff or updater file-loading flow was found.
Scope is the439bodies, instruction/MMIO inventory and prior descriptor scan;
absence from this application says nothing definitive about the missing prefix.
SPI D538/D588/D6BC erase/page/sector writes target external NOR, not MCU updates.

[test_update_flash_trace.py](../tools/test_update_flash_trace.py) executes
original D87C/D928 against the native persistence callback contract.
25commit cases cover status/failure boundaries;16load cases cover all256bytes.
SDK bodies execute; MMIO records intent in private Python memory, CSR800 is
skipped, and busy/ready/WRP responses are modeled.
**41 cases passed; physical erase/program and RDP behavior were not validated.**

### Is the first25KiB preserved during updates?

**Unknown from available evidence.** The BIN omits it, but the updater could
still erase overlapping memory. Hypothetical geometries illustrate the uncertainty:

| Possible erase unit | Rounded-down start for08006400 | Prefix overlap |
|---|---|---|
| 256-byte fast page | 08006400 | None |
| 4096-byte standard sector | 08006000 | 08006000–080063FF,1024bytes |
| 32KiB block | 08000000 | All25KiB; RDP/write protection may additionally affect it |

These are not updater traces.
RDP's documented automatic write protection covers only the first4KiB,
not the remaining21KiB.
WRP, rounding, mass erase, failed-update recovery, filename and signature
rules must be understood before presenting a payload to the updater.

For a **pristine complete dump**, installing export code already changes
application pages before export starts.
Restoring the official BIN does not prove those pages originally matched it.
Unless every overwritten byte is preserved independently, this cannot be
called recovery of the original complete Flash.
Even prefix-only recovery first requires proof the updater preserves that prefix.

## RDP access matrix and confidence limits

RM v2.5 §§32.2.2,32.5.1,32.6/RDPRT were reviewed.
§32.5.1 describes normal8/16/32-bit Flash reads through the general address
space, without a complete matrix for every RDP/execution context.
RDP removal automatically erases main Flash: **NO-GO** for preservation.
[RM pages603/616–617](https://ch32-riscv-ug.github.io/CH32V20x/datasheet_en/CH32FV2x_V3xRM.PDF#page=603).

| Active-RDP context | CPU/data reads | Erase/program | Assessment |
|---|---|---|---|
| Normal user Flash boot, no debugger | Application constant/D928 reads support this path; full missing-prefix access untested | First4KiB automatically protected; others depend on WRP/conditions | Conditional export candidate |
| Boot from SRAM | Selected RM lacks detailed CPU-read matrix beyond general protection wording | Normal erase/program forbidden | Lossless route unproven,NO-GO |
| Flash boot→SRAM routine | Boot source vs currentPC dependency unresolved | Not assumed safe | Separate experiment |
| System boot/ISP | No documented lossless Code Flash readout found | Normal writes restricted; mass-erase exception does not preserve contents | USB/USART ISP is not an RDP-readout guarantee |
| SDI/debug | RDP definition prevents readout | Normal writes restricted | ESP32-S3 interface does not grant access by itself |
| DMA/other bus master | Unverified for this target/context | Unused | Not selected |

It is unjustified to claim an RDP-protected CPU cannot read any Flash data:
the working application's constant/settings reads contradict that simplification.
Conversely, the selected RM does not prove Flash code can read every protected
address or that RAM code will succeed.
Normal-Flash export is **conditionally feasible**, physical success **unverified**.
Other MCU families' RDP-level models were not imported; no option-byte writes were added.

## Transfer-channel choice

**Candidate: USART2 TX/PA2, one-way115200 8N1.**
Main908A passes115200 to1A682, which configures GPIOA mask0004 as PA2
alternate output, USART2 at40004400, TX-only.
1A6E2 waitsTC and sends bytes.
This evidence is independent of the ASIC protocol.

| Channel | MCU pin evidence | DMT99 wiring evidence / result |
|---|---|---|
| USART2TX | PA2,64-pin package pin16 | Firmware setup exists; accessible pad/header/continuity unverified; strongest candidate |
| USART2RX | PA3,pin17 | Original debug setup is TX-only; pad/route unknown; no resend channel assumed |
| USART1 | PA9/PA10,pins42/43 | C350/97C4/C468,9600 8N1 ASIC; not selected |
| USBFS | PA11/PA12,pins44/45 | Package mux exists; connector routing/application stack unproven; not selected |
| USART3/newGPIO | Mux possibilities alone insufficient | GPIOB is display bus; no spare pin/new UART assumed |
| External SPIFlash | Existing SPI1/NOR | Writing export data would change external evidence; not selected |

[WCH pin tables](https://ch32-riscv-ug.github.io/CH32V307/datasheet_en/CH32V20x_30xDS0.PDF#page=31)
support package assignments, not a measured PCB pad map.
The prototype initializes neither USART2 nor GPIO.
Future binding needs MCU-TX-only reception, appropriate VIO/shared reference;
no host TX on unknown PA3 is planned.
Original debug printf must not contaminate binary transfer.

## Read-only offline prototype

[flash_export.c](../reconstructed/offline_export/flash_export.c) is new
experimental code, not an original-function translation.
Only read(address,out,n) and send(frame,n) callbacks are exposed.
There is no Flash unlock/erase/program, option-byte access, MMIO reader,
startup or updater integration. File backend opens the BIN rb and never invents a prefix.

Bounds combine manufacturer Code Flash range [08000000,08078000) and an
independently verified narrower caller policy.
Zero length, overflow, out-of-policy bounds and pass counts outside2–8 are rejected.
Each block of at most256bytes is read twice and compared before transmission.
Default two complete passes read each byte four times.
Read/send errors stop the transfer.

**D99Xv1 is this project's test protocol, not a FNIRSI/WCH update protocol.**
Multibyte fields are little-endian:

| Offset | Field |
|---|---|
| 0..3 | D99X magic |
| 4..7 | Version1,typeDATA1/END2/ERROR3,pass index/count |
| 8..11 | Caller-selected session |
| 12..15 | Block index; total blocks forEND |
| 16..19 | Block address; range end forEND |
| 20..27 | Expected range start/byte length |
| 28..31 | Payload length u16,status u8,reserved0 |
| 32.. | DATA1–256bytes; END pass-dataCRC32; ERROR no payload |
| Final4bytes | Header+payload CRC32/ISO-HDLC,EDB88320,initial/finalXORFFFFFFFF |

[flash_dump_receiver.py](../tools/flash_dump_receiver.py) reads capture files/stdin.
Expected range/session are supplied independently.
It resynchronizes after bad CRC; missing blocks/END, changing metadata,
inconsistent duplicates, unequal passes or ERROR cannot produce successful output.
Equal duplicates and reordered blocks are accepted.
Only after all checks does it create a new file, refusing overwrite.
Reports include SHA-256, passCRC and missing/noise counts.
--known-bin compares the overlapping application region and fails if no overlap exists.
compare checks two independent dumps' lengths/differing bytes/hashes.

The one-way transport has no host resend command; rereads occur in passes,
and lost transfers would need new independent sessions.
No PA3 wiring is invented.
Session/CRC do not authenticate source/freshness or prove RDP success:
repeated wrong/constant data can also match.

**File replay only**, from repository root:

```sh
make -C reconstructed/offline_export test sanitize
work_dir=$(mktemp -d /tmp/dmt99-export.XXXXXX)
reconstructed/build/offline_export/flash-export-replay \
  DMT-99_V2.4_20240306.bin 0x08006400 0x08006400 135096 123 \
  > "$work_dir/capture.d99x"
python3 tools/flash_dump_receiver.py receive "$work_dir/capture.d99x" \
  --start 0x08006400 --length 135096 --session 123 \
  --known-bin DMT-99_V2.4_20240306.bin \
  --output "$work_dir/application.bin" --report "$work_dir/report.json"
python3 tools/flash_dump_receiver.py compare \
  DMT-99_V2.4_20240306.bin "$work_dir/application.bin"
```

This is **not an MCU dump**, only transport testing of an existing file.
When an actual dump becomes available, ingest_mcu_dump.py can compare the
application incrementally; a prefix-only file must not be labeled a full dump.

## Separate Flash and RAM execution plans

Offline checks passed C protocol, independent Python oracle, bounds, CRC,
missing/corrupt/reordered/duplicate frames, read/send failure and unequal passes.
Six Python tests cover synthetic memory1byte–480KiB and the unchanged BIN.
C and ASan/UBSan executions passed, as did41boundedD87C/D928 comparisons.

Two RV32IMAC/ILP32 **relocatable objects** were compiled: normal .text.*
and DMT_EXPORT_RAM_EXPERIMENT .ramtext.dmt_export.
RAM direct code references remain in-section and have no undefined external
symbols, but actual callback locations depend on binding.
Neither object was linked/executed.
Evidence: analysis/flash_export_object_build.json,
logs/flash-export-object-build.log.
Section placement is not an RDP experiment or SRAM loader.

1. **Flash execution on a separate same-model spare:** establish known
   canaries withoutRDP, then compare normal protected Flash boot.
   Verify final ELF loader/reader/CRC/UART addresses, stack, interrupts and
   absence of Flash-write calls.
   Cold-boot without debug, separately recording low-alias execution and
   canonical reads of prefix/application/08040000/non-zero-wait regions.
   Meet RM clock conditions rather than blindly copying144MHz.
2. **SRAM boot on the same-model spare:** compare unprotected positive control
   with protected read/fault/timeout behavior. Executing code is not proof
   of successful reads. No option-byte writes, automatic unprotect or mass erase.
   If negative, do not repeat on the original device.
3. **Flash boot→SRAM jump:** separate from SRAM boot. Confirm currentPC,
   loader, callbacks and constants. Flash callbacks must not be credited as
   SRAM-only success. Test bus-fault/exception paths first.

None of these physical experiments, including spare-device work, was performed in this phase.

## Required gates before touching the original device

- Document actual MCU/lot,VIO,power/latch behavior and accessible PA2 route.
  Avoid back-powering or driving ASIC lines.
- Obtain updater code or trustworthy erase/program traces proving file/address
  mapping, erase unit, first/last page, option/RDP/WRP behavior, checks and failed/interrupted states.
- Explain preservation of **all** potentially lost original data.
  V2.4-only rollback is not complete restoration without prefix, settings,
  differing application bytes and device-specific data.
  SPI backup is not internal Flash backup.
- Validate the execution context on a separate protected same-model spare
  without debug, comparing two independent dumps and known V2.4 overlap.
  Constant00/FF/E339 is not success.
- Any future payload needs separate output/manifest, exact base/hash,
  clobber list and verified rollback. No such payload was produced here.

| Decision | Result |
|---|---|
| File-backend development/protocol validation | **GO, completed** |
| Separate-spare RDP execution-context investigation | Conditional next step; not performed |
| Install update/payload on current DMT99 | **NO-GO** |
| Preserve original by removing RDP | **NO-GO**, documented automatic erase |
| Claim lossless ESP32-S3/RAM bypass | **NO-GO**, no evidence |
| Guarantee first25KiB survives updates | **NO-GO**, updater/erase bounds unavailable |

Historical function counts remained **146complete/14partial/123incomplete**,
with partial included in incomplete.
Experimental exporter/tests earn no original-function credit.
STOCK_COMPATIBLE/CUSTOM_DMT99 application sources/ELFs were unchanged;
existing regressions were preserved without rerunning unrelated large suites.
Final audit: analysis/flash_export_validation.json.

This dated feasibility stage predates the later physical RVSWD experiments;
see [current research overview](RESEARCH.md) and
[recovery report](../projects/rvswd/FLASH_RECOVERY_REPORT.md).

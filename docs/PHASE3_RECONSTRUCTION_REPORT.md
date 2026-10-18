# Unified Phase3 reconstruction report

Phase3 delivered working code, verified extraction and expanded original-machine-code tests.
The complete application and all Phase3 objectives are **not complete**. The missing USB
bootloader, CP936 boundary and remaining UI/graph/startup work are stated below.

| Measure | Before | After |
|---|---:|---:|
| Decompiled / role-classified original functions |439 /439|439 /439|
| Complete native C original-function translations |80|87|
| Partial native C translations |36|36|
| Official compiled SDK counterparts |64|64|
| Additional qualified upstream counterparts |0|17|
| Incomplete non-trap application/library bodies |209|185|
| Direct original-entry behavior comparisons |34|51|
| Maintained native C units |27|32|
| C tests |19|26|
| Python forensic/reference tests |0|11|
| ASan/UBSan C tests |6|13|
| Differential cases |13740|15881|
| Live extracted files / JPEGs |unexamined|299 /297|
| New library Exact / Strong semantic matches |0 /0|1 /16|
| Reproducible cross-builds |both ELFs|both ELFs|

The7 new complete native entries are6708,6734,678c,ddb0,de12,109e4 and10bd0.
The existing1a360 bitmap routine was reused and newly compared against original pixels;
its coverage is not counted twice. Imported library counterparts are a separate category.
The detailed-semantics lower bound254 includes86 traps,64 SDK entries and17 library
counterparts. Those traps are **not** evidence of application completeness. Function count
coverage is not code-byte or whole-firmware equivalence. All validation is offline and bounded.

## Acquired Flash and executable evidence

Only `flashdump/dmt99_flash_1.bin` is available. It is8388608 bytes with the expected
SHA-256fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69.
No comparison with the independently acquired second reading can be made here.
Original firmware SHA-256 and all885 preserved baseline evidence hashes pass.

The MBR uses4096-byte sector units and type07 entries over two FAT12 volumes:
0x3f000/1638400 bytes and0x1cf000/6492160 bytes. Standard tools were tried first;
a bounded read-only parser then inventoried FAT copies, LFNs, chains, deleted entries,
orphans and every one of2048 physical4KiB sectors. The resource volume has299 live
files and12 directories, matching FAT copies, no crosslinks and no allocated orphans.
The smaller volume has57 deleted entries, one broken directory and4 orphan allocated
clusters. Deleted firmware/update/logo names remain historical leads, not live files.

All299 valid files are extracted with physical extents/hashes. All297 JPEGs decode
independently. Raw carving identifies605 top-level JPEG extents plus366 nested
preview/thumbnail extents, not971 distinct live resources. No external calibration or
measurement-history data structure has been established; `history/` contains JPEG labels.

A135036-byte raw RV32 application at0x4d000 has a coherent6400 entry/c65c reset and
initialized-data extent.103 complete contiguous code bodies match V2.4 exactly; initialized
data differs. Its historical filename/version remains unknown. Broad alternative-image
normalization and CFG comparison are unfinished. Four x86 PE extents are host-file residue,
including duplicate/nested executables; they are not MCU firmware.

See [Flash analysis](FLASH_DUMP_ANALYSIS.md), [firmware comparison](FIRMWARE_VERSION_COMPARISON.md)
and the associated machine-readable layout/files/regions/integrity/inventory/difference reports.

## Recovered source and behavior

Official ChaN TJpgDec R0.01b and FatFs R0.14b counterparts are pinned, licensed and
compiled. Exact original object layouts, constants, tables, compiled body comparisons,
normalized opcodes, CFG/memory fingerprints and original-code tests support17 qualified
counterparts. One whole routine compiles byte-identically;16 are Strong semantic matches.
The historical branch identification does not prove the exact original vendor source archive
or compiler. The remaining compiled FatFs APIs are not automatically credited as recovered.

Native resource adapters connect read-only Flash blocks to file lookup, JPEG MCU decoding
and display window/command/data or a host canvas. They preserve the shared FatFs mount
object, remount ordering, optional logo behavior and successful-open/silent-decode-error
return quirk. The caller owns the11KiB-plus resource workspace. Both JPEG and bitmap
numeric renderers preserve resource names, spacing and clear ordering. Exact bitmap data
was recovered from internal ROM.191 live resources have literal firmware crossrefs;
dynamic language/menu/unit filenames remain separately identified.

The native filesystem reads298 files byte-for-byte and renders296 JPEGs twice. One
Chinese-named JPEG reaches the original's absent/invalid CP936 table boundary. Native code
reports fc9e/fcfc and fails closed; it does not silently install a generic replacement table.
That resource is still extracted/validated by the forensic path. C write cases operate only
on private heap copies, including partition maps and corrupt-FAT tests.

Existing HOLD/relative/command/packet/menu/threshold/history/settings/power behavior remains.
New scheduler tests check latest-event replacement while menu parsing is suspended and
TIM3 updates continue. The original blocking-loop reference remains separate. Newly exposed
resumable yield points, interrupts, display delays and reentrancy are not cycle-equivalent proof.
See [resource architecture](RESOURCE_LOADING_ARCHITECTURE.md),
[library recovery](LIBRARY_SOURCE_RECOVERY.md) and [scheduling limits](SCHEDULING_SEMANTICS.md).

## USB findings and limits

No valid USB descriptor sets or confirmed MSC/SCSI device handlers were found in either
application or the SPI image. The update BIN omits internal addresses below6400; a bootloader
there is plausible, not confirmed. WCH USB library provenance, endpoint behavior, update-file
recognition and Linux/macOS failure causes remain unknown. MBR type/sector geometry,
transfer narrowing and BOT response handling are test hypotheses, not demonstrated device bugs.

The tested read-only SCSI/BOT reference oracle has no original-function credit, physical USB
API or write interface. It supplies future comparison expectations for capacity, READ10,
inquiry, sense/errors, readiness/removal/sync and wrapper framing. It is narrower than a
complete USB endpoint/reset/stall state machine. See [USB investigation](USB_STORAGE_RECONSTRUCTION.md).

## Validation, build and remaining work

All13740 prior differential cases were rerun successfully. The2141 additions cover JPEG
prepare/decompress (3839 actual pixel outputs), filesystem access,1678 pure-helper/partition
comparisons,216 JPEG numeric cases and240 bitmap-path cases.51 distinct original entries
have direct bounded behavior evidence. Tests also cover297 independently decoded images,
corrupt metadata/chains, read failures, missing/invalid resources, persistence, event slots and
reference USB transport.26 C tests,11 Python tests and13 Clang ASan/UBSan tests pass.
The existing ptrace-related LeakSanitizer exception remains; address/UB checks stay enabled.

Both fresh-build ELFs reproduce with the pinned official WCH SDK, RV32IMAC soft-float,
144MHz, origin6400, vectors6438, text6600 and stack20010000. No device-flashable release
BIN or flashing target is produced. Full startup/stack/interrupt timing and calibration have
not been validated on hardware. The supplied meter/ASIC has not been accessed or modified.

Remaining source priorities are9caa integration/extrema, graph strip rendering, menu/base scenes,
monitor/history formatting and target startup. File/JPEG dependencies now exist. Remaining
library internals need per-address qualification before increasing coverage. The alternative
application needs further normalized decoding. USB recovery needs missing bootloader/capture
evidence. Unknown ASIC internals/calibration algorithms remain uninterpreted.

Current evidence: `analysis/continuation_validation.json`, `phase3_validation.json`,
`source_coverage.json`, `library_matches.json` and per-test reports. Before-state reports
are preserved under `analysis/phase3/`. No full Ghidra pass or unchanged decompilation was
regenerated. Local milestones are accessible with `tools/milestone log`.

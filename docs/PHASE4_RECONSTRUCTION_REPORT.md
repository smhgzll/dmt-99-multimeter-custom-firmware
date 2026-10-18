# Unified reconstruction report: Phase4 continuation

This continuation adds **42 complete native original-function translations**, qualifies
three additional FatFs helpers and delivers a C offline simulator with real resources,
PNG screenshots, button/packet/menu/power-cycle input and an isolated custom mode feature.
Incomplete non-trap bodies fall from **185 to 140**. Whole-firmware reconstruction remains
unfinished: the exact menu dispatcher repaint/cache contract and CP936 physical read
boundary are explicit, and target timing/hardware are unvalidated.

| Measure | Unified Phase3 | Current |
|---|---:|---:|
| Decompiled / role-classified original functions | 439 / 439 | 439 / 439 |
| Complete / partial native translations | 87 / 36 | 129 / 29 |
| Official SDK / qualified additional library counterparts | 64 / 17 | 64 / 20 |
| Incomplete non-trap bodies | 185 | 140 |
| Direct original-entry comparisons | 51 | 91 |
| Maintained firmware / host-simulator C units | 32 / 0 | 45 / 3 |
| Differential cases | 15881 | 29421 |
| Stock / custom C test executions | 26 / — | 33 / 36 |
| Python tests / sanitizer C executions | 11 / 13 | 23 / 30 |
| Reproducible replay + retained-module ELFs | stock | stock and custom |

The original Phase3 report is preserved as [PHASE3_RECONSTRUCTION_REPORT.md](PHASE3_RECONSTRUCTION_REPORT.md),
with completed-state machine-readable baselines under `analysis/phase4/`.
The 129 native entries, 20 additional library counterparts and 64 SDK counterparts are
separate metrics. The conservative semantics floor 299 includes 86 traps, which do
not count toward application completeness. Compilation, bounded behavior tests and
hardware validation remain distinct. Simulator/custom functions earn no original credit.

## Connected recovered application

The maintained `9caa` path now connects packet text, relative/Fahrenheit values, both
numeric renderers, gauges, extrema, mode resets, graph strips and monitor alarms.
Native graph code preserves the index-zero preceding slot, 196 samples/modulo197,
uint16 count wrap, actual a1 reset and inherited s5 seed. Resistance stack bytes that
the binary reads before initializing are explicit input state. Those contracts are
retained rather than replaced by a guessed plotting/ranging algorithm.

Recovered scenes include mode/HOLD/REL labels, both layouts, categories/view choices,
threshold hints/JPEG variants, settings selections, theme/language, brightness/temperature,
timeout/reset/base screens, unit caches, monitor threshold/alarm images and history
formatting/highlighting. Original resource names, coordinate ordering and observed
label/cache quirks are retained. The cooperative `screen_menu` still repaints whole
scenes; full `169d4` transition/erase/cache ordering and every legacy-language path
remain partial. See [UI recovery](UI_RECOVERY.md) and [scheduling](SCHEDULING_SEMANTICS.md).

## Offline simulator and custom profile

The C simulator runs the controller, FatFs/TJpgDec and acquired Flash resources into
an RGB565 framebuffer and exports PNGs. CLI scripts and a localhost browser panel
provide explicit button, measurement, ACK, menu, charging, timer and power inputs.
No ASIC response or analog algorithm is synthesized. [The guide](OFFLINE_SIMULATOR.md)
contains build/replay commands, input semantics and examples. Exported
[regular](simulator_screenshots/regular.png), [recording graph](simulator_screenshots/recording-graph.png),
[settings](simulator_screenshots/settings.png), [monitor](simulator_screenshots/monitor.png)
and [classic menu](simulator_screenshots/classic-menu.png) frames come directly from C.

`STOCK_COMPATIBLE` and `CUSTOM_DMT99` have separate outputs. Custom remembers the
last acknowledged supported mode in a separate CRC-protected 16-byte record, restores
its observed command, aligns selectors and waits for ACK. Stock never invokes the
feature backend. Memory or simulator-file persistence is used; no device record
placement is assumed. Tests cover corruption, view mismatch, failed writes, reset,
selector continuity, stock isolation and power cycles.

## Qualified libraries and CP936

Three reviewed FatFs helpers add 1164 comparisons: drive-prefix parsing and directory
cluster load/store. The original cluster-load signature scalarizes fs_type; the native
oracle adapts it explicitly. Compiled object/opcode/CFG evidence supplements direct
original instruction tests. There are now 1 Exact and 19 Strong semantic additional
library counterparts; other imported routines do not gain automatic credit.

CP936 defined returns and the first invalid-table read are recovered. Most non-ASCII
paths read `23fe4` twice and then attempt `80023fe4`; the exceptional key031a returns
zero. The adapter reports that fault without guessing physical alias/bus behavior or
installing a generic mapping. One non-ASCII filename remains unavailable through stock
FatFs, while its extracted JPEG remains validated. See [library evidence](LIBRARY_SOURCE_RECOVERY.md).

## Validation and preservation

The RV32 oracle now sign-extends compressed LUI immediates from original halfwords.
All 15881 existing differential cases pass with that correction; 13540 new cases cover
UI leaf calls, graph state/pixels/blits, reading/extrema, gauges, monitor/history,
codepage paths and qualified library helpers. There are 91 directly compared original
entries. All per-test reports require the current native library hash.

Stock/custom C executions are 33/36, Python tests23 and ASan/UBSan executions30.
The fixed ROM helper keeps pointer arithmetic within the selected array; sanitizers
now halt on errors, and validation also scans their logs. Only the existing ptrace
LeakSanitizer exception remains. A temporary loopback HTTP test verifies the actual
C state/PNG response; full browser JavaScript automation and physical display timing
are not claimed.

Both ELF builds reproduce in both profiles with the pinned official WCH SDK and
original link geometry. The retained stock artifact has 124,876 code/constant bytes,
268 initialized-data bytes and 592 BSS bytes, plus the 2048-byte stack reservation.
The host framebuffer is outside that target memory model. No release BIN or flashing
target exists. Successful compilation/linking is not hardware readiness.

The original BIN, 885 baseline evidence files, 439 decompilations, instruction inventory,
Flash image/resources and previous forensic findings are preserved. No full Ghidra
pass or unchanged decompilation was regenerated. The first Flash reading remains
untracked and unchanged. No second acquisition is claimed.

## Next evidence and deferred work

Continue exact menu transition/cache recovery, remaining application glue and evidence-
qualified library/compiler matching. The full CH32V303 dump can be compared incrementally
with `tools/ingest_mcu_dump.py`, using an explicit application-offset hypothesis.
No earlier analysis must be restarted. Definitive bootloader work, actual USB MSC,
internal Flash layout confirmation and hardware update integration are deferred until
that evidence is supplied. No missing bootloader behavior is inferred.

Authoritative metrics: [Phase4 validation](../analysis/phase4_validation.json),
[continuation validation](../analysis/continuation_validation.json),
[source coverage](../analysis/source_coverage.json) and per-test reports.
See [remaining functions](UNRESOLVED_FUNCTIONS.md) and [next actions](NEXT_STEPS.md).
Local Git milestones use `tools/milestone log`.

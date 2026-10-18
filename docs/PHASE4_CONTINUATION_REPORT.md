# Unified reconstruction report: Phase4 continuation

The menu dispatcher now uses the original ordered regional erases, repaints and cache
checks in the C simulator. This continuation completes **11 of the29 partial native
translations** and adds the six-row threshold wrapper `1a336`: **141 complete native,
18 partial, 128 incomplete non-trap bodies** remain. The incomplete count includes
partial entries. The entire firmware is still incomplete and hardware is unvalidated.

| Metric | Previous Phase4 | Current continuation |
|---|---:|---:|
| Complete / partial native original entries | 129 /29 | 141 /18 |
| Incomplete non-trap bodies, including partial | 140 | 128 |
| Official SDK / qualified upstream counterparts | 64 /20 | 64 /20 |
| Conservative detailed-semantics floor, including86 traps | 299 | 311 |
| Distinct compared original entries across milestones | 91 | 100 |
| Newly compared entries in the current targeted run | — | 12, including9 new entries |
| Firmware / host-simulator C units | 45 /3 | 46 /3 |
| Historical full-suite differential vectors | 29421 | preserved |
| New menu / glue vectors | — | 6276 /1152 |

Counts refer to original entry bodies, not code-byte coverage. Full source credit covers
the documented valid input and external-call contracts. A completed startup caller does
not complete its peripheral callees. The trap floor does not count as application recovery.
The [previous Phase4 report](PHASE4_RECONSTRUCTION_REPORT.md) and machine-readable
baselines under `analysis/phase4_continuation/` remain preserved. Historical reports are
not relabeled as current-library test runs.

## Recovered behavior

`ui/menu.c` and `ui/menu_render.c` connect `1598c`, `15af6`, `164d4`, `169d4` and
`1a336`. The old full-screen observer has been replaced by incremental rendering.
Cache hits do no resource/pixel work; cursor transitions erase the original peers,
settings-page navigation preserves original order, BACK remains pending, and menu
exit clears/base-paints before resetting state/page and invalidating caches. Default
reset returns to the host scheduler without painting another menu scene, matching
the original nonreturning application-loop boundary. Supported menu domains are
layouts0/1, UI languages0/1, views1..3 and bounded history0..30.

`application_startup` recovers `908a`'s eleven calls with original priority, baud and
timer parameters. Typed callbacks retain every callee address and fail before any
side effect if a binding is absent. The offline executable does not activate this
startup path. `97f8` now implements the10ms debounce, exact GPIO masks, conditional
command packets and event clearing. No diagnostic/ASIC command meaning is invented.

Other completions cover all eight `7e68` scan directions/defaults, the `792e`500ms
beep, `d964`'s independent mutable language-prefix cache and invalid-selector behavior,
`da48`'s timeout side effects/spill semantics, and `dc46`'s defaults/logo/loaded-flag
ordering. Device placement, clock/CSR and programming behavior remain boundaries.

The connected simulator also restores the enclosing button handler's behavior after
menu exit: a recording-origin menu resets extrema, and a monitor-origin menu resets
its selector/state. Those enclosing button entries remain partial pending their complete
composed original-entry comparison; recovered leaves alone do not complete them.

## Verification and preservation

The targeted menu oracle executes original BIN instructions and compares ordered display
calls, settings/state/page/cache stores and state visible at the base-screen call. Its
6276 cases include all32 IDs, events0..6, both layouts/languages, all three views,
brightness/timeout values, cache hits/misses and nonempty/clamped history. Already
qualified threshold/history/version/base routines are explicit call boundaries.
The1152 glue vectors compare startup, forwarding, display, language, timeout, defaults
and beep. Restart comparisons stop at the original nonreturning boundary. Tests remain
bounded; they do not prove interrupt, busy-loop, resource-decoder or panel timing.

Stock and custom are built and checked separately:28/31 relevant C executions,7428
differential vectors per profile, and33 combined ASan/UBSan executions pass. Both
offline ELF pairs compile/link with the original geometry; reproducibility was not
rerun in this targeted continuation. The current run uses only tests
related to menu/controller/settings/display changes; the29,421-vector Phase4 full-suite
report remains historical evidence. Current detailed command results and hashes are
in `analysis/phase4_continuation_validation.json`. The source ledger is
`analysis/source_coverage.json`; per-entry recovery is
`analysis/phase4_continuation_function_recovery.json`.

All885 baseline evidence files, the original V2.4 BIN and the8MiB Flash image retain
their hashes. No disassembly/decompilation was regenerated. The Flash dump remains
read-only input and untracked. Stock/custom feature separation, mock persistence and
existing custom reset/power-cycle behavior are retained. No hardware was accessed and
no device-flashable release was produced.

## Remaining work

[The ranked140-body baseline](FUNCTION_PRIORITIES.md) records current status, dependencies,
callers, body sizes and reachability. The first remaining tasks are `8ed4` application
and diagnostic glue, `c248` startup/settings/language ordering, `15168` language polling,
`14cd6` charging restart, and the three enclosing button handlers. The regular-mode
`a910` live/NCV exit/debug side effects and general PWM setup also remain partial.
CP936's first unmapped physical read remains unresolved; no generic conversion table
has been installed. Calibration and ASIC internals remain uninterpreted.

RDP prevents the independent MCU dump at present. Application recovery continues with
the existing evidence. Definitive bootloader, actual USB MSC, internal Flash layout
confirmation and hardware update integration are deferred. No missing bootloader code
is inferred. A future dump can be integrated incrementally with the existing ingest tool.

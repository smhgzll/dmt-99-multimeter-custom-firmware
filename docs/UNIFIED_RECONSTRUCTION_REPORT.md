# Unified reconstruction report: ASIC control continuation

The original application caller, diagnostic entry and three measurement button handlers
now have complete C translations within their documented external-call contracts.
A portable ASIC driver sends only packet forms demonstrated by the V2.4 binary and
parses the recovered LCD/event fields. The physical peer has not been accessed; command
acceptance, analog behavior and UART timing remain unvalidated.

| Metric | Previous continuation | Current |
|---|---:|---:|
| Complete / partial native original entries | 141 /18 | 146 /14 |
| Incomplete non-trap bodies, including partial | 128 | 123 |
| Official SDK / qualified upstream counterparts | 64 /20 | 64 /20 |
| Conservative detailed-semantics floor, including86 traps | 311 | 316 |
| Distinct compared original entries across milestones | 100 | 102 |
| Original entries checked by this milestone's new tools | — | 7 |
| Firmware / host-simulator C units | 46 /3 | 48 /3 |
| New composed-button / ASIC-foreground vectors per profile | — | 5472 /1240 |
| Menu / preserved-parser regression vectors per profile | — | 6276 /1024 |
| Relevant C tests, stock / custom | 28 /31 | 34 /37 |
| ASan/UBSan C executions, both profiles combined | 33 | 45 |

Partial entries are included in the incomplete total. Counts are original entry bodies,
not code-byte coverage. Complete source credit covers a stated valid input domain and
external-call contract; it does not complete the callee internals or establish timing
or device equivalence. Traps and newly written API/simulator/example code do not inflate
native application recovery. [The preceding continuation report](PHASE4_CONTINUATION_REPORT.md)
and `analysis/asic_baseline/` preserve the141/18/128 state.

## Application and driver behavior

- `8ed4`: typed GPIO entry selection, normal loop and diagnostic loop, including the
  original nonreturning reference. Normal flow dispatches buttons before parsing,
  copies16 saved-event bytes, honors HOLD and orders charging/battery work. Diagnostic
  flow skips its first pending event and repeatedly renders without requiring a new IRQ.
- `b56a`: exact05/0b packet,200ms delay, then02/a0 packet. A0's functional meaning
  remains unknown. The connected simulator can explicitly select this boot path.
- `b5c8`, `b9e0`, `bc90`: complete button/menu entry/return, selectors, relative and
  recording/monitor side effects. The original menu dispatcher executes inside the
  binary comparisons; already-qualified base/history calls are explicit boundaries.
- The portable driver validates19 observed packet forms and provides mode/HOLD-related
  requests, accepted diagnostic-key encoding, explicit diagnostic replay and RX parsing.
  A successful send means local transport acceptance only. No response, timeout,
  retry, range policy or calibration is synthesized.

The16-byte parser comparison exposed a second NUL store omitted by the earlier native
adapter. The shared portable parser now preserves both stores and the stale text tail.
The normal controller also invalidates the original shared label cache after consuming
a saved FD event. Existing stock/custom mode-persistence separation remains intact;
diagnostic boot does not load settings or invoke custom restore/save callbacks.

See [the ASIC control report](ASIC_CONTROL_REPORT.md) for the packet table, SDIC protocol
comparison, minimal C example and logic-analyzer capture matrix. Reviewed public SoC
sheets provide no matching command table; the reviewed SD7501A6 application's15-byte,
2400-baud protocol differs. No silicon identity or command semantics were imported
from that comparison. Peer events are not demonstrated correlated acknowledgements.

## Validation and build

Both profiles pass5472 composed-button and1240 ASIC/foreground comparisons against
original BIN instructions. The latter cover B56A, accepted-key packets,8ED4 boot/normal/
diagnostic paths and the complete16-byte reading buffer. The6276 menu and1024 preserved
parser vectors per profile are regression reruns, separate from the6712 new tool vectors.
Custom repeats are not counted twice. Earlier29,421 Phase4 and7428 continuation vectors
remain historical evidence with their original library hashes; they were not all rerun.

The relevant C executions pass34 stock /37 custom; ASan/UBSan pass21 /24. The existing
ptrace-related leak-check exception remains, with fail-fast address/UB checking enabled.
The standalone driver/example builds without GUI, filesystem, application or SDK code.
The simulator exercises real Flash resources, diagnostic buttons/packets/repeated renders,
menu behavior and stock/custom persistence isolation. Virtual delay is not UART timing.
Both C CLI profiles also pass diagnostic entry, forwarded key, reading, repeated step
and PNG export smoke checks; their states and artifact hashes are recorded separately.

Both profiles cross-build and link the replay and retained-module ELFs against the
pinned WCH SDK. The original6400 entry,6438 vectors,6600 text and20010000 stack-top
assertions pass. Stock retained modules contain131400 bytes of code/constants,296
initialized data and592 actual BSS plus2048 reserved stack; custom code/constants are
132052 bytes with the same data/BSS. Fresh-build reproducibility was not rerun in this
milestone. No hardware-flashable release or programming target is produced.

Current commands/results, input/build hashes and preservation checks are recorded in
`analysis/asic_validation.json`. Per-entry scope is in `analysis/asic_function_recovery.json`
and the maintained `analysis/source_coverage.json`. All885 baseline evidence hashes,
the original BIN,8MiB Flash and pinned library sources are preserved. No disassembly
or unchanged decompilation was regenerated. Local Git milestones use `tools/milestone`.

## Remaining boundaries

The14 partial translations include settings/language/charging startup, regular live/NCV
event glue, PWM/panel setup, formatting, device power/settings programming and CP936
physical reads. [The priority report](FUNCTION_PRIORITIES.md) retains the140-body baseline
with123 currently incomplete bodies and their unresolved direct dependencies.

Known bytes suffice for a tested host-side protocol driver, but do not establish that
all requests control the analog ASIC as their UI labels suggest. A0,01/08 physical
functions, legacy00 acceptance, payloads4..8, RX bytes9..12, integrity rules, actual
framing and notification causality need physical traces. Unknown ranges/calibration
remain uninterpreted. A subsequent [RDP/update feasibility audit](FIRMWARE_DUMP_FEASIBILITY.md)
and isolated offline exporter now exist; their tests do not increase function coverage.
Bootloader/actual USB MSC recovery, device Flash layout confirmation and firmware
flashing remain unresolved. Active-device payload use is NO-GO. No physical hardware was touched.

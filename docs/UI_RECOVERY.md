# UI recovery and original-code contracts

`analysis/phase4_function_recovery.json` records each newly completed original
entry and its valid domain. New simulator, PNG and custom-feature functions earn
no original-function coverage. The original instruction inventory, raw image and
all decompilations remain preserved.

`ui/reading.c` implements `9caa`: raw segment text, numeric/relative/Fahrenheit
formatting, both numeric renderers, gauge normalization, unit-aware extrema, mode
change resets, graph updates and the monitor-alarm call. The controller binds the
alarm callback so its three images/buzzer changes precede the unit renderer,
matching the original call order. The resistance branch reads stack bytes before
initializing them in the binary. `resistance_guard` and `scratch_unit` expose that
input; zero initialization in the simulator is an explicit host choice.

`ui/graph.c` translates `17ab2`, `17c06`, `17c8c`, `17cce`. The ring index wraps
modulo 197, while the sample bank has 196 floats. Index zero writes the preceding
word; native C models that as `leading_sample` instead of an out-of-bounds access.
The uint16 count wraps. The reset condition actually reads `a1`, and the maximum
seed arrives in inherited `s5`. Those inputs are preserved, not corrected into a
different plotting algorithm. Eight 20-row strips use a column-major scratch bank
and row-major display output. Negative/overflow coordinate conversions follow the
observed unsigned soft-float helper before the original uint16 clamp.

`ui/gauge.c` recovers gauge geometry, packed ROM bitmaps and the legacy bar path.
One pixel comparison identified the exact double constant 2.01; zero has the
separate original threshold 205. `scene_rom.c` contains exact ROM fragments with
hashes in `analysis/scene_assets.json`; the existing numeric glyph bank is reused.
`tools/recover_scene_assets.py` verifies those bytes by default and rewrites only
when explicitly passed `--write`.

`ui/menu_scene.c`, `settings_scene.c`, `screen.c` recover category/view choices,
threshold hints and JPEG variants, theme/language, brightness/temperature,
timeout, reset, base and mode/HOLD/REL scenes. Settings selection preserves stock
side effects and quirks: selecting classic Chinese theme2 prints label1 in the
original helper; the compact English previous Recording row uses the Monitor y.
`ui/units.c` preserves the shared cache codes, conditional clears, mode-specific
unit eligibility and stale temperature symbol until cache invalidation. Some
recording-mode English filenames reverse voltage AC/DC words. Resource names are
retained exactly rather than interpreted as corrected artwork.

`ui/monitor.c` connects signed threshold words, current hundredths, the observed
temperature conversion, equality behavior, buzzer and ordered image triplets.
`ui/history_view.c` recovers record-page modes/units/overload strings and formatting,
35/42-row spacing, scrollbar requests, eight-row recording highlights and previous
unit/text caches. The count%5 empty-final-page quirk remains in the browser policy.

The dispatcher now uses `menu_step_rendered` and `menu_render.c` instead of the
full-scene observer. Original `1598c`, `15af6`, `164d4`, `169d4` and `1a336` have
ordered erasure/repaint/cache contracts for supported UI languages/layouts. All32 IDs
and events0..6 are compared against original instructions, including cache hits,
brightness, pending BACK, language/layout changes, reset and exit. The original
first Chinese heading colon has background001f; retaining that argument was required
by differential evidence. Editor/history/base calls are declared boundaries whose
leaf implementations have separate evidence. Busy-loop and interrupt timing remain
outside these comparisons. See `analysis/menu_differential.json` and the current
[unified report](UNIFIED_RECONSTRUCTION_REPORT.md).

The differential tools execute original machine instructions and compare ordered
primitive/resource calls, pixel streams, arithmetic or state. They isolate declared
external calls and check bounded domains. They are not comparisons to a second
implementation of Ghidra pseudocode. `tools/rv32_trace.py` now decodes the signed
RV32C `c.lui` immediate from the original instruction halfword. A preserved text
operand such as `0x3e` must become `-2`, not positive `62`. Tests check every such
instruction and all previous regression vectors are rerun with that correction.

For CP936, `fc9e`/`fcfc` have an empty-table upper bound `UINT32_MAX`. Most non-ASCII
inputs read `23fe4` twice, then attempt `80023fe4`, outside supplied ROM. The
exceptional key `031a` returns the zero value at `23fe6`. ASCII and wrong-page
returns are defined. `storage/codepage.c` preserves those paths and exposes the
first unknown read; FatFs fails closed. No alias, bus response or generic CP936
mapping is assumed. One Chinese-named resource remains available only through the
forensic extraction path. Physical compatibility beyond that read needs evidence.

# Reconstruction status

**Release 0.2.0 (18 October 2026): USB update image.** The firmware is now
published as a USB update file in the same format as `DMT-99_V2.4_20240306.bin`
(application at 0x08006400, identical header and vector-table position), to be
installed by the meter's own bootloader, which is kept. Release files and
installation steps are in [`final/`](../final/README.md); the About page shows
`DMT-99 V0.2.0`. Emulator-verified only. Details:
[FIRMWARE_RELEASE.md](FIRMWARE_RELEASE.md).

**Standalone firmware 0.1.0-rc1 (October 2026).** The reconstruction now builds
a complete CH32V303RCT6 image (`make -C reconstructed firmware-all`). All 439
V2.4 functions have a firmware disposition in `analysis/firmware_coverage.json`:
190 native translations, 8 native with documented deviations, 131 upstream
library/SDK counterparts, 22 helpers made unnecessary (heap, printf internals,
millicode), 86 unused vector slots, 2 evidence-limited (CP936 tables absent).
The image matches the V2.4 machine code at register level during start-up and
matches the V2.4 image itself screen-for-screen in the board emulator. It has
not run on hardware. Details: [FIRMWARE_RELEASE.md](FIRMWARE_RELEASE.md),
[FIRMWARE_VERIFICATION.md](FIRMWARE_VERIFICATION.md), [FEATURE_MATRIX.md](FEATURE_MATRIX.md).

The section below is the preceding host-reconstruction status, kept for its metrics.

The subsequent [firmware update/dump feasibility audit](FIRMWARE_DUMP_FEASIBILITY.md)
adds a separate offline C export protocol and host receiver. Its six Python tests,
C test/sanitizer execution and41 original settings entry comparisons pass. There is
no new original-function coverage credit. Current preservation and experiment results
are in `analysis/flash_export_validation.json`; prior application validation remains
in `analysis/asic_validation.json`. Active-device update/export remains NO-GO.

Current source coverage is **146 complete native /14 partial /123 incomplete non-trap
bodies**; partials are included in the incomplete total. The439 original functions remain
decompiled and role-classified. There are64 official SDK and20 qualified upstream
counterparts. The conservative detailed-semantics floor316 includes86 traps, which
do not count as native application recovery. Whole-firmware equivalence is unclaimed.

This milestone completed `8ed4`, `b56a` and the three enclosing measurement button
handlers `b5c8/b9e0/bc90`. It adds an independent C ASIC driver accepting only19
observed packet forms and the recovered RX fields. The normal/diagnostic loop reference
and connected cooperative simulator preserve documented caller behavior. Complete
callers do not complete their peripheral callees or establish device timing/analog behavior.
See the [ASIC control report](ASIC_CONTROL_REPORT.md) and [unified report](UNIFIED_RECONSTRUCTION_REPORT.md).

Current validation is `analysis/asic_validation.json`:6712 new original-code tool vectors
per profile,6276 menu and1024 parser regression vectors per profile,34/37 relevant C
executions and45 combined ASan/UBSan executions. Custom repeats do not inflate vector
counts. There are102 distinct original entries with bounded comparison evidence across
milestones;7 are checked by this milestone's new tools. Earlier reports retain their
historical hashes/results and are not presented as full current-library reruns.

The source ledger is `analysis/source_coverage.json`; per-entry scope and boundaries
are in `analysis/asic_function_recovery.json`. The141/18/128 baseline is preserved under
`analysis/asic_baseline/` and in [the preceding report](PHASE4_CONTINUATION_REPORT.md).
All885 baseline evidence files, BIN/Flash and pinned libraries retain their hashes.
No full Ghidra pass or unchanged decompilation was rerun. Both stock/custom offline
ELF pairs compile/link. Reproducibility was not rerun this milestone. Local Git commits
use `tools/milestone`.

The remaining14 partials cover startup/settings/language/charging waits, regular-mode
UI glue, PWM/panel integration, formatting, device power/programming and CP936 reads.
[Ranked dependencies](FUNCTION_PRIORITIES.md) record123 incomplete non-trap bodies.
Command acceptance, analog selection, unknown diagnostic/control fields and UART timing
require real two-direction logic-analyzer capture. No ASIC identity or calibration
algorithm is invented. The read-only RDP/update audit is documented separately;
bootloader/USB implementation recovery, device layout confirmation and flashing
remain unresolved. Hardware readiness and complete firmware equivalence are not claimed.

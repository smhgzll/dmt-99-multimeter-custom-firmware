# Reproducible workflow

`python3 tools/analyze_firmware.py --phase all` verifies the input, imports/analyzes with headless Ghidra,
exports disassembly and pseudocode, builds/tests recovered modules and writes reports. It never accesses hardware.
`GHIDRA_HOME` selects another installation. `make -C reconstructed CROSS=/path/to/riscv-prefix- all host-test`
selects another compatible compiler. Java must include a JDK; JAVA_HOME can be supplied explicitly.

`tools/fetch_sdk.py` obtains the pinned WCH revision and records hashes. Both the original fetched SDK and
the standalone project's vendor subset are included. WCH's notices are preserved; no substitute SDK is used.

## Ghidra stages

1. `prepare_ghidra.py`: derives a local RV32IMAF+C+Xwchc SLEIGH language from installed Ghidra sources.
   Uses the soft-float ABI and four GCC t0 save-helper callfixups; original installation stays unchanged.
2. `DmtSetup.java`: imports at 0x6400, separates nonexecuting vectors/rodata, initializes the SRAM load image,
   sets gp context and seeds reset/main/vector functions.
3. `DmtRecover.java`: finds validated save-helper prologues and pointer-table prologues, installs callfixups,
   marks MMIO volatile and repeats constant/switch analysis. This is essential: treating MMIO as ordinary memory
   incorrectly collapses polling loops, and ordinary C-call semantics corrupt helper argument tracking.
4. `DmtTypes.java`: applies curated protocol/history/register structs and function signatures.
5. `DmtExport.java`: exports listing instructions, functions, references, callgraph and per-function C-like output.
6. `enrich_analysis.py`: adds confidence/evidence, semantic pseudocode, MMIO groups, SQLite xrefs and reports.

Ghidra completed every discovered decompilation. Success means that C-like output was produced, not that every
type, return value or expression is verified. All warnings remain visible. The `clean` tree contains readable
semantic pseudocode; independently maintained native C is in `reconstructed/`.

## Incremental use

- `--phase forensics`: fast hash, startup, entropy, strings, memory map and vector reports.
- `--phase ghidra`: skip unchanged imports/exports using input/script/semantic fingerprints.
- `--phase ghidra --refresh`: reuse the project and regenerate semantic exports after a registry/type change.
- `--phase ghidra --force`: repeat a full import; appropriate after memory-map or language changes.
- `--phase reports`: regenerate confidence/status reports and SQLite references.
- `--phase validate`: compile, link, test, compare and verify repeat-build hashes.

Raw decompilation caching includes body bytes, function signature, language, curated names, globals, types,
signatures and save-helper definitions. An unchanged function is reused; semantic dependency changes invalidate
the relevant export epoch conservatively. Ghidra projects remain in `analysis/ghidra_projects/` for resumption.

## Validation boundaries

The small interpreter supports only instructions needed by five isolated differential targets and fails on
unsupported opcodes. It is not a peripheral, floating-point, timing or whole-device emulator. Decoder tests use
LLVM's independent assembler to produce boundary and one-hot offsets for all eight WCH encodings, then verify Ghidra operands
and pcode access widths. SDK byte comparisons are local function-body evidence; unequal code can still be
equivalent, and byte-identical helpers do not establish equivalence of callers or the whole application.

Build/test output and selected failed attempts are retained in `logs/`. The original BIN hash is checked before
analysis and validation and its file permissions are read-only. No programmer utility or hardware action exists
in the automated workflow. Private/device-specific calibration values are absent and never synthesized.

## Incremental continuation workflow

The current command is `python3 tools/validate_continuation.py`, with optional `--reproducible`.
It audits885 baseline evidence hashes, builds the native application/SDK modules, runs19 host tests
and13,740 differential cases, and caches the expensive application comparisons by source/evidence
fingerprint. It never invokes full Ghidra analysis. `analysis/progress.json` remains the earlier
baseline; `analysis/continuation_validation.json` is current.

`semantic_continuation.json` overlays186 role reviews. `source_coverage.json` tracks complete/partial
translations per original function address. The role count is explicitly distinct from detailed
semantic recovery, compilation and behavioral evidence.

An NCV test reached FF48..1014C, previously omitted from the exported instruction inventory.
`DmtExportGap.java` opened the saved project with `-noanalysis -readOnly`, cleared only that interval
in a transient listing, exported its instructions and discarded changes. The overlay has140 new
instruction records/516 bytes plus one already-known instruction. Original exports remain unchanged.
Do not run the older `analyze_firmware.py --phase all` merely to validate this continuation: its
initial-analysis workflow may refresh unchanged Ghidra outputs when script fingerprints change.

Git milestones use `tools/milestone` with an ignored `.history/git`, because `.git` is a read-only
empty workspace placeholder. The initial commit preserves the complete supplied baseline.

# Library source recovery

Official release archives were fetched from ChaN's [FatFs archive](https://elm-chan.org/fsw/ff/archives.html)
and [TJpgDec archive](https://elm-chan.org/fsw/tjpgd/archives.html). Archive SHA-256s,
URLs and sizes are pinned in `references/upstream_archives/manifest.json`; compiled
source hashes and local changes are in `references/recovered_library_manifest.json`.
Authorship and redistribution terms remain in every imported source. Downloaded
archives/extracted documentation are cached and ignored; selected source is tracked.

The original's124-byte JDEC object,32-bit width/height, pool at6c and callback/device
at74/78 identify the R0.01b configuration family. R0.01c/d layouts and R0.02a entropy
state differ. R0.01b's alloc_pool compiles byte-identically to original6838; the
Ipsf/Zig/Clip8 tables match exactly. Three actual resources produce identical prepare
status, workspace use and all3839 RGB565 MCU pixels under original instruction
execution. Five original decoder functions receive qualified upstream counterparts.
The decoder algorithm source is unchanged; RGB565 is selected and integer types are
bound to their upstream-required widths on LP64 hosts and RV32.

FatFs R0.14b's VBR checks require reserved sectors and minimum total sectors as seen
at e1bc. R0.14/a lack those checks; R0.12c/13c partition builders differ. RV32 FIL size4136,
fptr14, FATFS window38 and n_fatent1c agree. Original mount/open/read/seek/close execute
against the actual image, independently reproducing file bytes and missing-file errors.
Pure LE load/store, SFN checksum, cluster conversion and Unicode uppercase add1670
comparisons; two partition entry points add8 complete result/write comparisons on
four maps. All12 qualified FatFs counterparts are listed individually.

There are17 new qualified function counterparts:1 Exact and16 Strong semantic.
Exact means complete compiled function-body bytes match. Strong semantic combines
algorithm/layout/constants with bounded original-code behavior; it does not assert
identical timing or every malformed input. These17 are counted separately from
native application translations. Unmatched functions in the full imported FatFs
source compile, but do not automatically become completed original-function coverage.

`library_fingerprints.py` compiles source, reports exact bytes, normalized opcode
sequences (including preserved Xwchc decoding), constants and tables. `library_cfg.py`
adds linked CFG shape, back edges and memory-offset histograms. Differences, inlined
symbols and low similarity remain visible. Neither opcode score nor CFG shape is
used alone as source/version proof. The selected historical branches are strongly
supported; original vendor archive/compiler provenance remains unknown.

Original CP936 conversion code has a degenerate binary-search bound and no valid
mapping table at23fe8. Non-ASCII execution can make wrapped out-of-ROM accesses.
A generic full CP936 table would silently change that behavior, so the adapter fails
closed and reports fc9e/fcfc. One Chinese-named JPEG cannot be opened through this
binding; it is extracted/validated separately. It is not counted as working lookup.

The existing official WCH SDK64 comparisons/19 exact bodies remain preserved.
SPI NOR/graphics upstream provenance, exact GCC/newlib versions, bootloader and
USB library source remain Unknown. No WCH USB example is substituted without a
matching device executable.

## Phase4 extension

Three further FatFs R0.14b helpers are qualified by explicit binary evidence:
`dec0` → `get_ldnumber`, `e0f8` → `ld_clust`, and `e116` → `st_clust`.
There are now 20 qualified library counterparts (1 Exact, 19 Strong semantic),
counted separately from native C. The original `ld_clust` entry receives a
compiler-scalarized `fs_type` integer; the test adapter constructs the authentic
`FATFS` argument explicitly. This ABI difference is retained in the match record.
1,164 comparisons cover return values, whole directory-entry bytes and path
pointer advancement. Original code, configured upstream object bytes, normalized
operations/CFG and field offsets are preserved in
`analysis/library_extension_matches.json` and
`analysis/library_extension_differential.json`. Other imported helpers receive
no new coverage automatically. `qualify_library_extension.py` appends only these
reviewed mappings and requires current original-code test evidence.

The CP936 probe now distinguishes defined ASCII/wrong-page/exceptional-key returns
from the first unknown read at `80023fe4`; see `docs/UI_RECOVERY.md`. A generic
conversion table is still not substituted, and one filename remains unavailable
through the stock binding. The original 17-match baseline is preserved under
`analysis/phase4/baseline_library_matches.json`.

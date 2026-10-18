# Consolidation of the five projects

2026-10-18: the main reconstruction workspace was organized as a source
repository suitable for GitHub. Firmware algorithms, ASIC commands, the
menu dispatcher and stock/custom policy separation were unchanged.
No hardware operation, flashing or new long RV32 emulator session was performed.

## New layout

| Former project / path | Consolidated location |
|---|---|
| Main analysis | `reconstructed/`, `analysis/`, `decompiled/`, `disassembly/`, `tools/`, `docs/` |
| dmt_99_doom | `projects/doom/` |
| dmt99-studio | `projects/studio/` |
| dmt_99_asic_analysis | `projects/asic/` |
| dmt99-rvswd | `projects/rvswd/` |
| final/v* | `artifacts/firmware/potat-99/v*` |
| DOOM dist/v* | `artifacts/firmware/doom/v*` |
| Original BIN / flashdump | `artifacts/firmware/original/`, `artifacts/flash/original/` |
| Captures, extraction and themes | `artifacts/captures/`, `artifacts/flash/extracted/`, `artifacts/themes/` |

The four former sibling projects were **left unchanged** and are not runtime
or build dependencies of the consolidated sources. Existing main/DOOM
release contents were preserved. Former paths have relative symlinks that
stay inside the repository. New release tools use central `artifacts/` paths directly.

No persistent `THEME.PKG` release was found in the five project trees.
Only a new **78-byte palette example** was added to the central theme folder;
no recovered or physically tested theme release is claimed.

The previous main worktree was backed up outside the repository before
migration. Earlier `.history/` milestones remain local. Nested Git stores,
IDE settings, Python environments, SDK build caches and the upstream demo
video were not imported. The root `.git` is a fresh public-source history;
private history and old binaries were not automatically imported.
Nothing was pushed to a remote.

## Privacy and evidence boundaries

- Fixed user/computer paths were removed from source, Makefiles and documents.
  Toolchains are selected from PATH or the current user's tools installation.
- Relevant historical **text metadata** was converted to relative notation.
  Before/after hashes are in `integration/metadata_normalization.json`;
  these edits are not new experiment or analysis results.
- The original BIN, Flash dump, raw logic/sigrok samples and published files
  were unchanged. Full sessions, private USB/host inventory, older logs,
  debug ELFs and maps remain outside Git locally; these may retain original
  acquisition paths. Immutable debug files were not rewritten and their
  release hashes were not invalidated.
- Root Git contains selected source/documents, small regression excerpts,
  gallery images and selected release metadata. Uploading the entire local
  archive as a source ZIP would bypass this privacy filter.
- Licenses and attribution are retained. The root notice does not relicense
  all research/game data; raw manufacturer/game data is unpublished by default.

`tools/audit_repository.py` checks selected files for personal absolute
paths, certain key/token patterns, primary-guide local links and SHA-256
integrity. This limited automation is not a complete legal or security audit.

## Recorded validation results

| Check | Result |
|---|---|
| Original BIN/NOR, decompiled/disassembly and core instruction/function inventory | **884 files retain their SHA-256** |
| Imported releases and complete ASIC/RVSWD sessions | **637 unchanged files**, `integration/artifact_preservation.json` |
| POTAT-99 clean cross-build | **141868 bytes**, byte-identical to preserved 0.6.0 |
| DOOM cross-build in new location | **163444 bytes**, byte-identical to preserved 0.4.0 |
| Firmware STOCK_COMPATIBLE check | 13 core + 9 application/storage + 6 resource + 2 profile + 7 simulator checks passed |
| Firmware CUSTOM_DMT99 check | 13 core + 9 application/storage + 6 resource + 5 profile + 7 simulator checks passed |
| Studio native tools / GUI pytest | Build passed; **41 tests passed** |
| DOOM tests | **4 C groups, 6 host tests and 6000 upstream collision/raycast comparisons passed** |
| Independent ASIC driver | RV32 archive built; **31 Python/C tests passed**, including ASan/UBSan stream testing |
| RVSWD diagnostics | **3 C test executions and 34 Python tests passed** |
| ESP32 default | **ESP-IDF 5.5.5**, classic ESP32 build passed; lab/activation/halt gates disabled |
| Gallery | Existing POTAT/DOOM PNGs reused; two short captures from the actual offscreen Studio Qt UI |
| Build from Git-selected sources in a separate directory | Firmware, DOOM, ASIC and Studio native tools passed; both BINs again matched preserved releases |
| Former four sibling projects | **1048 source/data file hashes unchanged**; those trees were not written |

Running common stock/custom checks in both profiles does not double distinct
original-behavior coverage. The 6000 upstream cases do not establish classic
DOOM or AI equivalence. Prior RV32 differential/emulator reports were
preserved; no new full RV32 sessions were run during consolidation.
No firmware behavior changed, so screenshots were not regenerated through hours of emulation.

The host lacked GCC sanitizer runtimes, so the ASIC test harness now accepts
`SANITIZER_CC`; address/UB checks passed with available Clang.
Only leak checking was disabled under ptrace.
The first RVSWD attempt inherited an unavailable compiler through `CC`;
the complete run passed with `CC=cc`.

Rebuild hashes are in `integration/rebuilt_images.json`; the archive
inventory is `../artifacts/catalog.json`. `tools/check_reference.py` no
longer claims the entire maintained parent source is unchanged: the old
5017-file baseline is historical, and current verification explicitly covers
**884 immutable evidence files**. It was not silently rebaselined and reported as the same test.

For portability, Git-index files were checked out into a separate temporary
directory; only the three private inputs (V2.4, NOR, WAD) were supplied.
No former sibling sources or build outputs were used.
`integration/portable_checkout.json` records this and sibling-tree hash preservation.
Those three inputs are not added to the public source repository.

## Cleanup and future use

`docs/README.md` is the common documentation entry point. Current setup,
theme, research and legal guidance is consolidated; phase/evidence reports
remain separate. The old root README is retained as a historical document.

Reproducible build/bytecode/cache outputs are excluded from publication;
acquisitions and persistent archives are retained. Existing gallery images,
validation reports and original binaries remain available locally.
Use `BUILDING.md` for future work, bump versions for changed BINs and rerun
the necessary differential tests. A clean Git clone is not a backup of the private archive.

Cleanup removed **561659009 bytes (about 536 MiB)** of reproducible
build/objects, environments/caches, expanded upstream archives and Ghidra
working data. Small unpublished cache work was backed up externally;
old ELF/host libraries/screenshots/test reports were retained under local
`artifacts/validation/previous-build/`. Three Studio native tools and the
DOOM host simulator were retained for immediate use; MCU/ESP32 releases
are in the central archive. `integration/cleanup.json` records the totals.
`.gitattributes` preserves imported evidence/vendor line endings and hashes.

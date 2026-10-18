# Documentation index

Use the consolidated guides below for current instructions. Phase reports
and release directories retain historical results; older statements about
missing captures, version labels or folder locations do not override later evidence.

| Topic | Guide |
|---|---|
| Overview, status, hardware and gallery | [Root README](../README.md) |
| Building every project | [BUILDING](BUILDING.md) |
| Windows BIN installation and rollback | [UPDATING](UPDATING.md) |
| Themes / simple screen / THEME.PKG | [THEMES](THEMES.md) |
| Research, Flash, UART and MCU debugging | [RESEARCH](RESEARCH.md) |
| Risks, liability and independence | [LEGAL_AND_SAFETY](LEGAL_AND_SAFETY.md) |
| Folder migration, privacy and tests | [WORKSPACE_MIGRATION](WORKSPACE_MIGRATION.md) |
| Screenshots | [Gallery](gallery/README.md) |
| Artifacts and licensing/distribution boundaries | [Artifacts](../artifacts/README.md), [LICENSE](../LICENSE.md), [notices](../THIRD_PARTY_NOTICES.md) |

## Technical references

- Firmware: [source layout](../reconstructed/README.md),
  [architecture](FIRMWARE_ARCHITECTURE.md), [pins](HARDWARE_MAPPING.md),
  [image format](FIRMWARE_RELEASE.md), [verification](FIRMWARE_VERIFICATION.md),
  [feature matrix](FEATURE_MATRIX.md), [offline simulator](OFFLINE_SIMULATOR.md).
- Studio: [editor](../projects/studio/THEME_EDITOR.md),
  [Flash Manager](../projects/studio/FLASH_MANAGER.md),
  [protocol boundary](../projects/studio/DEVICE_PROTOCOL.md),
  [tests](../projects/studio/TEST_RESULTS.md).
- DOOM: [source and controls](../projects/doom/README.md),
  [tests](../projects/doom/TEST_RESULTS.md),
  [0.4.0 gallery](../projects/doom/docs/screenshots/v0.4.0/README.md).
- ASIC: [passive tools](../projects/asic/README.md),
  [wire specification](../projects/asic/docs/ASIC_UART_SPEC.md),
  [C integration](../projects/asic/firmware/INTEGRATION.md).
- RVSWD: [diagnostics](../projects/rvswd/DEBUG_CAPABILITIES.md),
  [recovery results](../projects/rvswd/FLASH_RECOVERY_REPORT.md).

## Preserved historical reports

Reconstruction history: [initial status](RECONSTRUCTION_STATUS.md),
[Unified Phase 3](UNIFIED_RECONSTRUCTION_REPORT.md),
[Phase 4](PHASE4_RECONSTRUCTION_REPORT.md),
[continuation](PHASE4_CONTINUATION_REPORT.md),
[ASIC control](ASIC_CONTROL_REPORT.md).

Address/algorithm evidence: [Flash forensics](FLASH_DUMP_ANALYSIS.md),
[library recovery](LIBRARY_SOURCE_RECOVERY.md),
[UI](UI_RECOVERY.md), [scheduler](SCHEDULING_SEMANTICS.md),
[USB limitations](USB_STORAGE_RECONSTRUCTION.md),
[dump feasibility](FIRMWARE_DUMP_FEASIBILITY.md).

The former root introduction is retained as `LEGACY_RECONSTRUCTION_README.md`.
Phase documents are not discarded or rewritten to imply new validation;
this index and RESEARCH bring their current scope together.

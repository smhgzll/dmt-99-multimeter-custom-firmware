# Belge rehberi

Güncel kullanım için aşağıdaki birleşik rehberler esas alınır. Aşama raporları
ve yayın klasörleri tarihsel sonuçları korur; eski “henüz donanım kaydı yok”,
sürüm etiketi veya klasör yolu ifadeleri daha sonraki bulguların yerine geçmez.

| Konu | Belge |
|---|---|
| Proje özeti, durum, donanım, galeri | [Kök README](../README.md) |
| Tüm projeleri build etme | [BUILDING](BUILDING.md) |
| Windows BIN yükleme ve geri dönüş | [UPDATING](UPDATING.md) |
| Tema / sade ekran / THEME.PKG | [THEMES](THEMES.md) |
| Araştırma, Flash, UART, MCU debug | [RESEARCH](RESEARCH.md) |
| Risk, sorumluluk ve bağımsızlık | [LEGAL_AND_SAFETY](LEGAL_AND_SAFETY.md) |
| Klasör değişiklikleri, kişisel veri ve testler | [WORKSPACE_MIGRATION](WORKSPACE_MIGRATION.md) |
| Ekran görüntüleri | [Galeri](gallery/README.md) |
| Dosyalar, lisans / dağıtım ayrımı | [Artifacts](../artifacts/README.md), [LICENSE](../LICENSE.md), [notices](../THIRD_PARTY_NOTICES.md) |

## Teknik kaynaklar

- Firmware: [kaynak düzeni](../reconstructed/README.md),
  [mimari](FIRMWARE_ARCHITECTURE.md), [pinler](HARDWARE_MAPPING.md),
  [imaj](FIRMWARE_RELEASE.md), [test yaklaşımı](FIRMWARE_VERIFICATION.md),
  [özellik matrisi](FEATURE_MATRIX.md), [offline simulator](OFFLINE_SIMULATOR.md).
- Studio: [editör](../projects/studio/THEME_EDITOR.md),
  [Flash Manager](../projects/studio/FLASH_MANAGER.md),
  [protokol sınırı](../projects/studio/DEVICE_PROTOCOL.md),
  [testler](../projects/studio/TEST_RESULTS.md).
- DOOM: [kaynak/tuşlar](../projects/doom/README.md),
  [testler](../projects/doom/TEST_RESULTS.md),
  [0.4.0 galerisi](../projects/doom/docs/screenshots/v0.4.0/README.md).
- ASIC: [pasif araçlar](../projects/asic/README.md),
  [wire spec](../projects/asic/docs/ASIC_UART_SPEC.md),
  [C entegrasyonu](../projects/asic/firmware/INTEGRATION.md).
- RVSWD: [teşhis](../projects/rvswd/DEBUG_CAPABILITIES.md),
  [recovery sonucu](../projects/rvswd/FLASH_RECOVERY_REPORT.md).

## Korunan tarihsel raporlar

Rekonstrüksiyon akışı: [ilk durum](RECONSTRUCTION_STATUS.md),
[Unified Phase 3](UNIFIED_RECONSTRUCTION_REPORT.md),
[Phase 4](PHASE4_RECONSTRUCTION_REPORT.md),
[continuation](PHASE4_CONTINUATION_REPORT.md),
[ASIC kontrolü](ASIC_CONTROL_REPORT.md).

Adres/algoritma kanıtı: [Flash forensics](FLASH_DUMP_ANALYSIS.md),
[library recovery](LIBRARY_SOURCE_RECOVERY.md),
[UI](UI_RECOVERY.md), [scheduler](SCHEDULING_SEMANTICS.md),
[USB sınırları](USB_STORAGE_RECONSTRUCTION.md),
[dump fizibilitesi](FIRMWARE_DUMP_FEASIBILITY.md).

Eski kök tanıtımı `LEGACY_RECONSTRUCTION_README.md` içinde tarihsel olarak
saklanır. Aşama belgeleri silinmez veya yeni doğrulanmışlık iddialarıyla
yeniden yazılmaz; bu sayfa ve RESEARCH güncel kapsamı bir araya getirir.

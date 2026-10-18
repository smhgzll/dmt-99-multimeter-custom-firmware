# Ekran galerisi

## POTAT-99 / custom firmware

Mevcut `docs/firmware_screens/` görüntüleri RV32 emülasyonundan alınmıştır;
normal ekranlar için V2.4/parity karşılaştırma kanıtına eşlik eder. Bunlar
0.6.0'ın fiziksel cihaz fotoğrafları veya yeni tema ekranlarının kanıtı değildir.

| Normal ölçüm | İkinci görünüm, HOLD/REL |
|---|---|
| ![DC V](../firmware_screens/02-regular-dc-volts.png) | ![Tema 2](../firmware_screens/04-theme2-hold-rel.png) |

| Grafik | Monitor |
|---|---|
| ![Grafik](../firmware_screens/05-record-graph.png) | ![Monitor](../firmware_screens/06-monitor-thresholds.png) |

| Menü | Geçmiş |
|---|---|
| ![Menü](../firmware_screens/07-settings-menu.png) | ![Geçmiş](../firmware_screens/08-history-browser.png) |

## DOOM 0.4.0

Önceki doğrulanmış 0.4.0 BIN'inin emülatör ekranları yeniden kullanıldı.
Uzun emülatör oturumu çalıştırılmadı. SRAM fixture içeren ileri test ekranları
kendi [ayrıntılı galerisinde](../../projects/doom/docs/screenshots/v0.4.0/README.md) etiketlidir.

| Oyun | Menü / ses |
|---|---|
| ![Oyun](../../projects/doom/docs/screenshots/v0.4.0/game.png) | ![Seçenekler](../../projects/doom/docs/screenshots/v0.4.0/sound-off.png) |

## Studio tema editörü ve Flash Manager

Aşağıdaki iki görüntü birleştirme sırasında **gerçek Qt uygulamasından**,
offscreen modda ve native C önizleme aracıyla alındı. Donanım bağlanmadı,
canlı ölçüm başlatılmadı. Yeniden üretim: `tools/capture_studio_gallery.py`.

![Studio tema editörü](studio-theme-editor.png)

![Studio Flash Manager](studio-flash-manager.png)

Oyun ve üretici görsellerinin hakları ilgili sahiplerine aittir;
[yeniden dağıtım sınırları](../LEGAL_AND_SAFETY.md) geçerlidir.

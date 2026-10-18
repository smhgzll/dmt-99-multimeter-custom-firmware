# DOOM 0.3.0 — gerçek BIN ekranları

Bu görüntüler mevcut DMT99 emülatöründe çalışan gerçek RV32 BIN'in panel
framebuffer'ından kaydedildi. Donanım deneyi değildir.

![Silah menüsü](shotgun.png)
![Yürüme/saldırı](enemy-motion.gif)
![Ölüm animasyonu](enemy-death.gif)

`enemy-*`, `corpse-*` ve duvar sahneleri, kontrollü SRAM actor fixture'larıdır;
üretim kodunda test API'si yoktur. Görüş/takip/saldırı/ölüm ve duvar engelleri
asıl CPU koduyla çalıştırılır. Bu fixture kareleri native piksel-eşitliği
iddiasına katılmadı. Normal GPIO/clock senaryolarının 40 karesi native C ile eşit.

GIF'ler PNG karelerinin önizlemesidir; oynatma hızı fiziksel FPS ölçümü değildir.
Kayıpsız PNG hash'leri [screenshots.json](screenshots.json) içinde. Ayrıntılı
CPU sonuçları [emülatör raporunda](../../validation/v0.3.0/emulator_results.json).

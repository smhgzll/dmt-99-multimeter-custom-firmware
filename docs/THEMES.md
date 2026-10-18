# Tema hazırlama ve cihaza yükleme

Studio'nun offline editörü tamamlanan işlevdir; gerçek cihazla USB yönetim ve
canlı ölçüm taşıması eksiktir. Tema yüklemek için o bağlantı gerekmez:
`THEME.PKG` mevcut bootloader diskine elle kopyalanır.

## Düzenleme

1. [Studio'yu başlatın](BUILDING.md#studio--tema-editörü), **File → Open original dump**
   seçin. Orijinal 8 MiB dosya salt okunur kaynak olarak kullanılır.
2. **Theme Editor** ile ekran/görsel seçin. PNG/JPEG içe aktarabilir,
   görsele metin işleyebilir veya **Batch colour** ile grup renklendirebilirsiniz.
3. Önizlemeleri kontrol edin. Bunlar firmware'in native C renderer'ı ve gerçek
   Flash kaynaklarıdır. JPEG uyumu firmware TJpgDec derlemesiyle sınanır.
4. Düzenlenebilir projeyi `artifacts/themes/<ad>/project/` altında saklayın.
   **Export THEME.PKG for the meter** ile `artifacts/themes/<ad>/THEME.PKG`
   üretin. Yeni temalar orijinal dump'ın üzerine kaydedilmez.

## Sürüm ve alan

| Özellik | Gerekli POTAT-99 |
|---|---|
| JPEG tema paketi | ≥0.4.0 |
| Firmware çizim renklerini eşleyen `THEME.PAL` paleti | ≥0.5.0 |
| Görsel gerektirmeyen Simple screen | ≥0.6.0 |

Paket yalnız değişen görselleri ve isteğe bağlı paleti içerir. Pozisyon/font/
kodla çizilmiş metinleri keyfî taşımak desteklenmez. Paket adı **THEME.PKG**
kalmalıdır. BOOT hacmi 1.5625 MiB toplamdır; yaklaşık 1.5 MB boş yerden söz
edilse de gerçek boşluğu kontrol edin. Assets hacmindeki toplam boş alan da
tüm kurulu temaları sınırlar; yaklaşık 3 MB gözlemi sabit kapasite garantisi değildir.

## Yükleme, seçme, silme

1. Önce uygun POTAT-99 sürümünü [Windows USB yöntemiyle](UPDATING.md) yükleyin.
2. Cihazı kapatın; **POWER + MENU** ile BOOT USB diskini Windows'a bağlayın.
3. Yalnız `THEME.PKG` kopyalayın, güvenli çıkarın ve normal açın.
4. Uygulama paketin sınırlarını/CRC'lerini kontrol eder, assets hacminde
   `themes/Tnn` altına kurar, doğrular, etkinleştirir ve paketi kaldırır.
   Kesintide kurulumun tamamlandığı varsayılmaz; hata mesajını inceleyin.
5. **Menu → About → ENTER → Display** ekranında temayı seçin. OK kullanır;
   ENTER seçili özel temayı onay sorusuyla siler. MENU/BACK çıkar.

**Original** seçimiyle özgün kaynaklara dönülür. Orijinal görsellerin üstüne
yazılmaz; tema kurulum/silme assets Flash'ına yazma işlemidir. Geri dönüş
gerektiğinde tam 8 MiB yedek ile MCU firmware yedeğini birbirinden ayırın;
ham dump'ı USB firmware BIN'i gibi kullanmayın.

0.6.0'da aynı Display ekranından **Simple screen** açılır; arka plan, rakam,
metin/vurgu rengi ve boyutu seçilebilir. Bunun için paket/Studio gerekmez.

## Küçük örnek

```bash
python3 tools/create_demo_theme.py
# artifacts/themes/green-demo/THEME.PKG — 78 bayt, yalnız 0xFD40 → 0x07E0 paleti
```

Bu örnek üretici JPEG'i kopyalamaz; ≥0.5.0 ister. Sadece eşlenen firmware çizim
renklerini değiştirir, JPEG içindeki renkleri değiştirmez. Offline paket
parse/CRC doğrulaması yapılır; fiziksel kurulum bu birleştirmede denenmedi.

İleri ayrıntılar: [editör](../projects/studio/THEME_EDITOR.md),
[Flash Manager](../projects/studio/FLASH_MANAGER.md),
[paket C kodu](../reconstructed/ui/theme_boot.c).

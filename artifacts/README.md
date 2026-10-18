# Yerel çıktılar ve edinilmiş veri arşivi

Beş projedeki imajlar/çıktılar bu dizinde toplanır. **Dosyalar birbirinin
yerine kullanılamaz.** Katalogdaki `flashable_via_meter_usb` sınıfına ve
yükleme rehberine bakın; salt `.bin` uzantısına güvenmeyin.

| Dizin | İçerik / hedef |
|---|---|
| `firmware/original/` | Değiştirilmemiş üretici V2.4 uygulaması, USB/MCU uygulaması |
| `firmware/potat-99/v*/` | Korunan POTAT-99 USB uygulama yayınları; `debug/` yalnız yerel |
| `firmware/doom/v*/` | Korunan DOOM USB uygulama yayınları; oyun verisi içerir |
| `firmware/esp32/<eski-build>/` | ESP32 adaptör imajları ve onun bootloader/partition dosyaları; **DMT99'a yüklenmez**, lab build'leri ayrıca etiketlenir |
| `firmware/development/` | Eski standalone/readonly/bringup/parity/restore build çıktıları; normal USB yükleme yayını değildir |
| `flash/original/` | 8 MiB W25Q64 edinimi; **MCU dump'ı değildir** |
| `flash/extracted/` | FAT12 dosyaları, carving ve hacim kopyaları; özgün kaynak değil türev |
| `captures/asic-captures/` | 25 gerçek UART oturumu, `.sr`/logic/metadata/türetilmiş raporlar |
| `captures/rvswd-captures/`, `captures/rvswd-recovery/` | Debug oturum kanıtları; başarısız Flash okumaları ve 4 bayt SRAM örneği |
| `themes/` | Tema projeleri ve THEME.PKG çıktıları; `green-demo` yalnız palet örneği |
| `game-data/` | Ayrı lisanslı DOOM WAD ve shareware arşivi |
| `research-logs/`, `local-environment/` | Tarihsel günlükler ve özel makine/USB envanteri; yayıma dahil edilmez |
| `validation/previous-build/` | Temizlikten önceki ELF/host kütüphanesi ve raporlar; yalnız yerel inceleme |

`catalog.json` yerel dosyaları boyut, SHA-256 ve amaçla listeler. Yayınlanmış
BIN/dump/veri içeriği değiştirilmemiştir. `final/`, `flashdump/`, `dist/` gibi
eski yollar göreli bağlantılardır. Sürüm klasöründeki eski README/raporlar
tarihsel olduğu için korunur; güncel kullanım için
[UPDATING](../docs/UPDATING.md), [THEMES](../docs/THEMES.md) ve
[BUILDING](../docs/BUILDING.md) esas alınır.

Bu dizindeki büyük, özel veya yeniden dağıtım hakkı belirsiz girdiler `.gitignore`
ile Git dışında tutulur. Yalnız katalog ve seçilmiş yayın metadata'sı takip
edilir. Temiz Git klonunda katalogdaki BIN/dump/WAD dosyalarının bulunmaması
beklenir; klon bir tam yedek değildir. [Girdi yerleştirme](../docs/BUILDING.md#girdi-dosyaları).

Yeni yayınları aynı sürümün üzerine yazmayın. Yeni derleme ara dosyaları
projenin `build/` dizininde kalır; POTAT-99 `make release` ve DOOM `make package`
doğrulanmış sürümleri buraya yayımlar. Tema export'unda bu dizini seçin.
Gelecekte eklenen veriyi yayımlamadan önce gizlilik ve lisansını ayrıca kontrol edin.

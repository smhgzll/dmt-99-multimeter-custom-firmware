# POTAT-99 USB uygulama imajı ve yayın düzeni

Son korunmuş yayın **0.6.0**. Bu sayfa imaj mühendisliğini açıklar;
[BUILDING](BUILDING.md), [UPDATING](UPDATING.md) ve [THEMES](THEMES.md) güncel
işlem rehberleridir. Önceki sürüm klasörleri değiştirilmemiştir. Uygulamanın
emülatör testleri fiziksel updater erase davranışı garantisi değildir.

## Girdiler ve çıktı

Üretici `DMT-99_V2.4_20240306.bin` ham uygulamadır: 135.096 bayt, SHA-256
`4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
Dosya ofseti `0` fiziksel `0x08006400` / yürütme alias `0x6400` adresidir.

`make -C reconstructed firmware-update` aynı uygulama düzenini seçer.
`tools/update_image_format.py` başlık/vektör/layout ve sürüm kaydını kontrol
eder; bulunmayan bir üretici imzası/CRC şeması icat etmez. Dosyanın
bootloader tarafından kabulü ayrı donanım kanıtıdır.

| Alan | Konum / sınır |
|---|---|
| Eksik bootloader | `0x08000000–0x080063FF`; USB uygulama BIN'ine dahil değil |
| Başlık | `0x08006400`: reset jump, 12×nop, ebreak |
| Vektörler | `0x08006438`: 104 slot; mtvec alias `0x6438` |
| Kod/veri load alanı | `0x6400` alias'ından başlar, `0x30000` altında; 167 KiB kapasite |
| SRAM | `0x20000000–0x20007FFF`; 32 KiB toplam, 6144 bayt stack rezervi |
| V2.4 ayar/geçmiş erişimi | `0x08040000`, 128 halfword; fiziksel tüm iç Flash düzeni henüz doğrulanmadı |

Linker: `reconstructed/firmware/dmt99-app.ld`, ortak
`dmt99-sections.ld`, başlangıç `startup.S`. RDP/option byte yazılmaz.
Uygulama dosyası bootloader içermese de onun gerçek erase aralığı tam dump
olmadan kesinleştirilmiş sayılmaz. “Bootloader asla silinemez” garantisi verilmez.

## Yayınlar ve profiller

`make release FW_VERSION=<yeni-sürüm>` doğrulanmış BIN'i
`artifacts/firmware/potat-99/v<version>/` altına koyar. `release.json` sürüm,
commit, adres ve hash kaydıdır. `SHA256SUMS` debug dahil yerel yayın dosyalarını
tanımlar. Yayınlanmış farklı imaj aynı sürümün üzerine yazılmaz.

| Hedef/profil | Amaç |
|---|---|
| `firmware-update` | `0x6400` uygulaması, POTAT yayın özellikleri açık |
| `firmware-update-parity` | V2.4 etiketi/stock davranış karşılaştırması; normal kullanım yayını değil |
| `CONFIG=STOCK_COMPATIBLE` | Stock analiz/test profili |
| `CONFIG=CUSTOM_DMT99` | Ayrı last-mode test profili; enjekte edilmiş backend sınırı |
| `DMT_RELEASE_FEATURES` | POTAT yayınında last mode, tema/palet ve sade ekran gibi özellikler |
| `firmware`, readonly/bringup/restore standalone | Tarihsel `0x0` analiz imajları; normal USB yükleme dosyası değil |

Standalone imaj bootloader'ın yerine geçer. Korunmuş MCU'ya bu yolu açmak
RDP kaldırma/mass erase riski taşır; bu depoda günlük kullanım veya geri dönüş
yöntemi olarak önerilmez. Genel yükleme rehberinde unprotect/erase adımı yoktur.

## Uygulama davranışı ve kalıcı kayıt

Özgün USART1 ölçüm bağı, menüler, HOLD/REL, üç ölçüm görünümü, grafik/geçmiş,
ayarlar, güç/şarj akışı ve kaynak hacimleri kullanılır. Native controller
blocking-loop davranışını kooperatif durumlarla temsil eder; cycle-equivalence
iddiası yoktur. Orijinal menü cache/erase davranışı ve ilgili komutlar ayrı
differential testlerle sınanır.

Yayın farkları: About etiketi, kesilmiş IRQ/SysTick durumunu temizleyen reset,
foreground'a taşınmış uzun ISR işleri, HSI fallback, UART RX snapshot,
compare-before-write NOR, fault kaydı; last mode/tema/sade ekran isteğe bağlı
özellikleri. Bilinmeyen iki tuşlu A0 tanı komutu normal açılışta etkin değildir.
Detaylar kaynak ve [doğrulama raporunda](FIRMWARE_VERIFICATION.md).

POTAT-99 0.6.0 ayar uzantıları: halfword 110…113 sade ekran, 118/119 etkin
tema, 120…127 last mode kaydı. Bunlar özgün firmware'in eriştiği 256 baytlık
imajda tanımlı uzantılardır; tüm MCU fiziksel layout'unun doğrulanması değildir.
DOOM bu sayfaya yazmaz; kendi seçeneklerini yalnız RAM'de tutar.

Harici Flash: BOOT 63+400, assets 463+1585 adet 4096 bayt sektördür. Tek
FatFs mount nesnesinin özgün sırası korunur. V2.4'ün MBR/label yazmaları
compare-before-write ile içeriği değişmiyorsa atlanır. Tema kurulum/silme
assets'e yazar; orijinal görseller korunur. CP936 tabloları eksik/invalid
sınırında isimler fail-closed kalır; genel bir tablo sessizce takılmaz.

## Doğrulama ve açık noktalar

Build bellek/layout/stack sınırlarını kontrol eder. Native, orijinal makine
kodu ve board emülatörü testleri [FIRMWARE_VERIFICATION](FIRMWARE_VERIFICATION.md)
ve [FEATURE_MATRIX](FEATURE_MATRIX.md) altında sınıflıdır.

Gerçek UART gözlemleri [ASIC wire spec](../projects/asic/docs/ASIC_UART_SPEC.md)
içindedir; DMA tamponu boyutu tel frame boyutu olarak genellenmez. Ölçüm
doğruluğu, analog kalibrasyon, panel/interrupt zamanlaması, her komutun yan
etkisi ve bütün sürümlerin fiziksel doğrulaması tamamlanmış değildir.

Bootloader'ın dosya adı/sürüm/imza kontrolleri, erase aralığı, kesinti sonrası
durumu ve gerçek USB MSC implementasyonu elde değildir. Windows VM başarılı
kopyalama gözlemi [UPDATING](UPDATING.md) içinde ayrıca belirtilir.

Üretici V2.4'e veya önceki POTAT-99'a dönüş, bootloader ve hacimler çalışır
durumdaysa aynı Windows USB uygulama yükleme yoluyla yapılır; kesin kurtarma
garantisi verilmez. [Riskler](LEGAL_AND_SAFETY.md).

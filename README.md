# POTAT-99 · FNIRSI DMT99 araştırma ve firmware çalışma alanı

> **Kendi sorumluluğunuzda kullanın.** Bu bağımsız ve deneysel çalışma FNIRSI ile
> bağlantılı, onaylı veya desteklenen bir ürün değildir. Hatalı yazılım/yükleme
> cihazı çalışmaz hâle getirebilir, verileri silebilir veya yanlış ölçüm
> gösterebilir. Mevzuatın izin verdiği ölçüde garanti verilmez ve geliştiriciler
> zararlardan sorumluluk kabul etmez. [Ayrıntılı risk ve lisans açıklaması](docs/LEGAL_AND_SAFETY.md).

DMT99'un V2.4 uygulamasının tersine mühendisliği, **POTAT-99** C firmware'i,
masaüstü tema editörü, küçük bir DOOM portu, pasif ASIC UART çözümlemesi ve
ESP32 RVSWD teşhisi bu depoda birlikte bulunur. Amaç, özgün kanıttan
izlenebilir kaynak koduna ve tekrarlanabilir derlemelere ulaşmaktır.

**Başlangıç:** [Derleme](docs/BUILDING.md) · [BIN yükleme / geri dönüş](docs/UPDATING.md)
· [Tema yükleme](docs/THEMES.md) · [Galeri](docs/gallery/README.md)
· [Araştırma ve kanıtlar](docs/RESEARCH.md) · [Tüm belgeler](docs/README.md)

## Projeler ve mevcut durum

| Bileşen | Kaynak | Durum |
|---|---|---|
| POTAT-99 firmware | [reconstructed/](reconstructed/README.md) | Son korunmuş sürüm **0.6.0**; özgün ekranlar, menüler, ölçüm bağlantısı, geçmiş, temalar, sade ekran; çevrimdışı/emülatör kanıtı |
| DOOM | [projects/doom/](projects/doom/README.md) | **0.4.0**; küçük E1M1 raycaster, altı silah, animasyon, buzzer, god mode, yumuşak hareket; tam klasik DOOM motoru değil |
| DMT99 Studio | [projects/studio/](projects/studio/README.md) | Tema editörü, Flash yöneticisi ve firmware önizlemesi çalışır; gerçek cihazla yönetim/canlı veri bağlantısı **eksik** |
| ASIC UART | [projects/asic/](projects/asic/README.md) | 25 fiziksel kayıt, 24 regresyon kesiti, bağımsız C parser/serializer; analog doğrulama sınırlı |
| ESP32 RVSWD | [projects/rvswd/](projects/rvswd/README.md) | Klasik ESP32 üzerinden teşhis; RDP aktif, özgün MCU Flash'ından **0 bayt** kurtarıldı; başarıyla tekrarlanan SRAM örneği yalnız **4 bayt** |

Bu birleştirme yeni bir donanım doğrulaması veya yeni firmware sürümü değildir.
Önceki sürümler korunur. **STOCK_COMPATIBLE** ve **CUSTOM_DMT99** ayrımı sürer;
POTAT-99 yayın özellikleri ayrıca `DMT_RELEASE_FEATURES` ile açılır. Studio,
DOOM ve UART teşhis araçlarının tek klasörde olması onları tek firmware imajı
yapmaz. DOOM çalışırken multimetre işlevi yoktur.

## Galeri

Ekranlar emülatör/native C renderer ve gerçek Studio arayüzünden alınmıştır;
cihaz fotoğrafı veya fiziksel performans kanıtı değildir.

| POTAT-99: normal ölçüm | POTAT-99: kayıt görünümü |
|---|---|
| ![Normal ekran](docs/firmware_screens/02-regular-dc-volts.png) | ![Grafik](docs/firmware_screens/05-record-graph.png) |

| DOOM: oyun | DOOM: seçenekler |
|---|---|
| ![DOOM oyun](projects/doom/docs/screenshots/v0.4.0/game.png) | ![DOOM menü](projects/doom/docs/screenshots/v0.4.0/sound-off.png) |

![Studio tema editörü — gerçek çevrimdışı Qt arayüzü](docs/gallery/studio-theme-editor.png)

[Daha fazla görüntü ve kaynakları](docs/gallery/README.md).

## Cihaz ve bellek

| Bölüm | Bu projedeki bulgu / yapılandırma |
|---|---|
| MCU | **WCH CH32V303RCT6**, RISC-V RV32IMAC, soft-float |
| Saat | 8 MHz HSE ×18 PLL = **144 MHz** |
| RAM | Yeni imajlar **32 KiB** sınırında linklenir; option byte/RDP değiştirilmez |
| Ekran | **320×240**, GPIOB 16 bit paralel veri; PC3/4/5/13 kontrol |
| Harici Flash | **Winbond W25Q64, 8 MiB**, SPI1: PA4 CS, PA5/6/7 |
| Tuşlar / güç | PC2 ve PC8…PC12; PC1 güç tutma, PA8 şarj algılama |
| Buzzer / aydınlatma | PA1 TIM2 CH2 / PC6 TIM8 CH1 |
| Ölçüm ASIC bağlantısı | USART1 PA9 TX / PA10 RX, **9600 8N1**; SD7502/yakın SDIC ailesi kimliği hipotezdir |
| Debug çıkışı | USART2 PA2, **115200 8N1**; özgün debug yolu TX odaklıdır |

| Adres / sektör | Anlamı |
|---|---|
| `0x08000000–0x080063FF` | 25 KiB özgün bootloader alanı; elimizde dump'ı yok |
| `0x08006400` | USB uygulama BIN başlangıcı; dosya ofseti 0 buraya eşlenir |
| `0x08006438` | Vektör tablosu; yürütme alias'ı `0x6438` |
| `0x08030000` | Mevcut uygulama linker üst sınırı; `0x6400` sonrasında 167 KiB alan |
| `0x08040000` | V2.4'te gözlenen 256 bayt ayar/geçmiş sayfası; fiziksel Flash kapsamı ayrıca doğrulama ister |
| Harici sektör 0 | MBR; sektör birimi **4096 bayt** |
| Harici sektör 63…462 | FAT12 BOOT, USB aktarım hacmi; 400 sektör = 1.5625 MiB |
| Harici sektör 463…2047 | FAT12 assets; JPEG/font/tema kaynakları |

USB diskteki yaklaşık 1.5 MiB boşluk MCU uygulama kodu için kullanılabilir RAM
veya Flash alanı değildir. Pin/işlev dayanakları:
[donanım eşlemesi](docs/HARDWARE_MAPPING.md),
[imaj ve bellek açıklaması](docs/FIRMWARE_RELEASE.md).

## Araştırma süreci

1. **V2.4 uygulaması:** 135.096 bayt BIN, `0x6400` alias'ına eşleme,
   439 fonksiyonun dekompilasyonu, WCH özel komutlarının çözümü ve adres bazlı C rekonstrüksiyonu.
2. **Harici Flash:** 8 MiB dump'ta MBR/FAT12, kaynak dosyaları ve tarihsel
   kalıntılar incelendi. 299 canlı dosya / 297 JPEG envanteri çıkarıldı;
   ChaN FatFs ve TJpgDec karşılıkları ikili kanıtla nitelendirildi.
3. **Davranış karşılaştırması:** orijinal komutlar, menü/cache, ekranlar,
   zamanlayıcılar ve paketler native C testleri ve RV32 emülasyonuyla karşılaştırıldı.
4. **Gerçek UART:** pasif D0/D2 logic analyzer kayıtları, DMA tamponuyla tel
   paketinin aynı olmadığını gösterdi: MCU TX 10 bayt; gözlenen ölçüm burst'leri
   12 bayt; bazı ASIC event adayları 10 bayt. RX checksum ve analog kalibrasyon
   tamamlanmış sayılmaz.
5. **MCU debug:** kullanılan bağlantı ARM JTAG değil, PA14/PA13 üzerinden
   iki telli **RVSWD**. Debug teşhisi çalışması tüm Flash'ı okuyabilmek veya
   güvenli programlayabilmek anlamına gelmez. RDP altında Flash kurtarma başarılı değildir.

[Araştırma özeti ve ham kanıta bağlantılar](docs/RESEARCH.md).
Eski Phase 3/4 sayıları tarihsel kapsamı anlatır; farklı kategoriler toplanıp
donanım eşdeğerliği veya yüzde 100 tamamlanmışlık iddiası yapılmaz.

## Hızlı derleme

Linux/WSL üzerinde GNU make, C derleyicisi ve Python gerekir. MCU için
RV32IMAC/ILP32 destekleyen RISC-V GCC, ESP32 adaptörü için ayrı **ESP-IDF 5.5.5**
kullanılır. [Tüm kurulum, test ve BIN'e kadar adımlar](docs/BUILDING.md).

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements-dev.txt

make firmware       # reconstructed/build/firmware-update/dmt99-update.bin
make studio-tools   # Studio'nun native renderer/JPEG kontrol araçları
make asic           # projects/asic/build/libdmt99_asic_rv32.a (firmware değildir)

# DOOM için yerel/yasal oyun verisi gerekir; veri Git'e eklenmez.
python3 projects/doom/tools/fetch_wad.py
make doom           # projects/doom/build/dmt99-doom.bin

projects/studio/dmt99-studio
```

Kaynaklar ve küçük regresyon kesitleri Git'e uygundur. Üretici BIN'i, 8 MiB
dump, tam kayıtlar ve DOOM oyun verileri otomatik yayımlanmaz; yerel
[`artifacts/`](artifacts/README.md) arşivinde saklanır. Temiz klonda eksik
girdileri [derleme rehberindeki](docs/BUILDING.md#girdi-dosyaları) konumlara
yerleştirin. [SHA-256 kataloğu](artifacts/catalog.json) özgün dosyaları tanımlar.

## BIN yükleme ve geri dönüş

**Şimdilik Windows 10/11 kullanın. Sanal Windows 11 + USB aygıt aktarımı ile
BIN kopyalama kullanıcı tarafından başarılı olarak bildirildi.** Bu gözlem
her VM/USB denetleyicisi için garanti değildir. FNIRSI'nin
[güncel indirme talimatı](https://www.fnirsi.com/pages/manuals-firmwares)
Windows 10/11 belirtir; macOS kullanılmamasını ister. Linux üzerinden yükleme
bu projenin desteklenen yolu değildir; Linux/WSL derleme için kullanılabilir.

1. Bataryayı doldurun; probları ve debug/analyzer bağlantılarını ayırın.
2. Cihaz kapalıyken **POWER + MENU** ile USB güncelleme diskini açın.
3. Doğru SHA-256 kontrolünden sonra **yalnız uygulama USB BIN'ini** kopyalayın.
4. Kopyalama bitince güvenli çıkarın; üreticinin yönergesine göre bağlantıyı
   kesin, normal açılışı ve sürümü kontrol edin.
5. DOOM'dan ölçü aletine dönüş için aynı yolla çalıştığı bilinen POTAT-99
   uygulama BIN'ini; orijinale dönüş için V2.4 BIN'ini yükleyin.

**ESP32 BIN'i, 8 MiB dump, `.elf`, standalone imaj veya tema paketi firmware
BIN'i yerine kopyalanmaz.** İmaj bootloader bölgesini içermez; bootloader'ın
gerçek erase/check davranışı okunamadığı için kesinti sonrası geri dönüş
garantisi verilmez. [Ayrıntılı kurulum ve kurtarma sınırları](docs/UPDATING.md).

## Tema güncelleme

POTAT-99 **≥0.4.0** için Studio'da görselleri düzenleyin ve **Export THEME.PKG**
ile dışa aktarın. Firmware çizim renk paleti **≥0.5.0**, görselsiz sade ekran
**≥0.6.0** ister. Windows USB güncelleme diskine adı değişmeden `THEME.PKG`
kopyalanır; sonraki normal açılışta uygulama doğrular, `themes/Tnn` dizinine
kurar ve paketi siler. **Menu → About → ENTER → Display** ile tema seçilir/silinir.

Orijinal görsellerin üzerine yazılmaz; aktif tema öncelikli kaynak olarak
kullanılır. Bu işlem assets bölümüne yazdığı için Flash yedeği önemlidir.
[Tema hazırlama, boyut sınırı ve geri alma](docs/THEMES.md).

## Kısıtlar, tehlikeler ve lisanslar

- Özel firmware ölçüm doğruluğu/kalibrasyon veya güvenlik sınıfı sertifikası
  sağlamaz. Kritik, şebeke veya güvenlik amaçlı ölçümlerde kullanılmamalıdır.
- USB, ESP32 ve logic analyzer ölçüm devresine toprak yolu açabilir. Debug
  kabloları bağlıyken enerjili/şebeke devrelerde ölçüm yapmayın.
- **RDP kaldırma mass erase doğurabilir:** orijinal bootloader/ayarlar kalıcı
  kaybolabilir. Normal USB yükleme için RDP açılmaz, option byte değiştirilmez.
- Bootloader kodu, gerçek USB MSC/update algoritması ve dahili Flash
  kapsamının tümü elde değildir. Varsayımsal bir bootloader eklenmedi.
- Studio canlı cihaz yönetim protokolünün gerçek USB taşıması tamamlanmadı;
  simulated device verileri gerçek ölçüm değildir.
- Kod, üretici firmware/görselleri, WCH SDK ve DOOM oyun verilerinin hakları
  ayrıdır. Depoyu birleştirmek tüm dosyalara tek açık kaynak lisansı vermez.
  [Lisans kapsamı](LICENSE.md) · [Üçüncü taraf bildirimleri](THIRD_PARTY_NOTICES.md).

## Referanslar

- [FNIRSI DMT-99 ürün sayfası](https://www.fnirsi.com/products/dmt-99)
  ve [üretici kılavuz/firmware kaynakları](https://www.fnirsi.com/pages/manuals-firmwares).
- [WCH CH32V303](https://www.wch-ic.com/products/CH32V303.html),
  [resmî CH32V307 SDK](https://github.com/openwch/ch32v307);
  yerel sürüm/hash'ler `references/` ve `reconstructed/vendor/` içinde.
- [ChaN FatFs](https://elm-chan.org/fsw/ff/) ve [TJpgDec](https://elm-chan.org/fsw/tjpgd/).
- [Artfical/doom-avr](https://github.com/Artfical/doom-avr/) — DOOM portunun kaynağı,
  pinlenmiş revizyon `f584566ec2e18041725e1de5668c6cb1f764ab58`.
- [sigrok](https://sigrok.org/), [Unicorn](https://www.unicorn-engine.org/),
  [Ghidra](https://github.com/NationalSecurityAgency/ghidra).
- [Nicolai Electronics RVSWD](https://github.com/Nicolai-Electronics/esp32-component-rvswd),
  [wch-protocols](https://github.com/ch32-riscv-ug/wch-protocols),
  [Espressif ESP-IDF](https://github.com/espressif/esp-idf).

Birleştirme doğrulaması, eski klasörlerin durumu ve yayıma dahil edilmeyen
dosyalar: [WORKSPACE_MIGRATION.md](docs/WORKSPACE_MIGRATION.md).

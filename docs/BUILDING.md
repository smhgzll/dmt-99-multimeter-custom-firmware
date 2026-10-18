# Derleme ve test

Komutlar depo kökünden çalıştırılır. Derleme Linux/WSL içindir; **cihaza USB
kopyalama Windows 10/11'de** yapılır. Hiçbir aşağıdaki build/test komutu bağlı
cihazı programlamaz veya ölçüm ASIC'ine komut göndermez.

## Girdi dosyaları

Yayımlama hakkı belirsiz dosyalar Git'e dahil değildir. Kendi yedeklerinizi
şu yerlere koyun; mevcut dosyanın üzerine yazmayın:

| Dosya | Yer | SHA-256 |
|---|---|---|
| Üretici V2.4 uygulaması | `artifacts/firmware/original/DMT-99_V2.4_20240306.bin` | `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e` |
| W25Q64 dump, 8 MiB | `artifacts/flash/original/dmt99_flash_1.bin` | `fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69` |
| DOOM shareware 1.9 WAD | `artifacts/game-data/doom1.wad` | `1d7d43be501e67d927e415e0b8f3e29c3bf33075e859721816f652a526cac771` |

Kökteki özgün BIN, `flashdump/`, `final/`, `extracted_flash/` ve proje capture
yolları **göreli uyumluluk bağlantılarıdır**. Hedefler `artifacts/` içindedir.
Linux/WSL Git klonu bu bağlantıları korumalıdır. Windows'ta symlink yerine
metin dosyası oluşturan bir Git ayarıyla build yapmayın; WSL kullanın.

Klasörleri gerekirse `mkdir -p artifacts/firmware/original artifacts/flash/original`
ile oluşturun. Dump, simulator/Studio kaynak önizlemelerinde gerekir;
POTAT-99 C derlemesi mevcut MCU ROM tablolarını `ui/*rom*` kaynaklarından alır.
V2.4 BIN'i imaj formatı ve orijinal kod karşılaştırmaları için gereklidir.

## Araçlar

- GNU make, GCC/Clang, Python ≥3.10; GUI için Python ortamının desteklediği Qt.
- RISC-V GCC: `rv32imac_zicsr_zifencei`, `ilp32`, soft-float ve `libgcc`/`libm`
  multilib'i. Kullanılan derleyici ESP-IDF'in riscv32-esp-elf GCC 14.2.0 paketidir.
- `tools/find_riscv_toolchain.py` PATH'i ve mevcut kullanıcının ESP araç
  dizinini arar. Otomatik seçim uygun değilse `CROSS=/path/to/riscv32-esp-elf-`
  verin. Her RISC-V derleyicinin gerekli multilib'e sahip olduğu varsayılmaz.
- Python bağımlılıkları:

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements-dev.txt
```

WCH/FatFs/TJpgDec kaynakları pinlenmiş olarak depodadır. ESP-IDF ve Python
ortamları depoya kopyalanmaz. Teşhis adaptörü için ayrıca **ESP-IDF 5.5.5**
ve onun ESP32/Xtensa toolchain'i gerekir.

## POTAT-99: USB uygulama BIN'i

```bash
make -C reconstructed -j4 firmware-update
# reconstructed/build/firmware-update/dmt99-update.bin
# .elf, .map, .lst, .size.txt aynı dizinde; bellek/stack kontrolleri build içindedir.

make -C reconstructed studio-tools
make -C reconstructed CONFIG=STOCK_COMPATIBLE check
make -C reconstructed CONFIG=CUSTOM_DMT99 check
```

`firmware-update` **0x08006400** uygulama düzenini seçer ve POTAT yayın
özelliklerini açar. `CONFIG` stock/custom analiz profilidir; yayın özellikleri
ayrı `DMT_RELEASE_FEATURES` anahtarıdır. Mevcut yayınlar değiştirilmez. Yeni bir
sürüm hazırlamak için örnek:

```bash
make -C reconstructed release FW_VERSION=0.6.1
# artifacts/firmware/potat-99/v0.6.1/DMT-99_V0.6.1_<tarih>.bin
```

`make release` farklı BIN ile aynı sürümü ezmeyi reddeder. 0.6.0'ın kaynak
etiketini yeniden üretmek için `FW_VERSION=0.6.0 FW_GIT=61bbf59` kullanılır;
aynı compiler/flags da gerekir. Bu birleştirmede 0.6.0 BIN'i değiştirilmedi.
Kök depo yeni Git geçmişi kullanır; eski kaynak kilometre taşları yalnız yerel
`.history/` arşivinde korunur, otomatik GitHub geçmişine eklenmez.

`firmware-all`, `firmware` ve standalone/restore hedefleri tarihsel analiz
için bulunur. Bunlar USB güncelleme dosyası olarak kullanılmaz; `0x0` düzeni
bootloader'ı kaybetme riski taşır. Normal süreç yalnız `firmware-update` kullanır.

## DOOM

```bash
python3 projects/doom/tools/fetch_wad.py  # ağdan ayrı lisanslı shareware girdisi
make -C projects/doom -j4 all
# projects/doom/build/dmt99-doom.bin
make -C projects/doom test
make -C projects/doom verify
.venv/bin/python projects/doom/tools/simulator.py
```

Upstream gerekli kaynakları pinlenmiş kopya olarak içerir; submodule init
gerekmez. WAD ve `generated/` Git dışındadır. Oyun verisini yeniden dağıtma
şartları AGPL kod lisansından ayrıdır. Masaüstü simülatörde gerçek buzzer sesi yoktur.

Yeni BIN'i yayımlamadan önce `make -C projects/doom emulate
EMU_PYTHON=<depo>/.venv/bin/python` tam RV32 testini çalıştırır. Ardından
`make -C projects/doom package` **kanıtın mevcut BIN hash'iyle aynı olduğunu**
kontrol ederek `artifacts/firmware/doom/v<version>/` üretir. Emülasyon uzun
sürebilir; bu birleştirmede yeniden koşulmadı, önceki görüntüler kullanıldı.
Kaynak değiştiyse `src/version.h` sürümü yükseltilir, eski yayın ezilmez.

## Studio / tema editörü

```bash
make studio-tools
projects/studio/dmt99-studio
# Launcher kendi projects/studio/.venv ortamını gerekirse oluşturur.
# Ortak ortam tercih edilirse:
cd projects/studio
../../.venv/bin/python -m dmt99studio
QT_QPA_PLATFORM=offscreen ../../.venv/bin/python -m pytest -q
```

Studio ana kaynakları ve `artifacts/flash/original/` dump'ını otomatik bulur;
eski kardeş proje klasörleri gerekli değildir. `DMT99_FIRMWARE_PROJECT` ve
`DMT99_ASIC_ANALYSIS` ortam değişkenleri isteğe bağlı farklı konumlar içindir.
Studio bir MCU BIN'i üretmez; firmware build'ini kullanır, `THEME.PKG`, tema
projesi veya kopya Flash imajı dışa aktarır. [Tema rehberi](THEMES.md).

## ASIC UART / bağımsız C sürücü

```bash
cd projects/asic
python3 -m unittest discover -s tests -v
make -C firmware
# build/libdmt99_asic_rv32.a
python3 tools/dmt99.py --help
```

Bu bir **statik kütüphanedir**, tek başına yüklenebilir BIN değildir. Uygulama
ile bağlamak için `firmware/INTEGRATION.md` geçerlidir. Gerçek pasif kayıt
için ayrıca sigrok-cli, analyzer firmware'i ve fiziksel bağlantı gerekir;
`capture`/`probe` komutları build/test adımı değildir. Varsayılan RX profilini
13 bayt DMA boyutuyla değiştirmeyin; gerçek kayıt profili 12 bayttır.

Sanitizer testi Clang varsa onu, yoksa GCC'yi kullanır; `SANITIZER_CC` ile
seçilebilir. GCC için `libasan`/`libubsan` kurulu olmalıdır. ptrace altındaki
hostlarda `ASAN_OPTIONS=detect_leaks=0` gerekebilir; adres/UB kontrolleri açık kalır.

## ESP32 RVSWD teşhis adaptörü

Bu hedef **klasik ESP32** içindir; önceki ESP32-S3 planı ile karıştırılmamalıdır.
ESP-IDF 5.5.5 ortamını etkinleştirdikten sonra:

```bash
cd projects/rvswd
CC=cc bash tests/run.sh
idf.py -B build-default -D IDF_TARGET=esp32 \
  -D SDKCONFIG=build-default/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' build
# build-default/dmt99_rvswd_diag.bin
# bootloader/bootloader.bin ve partition_table/partition-table.bin de ESP32'ye aittir.
```

Lab/activation/halt/progbuf kapıları normal build'de kapalıdır. Hazır arşivdeki
eski `*-lab` imajları normal teşhis yayını değildir. ESP32 BIN'ini DMT99 USB
diskine **kopyalamayın**. ESP32'ye yükleme ayrı donanım işlemidir; bu rehber
yalnız build'e kadardır. [Bağlantı ve teşhis kuralları](../projects/rvswd/DEBUG_CAPABILITIES.md),
[RDP sonuçları](../projects/rvswd/FLASH_RECOVERY_REPORT.md).

## Kısa doğrulama

```bash
ASAN_OPTIONS=detect_leaks=0 make test
make audit
```

Studio pytest'i ve ESP-IDF derlemesi ayrı adımlardır. Tam UART kayıtları,
dump veya WAD mevcut değilse ilgili araç bunları gerektirir; eksik girdili
testler sessizce donanım başarısı olarak sayılmaz. Bu birleştirmeye ait gerçek
test sonuçları [WORKSPACE_MIGRATION.md](WORKSPACE_MIGRATION.md) içindedir.

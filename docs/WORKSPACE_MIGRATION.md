# Beş projenin birleştirilmesi

2026-10-18: ana rekonstrüksiyon çalışma alanı GitHub'a uygun bir kaynak
deposu olarak düzenlendi. Firmware algoritmaları, ASIC komutları, menü
dispatcher'ı ve stock/custom politika ayrımı değiştirilmedi. Donanım işlemi,
flashlama veya yeni uzun RV32 emülasyonu yapılmadı.

## Yeni yerleşim

| Eski proje / yol | Birleşik yer |
|---|---|
| Ana analiz | `reconstructed/`, `analysis/`, `decompiled/`, `disassembly/`, `tools/`, `docs/` |
| dmt_99_doom | `projects/doom/` |
| dmt99-studio | `projects/studio/` |
| dmt_99_asic_analysis | `projects/asic/` |
| dmt99-rvswd | `projects/rvswd/` |
| final/v* | `artifacts/firmware/potat-99/v*` |
| DOOM dist/v* | `artifacts/firmware/doom/v*` |
| Özgün BIN / flashdump | `artifacts/firmware/original/`, `artifacts/flash/original/` |
| Kayıtlar, extraction, temalar | `artifacts/captures/`, `artifacts/flash/extracted/`, `artifacts/themes/` |

Dört eski kardeş proje **değiştirilmeden** yerinde bırakıldı; birleşik kaynaklar
onlara bağımlı değildir. Mevcut main/DOOM yayınlarının içerikleri korunmuştur.
Eski yollar için yalnız depo içinde kalan göreli symlink'ler vardır. Yeni
yayın araçları doğrudan merkezi `artifacts/` dizinini kullanır.

Mevcut beş proje ağacında kalıcı bir `THEME.PKG` yayını bulunmadı. Merkezi tema
dizinine yalnız yeni, **78 baytlık palet örneği** eklendi; mevcut bir tema
yayınının kurtarıldığı veya fiziksel olarak denendiği iddia edilmez.

Ana projenin eski çalışma ağacı değişiklik öncesi depo dışında yedeklendi.
Eski `.history/` kilometre taşları yerelde korunur. İç içe Git depoları, IDE
ayarları, Python ortamları, SDK build cache'leri ve upstream video demosu
birleşik kaynaklara kopyalanmadı. Kök `.git` yeni/temiz yayın geçmişidir;
kişisel geçmiş ve eski binary'ler otomatik ithal edilmedi. Hiçbir remote'a push yapılmadı.

## Gizlilik ve kanıt ayrımı

- Sabit kullanıcı/bilgisayar dizinleri kaynak/Makefile/belgelerden çıkarıldı.
  Toolchain PATH veya mevcut kullanıcının araç kurulumundan seçilir.
- Gerekli tarihsel **metin üstverisi** göreli gösterime çevrildi. Değişen
  metinlerin önce/sonra hash'leri `integration/metadata_normalization.json`
  içindedir; bunlar yeni deney/analiz sonucu değildir.
- Özgün BIN, Flash dump, ham logic/sigrok örnekleri ve yayımlanmış dosyalar
  değiştirilmedi. Tam kayıtlar, özel USB/host envanteri, eski loglar, debug ELF
  ve map dosyaları Git dışında yereldir; bunlarda eski edinim yolları bulunabilir.
  Immutable debug dosyalarını yeniden yazarak yayın hash'leri bozulmadı.
- Kök Git'e yalnız incelenmiş kaynak/belgeler, küçük regresyon kesitleri,
  galeri ve seçilmiş release metadata'sı girer. Tüm yerel arşivi ZIPleyip
  kaynak yayını diye yüklemek bu gizlilik filtresini atlar.
- Lisans/atıflar korunur; root lisansı tüm araştırma ve oyun verilerini
  yeniden lisanslamaz. Ham üretici/oyun verileri varsayılan olarak yayımlanmaz.

`tools/audit_repository.py` seçilmiş dosyaları kişisel mutlak yollar, bazı
anahtar/token biçimleri, ana rehberlerin yerel bağlantıları ve SHA-256
bakımından denetler. Bu sınırlı otomasyon hukukî veya tam güvenlik incelemesi değildir.

## Gerçek doğrulama sonuçları

| Kontrol | Sonuç |
|---|---|
| Orijinal BIN/NOR, decompiled/disassembly ve temel instruction/function envanteri | **884 dosya aynı SHA-256** |
| İthal edilmiş yayınlar ve tam ASIC/RVSWD kayıtları | **637 dosya değişmemiş**, `integration/artifact_preservation.json` |
| POTAT-99 temiz cross-build | **141868 bayt**, korunmuş 0.6.0 BIN'iyle birebir aynı |
| DOOM yeni dizinden cross-build | **163444 bayt**, korunmuş 0.4.0 BIN'iyle birebir aynı |
| Firmware STOCK_COMPATIBLE check | 13 çekirdek + 9 uygulama/storage + 6 kaynak + 2 profil + 7 simulator kontrolü geçti |
| Firmware CUSTOM_DMT99 check | 13 çekirdek + 9 uygulama/storage + 6 kaynak + 5 profil + 7 simulator kontrolü geçti |
| Studio native araçlar / GUI pytest | Build geçti; **41 test geçti** |
| DOOM test | **4 C test grubu, 6 host testi, 6000 upstream collision/raycast karşılaştırması geçti** |
| ASIC bağımsız driver | RV32 arşivi üretildi; **31 Python/C testi geçti**, ASan/UBSan stream testi dahil |
| RVSWD teşhis | **3 C test çalıştırması ve 34 Python testi geçti** |
| ESP32 default | ESP-IDF **5.5.5**, klasik ESP32 build'i geçti; lab/activation/halt kapıları kapalı |
| Galeri | Mevcut POTAT/DOOM PNG'leri kullanıldı; Studio gerçek Qt offscreen UI'sinden 2 kısa görüntü alındı |
| Ayrı dizinde yalnız Git'e seçilmiş kaynaklarla build | Firmware, DOOM, ASIC ve Studio native araçları geçti; BIN'ler yine eski yayınlarla birebir aynı |
| Eski dört kardeş proje | **1048 kaynak/veri dosyasının hash'i aynı**; bu dizinlere yazılmadı |

Stock/custom ortak kontrollerin iki profilde çalışması iki kat özgün davranış
kapsamı değildir. 6000 upstream vaka, klasik DOOM veya AI eşdeğerliği iddiası
değildir. Önceki RV32 differential/emülatör raporları korunmuştur; bu
birleştirmede yeni tam RV32 seansları çalıştırılmadı. Yeni firmware davranışı
eklenmediği için ekranları tekrar üretmek üzere saatlerce emülasyon yapılmadı.

GCC sanitizer runtime'ının bu hostta eksik olması nedeniyle ASIC test harness'i
`SANITIZER_CC` seçimini destekler; mevcut Clang ile adres/UB kontrolleri geçti.
ptrace altında yalnız leak kontrolü kapalıydı. İlk RVSWD çalışmasında ortamın
geçersiz `CC` seçimi derleyiciyi bulamadı; `CC=cc` ile tam test geçti.

Tekrar derleme hash'leri `integration/rebuilt_images.json`, arşiv envanteri
`../artifacts/catalog.json` içindedir. `tools/check_reference.py` artık tüm
ebeveyn kaynağı değişmemiş saymaz: eski 5017 dosyalık baseline tarihsel olarak
korunur, güncel kontrol açıkça **884 immutable kanıt dosyasıyla** sınırlıdır.
Baseline'ı sıfırlayıp aynı testmiş gibi başarı raporlanmadı.

Taşınabilirlik kontrolünde Git indeksindeki dosyalar ayrı bir geçici dizine
alındı; yalnız gerekli üç özel girdi (V2.4, NOR, WAD) kendi konumlarına
kopyalandı. Eski kardeş kaynaklar veya build dosyaları kullanılmadı.
`integration/portable_checkout.json` bu kontrolü ve kardeş ağaçların hash
korunmasını kaydeder. Bu üç girdi temiz kaynak deposuna eklenmez.

## Temizlik ve sonraki kullanım

Yeni belgeler tek giriş altında toplandı: `docs/README.md`. Güncel kurulum,
tema, araştırma ve yasal sınırlar ortak rehberlere taşındı; aşama/kanıt
raporları ayrı kalır. Eski root README tarihsel belge olarak korunur.

Yeniden üretilebilir build/bytecode/cache dosyaları yayın kapsamından
çıkarılır; edinim ve kalıcı arşiv silinmez. Mevcut galeri, doğrulama raporları
ve özgün binary'ler tutulur. Sıradaki değişiklikte `BUILDING.md` hedeflerini
kullanın; yeni BIN için sürüm yükseltin ve gerekli differential testleri
yenileyin. Temiz Git klonu yerel özel veri arşivinin yedeği değildir.

Fiziksel temizlikte **561659009 bayt (yaklaşık 536 MiB)** yeniden üretilebilir
build/obje, ortam/cache, açılmış upstream arşivi ve Ghidra çalışma verisi
kaldırıldı. Küçük yayımlanmamış cache çalışması dış yedeğe, eski ELF/host
kütüphanesi/ekran/test raporları yerel `artifacts/validation/previous-build/`
arşivine alındı. Studio'nun üç native aracı ve DOOM host simülatörü hızlı
kullanım için tutuldu; MCU/ESP32 yayınları merkezi arşivde bulunur.
`integration/cleanup.json` bu temizliğin sayısal kaydıdır. `.gitattributes`
ithal kanıt/vendor dosyalarının satır sonlarını değiştirerek hash bozmaz.

# Araştırma ve kanıt özeti

Bu sayfa beş projenin bulgularını birbirine bağlar. Ham kayıt, tahmin,
simülasyon ve cihaz doğrulaması aynı kanıt sınıfı olarak sunulmaz.

## V2.4 → C kaynakları

135.096 bayt V2.4 dosyasının fiziksel uygulama başlangıcı `0x08006400`,
yürütme alias'ı `0x6400` olarak çözülmüştür. İlk 25 KiB dosyaya dahil
değildir. 439 fonksiyon dekompile edilmiştir; daha sonraki aşamalar farklı
native/SDK/library/partial/replaced kategorileri kullanır. Eski Phase 3/4
sayıları korunmuş tarihsel raporlardır, güncel bütüne eklenmez.

- [Mimari](FIRMWARE_ARCHITECTURE.md), [özellik matrisi](FEATURE_MATRIX.md),
  [son firmware doğrulama yaklaşımı](FIRMWARE_VERIFICATION.md).
- `analysis/firmware_coverage.json`: adres bazlı dispozisyon; native/library/
  deviation/partial/replaced/default kategorileri ayrı tutulur.
- `decompiled/`, `disassembly/`, `analysis/instructions.json`: orijinal kod
  kanıtı; bu birleştirmede yeniden disassembly/decompile çalıştırılmadı.
- [Phase 4 continuation](PHASE4_CONTINUATION_REPORT.md),
  [ASIC kontrol raporu](ASIC_CONTROL_REPORT.md): aşama sonuçları ve sınırları.

## 8 MiB Winbond Flash

Mevcut tek tam Flash okuması `artifacts/flash/original/dmt99_flash_1.bin`;
SHA-256 `fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69`.
İkinci bağımsız edinim dosyası yoktur; iki okuma eşleştiği iddia edilmez.

4 KiB sektörlü MBR ve iki FAT12 hacimden 299 canlı dosya, 297 JPEG
çıkarılmıştır. Çıkarılan tüm dosyaların fiziksel extents/hash envanteri
korunur. Bir tarihsel RV32 uygulama gövdesi ve PC dosya kalıntıları da vardır;
bunlar bootloader veya güncel kalibrasyon bölgesi olarak adlandırılmaz.

[Flash analizi](FLASH_DUMP_ANALYSIS.md),
[sürüm karşılaştırması](FIRMWARE_VERSION_COMPARISON.md),
[kaynak yükleme mimarisi](RESOURCE_LOADING_ARCHITECTURE.md),
[upstream eşleştirme](LIBRARY_SOURCE_RECOVERY.md).
`artifacts/flash/extracted/` yerel çıkarım arşividir; tekrar edinim değildir.

## ASIC: pasif UART / logic analyzer

Gerçek D0/D2 bağlantısı: **D0 → MCU PA9 TX**, **D2 → MCU PA10 RX**, ortak GND.
İki hat **9600 8N1** olarak çözülür; analyzer VCC kullanılmaz. USART1
ölçüm ASIC'ine ayrılmıştır. SD7502/yakın SDIC ailesi yayımlanmış protokol
benzerliği kimlik tespiti olarak kabul edilmez.

**Kritik düzeltme:** eski firmware DMA tamponu 13 bayttır; gerçek ölçüm
burst'lerinin gözlenen boyutu **12 bayt**tır. MCU→ASIC literal komutları
**10 bayt**, `AA 55 FD ...` ASIC event adayları da **10 bayt** gözlenmiştir.
Bu fark iki parser'ın bütün modlarda birleştirildiği anlamına gelmez.

25 gerçek oturum ve 24 sıkıştırılmış regresyon kesiti korunmuştur. Açık uç
DC/AC akım, NCV/LIVE, frekans, harici probesiz sıcaklık ve bildirilen direnç
referansları vardır. Kapasitans, gerilim kaynağı ve kalibrasyonun tüm
aralıkları fiziksel olarak doğrulanmış değildir. Checksum hipotezleri,
ACK nedenselliği ve bilinmeyen komutlar uydurulmaz.

- [Tel protokolü](../projects/asic/docs/ASIC_UART_SPEC.md)
- [Donanım bulguları](../projects/asic/docs/HARDWARE_FINDINGS.md)
- [Direnç doğrulaması](../projects/asic/docs/RESISTANCE_VALIDATION.md)
- [Kayıt indeksi](../projects/asic/docs/CAPTURE_INDEX.md)
- [Bağımsız C arayüzü](../projects/asic/firmware/INTEGRATION.md)

Tam kayıtlar `artifacts/captures/asic-captures/` içinde, regresyon kesitleri
`projects/asic/tests/fixtures/` içindedir. Sample indeksi/hata bilgisi korunur;
üstverideki bilgisayar yolları yayıma uygun göreli gösterime çevrilmiştir.

## MCU debug / JTAG adıyla anılan RVSWD

Kullanılan adaptör **klasik ESP32**, GPIO25 SWCLK → PA14, GPIO26 SWDIO → PA13;
UART0 GPIO1/3 → CP2102 115200 8N1. Bu ARM SWD veya klasik dört telli JTAG
protokolü değildir. ESP32-S3 önceki plan olup doğrulanan kart değildir.

Son fiziksel kanıt: RDP aktif; running/halted Abstract Access Memory ve
Program Buffer Flash/alias okumaları başarılı olmadı. Özgün MCU Flash'ından
**0 bayt** elde edildi. SRAM `0x20000000` tek word'ü iki okumada aynı
`00 80 02 02` döndü; bu **4 bayt SRAM**, bootloader dump'ı değildir.
`cmderr=3/4` tek başına hatanın tüm nedenini açıklamaz.

Default build teşhis odaklıdır; lab/activation/halt seçenekleri ayrı ve
kapalıdır. Eski programmer referansının depoda bulunması aktif build'de Flash
programlama/unlock yolu bulunduğu anlamına gelmez. Bu birleştirmede cihaza
hiçbir komut gönderilmedi.

[Son recovery raporu](../projects/rvswd/FLASH_RECOVERY_REPORT.md),
[teşhis ve bağlantı kanıtları](../projects/rvswd/DEBUG_CAPABILITIES.md),
[uygulamadan export fizibilitesi](FIRMWARE_DUMP_FEASIBILITY.md).
Tam oturumlar `artifacts/captures/rvswd-*` altında yereldir.

## Birleştirmede yapılan doğrulama

Adresler, ölçüm politikası ve menü state machine'i bu düzenlemede değiştirilmedi.
İlgili değişiklikler proje konumları, build yolları, yayın dizinleri ve
belgelerdir. Tam emülatör senaryoları tekrar koşturulmadı. Mevcut ekranlar
kullanıldı, Studio için kısa native/offscreen görüntüler alındı.

[Gerçek test/build sonuçları ve hash denetimi](WORKSPACE_MIGRATION.md).

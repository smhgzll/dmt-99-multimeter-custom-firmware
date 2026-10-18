# DMT99 firmware update ve korunmuş Flash export fizibilitesi

17 Ekim 2026. Hedef: kullanıcı tarafından bildirilen CH32V303RCT6, RDP aktif.
**Offline prototip için GO; çalışan orijinal cihaza kod yükleme ve kayıpsız tam dump
iddiası için NO-GO.** RDP kaldırılmadı, debug/seri/USB cihaza bağlanılmadı. Firmware
BIN, payload, patch veya flashlanabilir yeni imaj üretilmedi.

Flash'tan normal kullanıcı kodu çalıştırarak okuma yapılması koşullu olarak mümkün
görünüyor. Ancak bu kodu **orijinal içeriği değiştirmeden** başlatacak doğrulanmış
bir yol bulunmadı. Updater'ın silme sınırları ve geri dönüş davranışı bilinmiyor.
Bu nedenle bu çalışma RDP bypass başarısı veya uygulanabilir cihaz prosedürü değildir.

## Kanıt kapsamı

439 fonksiyonun mevcut decompilation/disassembly, çağrı grafiği, USART/Flash/MMIO
envanteri, native persistence kaynakları, önceki USB ve SPI incelemeleri okundu.
Ghidra analizi yeniden üretilmedi. Yeni dar inceleme
[`firmware_update_evidence.json`](../analysis/firmware_update_evidence.json) içinde
orijinal instruction adresi, ham BIN baytları ve çağrı yerlerini saklar.

Üretici yazarlı WCH **CH32FV2x_V3xRM v2.5** ve **CH32V307DS0 v3.9** incelendi.
Resmî [referans el kitabı sayfası](https://www.wch.cn/downloads/CH32FV2x_V3xRM_PDF.html)
ve [datasheet sayfası](https://www.wch.cn/downloads/CH32V307DS0_PDF.html) erişilebilir;
PDF indirme API'si tekrar deneme hatası verdi. PDF'ler erişilebilir aynadan alındı;
üretici sunucusundan doğrudan alınmış dosya oldukları iddia edilmiyor. URL, SHA-256,
üretici API metadatası ve ilgili sayfaların metni
[`references/wch-dump-feasibility/`](../references/wch-dump-feasibility/) altında.
SDK'nin sabitlenmiş WCH Flash sürücüsü ayrıca kontrol edildi. Forum veya ticari
“unlock” iddiası erişim garantisi olarak kullanılmadı.

## BIN formatı ve adres haritası

`DMT-99_V2.4_20240306.bin`: **135096 / 0x20FB8 bayt**;
SHA-256 `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
Dosya, başlangıcı kesilmiş tam MCU dump'ı veya ARM vektör tablosu değildir; doğrudan
RV32 uygulama görüntüsüdür. Dosya ofseti `f` için amaçlanan eşleme:

```
link/normal yürütme adresi = 0x00006400 + f
kanonik Code Flash adresi = 0x08006400 + f
```

| Bölge | Kanonik fiziksel adresler, son dahil | BIN ofseti | Kanıt |
|---|---|---|---|
| Eksik uygulama öncesi alan | `08000000–080063FF` | yok | 25 KiB; içeriği ve görevi bilinmiyor |
| Entry ve dolgu | `08006400–08006437` | `00000–00037` | `6f 60 80 26`: `j 0000C668`; ardından NOP/EBREAK |
| Vektörler/dolgu | `08006438–080065FF` | `00038–001FF` | düşük alias adresli vektörler |
| Mevcut text bölgesi | `08006600–0801E81F` | `00200–1841F` | korunmuş talimat envanteri |
| Sabitler | `0801E820–080271CF` | `18420–20DCF` | font, tablo ve metinler |
| Başlatılmış SRAM verisinin yük kopyası | `080271D0–080273B7` | `20DD0–20FB7` | reset döngüsü, 488 bayt |
| BIN sonrasındaki Code Flash | `080273B8–08077FFF` | yok | üretici haritası; bu cihazdan okunmadı |
| Ayar/geçmiş okuma/yazma niyeti | `08040000–080400FF` | yok | `D928`, `D87C` |
| Varsayılan SRAM | `20000000–2000FFFF` | reset ile kopyalanır | stack top `20010000`, BSS sonu `200054D8` |
| WCH sistem boot alanı | `1FFF8000–1FFFEFFF` | yok | RM tablo 32-1; **eksik ilk 25 KiB ile aynı alan değil** |
| User option bytes | `1FFFF800–1FFFF87F` | yok | RM tablo 32-1; prototip kapsamı dışında |

WCH datasheet tablo 2-1-1 ve notu RC modelinde toplam **480 KiB Code Flash** ile
256 KiB sıfır beklemeli alan / 64 KiB SRAM varsayılanını ayırır. Bu, `08040000`
adresini nominal 256 KiB'ın dışında diye geçersiz sayan eski yorumu düzeltir;
cihazın mevcut partition seçeneği ve o adresteki veri hâlâ okunmuş değildir.
[WCH datasheet, PDF s.5–6](https://ch32-riscv-ug.github.io/CH32V307/datasheet_en/CH32V20x_30xDS0.PDF#page=5).

`C668–C6F2` reset talimatları düşük alias üzerinde `gp=200009B0`, `sp=20010000`,
data source `271D0`, destination `20000000`, data end `200001E8`,
`mtvec=643B`, `mepc=908A` kurar. **Fiziksel adres ile yürütme adresi birbirinin yerine
konamaz:** aynı AUIPC dizisini `0800C668` üzerinde yürütmek `gp=280009B0` üretir.
Üst alias'a kör atlama veya raw BIN'i sıfır adresine yazma bir yükleme planı değildir.
Gerçek cihazda bu dosyanın birebir yüklü olduğu henüz karşılaştırılmadı.

### Header, CRC, imza ve sürüm

İlk baytta geçerli entry bulunduğu için koddan önce ayrı bir container header yok.
Dosyanın son baytı resetin kopyaladığı data extentinin tam sonudur; onun arkasında
ayrı trailer yok. Son 4 bayt `50 01 00 20`, yani SRAM adresi değeridir; önceki
baytların standart CRC32'si değildir. Tüm dosyanın CRC32'si `8EF0046F` yalnızca
bizim envanter değerimizdir, cihazın kabul ettiği checksum olarak sunulmuyor.

Sabit `DMT-99 V2.4`, link `24BA0` adresindedir; `143E0` bunu metin renderer'ına
gönderir. Bu bir sürüm **gösterimidir**. Uygulamada güncelleme dosyası boyutu/sürümü,
CRC veya imza doğrulayan bir akış kurulamadı. Başlatma verisinin içinde kontrol
alanı bulunması, güncelleyicinin tüm dosyaya haricen kontrol uygulaması veya dosya
adını sınaması dışlanmış değildir. Birkaç CRC denemesinden “imza yok / her payload
kabul edilir” sonucu çıkarılmadı.

## Flash yazma ve updater bulguları

| Orijinal link entry / çağrı yeri | Gerçek davranış / hedef |
|---|---|
| `C320`, call `C340 → D87C` | shutdown: ekran/buzzer/backlight, ayar commit, PC1 düşük, sonsuz döngü |
| `D928`, call `D93C → D908` | `08040000`'dan `20000020`'ye 128 halfword okur |
| `D87C`, call `D8AC → CA52` | `08040000` için tek fast erase isteği |
| `CA52`, stores `CA62/CA64/CA6C` | CTLR `40022010`: page erase `20000`, ADDR `40022014`: `address & FFFFFF00`, START `40` |
| `D87C`, call `D8F8 → C9EE` | status `4` kaldıkça `08040000, ... ,080400FE`'ye en çok 128 halfword |
| `C9EE`, store `CA14` | CTLR PG biti açılır, hedefe 16-bit store, bekle ve PG temizle |
| `C982/CA26`, `C99C/CA44` | standart/fast unlock ve lock; option-byte/RDP kaldırma akışı değil |
| `DD4E → FB48("0:BOOT")` | SPI FatFs üzerinde dizin oluşturur; MCU bootloader geçişi değil |
| `C5F2/C5F4/C644` | USB vektör hedefleri default trap; MSC işleyicisi değil |

`FLASH_ProgramHalfWord`, `FLASH_ErasePage_Fast` ve unlock/lock rutinlerinin mevcut
doğrudan çağıranı yalnızca `D87C`'dir. Bunun çağıranı shutdown `C320`'dir.
Firmware uygulama alanına erase/program, USB MSC komut işleyicisi, reset bayrağıyla
bootloader'a geçiş veya updater dosya yükleme akışı **bulunamadı**. Bu sonuç mevcut
439 body, instruction/MMIO inventory ve önceki descriptor taraması kapsamındadır;
eksik bölgede böyle kod bulunmadığını kanıtlamaz. SPI Flash'ın `D538/D588/D6BC`
erase/page/sector yazmaları harici NOR içindir ve MCU update ile karıştırılmadı.

[`test_update_flash_trace.py`](../tools/test_update_flash_trace.py), `D87C` ve `D928`
orijinal entry'lerini çalıştırarak C persistence callback sözleşmesiyle karşılaştırır.
25 commit vakası status/failure sınırlarını, 16 load vakası tüm 256 baytı kontrol eder.
SDK gövdeleri de yürütülür; MMIO yalnızca özel Python belleğinde niyet kaydıdır,
CSR `800` atlanır, busy/ready/WRP yanıtları modellenir. **41 vaka geçti; fiziksel
erase/program veya RDP modeli doğrulanmadı.**

### İlk 25 KiB güncellemede korunur mu?

**Mevcut kanıtla bilinmiyor.** BIN bu bölgeyi içermez; bu, updater'ın orayı silmediği
anlamına gelmez. Üç ayrı varsayımsal silme geometrisi sonucu değiştirir:

| Olası erase birimi | `08006400` için aşağı yuvarlanan başlangıç | Eksik alanla örtüşme |
|---|---|---|
| 256 bayt fast page | `08006400` | yok |
| 4096 bayt standart sector | `08006000` | `08006000–080063FF`, 1024 bayt |
| 32 KiB block | `08000000` | tüm 25 KiB; RDP/yazma koruması işlemi ayrıca etkiler |

Bunlar gerçek updater izleri değildir. RDP'nin otomatik yazma koruması yalnız ilk
4 KiB için belgelenmiştir; sonraki 21 KiB için koruma garantisi sağlamaz. WRP
seçenekleri, round-down/round-up davranışı, mass erase, başarısız güncellemeden
kurtarma, dosya adı ve imza politikası bilinmeden updater'a payload verilemez.

**Pristine tam dump açısından daha temel engel:** export kodunu uygulama sayfalarına
yazmak, o sayfaların orijinal baytlarını daha export başlamadan değiştirir. Resmî
BIN'den sonra yerine koymak, cihazdaki önceki baytların bu BIN ile aynı olduğunu
kanıtlamaz. Dump öncesi değişen sayfalar başka kanaldan korunmadan bu yöntemle
“orijinal tam Flash” elde edildiği söylenemez. Sadece eksik prefix'i kurtarma hedefi
bile önce updater'ın o prefix'i koruduğunun kanıtını gerektirir.

## RDP erişim matrisi ve kesinlik sınırı

RM v2.5 §32.2.2, §32.5.1, §32.6 ve `RDPRT` tanımı incelendi. §32.5.1 kullanıcı
kodunun genel adres alanından 8/16/32-bit Flash verisi okuyabildiğini tanımlar;
tek tek RDP/yürütme bağlamlarını ayıran kapsamlı bir okuma matrisi vermez.
RDP kaldırılması otomatik ana
Flash silmesi başlatır; bunu koruma yöntemi olarak kullanmak **NO-GO**.
[WCH RM, PDF s.603 ve 616–617](https://ch32-riscv-ug.github.io/CH32V20x/datasheet_en/CH32FV2x_V3xRM.PDF#page=603).

| RDP aktif bağlam | CPU/data okuma | Erase/program | Karar / kanıt sınırı |
|---|---|---|---|
| Normal user Flash boot, debug bağlantısı olmadan | ROM sabitlerini ve `D928` verisini okuyan uygulama bu yolu destekliyor; eksik prefix'in tamamı cihazda doğrulanmadı | ilk 4 KiB otomatik korumalı; diğerleri WRP/çalışma koşullarına bağlı | Flash export için koşullu aday |
| SRAM'den boot | seçilen RM genel RDP “unreadable” tanımı dışında ayrıntılı CPU-read tablosu vermiyor | RM normal erase/programı yasaklar | kayıpsız okuma yolu olarak kanıtlanmadı; NO-GO |
| Flash boot sonrasında SRAM rutinine atlama | boot kaynağı mı anlık PC mi belirleyici: seçilen belgelerden kesinleşmedi | güvenli kabul edilmez | ayrı deney gerekir |
| Sistem boot kodu / ISP | belgelenmiş kayıpsız readout servisi bulunmadı | normal erase/program sınırlı; mass erase istisnası korumaz | USB/USART ISP, RDP readout garantisi değildir |
| SDI/debug | RDP tanımı readout'u engeller | normal erase/program sınırlı | ESP32-S3 debug arayüzü bu yetkiyi kendiliğinden vermez |
| DMA veya başka bus master | bu hedef/koruma bağlamında doğrulanmadı | kullanılmıyor | export yolu seçilmedi |

“RDP aktifken CPU hiçbir Flash verisini okuyamaz” sonucu çıkarılamaz: çalışır
uygulamanın sabit veri ve ayar okuması bununla uyumlu değildir. Buna karşılık
**seçilen resmî RM bütün yürütme/okuma bağlamlarını açıkça ayırmadığı için**
“Flash kodu her korumalı adresi kesin okur” veya “RAM kodu başarılı olur” da
kanıtlanmış değildir. Normal Flash boot üzerinden export **koşullu fizibilite**,
cihazda başarı ise **doğrulanmamış** durumdadır. Diğer MCU ailelerinin RDP seviyeleri
bu parçaya aktarılmadı; seçenek baytlarına yazma prototipe eklenmedi.

## Aktarım kanalı seçimi

**Aday: USART2 TX / PA2, tek yönlü 115200 8N1.** `main 908A`, `1A682`'ye 115200
verir. `1A682`, GPIOA pin maskesi `0004` ile PA2 alternate output'u, USART2
`40004400`'ı ve TX-only mode'u kurar. `1A6E2`, TC bekleyip USART2'ye bayt gönderir.
Bu uygulama kanıtı ASIC protokolünden bağımsızdır.

| Kanal | MCU pin kanıtı | DMT99 bağlantı kanıtı / sonuç |
|---|---|---|
| USART2 TX | PA2, 64 pin pakette pin 16; USART2_TX mux | firmware GPIO/USART kurulumu var; PCB pad/header ve süreklilik ölçümü yok. En güçlü aday |
| USART2 RX | PA3, 64 pin pakette pin 17; USART2_RX mux | orijinal debug kurulumu RX'i açmaz; pad/hat bilinmiyor. Tekrar istek kanalı varsayılmadı |
| USART1 | PA9/PA10, 64 pin pakette 42/43 | `C350/97C4/C468`, 9600 8N1 ASIC TX/RX. Kullanılmayacak |
| USB FS | PA11/PA12, 64 pin pakette 44/45 | paket mux'u var; DMT99 konektör rotası/aktif application stack kanıtı yok. Şimdilik seçilmedi |
| USART3 / yeni GPIO | mux olasılığı tek başına yeterli değil | GPIOB display veri portu; spare pin veya yeni UART hattı varsayılmadı |
| Harici SPI Flash | mevcut SPI1/NOR yolu | export için NOR yazmak orijinal harici kanıtı değiştirir; seçilmedi |

Pin eşlemesi [WCH datasheet pin tabloları](https://ch32-riscv-ug.github.io/CH32V307/datasheet_en/CH32V20x_30xDS0.PDF#page=31)
ile kontrol edildi; fiziksel cihaz pin/pad haritası değildir. Prototip USART2'yi veya
GPIO'yu başlatmaz. Gerçek bağlamada yalnız MCU TX'ten alım, uygun VIO seviyesi ve
ortak referans gerekir; bilinmeyen PA3'e TX bağlanması planlanmadı. Binary aktarım
sırasında orijinal debug printf karışması engellenmelidir.

## Salt okunur offline prototip

[`flash_export.c`](../reconstructed/offline_export/flash_export.c) yeni deneysel C
kodu; orijinal fonksiyon çevirisi değildir. Sadece `read(address, out, n)` ve
`send(frame, n)` callback'leri vardır. Flash controller unlock/erase/program,
option-byte erişimi, gerçek MMIO okuyucusu, startup veya updater entegrasyonu yok.
Dosya backend'i orijinal BIN'i `rb` açar; bulunmayan prefix'i sentezlemez.

İki sınır birlikte kontrol edilir: üretici Code Flash aralığı
`[08000000,08078000)` ve çağıranın daha dar, bağımsız doğrulanmış okuma politikası.
Sıfır uzunluk, taşma, policy dışı başlangıç/son ve 2–8 dışında pass reddedilir.
Her en çok 256 bayt blok, gönderilmeden iki kere okunup karşılaştırılır. Varsayılan
iki tam pass ile her bayt dört kez okunur; read/send hatasında işlem durur.

**D99X v1 bizim test protokolümüzdür; FNIRSI/WCH update protokolü değildir.**
Tüm çok baytlı alanlar little endian:

| Ofset | Alan |
|---|---|
| `0..3` | `D99X` magic |
| `4..7` | version=1, type: DATA=1/END=2/ERROR=3, pass index, pass count |
| `8..11` | çağıranın belirlediği session |
| `12..15` | block index; END için toplam blok sayısı |
| `16..19` | blok adresi; END için aralığın sonu |
| `20..27` | beklenen range start ve byte length |
| `28..31` | payload length u16, status u8, reserved=0 |
| `32..` | DATA: 1–256 bayt; END: pass veri CRC32'si, ERROR: veri yok |
| son 4 bayt | header+payload CRC32/ISO-HDLC (`EDB88320`, init/final XOR `FFFFFFFF`) |

Host [`flash_dump_receiver.py`](../tools/flash_dump_receiver.py) capture dosyası/stdin
okur. Beklenen range/session dışarıdan verilir; bozuk CRC'de yeniden senkronize olur,
eksik blok/END, değişen metadata, tutarsız duplicate, farklı iki pass veya ERROR
başarılı dump üretemez. Eşit duplicate ve farklı blok sırası kabul edilir. Tüm
kontrollerden sonra yeni dosya oluşturur, var olan dosyayı değiştirmez. SHA-256,
pass CRC, kayıp/gürültü istatistikleri raporlanır. `--known-bin` kesişen uygulama
bölgesini karşılaştırır; kesişim yoksa başarısız olur. `compare` iki ayrı dump'ın
uzunluk, farklı bayt ve SHA-256 karşılaştırmasını yapar.

Tek yönlü kanalda host resend komutu yoktur: tekrar okuma iki pass içinde gerçekleşir;
kayıp aktarım için ileride bağımsız yeni oturum alınacaktır. PA3 bağlantısı uydurulmadı.
Session ve CRC kimlik doğrulama/freshness veya RDP başarısı kanıtı değildir; aynı
yanlış/sabit veriyi tekrar okumak da aynı CRC'yi üretir.

Örnek **yalnız dosya replay'i**, proje kökünde:

```sh
make -C reconstructed/offline_export test sanitize
work_dir=$(mktemp -d /tmp/dmt99-export.XXXXXX)
reconstructed/build/offline_export/flash-export-replay \
  DMT-99_V2.4_20240306.bin 0x08006400 0x08006400 135096 123 \
  > "$work_dir/capture.d99x"
python3 tools/flash_dump_receiver.py receive "$work_dir/capture.d99x" \
  --start 0x08006400 --length 135096 --session 123 \
  --known-bin DMT-99_V2.4_20240306.bin \
  --output "$work_dir/application.bin" --report "$work_dir/report.json"
python3 tools/flash_dump_receiver.py compare \
  DMT-99_V2.4_20240306.bin "$work_dir/application.bin"
```

Bu çıktı **MCU dump'ı değildir**, zaten var olan uygulama dosyasının aktarım testidir.
Tam dump geldiğinde `tools/ingest_mcu_dump.py` mevcut uygulama karşılaştırmasını
yeniden analiz başlatmadan yapabilir; prefix-only dosya tam dump diye içeri alınmamalı.

## Flash ve RAM yürütme için ayrı doğrulama planları

**Önce offline / ayrı test donanımı:** C protokol, bağımsız Python wire oracle'ı,
sınırlar, CRC, eksik/bozuk/sırası değişmiş/tekrarlanmış frame, read/send failure ve
farklı pass vakaları geçti. Altı Python testi 1 bayttan 480 KiB'a sentetik belleği
ve değişmeyen orijinal BIN'i kullanır. C test yürütülebiliri ve ASan/UBSan yürütmesi
geçti. `D87C/D928` için 41 bounded karşılaştırma geçti.

RV32IMAC/ILP32 için iki **relocatable object** derlendi: normal `.text.*` ve
`DMT_EXPORT_RAM_EXPERIMENT` ile `.ramtext.dmt_export`. RAM varyantının doğrudan
kod referansları aynı section'da, dış unresolved sembolü yok; iki IO callback'inin
gerçek konumu yine bağlayana bağlıdır. Bunlar link edilmedi ve yürütülmedi.
Object/komut/hash bilgisi `flash_export_object_build.json`, section/relocation
çıktısı `logs/flash-export-object-build.log` içindedir. Section derlemesi RDP testi
veya SRAM yükleme yöntemi değildir.

1. **Flash'tan çalışma, ayrı CH32V303RCT6:** önceden içeriği bilinen canary desenleri
   bulunan test parçasında RDP kapalı baseline, sonra aynı parçanın RDP aktif
   normal Flash boot çalışması karşılaştırılmalı. Loader/okuyucu/CRC/UART callback
   adresleri, stack, interrupt kaynakları ve hiçbir Flash-write çağrısı bulunmadığı
   son ELF üzerinden kontrol edilmeli. Debug bağlantısı olmadan cold boot; düşük
   alias kodu ile kanonik Flash okumaları ayrı kaydedilmeli. Eksik prefix, application,
   `08040000` ve üst non-zero-wait alan örnekleri doğrulanmalı. RM'nin Flash erişim
   clock koşulları sağlanmalı; orijinal 144 MHz konfigürasyonu kör kopyalanmamalı.
2. **SRAM'den boot, aynı model test parçası:** RDP kapalı pozitif kontrol, RDP aktif
   CPU read sonucu/fault/timeout karşılaştırması ayrı yapılmalı. Programın çalışması
   read erişiminin başarılı olduğu anlamına gelmez. İstenmeyen option-byte yazma,
   otomatik unprotect veya mass erase seçenekleri kullanılmamalı. Sonuç negatifse
   orijinal cihaz üzerinde denenmemeli.
3. **Flash boot → SRAM'a atlama:** ikinci testten ayrı kayıt; anlık PC, loader ve
   tüm callback/constant konumları doğrulanmalı. Flash'a geri çağrı sonucu SRAM
   başarısı diye raporlanmamalı. Bus fault/exception yolları önceden test edilmeli.

Bu deneyler burada gerçekleştirilmedi; test parçasına işlem yapmak dahi bu turda
yapılmış bir eylem değildir.

## Orijinal cihaza müdahaleden önce zorunlu kapılar

- Mevcut MCU/lot, VIO, güç/power-latch davranışı ve PA2'nin gerçekten erişilebilir
  hattı belgelenmeli. USB/USART kablosu ile geri besleme veya ASIC hattı sürülmemeli.
- Güncelleyicinin kendisi veya güvenilir eşdeğer erase/program izi bulunmalı:
  dosya→adres eşlemesi, erase birimi, ilk/son sayfa, option-byte/RDP/WRP davranışı,
  kabul kontrolleri ve yarıda kesilme/başarısız kabul sonrası durum kanıtlanmalı.
- Kaybolabilecek **bütün** içeriğin güncelleme öncesi korunması açıklanmalı.
  İlk 25 KiB, ayar/geçmiş, uygulamadan farklı baytlar ve cihaz özel verileri
  olmadan sadece V2.4 BIN'e dönüş “tam rollback” değildir. 8 MiB SPI kopyası
  dahili Flash yedeğinin yerine geçmez.
- İlgili yürütme bağlamı ayrı aynı model test parçasında, RDP açık ve debug'sız
  doğrulanmalı. İki bağımsız oturumun byte-for-byte karşılaştırması ve bilinen
  V2.4 kesişimi kontrol edilmeli; `00/FF/E339` sabit veri başarı sayılmamalı.
- Sonradan bir payload gerekiyorsa ayrı çıktı/manifest, exact base/hash, clobber
  listesi ve geri dönüş doğrulaması hazırlanmalı. Bu çalışma böyle payload üretmedi.

| Karar | Durum |
|---|---|
| Dosya backend'iyle geliştirme ve protokol doğrulama | **GO, tamamlandı** |
| Ayrı test parçasında RDP bağlamlarını araştırma | koşullu sonraki adım; burada yapılmadı |
| Mevcut DMT99'a güncelleme/payload yükleme | **NO-GO** |
| RDP kaldırarak orijinali koruma | **NO-GO**, belgelenmiş otomatik silme |
| ESP32-S3/RAM üzerinden kayıpsız bypass başarılı iddiası | **NO-GO**, kanıt yok |
| Güncellemede ilk 25 KiB korunur garantisi | **NO-GO**, updater yok ve erase sınırı belirsiz |

Fonksiyon metrikleri **146 complete / 14 partial / 123 incomplete** olarak kaldı;
partial, incomplete toplamına dahildir. Deneysel exporter ve yeni testler orijinal
fonksiyon kredisi kazanmaz. STOCK_COMPATIBLE/CUSTOM_DMT99 uygulama kaynakları ve
ELF'leri değiştirilmedi; mevcut regression raporları korunup bu kapsam dışı büyük
suiteler yeniden çalıştırılmadı. Son denetim: `analysis/flash_export_validation.json`.

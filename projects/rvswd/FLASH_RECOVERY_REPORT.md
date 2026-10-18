# DMT99 orijinal dahili Flash kurtarma araştırması

**Güncel sonuç: orijinal Flash'tan 0 bayt; SRAM'den 4 bayt, iki bağımsız okumada
aynı sonuç.** Flash recovery / RDP bypass gerçekleşmedi. Son ayrı kontrol:
8/8 stable, DMCONTROL=`00000001`, DMSTATUS=`00000c82`, ABSTRACTCS=`08000002`,
pinler bırakılmış; CPU çalışıyor.
[Son durum kaydı](recovery/20261017-final-after-word-tests.json).

## Tamamlanan gerçek deneyler ve ham veri

| Mekanizma / adres | Gerçek sonuç | Restorasyon |
|---|---|---|
| Running Access Memory, `08006400` | cmderr=4, bayt yok | DATA geri yüklendi, CPU running |
| Halted Access Memory, `08006400` | cmderr=3, bayt yok | DATA geri yüklendi; eski ACK kontrolünün belirsizliği aşağıda |
| Program Buffer `lw/ebreak`, `08006400` | cmderr=3, bayt yok | GPR, buffer, DATA ve CPU running doğrulandı |
| Program Buffer, doğrudan prefix `08000000` | cmderr=3, bayt yok | Tam cleanup, PC değişmedi |
| Program Buffer, boot alias `00000000` | cmderr=3, bayt yok | Tam cleanup, PC değişmedi |
| Program Buffer, uygulama alias `00006400` | cmderr=3, bayt yok | Tam cleanup, PC değişmedi |
| Program Buffer, SRAM `20000000` | **02028000**, iki okumada aynı | Tam cleanup, PC değişmedi |

Her yeni profil ayrı halt/read/restore/resume çevrimidir. Hata alan bir load
aynı çevrim içinde tekrar edilmedi; sonraki profil ancak önceki çevrimin
restorasyonu doğrulandıktan sonra başlatıldı. Genel DMI yetenek taraması
tekrarlanmadı; her çevrimin 8 örnekli güvenlik preflight'ı yapıldı.

Kanıtlar:
[Program Buffer uygulama deneyi](recovery/20261017-original-progbuf-01/session.json),
[prefix](recovery/20261017-word-prefix-01/session.json),
[boot alias](recovery/20261017-word-boot-alias-01/session.json),
[uygulama alias](recovery/20261017-word-app-alias-01/session.json),
[SRAM](recovery/20261017-word-sram-01/session.json).
Her oturumda ham UART, DMI trace, gerçek komut/adres, hata ve cleanup alanları vardır.
[Toplu hash/trace denetimi](recovery/20261017-word-profiles-validation/hardware-results.json)
21 eski capture'ın değişmediğini de doğrular. Timeout/crash oturumlarının eksik
trace'i tam işlem kanıtı olarak sayılmaz.

SRAM raw dosyaları (Flash dump **değildir**):

| Adres | Boyut | Little-endian baytlar | Dosyalar |
|---|---:|---|---|
| `0x20000000` | her pass 4 bayt | `00 80 02 02` | [pass 1](recovery/20261017-word-sram-01/original-sram-pass1.unverified.bin), [pass 2](recovery/20261017-word-sram-01/original-sram-pass2.unverified.bin) |

İki dosyanın SHA-256 değeri:
`47954a621ff103a08d6249925cb355e9c1fb8676c111128631d454b31f3cf1ba`.
Dosyalar birebir karşılaştırıldı ve DMI word'lerinden oluşan byte dizisiyle
eşleştirildi. SRAM'in önceden bilinen bir referansı olmadığı için `.unverified`
adı korunuyor; bağımsız tekrar eşitliği gerçek bir ölçümdür.

Bu pozitif SRAM kontrolü, aynı `lw/ebreak` ve register aktarım yolunun veri
döndürebildiğini gösterir. Böylece Flash'taki sonuç yalnız UART/bağlantı veya
genel Program Buffer başarısızlığıyla açıklanamaz. Flash/alias exception'ları
aktif RDP ile **uyumludur**, ancak cmderr=3 tek başına özel RDP neden kodu değildir.

Alias adayının kaynağı WCH CH32FV2x/V3x RM v2.5 §1.3'tür: boot seçimine göre
Flash veya sistem belleği `00000000` bölgesine eşlenir. BOOT pinleri, reset
ve mapping register'ları değiştirilmedi; alias'tan hiç veri alınamadığı için
hangi kaynak belleğin döndüğü varsayılmadı.

Son derlemeler: normal build ve `build-recovery-progbuf-lab` başarılı;
**C mock/fault testleri ve 34 Python testi geçti.** Program Buffer çekirdeği
ayrıca ASan/UBSan + 33 host testiyle geçti; son sabit word profilleri C/34 host
paketiyle doğrulandı. [Son build/source manifesti](recovery/20261017-word-profiles-validation/manifest.json),
[testler](recovery/20261017-word-profiles-validation/word-profiles-tests.log),
[çekirdek sanitizer kaydı](recovery/20261017-progbuf-validation/progbuf-sanitize.log).
ESP32'ye yüklenen son imaj deney izinlerini açık tutar; hiçbir hedef komutu
başlangıçta otomatik çalışmaz. Normal build'de lab/activation/halt izinleri kapalıdır.

## Mevcut teknik sınır ve sonraki araştırma

Doğrudan Flash, eksik prefix, alias ve iki farklı debug okuma mekanizması
gerçek cihazda denendi. SBA ilan edilmiyor. İncelenen ROM ISP arayüzünde genel
Code Flash readout komutu gösterilmedi. Autoexec veya sık yeniden deneme,
denenmiş load'a bağımsız bir erişim yetkisi sağlamaz; bu nedenle tekrarlanmadı.

Bir sonraki farklı araştırma, **debug modundan çıkıp normal CPU bağlamında
SRAM'den kod yürütmenin** RDP altında nasıl davrandığıdır. Bu henüz orijinal
cihaz için doğrulanmış bir deney değildir: DMA'nın kullanmadığı scratch RAM,
trap/interrupt davranışı ve tüm değiştirilen CPU durumunun geri dönüşü önce
belirlenmelidir. Analiz deposunda bulunan ELF/map/linker dosyaları reconstruction
ve offline harness çıktılarıdır; canlı orijinalde boş scratch alanını kanıtlamaz.
Bu nedenle orijinal SRAM'e kod yazılmadı veya bilinmeyen Flash fonksiyonuna
atlanmadı. Buradaki sınır yeni kullanıcı onayı eksikliği değil, mevcut
Flash'ı koruma koşuluyla bu yürütme yolunun henüz doğrulanmamış olmasıdır.

Hazır, ayrı ve sabit word komutlarının arayüzü şöyledir (bu dört profil yukarıda
zaten denendi; aynı hata için tekrar çalıştırmak yeni kanıt üretmez):

```sh
python3 tools/recovery.py original-progbuf-word --word-profile prefix \
  --port /dev/ttyUSB0 --output recovery/NEW-UNUSED-DIRECTORY \
  --voltage-mv 3000 --board-pins-and-wiring-checked \
  --allow-original-halt-read-resume --allow-original-progbuf-load
```

Diğer sabit profil adları `boot-alias`, `app-alias`, `sram`dır. Ölçüm alanı
operatörün gerçek güncel değeridir. Host keyfî adres veya kod kabul etmez.
SRAM oturumundaki eski `no_larger_read_authorized` karar metni host'un eski
sabit scope etiketidir; kullanıcının sonraki geniş iznini iptal eden bir karar
değildir. Host'ta bu etiket `bounded_original_samples_checked_no_automatic_expansion`
olarak düzeltildi; gerçek capture değiştirilmedi.

## 2026-10-17: Kullanıcı izniyle halt ve alternatif okuma deneyleri

Kullanıcının “tekrar dene flashı bozmadığın sürece her şeyi dene” talimatı,
geçici halt/resume ve restore edilen debug/CPU register değişiklikleri için
izin olarak alındı. Hedef Flash/option byte/RDP/WRP yazması, erase, reset veya
unlock eklenmedi. Normal build'in bütün deneysel izinleri kapalı kaldı.

`lab-memory-halt-cycle` tek adaptör işlemi içinde preflight, gerekiyorsa idle
ABSTRACTCS.cmderr W1C (`00000700`), halt, sabit Access Memory okuması, DATA
restorasyonu, resume ve pin release yapar. Host bağlantısının kesilmesi bu
firmware cleanup akışını iptal etmez. Bağlantı/abstract busy arızasında kör
resume yapılmaz. Eski kayıtlar değiştirilmedi.

- İlk yeni deneme yanıt vermedi; kullanıcı sonrasında sök-tak yaptığını bildirdi.
  Bu denemenin hedef işlemlerinin tamamlandığı iddia edilmiyor.
- İkinci denemenin [ham UART kaydı](recovery/20261017-original-halt-memory-02/uart.raw.bin)
  **ESP32 main task stack overflow** gösterdi. Bu hedef RDP hatası değil,
  eklenen büyük yerel rapor nesnelerinin seri çıktı sırasında stack tüketmesiydi.
  Raporlar tek console task'a ait static alana taşındı; host'a ham UART kaydı
  ve işlem sırasında adapter yeniden başlarsa anında hata verme eklendi.
  [Düzeltme ve yeniden yükleme](recovery/20261017-halt-cycle-stackfix/manifest.json).
- [Üçüncü deneme](recovery/20261017-original-halt-memory-03/session.json):
  halt gerçekten gözlendi (`DMSTATUS=00000382`). İlk `08006400` adresi için
  `COMMAND=02200000` ardından **ABSTRACTCS=08000302, cmderr=3** geldi.
  DATA0/DATA1 restore edildi; Flash verisi okunmadı, prefix denenmedi.
  Resume yazmasından sonra sekiz ayrı okumada `DMSTATUS=00000c82` görüldü,
  `DMCONTROL=1`, pinler bırakıldı. Buna karşın resumeack bitleri gelmediği için
  o sürüm `cleanup_unconfirmed/poll_timeout` raporladı; bu kayıt aynen korundu.
- [Bağımsız son durum kontrolü](recovery/20261017-post-cycle03-identify.json)
  8/8 stable, running, DMCONTROL=1, ABSTRACTCS=08000302 ve pin release doğruladı.
  Dolayısıyla son durumda CPU'nun çalıştığına dair bağımsız kanıt vardır.

WCH V4 v1.3 tablo 7-4 resumereq'in donanım tarafından geri temizlendiğini,
tablo 7-5 ise resumeack alanlarını tanımlar; metin, bunların bu cihazdaki
örnekleme anında mutlaka gözleneceğini kanıtlamaz. Sonraki **explicit memory
cycle** için dönüş ölçütü ayrıca şu şekilde sınırlandı: önce bizim halt'ımız
gözlenmeli, sonra üç ardışık authenticated/running/no-reset örneği ve son
DMCONTROL=1 doğrulanmalı. `resume_ack_observed` ayrıca raporlanır; ACK görülmüş
gibi gösterilmez. Genel diagnostic halt-cycle'ın eski ACK şartı değiştirilmedi.

`cmderr=3` exception'dır; tek başına RDP nedenini kanıtlamaz. Running'deki
cmderr=4 ile halted'daki cmderr=3 ayrımı artık gerçek cihazda gözlendi.
[RISC-V Debug 0.13.2 §3.12.7](https://riscv.org/wp-content/uploads/2024/12/riscv-debug-release.pdf#page=37)
idle cmderr'nin W1C temizlenmesini ve hata kodlarını tanımlar.

Bir sonraki farklı yol `lab-progbuf-halt-cycle` olarak hazırlandı:
yalnız `lw x9,0(x8)` (`00042483`) ve `ebreak` (`00100073`). Hedef belleğe store
talimatı içermez. x8/x9, DATA0, iki Program Buffer word'ü ve PC önce okunur;
her Flash adresi sabit allowlist'tendir; yürütmeden önce buffer readback'i
doğrulanır. x8/x9 ve buffer geri yüklenip doğrulanır, PC değişmişse resume
edilmez. Exception sonrası yalnız restorasyon için sticky hata temizlenir;
başarısız load tekrarlanmaz. Ayrı default-off `CONFIG_DIAG_PROGBUF_LAB` ve
explicit token gerektirir. Kaynak/build/test kanıtları
[manifestte](recovery/20261017-progbuf-validation/manifest.json).

## Önceki running deneyinin kaydı

> Son gerçek deney — 2026-10-17 22:59 Türkiye saati: DM aktivasyonu başarılı.
> Çalışan orijinal MCU'ya gönderilen ilk Access Memory komutu `cmderr=4`
> döndürdü. **Hiç Flash baytı alınmadı; prefix adresine geçilmedi.** CPU çalışır
> durumda kaldı, DATA0/DATA1 geri yüklendi, pinler bırakıldı. Yeniden deneme yok.

Tarih: 2026-10-17. **Bu çalışmada orijinal MCU'dan kurtarılan yeni Flash verisi: 0 bayt.**
İlk `0x6400` bayt henüz elde edilmedi. Aşağıdaki ESP32 BIN'leri adaptör
firmware'idir; DMT99 dump'ı değildir.

## Önceki activation+running yüklemesi ve orijinal MCU deneyi

Kullanıcının ESP32 güncelleme ve sınırlı running-read izniyle
`build-recovery-active-lab` yüklendi: activation/lab açık, halt/resume kapalı.
179968 bayt uygulama SHA-256:
`5e9d8d30129c497c39b7a55b9f46ae895936d2775d19bfe356ae8643512eaf89`.
[Yükleme kaydı](recovery/esp32-active-lab-upload-20261017T195734Z/upload.log)
üç segment için hash doğrulaması ve çıkış kodu 0 gösterir.
Yükleme sonundaki RTS reset yalnız ESP32 adaptörünedir.
Kullanıcının taze hedef besleme ölçümü 3000 mV olarak kullanıldı.

[Aktivasyon kaydı](recovery/20261017-active-lab-activation-01.json):
8/8 kararlı preflight, chip `30310524`; tek DMI yazması
`DMCONTROL(0x10)=00000001`. Readback `DMCONTROL=00000001`,
`DMSTATUS=00000c82`; hart running/authenticated, pin release başarılı.
Haltreq/resumereq/reset komutu gönderilmedi.

[Orijinal deney kaydı](recovery/20261017-original-running-memory-01/session.json)
65 eksiksiz DMI trace kaydı içerir. DATA1'e `08006400` yazılıp doğrulandı;
COMMAND'a bir kez `02200000` gönderildi. ABSTRACTCS `08000002` değerinden
`08000402` değerine geçti: busy=0, **cmderr=4**. DATA0'dan Flash sonucu
okunmadan duruldu; iki pass de boş, `.bin` çıktısı yok. Önceki DATA0/DATA1
değerleri `00000000` geri yazılıp doğrulandı. Son durum:
`DMSTATUS=00000c82`, `DMCONTROL=00000001`, `data_restored=true`,
`final_valid=true`, `pins_released=true`, `target_state_uncertain=false`.
Sticky cmderr temizlenmedi; DM aktif bırakıldı. Tam debug durum restorasyonu
iddia edilmiyor. RDP/option byte/Flash/CPU register yazması yapılmadı.

Bu bir bağlantı veya timeout hatası değildir; hedef abstract komuta hata
döndürdü. Belgelerdeki cmderr=4 hart durumu hatasıyla uyumludur. Running
bağlamındaki bu komut **engelleniyor**; halted desteği ve RDP'nin ayrıca
etkisi **bilinmiyor**. Sonuç, halted okumanın başarılı olacağını veya RDP
bypass'ını kanıtlamaz. Host'un boş örnekler için ürettiği reference/stale
kontrol hataları gerçek sıfır/stale Flash baytlarının gözlendiği anlamına gelmez.

Normal ve ilk lab build/test kanıtları aşağıdadır. Son host arayüzü ayrıca
`original-lab --allow-original-running-read` ile açık orijinal hedef iznini
kaydeder; spare kimliği iddia etmez. Tel protokolündeki eski
`lab-memory-running-spare` adı aynı running motorunu seçer. Bu ekleme sonrası
28 Python testi geçti. Güncel kanıt/hash envanteri:
[donanım sonuç manifesti](recovery/20261017-original-running-memory-01/evidence-manifest.json).

**Tek sonraki teknik doğrulama:** bilinen içerikli korumasız aynı model spare
MCU üzerinde running/halted Access Memory karşılaştırması. Orijinal MCU'da
halted karşılaştırmaya geçmek ayrı açık halt/resume izni ve güvenilir geri
dönüşü doğrulanmış deney gerektirir; mevcut yüklemede halt/resume kapalıdır.
Orijinal cihazda başka komut otomatik çalıştırılmadı.

## Gerçek kanıt ve önceki sonucun düzeltilmesi

`captures/dm-capabilities-1792233896.json` gerçek önceki capture'dır:
SHA-256 `9dc61b7eebdbf04855b9c2996ba692a536497d34ee68224005392224dcfbb3e8`.
8 kararlı örnek, chip `30310524`, DMCONTROL `00000001`, DMSTATUS `00000c82`,
ABSTRACTCS `08000002`, ABSTRACTAUTO `00000000`, SBCS `00000000` ve başarılı
pin bırakma kaydedilmiştir. Bu veriler standart SBA desteği göstermiyor;
abstract Access Memory desteğini ölçmüyor. authenticated biti RDP'nin kapalı
olduğunu göstermez.

`captures/dmt99-20261017-031509-dump.json` içindeki `read_protection_enabled`
sonucunun kökeni [eski okuyucu](main/rvswd_ro.c), `transfer_flash()` fonksiyonudur:
OBR okumasından sonra `obr & 2` doğruysa **Flash okumasına başlamadan döner**.
OBR `03fffefe`, RDPRT=1 ve `bytes=[0,0]` kaydı bu akışla uyumludur.
İki `.bin.partial` dosyası sıfır uzunluktadır; bunlar sıfır dolu Flash verisi
değil, **hiç Flash verisi alınmadığının** kanıtıdır. Önceki oturumda RDP'nin
belirli bir Flash load komutunu reddettiği veya sıfır döndürdüğü kanıtlanmadı.
Eski akışın x8–x11 ve Program Buffer kullanarak OBR okuması, Flash okuma başarısı
olarak yorumlanmadı. Eski okuyucu normal/deneysel yeni görüntülere bağlanmıyor.

V2.4 referansı `./DMT-99_V2.4_20240306.bin`:
135096 bayt, beklenen taban `08006400`, SHA-256
`4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
Bu dosya güncelleme uygulamasıdır; kayıp prefix veya orijinal MCU'nun tam yedeği değildir.
Mevcut capture'lar ve analiz deposu değiştirilmedi; hash envanteri oturum JSON'larında.

## Kaynaklardan çıkan erişim yolları

| Yol | Kaynak ve somut sonuç | Bu turdaki karar |
|---|---|---|
| Standart SBA | Gerçek SBCS=0; mevcut snapshot kodu SBA ilanı gerektiriyor | Bilinen tarama tekrarlanmadı; Flash için kullanılamayan mevcut yol |
| Abstract Access Register, cmdtype=0 | GPR/CSR/FPR aktarır; tek başına genel bellek okuyucusu değildir | Mevcut register snapshot korunuyor; Flash yolu gibi sunulmadı |
| Abstract Access Memory, cmdtype=2 | WCH belgesinde destek konusunda iç çelişki var; aşağıda ayrıntı | Orijinal running MCU'da ilk komut cmderr=4; halted/RDP ayrımı bilinmiyor |
| Program Buffer | Mevcut eski okuyucu ve minichlink okuma yolu GPR değiştirir ve hedefte load/store talimatları yürütür | Orijinal MCU için ayrı müdahale onayı gerektirir; yürütülmedi |
| Quick Access | Standarda göre geçici halt, Program Buffer yürütme ve resume yapar; WCH ayrıca desteklemediğini belirtiyor | Müdahalesiz yol değil; eklenmedi |
| WCH-Link bellek komutu | İncelenen `wlink` uygulaması halted MCU gerektirdiğini belirtir | USB probe komutu, ESP32'ye doğrudan gönderilebilecek yeni bir DMI read değildir |
| Bellek alias'ları | Sıfır adresindeki eşleme boot seçimine bağlıdır; ayrı okuma mekanizması veya belgelenmiş RDP istisnası değildir | Adres taraması yapılmadı |
| ROM ISP | `wchisp` kodunda Code Flash için program/verify, ayrıca config ve ayrı EEPROM/Data Flash read ayrılıyor | EEPROM read komutunu Code Flash read gibi kullanmak için kanıt yok; ROM'a geçiş de boot değişikliği gerektiriyor |
| Çalışan orijinal uygulama | Mevcut analizde sabit/ayar okuması ve USART2 TX var; genel Flash export komutu gösterilmemiş | CPU'nun kendi verisini okuması debugger readout yetkisi kanıtı değil; ASIC/USART komutu gönderilmedi |

Program Buffer değerlendirmesi [minichlink `DefaultReadWord`](https://github.com/cnlohr/ch32fun/blob/master/minichlink/minichlink.c)
ve yerel eski okuyucunun ayrı incelenmesine dayanır. Hazır minichlink başlatma
akışı çalıştırılmadı; otomatik setup/unlock gibi yan etkiler taşınmadı.
[wlink `read_memory`](https://github.com/ch32-rs/wlink/blob/main/src/operations.rs)
bir WCH-Link probe API'sidir; protokolün başarılı olması korunan Flash'tan doğru
bayt döndüğü anlamına gelmez.
[wchisp komutları](https://github.com/ch32-rs/wchisp/blob/main/src/protocol.rs)
ve [EEPROM okuma uygulaması](https://github.com/ch32-rs/wchisp/blob/main/src/flashing.rs)
incelendi. Verify, verilen veriyle karşılaştırma yapar; bilinmeyen Flash verisini
döndüren genel bir readout arayüzü olarak değerlendirilmedi. Belgelenmemiş ISP
komutu veya karşılaştırma üzerinden tahmin taraması yapılmadı.

WCH SDK'nın yerel `ch32v30x_flash.c:FLASH_GetReadOutProtectionStatus()` fonksiyonu
OBR bitini okur; bir Flash export servisi sağlamaz.
[WCH Flash başvuru el kitabı](https://www.wch-ic.com/downloads/CH32FV2x_V3xRM_PDF.html),
yerel `references/wch-dump-feasibility/CH32FV2x_V3xRM_EN.PDF` v2.5,
§32.2.2/§32.4/§32.5.1/§32.6.4 incelendi: normal bellek okuması ile RDP ve
unprotect ayrı konulardır; RDP kaldırma otomatik silme başlatır. Bu belge,
her debug komutu ve yürütme bağlamı için ayrı bir izin matrisi sunmuyor.
Bu nedenle exception/bus error doğrudan “RDP reddi” olarak etiketlenmiyor.

Yerel üretici uygulaması, SPI'dan daha önce çıkarılmış 135036 baytlık uygulama
ve analiz raporları da incelendi. SPI içindeki aday uygulama `0x6400` tabanlıdır;
ilk 25 KiB'yi sağlamaz. x86 PE kalıntıları MCU bootloader'ı değildir.
[FNIRSI indirme sayfası](https://www.fnirsi.com/pages/manuals-firmwares) ve
[ürün sayfası](https://www.fnirsi.com/products/dmt-99) kontrol edildi; erişilen
sayfa içeriğinden doğrulanabilir tam MCU/boot-prefix paketi elde edilmedi.
Bu, üreticinin hiçbir yerde böyle bir dosyası bulunmadığı iddiası değildir.

## Access Memory ve halted-hart şartı

[RISC-V Debug 0.13.2, §3.6.1.3](https://riscv.org/wp-content/uploads/2019/03/riscv-debug-release.pdf#page=24)
Access Memory'yi isteğe bağlı tanımlar. Uygulama running veya halted erişimi
destekleyebilir; running destekleniyorsa halted erişim de desteklenmelidir.
**Standart bütün MCU'lar için halted olmayı zorunlu tutmaz.** DXLEN=32 için
adres DATA1, okunan word DATA0'dadır. Kullanılan `02200000` komutunda cmdtype=2,
32-bit fiziksel read seçilir; write, postincrement, target-specific alanları sıfırdır.

[WCH QingKe V4 v1.3 §7.1](https://atta.szlcsc.com/upload/public/pdf/source/20250114/61FF18B592E95E2E16D9BB34F2D10746.pdf?Expires=4070880000&OSSAccessKeyId=LTAIJDIkh7KmGS1H&Signature=d0xdk0j%2BxUn6j6Qz14ldAahJFPw%3D&response-content-disposition=attachment%3Bfilename%3DC5456849_%25E5%258D%2595%25E7%2589%2587%25E6%259C%25BA%2528MCU-MPU-SOC%2529_CH32V303RCT6_%25E8%25A7%2584%25E6%25A0%25BC%25E4%25B9%25A6_WJ1573486.PDF)
(üretici belgesinin dağıtıcı kopyası; [resmî belge kaydı](https://www.wch.cn/downloads/QingKeV4_Processor_Manual_PDF.html))
özetinde Access Register ve Access Memory desteği anlatılıyor. Tablo 7-8 ise
type 2 için “Access to memory (not supported)” diyor; tablo 7-9 type 2 alanlarını
ayrıntılı veriyor. Destek **bu silikon revizyonunda ölçülmeden kesinleştirilemez**.
WCH tablo 7-7'de cmderr=4 için suspended olmayan/unavailable işlemci durumu
veriliyor; bu hata tanımı bütün bellek okumalarının halt gerektirdiğini tek
başına ispatlamaz. Tablo 7-9 ayrıca bit18 postexec tanımlar; deney bunu sıfır tutar.

İlk `lab-memory` yolundaki halted şartı bu belirsizlik karşısında **uygulama
güvenlik tercihidir**, doğrulanmış CH32V303 donanım zorunluluğu değildir.
Bunu sınamak için ayrı `lab-memory-running-spare` yolu eklendi. Başlangıçta
korumasız test MCU'su için hazırlandı; sonradan ayrı açık kullanıcı izniyle
orijinal cihazda bir kez denendi ve cmderr=4 alındı. İki yol da mevcut hart
durumunu değiştirmeden işlem yapmayı dener;
uygunsuz durumda otomatik halt/resume, retry veya başka komuta fallback yoktur.

## Tamamlanan implementasyon

[main/rvswd_memory_lab.c](main/rvswd_memory_lab.c), mevcut transportu kullanır.
`CONFIG_DIAG_ABSTRACT_MEMORY_LAB` varsayılan kapalıdır ve activation/halt build
izinlerinden bağımsızdır. Normal build, explicit token gelse bile lab erişimini
politika nedeniyle reddeder. İlk lab build'inde activation ve halt/resume kapalıdır;
son yüklenen active-lab build'inde yalnız activation izni ayrıca açılmıştır.

- Sabit sıra: `08006400–0800640f` uygulama örneği, `08000000–0800000f` prefix
  örneği; iki bağımsız tam okuma turu. En fazla 16 abstract komut.
- Stabil preflight, active DM, authentication, seçilmiş mevcut hart durumu,
  ABSTRACTAUTO=0, idle/no-error ABSTRACTCS ve iki DATA register kontrol edilir.
- Her word için DATA1 adres yazması/readback, yalnız COMMAND=`02200000`,
  bounded busy poll, DATA0 okuması uygulanır. Bu yazmalar debug modülünedir;
  hedef Flash/SRAM/MMIO/CPU register yazması veya Program Buffer yürütmesi yoktur.
- DATA0/DATA1 başlangıçta saklanır. Sağlıklı bağlantı, aynı hart durumu, kapalı
  autoexec ve idle koşulları varsa geri yazılıp okunarak doğrulanır. Sticky cmderr
  temizlenmez; COMMAND yazmaç geçmişi geri yüklenemez. Tam DM rollback iddiası yoktur.
- Frame timeout 100 ms, abstract poll 200 ms. Poll içindeki son frame nedeniyle
  duvar süresi polling bütçesini bir frame kadar aşabilir. İşlem sayısı sabittir;
  UART yanıtları pinler bırakıldıktan sonra yazılır. Host cevap sınırı 45 saniyedir.
- Bağlantı bozulduğunda kör cleanup write yapılmaz. Busy/state değişikliği,
  readback veya cleanup hatasında ham durum, tamamlanan word sayısı ve belirsizlik
  saklanır. Tüm geçerli çağrı çıkışlarında pin bırakma denenir.
- Alias, option byte, settings, kalibrasyon, ROM veya genişletilmiş Flash aralıkları
  bu deneyin adres listesinde yoktur. `08040000` ayrıca doğrulanmadan 256 KiB
  dump varsayımına eklenmez; datasheet'teki 480 KiB toplam ve zero-wait bölünmesi
  gerçek cihaz konfigürasyonu ile ayrı teyit edilmelidir.

Ham `cmderr` her zaman korunur: 2 desteklenmeyen komut, 4 hart durumu, 3 exception,
5 bus error; son iki hata RDP sebebini kanıtlamaz. Transport parity/no-response
ve timeout ayrı sonuçlardır. `result=ok` yalnız deney transferinin tamamlanmasıdır;
firmware `recovery_verified:false` döndürür.

[tools/recovery.py](tools/recovery.py) tek host denetleyicisidir:

- `audit`: mevcut dosyaların hash envanteri ve teknik kararlar; seri porta dokunmaz.
- `connect`: modem hatlarını önceden pasif ayarlayarak bounded `hello`, firmware/
  pin/interlock kontrolü; bilinen capability taramasını tekrarlamaz ve orijinal
  hedef üzerinde lab komutu göndermez.
- `spare-lab`: açık spare doğrulaması, mevcut `--hart-state`, taze ölçülmüş rail,
  wiring ve bilinen prefix test verisi gerektirir. Seçilen tek deneyi gönderir.
  Bütün response/trace yeni oturum JSON'una yazılır; otomatik tekrar yoktur.
- Dönen word adres/sıra/sınırları, iki pass, uygulama referansı, ayrı 16 bayt
  prefix canary, constant/stale ve alias benzerliği kontrol edilir. Alınabilen
  ham parçalar ayrı `.unverified.bin` dosyaları, adres/uzunluk/pass/SHA-256 ile
  kaydedilir. Başarılı spare kontrolü bile orijinal DMT99 kurtarması sayılmaz.

Sabit/repetitif bir gerçek prefix mümkün olabilir; bu kontrol laboratuvarın
pozitif kontrol desenini reddeder, cihazdaki gerçek verinin imkânsızlığını iddia
etmez. Aynı hash tek başına gerçek kaynak veya adres doğruluğunu kanıtlamaz.

## Önceki offline doğrulama ve ilk bağlantı aşaması

Offline C transport, normal diagnostic ve lab diagnostic testleri geçti.
Lab testi hem halted hem running sentetik peer ile her başarılı DMI-read
konumunda parity ve her GPIO write konumunda bağlantı arızası enjekte eder.
cmderr 1–7, busy timeout, autoexec, yanlış hart durumu, değişen hart durumu,
repeat mismatch, inactive DM, eksik DATA register, başlangıç sticky error,
voltaj/wiring/build/runtime gate ve pin-release hataları kapsanır.

**26 Python testi geçti.** Host testsiz “başarılı” sayılmadı: seri peer ile
iki ayrı lab profili, yalnız hello yolu, tam raw dosya/hash/adres üretimi,
referans/prefix uyuşmazlığı, eksik ve stale veri, bozuk adres/word sınırı,
uyumsuz firmware, eksik izin ve no-retry doğrulanır.
Clang AddressSanitizer + UndefinedBehaviorSanitizer ile C ve host paketleri
de geçti (`ASAN_OPTIONS=detect_leaks=0`). Bunların tümü offline/sentetiktir.

ESP-IDF 5.5.5 / klasik ESP32 ile iki build tamamlandı; `esptool image_info`
ikisi için checksum ve validation hash doğruladı:

Kalıcı [doğrulama manifesti](recovery/20261017-validation/manifest.json),
[C/host test kaydı](recovery/20261017-validation/tests-final.log),
[ASan/UBSan kaydı](recovery/20261017-validation/sanitize.log) ve
[son host test kaydı](recovery/20261017-validation/host-final.log) saklandı.
Manifest source/ELF/BIN/config hash'lerini ve eski capture hash karşılaştırmasını içerir.

| Build | İzinler | BIN boyutu | SHA-256 |
|---|---|---:|---|
| `build-recovery-default` | lab/activation/halt kapalı | 178048 | `14c1745a3099f7a8673dacd6a975bcf6843aa744ee706f6697a2633d28bdac84` |
| `build-recovery-lab` | yalnız lab açık | 179968 | `8e06d06f7a5368e62169ca63214fee76407378f8e8327de53bd755aa308cadc2` |

İlk seri bağlantı sandbox içinde `ENOENT` ile başarısız oldu:
[ilk oturum](recovery/20261017-session-01/session.json).
`/sys/class/tty/ttyUSB0` ve CP2102 `10c4:ea60` mevcutken sandbox'ın `/dev`
mount'u ayrı tmpfs'tir. İzinli ana ortam kontrolünde `/dev/ttyUSB0` character
device `188:0`, izin `0666` olarak bulundu. Dolayısıyla arıza sürücü/RDP veya
fiziksel USB yokluğu değil, **sandbox cihaz görünürlüğüdür**.

Ana ortamdan [pasif gerçek adaptör oturumu](recovery/20261017-host-session-01/session.json)
başarılı: protocol 3, GPIO25/26, initialized/board_verified=true,
dm_activation_enabled=true, halt_resume_enabled=false. Yüklü görüntü eski
diagnostic sürümdür; yeni lab alanları yoktur. Yalnız `hello` gönderildi;
**yeni DMI veya bellek deneyi yürütülmedi**. DMT99 RDP'si, Flash'ı, çalışma
durumu ve mevcut capture dosyaları değiştirilmedi.

## İlk önerilen spare deneyi (orijinal cihaz deneyi öncesi)

**Orijinal DMT99 yerine, bilinen test baytları içeren korumasız aynı model
CH32V303 üzerinde Access Memory'yi doğrulamak.** Önce DMT99 SWCLK/SWDIO
hatları adaptörden fiziksel olarak ayrılmalı; yalnız bundan sonra deneysel
ESP32 görüntüsü yüklenebilir. Otomatik adaptör yüklemesi yapılmadı.
Bu aşama için hazır spare/test deseni yoksa donanım önkoşulu eksiktir;
mevcut orijinali test MCU'su yerine kullanmak onaylanmış sayılmaz.

Spare'de ilk aday zaten **running** durumu test eder. Başarılı byte kontrolleri
ve değişmeyen DMSTATUS/DMCONTROL ile beraber destek değerlendirilebilir.
`cmderr=4` gelirse durulur; otomatik halt edilmez. Halted karşılaştırma ayrı
hazırlanmış spare oturumudur. RDP açık orijinalde başarı ayrıca kanıtlanmalıdır;
spare başarısı bunu garanti etmez.

Aşağıdaki komutlar laboratuvar adımının incelemeye hazır arayüzüdür; bu oturumda
çalıştırılmadı. `SPARE_MV` gerçek taze ölçüm, prefix dosyası spare'deki bilinen
16 bayttır; uygulama referansının ilk 16 baytı `08006400`'daki gerçek spare
verisiyle eşleşmelidir. Bunlar MCU'ya test verisi programlayan komutlar değildir.

```sh
python3 tools/recovery.py spare-lab --port /dev/ttyUSB0 \
  --output recovery/NEW-SPARE-RUNNING \
  --unprotected-spare-confirmed --hart-state running \
  --board-pins-and-wiring-checked --voltage-mv "$SPARE_MV" \
  --reference /path/to/known-spare-application.bin \
  --prefix-reference /path/to/known-spare-prefix-16.bin
```

Son orijinal deney bu ilk planı ilerletmiştir: type 2 running bağlamında
cmderr=4 döndürdü. Halted/RDP ayrımı hâlâ ölçülmedi; normal firmware'de
bu deneysel erişim yoktur.
Program Buffer, halt, reset, CPU register yazma veya hedef kod yürütme gerektiren
orijinal cihaz deneyi için ayrı açık onay hâlâ gereklidir. Böyle bir işlem bu
çalışmada yapılmadı. Gerçek prefix baytları olmadan bootloader disassembly veya
tam kurtarma hash'i üretilmedi.

# DMT99 CH32V303RCT6 debug erişim yetenekleri

## Son gerçek donanım sonucu

| Yetenek | Sınıf | Kanıt |
|---|---|---|
| DMI / DM activation | destekleniyor | 8/8 stabil, DMCONTROL=1 |
| Halt | destekleniyor | DMSTATUS=382 gözlendi |
| Resume / running durumu | destekleniyor | Gözlenen halt sonrası üç running örneği; son bağımsız 8/8 c82 |
| Resumeack bitlerinin gözlenmesi | bilinmiyor | WCH'de sampled değer 0; ACK görülmüş gibi raporlanmıyor |
| Program Buffer load + SRAM | destekleniyor (sınırlı örnek) | 20000000 → 02028000, iki bağımsız okuma |
| Halted Flash / prefix / alias load | engelleniyor, neden özel kodla kanıtlanmadı | 08006400, 08000000, 00000000, 00006400: cmderr=3 |
| GPR/Program Buffer/DATA restorasyonu | destekleniyor (bu deneylerde) | Readback ve PC değişmedi kontrolü başarılı |
| Normal modda SRAM helper ile Flash okuma | bilinmiyor | Scratch RAM ve trap/CPU geri dönüşü doğrulanmadı; yürütülmedi |

**Flash'tan yeni veri: 0 bayt.** SRAM raw dosyaları ve SHA-256, gerçek deney
tablosu ve tüm log bağlantıları [FLASH_RECOVERY_REPORT.md](FLASH_RECOVERY_REPORT.md)
içindedir. Son durum DMSTATUS=c82, DMCONTROL=1, ABSTRACTCS=08000002; CPU running,
pinler bırakılmış. Hedef Flash/option byte/RDP/WRP programlama veya silme yok.

Eski bölümler aşağıda tarihsel kanıt olarak korunuyor.

> Sonraki kullanıcı izniyle orijinal MCU halt edildi: `DMSTATUS=382` doğrulandı.
> Halted Access Memory ilk `08006400` adresinde `cmderr=3` döndürdü; bayt yok.
> Resume sonrası running durumu görüldü, resumeack görülmedi. Eski sürümün
> cleanup belirsizliği korunuyor; bağımsız 8/8 running kontrolü ayrıca kaydedildi.
> [Halted deney](recovery/20261017-original-halt-memory-03/session.json),
> [son durum](recovery/20261017-post-cycle03-identify.json),
> [güncel araştırma raporu](FLASH_RECOVERY_REPORT.md).
> ESP32 seri raporlamasında yakalanan stack overflow düzeltildi. Yeni Program
> Buffer adayı ayrı explicit/default-off izinle yalnız load/ebreak içerir.

> 2026-10-17 sonraki araştırma: [FLASH_RECOVERY_REPORT.md](FLASH_RECOVERY_REPORT.md).
> Aşağıdaki tarihsel “Flash dump engelleniyor” sınıfı, önceki okuyucunun RDP
> kontrolünde durmasını ifade eder; sıfır dolu Flash veya donanımda denenmiş
> alternatif Flash-read reddi kanıtı değildir. Son capture SBCS=0 gösteriyor.
> 22:59 Türkiye saati: activation+lab açık, halt/resume kapalı ESP32 görüntüsü
> yüklendi. Orijinal MCU'da DMCONTROL=1 aktivasyonu doğrulandı. İlk running
> Access Memory komutu cmderr=4 döndürdü; hiç Flash baytı alınmadı.
> [Aktivasyon](recovery/20261017-active-lab-activation-01.json) ve
> [deney](recovery/20261017-original-running-memory-01/session.json) yeni
> dosyalardadır. Mevcut capture'lar değiştirilmedi.

| Güncel yetenek | Sınıf | Gerçek kanıt |
|---|---|---|
| DMI ve explicit DM aktivasyonu | destekleniyor | 8/8 stabil, DMCONTROL=1 readback |
| Running hart Access Memory `02200000` | engelleniyor (neden RDP olarak kanıtlanmadı) | İlk adres 08006400, ABSTRACTCS=08000402, cmderr=4 |
| Halted Access Memory / RDP altında Flash okuma | bilinmiyor | Halt denenmedi; Flash baytı yok |
| Deney sonrası DATA restorasyonu ve pin release | destekleniyor (bu denemede) | DATA0/1 readback, DMSTATUS=c82, state_uncertain=false |

DM aktif ve sticky cmderr=4 bırakıldı; hata otomatik temizlenmedi. CPU running
kaldı. Aşağıdaki bölümler önceki geliştirme aşamalarının tarihsel kaydıdır;
güncel yükleme ve deney sonucu yukarıdadır.

Tarih: 2026-10-17. Probe: `projects/rvswd`.
Analiz kaynakları: `.` (yalnızca okundu).

Bu turda salt okunur diagnostic mod geliştirildi, önce offline mock testleri
çalıştırıldı ve ESP32 görüntüsü derlenecek. Fiziksel seri port açılmadı; adaptöre
yükleme, yeni hedef oturumu, Flash dump, hedef firmware patch, halt veya reset
çalıştırılmadı. Bu dokümanın başındaki son gerçek cihaz bulgusu kullanıcı tarafından
bu oturumda bildirilen capture özetidir. Yeni kodun mock başarısı, CH32V303 üzerinde
donanım doğrulaması değildir.

## Mevcut oturum kanıtı

Son bildirilen gerçek cihaz capture özeti: Chip ID `30310524`, DMSTATUS
`00000c82`, 8/8 stable preflight, hart running, authenticated=true,
DMCONTROL `00000000`, sonuç `debug_module_inactive`. Bu bulgu, DM aktivasyonunun
artık ayrı kontrollü bir sonraki aşama olarak ele alınmasını destekler. Capture
dosya adı/hash bu mesajda verilmedi; mevcut capture dosyaları değiştirilmedi.

[captures/dmt99-20261017-031509-dump.json](captures/dmt99-20261017-031509-dump.json)
2026-10-17 00:15:09 UTC tarihli önceki kayıttır. Dosya değiştirilmedi.
SHA-256: `eb4dc1e6dadeb3554c9f64efabe87e5627b9dccc34f5f3bb89d393c3366d511c`.

| Alan | Kaydedilmiş değer | Yorum |
|---|---|---|
| Chip ID, DMI `0x7f` | `30310524` | CH32V303 aile ipucu; tam paket/kapasiteyi tek başına kanıtlamaz |
| DMCONTROL, `0x10` | `00000001` | `dmactive=1`, seçili hart 0, okunabilir reset alanları 0 |
| DMSTATUS, `0x11` | `00000c82` | version 2, authenticated 1, authbusy 0, any/allrunning 1, any/allhalted 0 |
| HARTINFO, `0x12` | `00312380` | Ham değer korundu |
| ABSTRACTCS, `0x16` | `08000002` | busy 0, cmderr 0, datacount 2, progbufsize 8 |
| CPBR, `0x7c` | `00000000` | Okuma başarılı; sıfır değer ek bir yetenek kanıtlamaz |
| Kimlik örnekleri | 8, result `ok` | 41 DMI trace kaydı; bu trace kimlik aşamasına aittir |
| Önceki OBR okuması | `03fffefe` | RDPRT bit 1 set: RDP aktif |
| Önceki işlem sonucu | `read_protection_enabled` | Her iki pass için 0 bayt; Flash kapasitesi okunmamış (`flash_kib=0`) |
| Önceki cleanup bildirimi | restored/resumed/pins_released `true` | Eski okuyucunun bildirimi; yeni resumeack kontrolünün doğrulaması değildir |

Önceki iki `.bin.partial` dosyası da 0 bayttır; geçerli Flash dump olarak
kullanılmamalıdır. Bu dosyalara yeni veri yazılmadı. Eski OBR değeri yeniden
okunmadı; yeni oturum JSON'undaki `read_protection: unknown`, **o oturumda yeniden
ölçüm yapılmadığını** anlatır ve mevcut RDP kanıtını geçersiz kılmaz.

Daha erken [diagnose kaydı](captures/diagnose-20261017-025841.json), CPBR'de
`parity_error` gösterir. [Sonraki kayıt](captures/test-20261017-030513-diagnose.json),
üç tamamlanmış örnekten sonra chip ID okumasında `no_response` gösterir. Dolayısıyla
önceki başarılı oturum, sonraki her bağlantının sağlıklı olacağını garanti etmez;
bu hatalar RDP kanıtı olarak sınıflandırılmadı.

## Cihazın bilinen yetenekleri

| Yetenek | Sınıf | Kanıt / sınır |
|---|---|---|
| DMI kimlik, DMCONTROL ve DMSTATUS okuma | **destekleniyor** | Önceki oturumda sekiz tutarlı örnek |
| CPU halt | **destekleniyor, geçmiş oturum kanıtı** | Eski okuyucu OBR okuma ve restore aşamalarını tamamlamış; bağımsız halt izi yok |
| CPU resume | **destekleniyor, geçmiş oturum bildirimi** | `resumed=true`; yeni any/allresumeack kontrolü cihazda denenmedi |
| x8–x11 okuma/koruma | **destekleniyor, eski akıştan çıkarım** | Eski OBR okuyucusunun başarılı hazırlık/restore akışı; register değerleri capture'da yok |
| PC ve x0–x31 tam snapshot | **bilinmiyor** | Yeni read-only abstract register yolu yalnızca mock üzerinde doğrulandı |
| SRAM snapshot | **bilinmiyor** | Cihazdan SRAM alınmadı; SBA desteği henüz okunmadı |
| GPIO / USART / SPI snapshot | **bilinmiyor** | Yeni güvenli profiller yalnızca mock üzerinde doğrulandı |
| Flash dump | **engelleniyor** | RDP kanıtı ve önceki 0 baytlık sonuç; bu sürümde komut kaldırıldı |
| Flash / option byte / RDP / WRP / kalibrasyon yazma | **engelleniyor, yazılım politikası** | Host komutu ve hedef bellek yazma yolu yok |
| Reset | **engelleniyor, yazılım politikası** | Komut yok; reset bitlerini içeren DMCONTROL değerleri reddediliyor |

## Yeni diagnostic arayüzü

Firmware kimliği `protocol: 3`, `build: read-only-diagnostic`.
`tools/diag.py`, eski `flash-reader` görüntüsüne diagnostic göndermez.
`info` içindeki `flash_dump`, `flash_write`, `target_memory_write` ve
`reset_command` alanları `false` olmalıdır.

| Komut | İşlem | Hedef CPU çalışma durumu |
|---|---|---|
| `info` | Adaptör bilgisi; hedef sinyallerini sürmez | Değiştirmez |
| `diagnose` / `identify` | Mevcut beş DMI sorgusu; CPBR ayrı, opsiyonel sonuç | Değiştirmez; DM etkinleştirmez |
| `capabilities` | Beş sorguyla kararlı preflight; aktif DM'de ABSTRACTAUTO/SBCS ve son durum | Değiştirmez; varsayılan seçim maskesi 0, **DMI write sayısı 0** |
| `capabilities --registers` | Zaten halted hart için DPC ve x0–x31 okur | Running ise `hart_running_no_implicit_halt`; halt göndermez |
| `capabilities --sram/--gpio/--usart/--spi` | Açıkça seçilen sabit snapshot profili; SBA varsa | Halt/resume/reset göndermez |
| `halt-cycle` | Açıkça seçilen halt ve eski running durumuna dönüş deneyi | Varsayılan build'de kapalı |
| `activate-dm` | Ayrı, açık DM etkinleştirme deneyi | Varsayılan build'de kapalı |

`memory_read:true` ve `dmi_write:true` info alanları yazılımın isteğe bağlı
snapshot yeteneğini belirtir; bir sorgunun bellek okuduğu veya cihaza bu yeteneğin
verildiği anlamına gelmez. Gerçek snapshot'lar JSON `snapshots` alanında bulunur.

Her capability JSON'da `status: supported / blocked / unknown`, `reason` ve
`failure_class` taşır. `supported`, ilgili oturumda ilgili yolun başarılı olduğu
anlamına gelir. `blocked`, açık authentication/protection reddi veya seçilen
protokolün desteklenmemesi gibi somut kanıt gerektirir. Test edilmemiş erişim,
kapalı clock, running hart, transport hatası veya nedeni belirsiz erişim hatası
`unknown` kalır. SBA desteklenmiyorsa `unsupported_debug_capability`, **bu sürümün
SBA yolunun** engellendiğini belirtir; başka debug yöntemlerinin imkânsızlığını veya
RDP'nin SRAM'ı engellediğini kanıtlamaz.

### Register okuma ve snapshot sınırları

`--registers`, RV32 Access Register komutunda transfer=1, write=0, postexec=0 ve
postincrement=0 kullanır. PC, halted durumdaki **DPC (`0x7b1`)** değeridir; running
CPU'nun anlık PC örneklemesi değildir. Her komuttan önce halted durumu ve kapalı
autoexec kontrol edilir. CPU register'larına yazılmaz. Debug DATA0 önce saklanır,
sonunda geri okunarak restore edilir. PC erişimi desteklenmezse sticky cmderr
korunur ve işlem durur; arkadan GPR veya program-buffer denemesi yapılmaz.
`pc_valid` ve `gpr_valid_mask`, kısmi veriyi gerçek sıfır register değerinden ayırır.

Bellek yolu yalnızca SBCS version 1, 32-bit adres ve gerekli erişim genişliği
bildirildiğinde çalışır. Otomatik DATA read ve adres artırma kapalıdır. Her sabit
adrese bir bus-read tetiklenir, busy/error kontrol edilir, veri alınır. Snapshot
başına en fazla 48 değer alınır:

| Profil | İzinli adresler | Genişlik / ek kapı |
|---|---|---|
| SRAM | `20000000–2000003c`, 16 word / 64 bayt | 32 bit; sadece bu sabit pencere |
| RCC clock maskeleri | `40021018`, `4002101c` | Çevre birimi seçilmişse önce okunur; RCC'ye yazılmaz |
| GPIOA/B/C | taban `40010800`, `40010c00`, `40011000`; `+0/+4/+8/+c` | CFGLR, CFGHR, INDR, OUTDR; APB2 clock biti açık olmalı |
| USART1/2/3 | taban `40013800`, `40004400`, `40004800`; `+8/+c/+10/+14` | BRR, CTLR1/2/3; ilgili APB clock biti açık olmalı |
| SPI1/2/3 | taban `40013000`, `40003800`, `40003c00`; `+0/+4` | CTLR1/2; **16 bit SBA desteği** ve açık APB clock gerekir |

GPIO BSHR/BCR/LCKR, USART STATR/DATAR ve SPI STATR/DATAR okunmaz. FIFO, veri tüketen,
read-to-clear veya kilit sıralamasını tamamlayabilecek register'lar profile dahil
değildir. Kapalı clock açılmaz; ilgili blok atlanır. Bir gruptaki `supported`,
okunan açık bloklar için geçerlidir; atlananları RCC maskeleri ve adres listesiyle
ayırt edin. Running snapshot atomik değildir; firmware bu sırada SRAM, GPIO ve
clock'ları değiştirebilir. GPIO snapshot'ının kendisi pin sürmez, USART veri
iletmez, SPI transferi veya harici Flash komutu başlatmaz.

Snapshot seçimi bazı **debug register yazmaları** gerektirir: COMMAND/DATA0 veya
SBCS/SBADDRESS0. Hedef SRAM/MMIO/Flash yazması yapılmaz. SBA data register'ına hiçbir
zaman yazılmaz. SBA yapılandırması geri doğrulanır; son SBADDRESS/SBDATA latch'i
snapshot sonucunu taşımaya devam edebilir. Hata bitleri temizlenmez. Bu nedenle
salt okunur tanımı hedef belleği ve CPU register'ları içindir; debug modülünün
bütün latch'lerinin değişmez kaldığı iddia edilmez.

### Halt/resume araştırması ve geri dönüş

`CONFIG_DIAG_ALLOW_DM_ACTIVATION` ve `CONFIG_DIAG_ALLOW_HALT_RESUME` ayrı ayrı,
varsayılan `n`'dir. DM aktivasyonu yalnızca `activate-dm`, build izni ve host'taki
`--allow-dm-activation` ile wire token `EXPLICIT_DM_ACTIVATION` birlikte seçilirse
çalışır. CPU halt/resume ayrıca `CONFIG_DIAG_ALLOW_HALT_RESUME=y`,
`--allow-halt-resume` ve `EXPLICIT_HALT_RESUME` gerektirir. Activation izni
halt/resume yetkisi vermez. Normal diagnostic build'de ikisi de kapalı kalmalıdır.

### Kontrollü DM activation

`activate-dm` önce tam stable preflight yapar. Zaten aktif DM'de write yapmadan
DMCONTROL ve DMSTATUS'ı tekrar okur. İnaktif DM'de yalnızca bir adet
`DMCONTROL=0x00000001` write frame'i gönderebilir: bu değer yalnızca dmactive bitini
seçer. Kodda retry veya başarısız write sonrası otomatik cleanup write yoktur.
Ardından DMCONTROL ve DMSTATUS readback edilir; authentication ve hart running/
halted durumunun değişmediği kontrol edilir. JSON `dmactive_write_attempted`,
`dmcontrol_readback_valid`, `dmstatus_readback_valid`, `dmactive_set`,
`activation_state_uncertain` ve `pins_released` alanları bu kanıtı ayırır.
Readback kaybolursa `activation_state_uncertain` true olabilir; komutu otomatik
tekrarlamayın. Başarılı aktivasyon DM'yi etkin bırakır; otomatik deactivation
yapılmaz çünkü target debug durumunu değiştirmek auth/oturum durumunu belirsiz
hale getirebilir.

Aktivasyon başarılı olduktan sonra ayrı `capabilities` çağrısı ABSTRACTAUTO
(`0x18`) ve SBCS (`0x38`) okur; varsayılan mask `0` bu aşamada hiçbir DMI write
yapmaz. SBA desteklenirse `--sram`, `--gpio`, `--usart`, `--spi` seçimleri yalnızca
yukarıdaki sabit allowlist adreslerini okur. Bu snapshot'lar CPU halt etmez; CPU
register'ları istenirse `--registers` yalnızca CPU zaten halted iken salt okunur
abstract transfer yapar ve running hart için otomatik halt etmez. Flash, option
byte, RDP, WRP ve kalibrasyon adresleri allowlist dışıdır. Target SRAM/MMIO/CPU
register write veya program-buffer execution yolu eklenmemiştir.

Önce adapter ESP32 firmware'ini güvenli biçimde oluşturup adapter'a yükleyin;
DMT99 debug kablosu bu sırada ayrılmış olmalıdır. Menüconfig'te sadece
`Allow explicit DM activation (DMCONTROL.dmactive only)` seçeneğini etkinleştirin;
halt/resume seçeneğini kapalı bırakın. Aşağıdaki testler yalnızca operatörün her
adımda devam etmesiyle yürür; otomatik cihaz testi değildir. `/dev/ttyUSB0` ve
`3300` değerlerini gerçek port ve taze ölçümle değiştirin:

```sh
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-info-NEW.json info
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-preflight-NEW.json diagnose --voltage-mv 3300 --board-pins-checked --wiring-checked
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-activate-NEW.json activate-dm --voltage-mv 3300 --board-pins-checked --wiring-checked --allow-dm-activation
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-capabilities-NEW.json capabilities --voltage-mv 3300 --board-pins-checked --wiring-checked
```

Komutları sırayla uygulayın ve her adımın JSON'unu inceleyin. Info'da
`dm_activation_enabled=true`, `halt_resume_enabled=false` ve doğru ESP32/GPIO
pinleri olmalı. Preflight tekrar `30310524`, authenticated=true, hart running ve
DMCONTROL=0 vermeli. Activation sonucunda `dmactive_set=true`, iki readback valid,
`dmcontrol=1`, `dmstatus` running/authenticated ve `pins_released=true` beklenir.
Farklı/eksik sonuçta durun, tekrar etmeyin. Ardından mask-0 capabilities ile
ABSTRACTAUTO/SBCS'yi okuyun. Yalnızca SBCS açıkça SBA'yı desteklerse ve mask-0
sonucu temizse, ayrı dosya ve operatör seçimiyle istenen sınırlı snapshot grubunu
çağırın, örneğin:

```sh
python tools/diag.py --port /dev/ttyUSB0 --output captures/dm-sram-NEW.json capabilities --voltage-mv 3300 --board-pins-checked --wiring-checked --sram
```

Bu işlem dizisinde `halt-cycle`, `reset`, `dump`, `identify` sonrası gizli
activation veya cihaz üzerinde otomatik test çalıştırmayın. `activate-dm` dışında
hedef durumunu değiştiren işlem seçilmemiştir.

Deney kodu önce stable, active, authenticated ve running hart gerektirir.
Baştan halted bir CPU'yu sahiplenmez veya resume etmez. Halt isteği gönderilmeden
önce ownership kaydedilir; yarıda kesilmiş write'ın hedefe ulaşmış olabileceği
varsayılır. Halt, any/allhalted ile; resume, any/allrunning **ve** any/allresumeack
ile doğrulanır. Haltreq/resumereq write-only alanlarının readback'i ACK sayılmaz.
Okunabilir DMCONTROL seçim/reset/active alanları ayrıca kontrol edilir.

Halt timeout'unda link güvenilir ve abstract engine idle ise haltreq temizlenir ve
bounded resume cleanup uygulanır. Register okunduysa DATA0 restore edilmeden
başarılı dönüş bildirilmez. Parity, GPIO, line veya frame hatasında kör write
cleanup/retry yapılmaz. `operation_result` ilk hatayı, `cleanup_result` ayrı geri
dönüş sonucunu saklar. `target_state_uncertain:true`, CPU'nun running olduğunun
kanıtlanamadığını belirtir; GPIO'ların Hi-Z olması tek başına resume kanıtı değildir.
Reset, DM'yi kapatıp açma, unprotect veya program-buffer fallback yoktur.
Fiziksel bağlantı kaybında yazılımla mutlak geri dönüş garantisi verilemez.

### Hata ayrımı ve süre sınırları

| Sonuç / sınıf | Yorum |
|---|---|
| `read_protection_enabled` / `rdp_blocked` | Önceki oturumdaki OBR kanıtıyla bilinen Flash engeli. Yeni tanı yolu OBR'yi okumaz, bunu genel bus hatasından üretmez |
| `authentication_required` / `authentication_blocked` | DMSTATUS authentication kapısı; doğrudan RDP ile eşitlenmez |
| `parity_error`, `no_response`, `line_held_low`, `gpio_error` / `connection_error` | Güvenilir iletişim yok; RDP diye etiketlenmez |
| `frame_timeout` / `timeout` | Bir senkronizasyon veya DMI frame'i 100 ms sınırını aştı |
| `poll_timeout` / `timeout` | Halt/resume, abstract engine veya SBA beklemesi doldu; ayrıca sberror=1 bus timeout |
| `abstract_command_error`, `system_bus_error` / `access_error_cause_unproven` | Ham cmderr/sberror saklanır. Genel exception/bus hatası, tek başına RDP kanıtlamaz |
| `unsupported_debug_capability` / `unsupported` | Bildirilmeyen SBA genişliği/version veya cmderr=2; başka yol denenmez |
| `disabled_by_policy` | Çalışma durumunu değiştirme kapısı kapalı |
| `cleanup_unconfirmed` | Cleanup veya son durum doğrulaması başarısız; ayrı ham sonuçlar incelenmeli |

Her poll döngüsünün bütçesi 200 ms'dir; devam eden tek DMI frame'i ve scheduler
gecikmesi buna eklenebilir. Döngüler sınırlıdır; başarısız işlemin tekrarı yoktur.
Varsayılan half-period 250 µs, turnaround 50 µs ve 8 kimlik örneği korundu.
Frame aralarında RTOS'a yield edilir; UART trace'leri hedef pinleri bırakıldıktan
sonra basılır. Trace tamponu 1024 kayıt; overflow JSON'da görünür ve host hata verir.
Host yanıt deadline'ı 60 saniyedir. Yalnızca GPIO-passive `hello` el sıkışması
tekrarlanabilir. Host timeout'u adaptörün cleanup yaptığının kanıtı sayılmaz.

## Offline doğrulama

İlk olarak mevcut testler `CC=cc bash tests/run.sh` ile çalıştırıldı: eski iki C
mock testi ve 11 Python testi geçti. Ortamdaki `CC=gcc-14` yürütülebiliri yoktu;
mevcut `/usr/bin/cc` açıkça seçildi. İlk testlerdeki eski Flash senaryoları tamamen
sentetikti, cihaz veya mevcut dump dosyaları kullanılmadı.

Güncel varsayılan suite, eski Flash okuyucu mock'u yerine yeni diagnostic testini
çalıştırır. Sonuçlar: iki C test yürütülebiliri ve **17 Python testi başarılı**.

- Mevcut transport testinde 4519 GPIO hata konumunda pin bırakma.
- Diagnostic testinde 3418 GPIO, 338 kısmi halt/resume write ve 542 DMI parity
  hata konumu; örtük halt ve sınırsız retry bulunmaması.
- 33 register okuması, 48 güvenli snapshot değeri, running ve halted başlangıçları,
  autoexec/clock kapıları, eksik SBA/16-bit desteği, sticky hata ve timeout.
- Halt timeout'undan başarılı cleanup, resume timeout'unda belirsiz durum bildirimi,
  dışarıdan halted CPU'nun resume edilmemesi ve release hatası.
- Gerçek CLI/pyserial'ın **sentetik PTY** üzerinde command maskesi, eski firmware
  reddi, birbirinden bağımsız activation/halt-resume build gate'leri, açık wire
  token'ları ve capture koruması.
- Activation mock'unda inaktif DM için tek DMCONTROL=1 write, iki readback,
  already-active durumda sıfır write ve pin-release hata bildirimi.
- Flash/option/kalibrasyon, veri/FIFO ve lock adreslerinin allowlist dışında kalması;
  memory-write, progbuf, register-write, error-clear ve reset DMI yazmalarının reddi.

Tekrarlama:

```bash
cd projects/rvswd
CC=cc bash tests/run.sh
ASAN_OPTIONS=detect_leaks=0 CC=clang SANITIZE=1 bash tests/run.sh
```

GCC sanitizer linki bu ortamda eksik libasan/libubsan nedeniyle kullanılamadı.
Clang ASan/UBSan ile doğrulandı; ptrace ortamında LeakSanitizer çalışmadığından
sadece leak taraması kapatıldı. Bu testler gerçek hedef erişimi içermez.

ESP-IDF v5.5.5 ile `build-diagnostic` dizininde ESP32 ELF/BIN derlendi. Varsayılan
sdkconfig'te `CONFIG_DIAG_ALLOW_DM_ACTIVATION` ve `CONFIG_DIAG_ALLOW_HALT_RESUME`
kapalıdır. Eski reader sadece
`RV_LEGACY_READER_OFFLINE_TEST` makrosu altında tarihsel kaynak olarak tutuldu;
normal build'e ve UART komut tablosuna dahil değildir. `libmain.a` sembollerinde eski
`rv_dump_flash`, `read_program`, `write_gpr` ve `rv_crc32` bulunmadığı doğrulandı. `tests/test_reader.c` artık
varsayılan test runner'da çalıştırılmaz. Orijinal programmer referans bileşeni
hâlâ build dışında tutulur.


Bu turda `CC=cc bash tests/run.sh` başarılı oldu: 18 Python testi ve iki C mock
paketi geçti. ESP-IDF v5.5.5 `reconfigure` ardından `ninja -C build-diagnostic
-j 2` başarılı oldu. Varsayılan sdkconfig'te her iki yeni izin de kapalıdır.
Güncel adapter BIN `177584` bayt, SHA-256
`8e30a26889a3adedfc0100a59e381dbc31d495c214f91965a75aab248ea5cd7c`.
Eski reader sembolleri `libmain.a` içinde bulunmadı. Görüntü DMT99 üzerinde
çalıştırılmadı ve adaptöre yüklenmedi.

Aktivasyon izniyle ayrı adapter firmware'i hazırlamak gerekirse, DMT99 SWCLK/SWDIO
hatları fiziksel olarak ayrıyken ESP-IDF ortamında şu adımları uygulayın. Menüde
sadece DM activation seçeneğini açıp halt/resume seçeneğini kapalı bırakın:

```bash
cd projects/rvswd
. ${IDF_PATH}/export.sh
idf.py -B build-diagnostic -D IDF_TARGET=esp32 -D SDKCONFIG=build-diagnostic/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' menuconfig
```

Menüden çıkıp kaydettikten sonra izinleri ve adapter build'ini doğrulayın:

```bash
grep -E 'CONFIG_DIAG_ALLOW_(DM_ACTIVATION|HALT_RESUME)' build-diagnostic/sdkconfig
idf.py -B build-diagnostic -D IDF_TARGET=esp32 -D SDKCONFIG=build-diagnostic/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' reconfigure
ninja -C build-diagnostic -j 2
```

Beklenen ayarlar `CONFIG_DIAG_ALLOW_DM_ACTIVATION=y` ve
`# CONFIG_DIAG_ALLOW_HALT_RESUME is not set` olmalı. Sonra yalnızca ESP32
adapter'ı yükleyin; DMT99 bağlantısı ayrılmış kalmalı:

```bash
idf.py -B build-diagnostic -D SDKCONFIG=build-diagnostic/sdkconfig -p /dev/ttyUSB0 -b 115200 flash
```

## Gerçek cihaz için bağlantı ve yalnızca güvenli test komutları

Aşağıdaki komutlar bu turda cihaz üzerinde çalıştırılmadı. Yeni host aracı protocol
3 görüntüsü gerektirir. Adaptörde eski `flash-reader` varsa hedef sinyalleri
ayrılmışken yalnızca **ESP32 adaptörünü** güncelleyin; eski görüntüyle yeni komutları
denemeyin. Hedef CH32'ye yazan bir yükleme veya hedef firmware patch adımı yoktur.

1. DMT99 test uçlarını tüm devrelerden ayırın; hedefin USB/şarj kablosunu çıkarın.
   Kablolama sırasında cihazı kapatın. Test sırasında hedef kendi bataryasından,
   ESP32 USB'den beslensin.
2. Ortak GND → header 1. GPIO25 → 330 Ω → header 2 / PA14 / SWCLK.
   GPIO26 → 330 Ω → header 3 / PA13 / SWDIO. Mevcut doğrulanmış eşlemeyi koruyun;
   GPIO16/17 kullanılmaz.
3. SWCLK ve SWDIO target tarafında hedef 3.3 V'a gerekli pull-up'lara sahip olmalı
   (proje bağlantısı: her hatta 4.7 kΩ). Sürücü open-drain'dir; internal pull-up'lar
   kapalıdır. ESP32 3V3/5V/VIN'i hedef header 4'e bağlamayın.
4. Her iki kart açıkken GND'yi önce, sinyalleri sonra bağlayın. Header 4–1 gerilimini
   harici ölçü aletiyle ölçün; geçerli aralık 3000–3450 mV. GPIO25/26 doğrulamasını
   ve mevcut kısa bağlantıları kontrol edin. Seri monitörleri kapatın.
5. ESP32 reset/yükleme ve herhangi bir kartı kapatma öncesinde hedef sinyallerini
   ayırın. Host DTR/RTS'yi bilerek reset için kullanmaz; USB sürücüsü/board açılış
   davranışının fiziksel doğrulaması yine gereklidir.

Güncel adaptör görüntüsünü hazırlama (offline):

```bash
cd projects/rvswd
export PYTHONDONTWRITEBYTECODE=1
. ${IDF_PATH}/export.sh
idf.py -B build-diagnostic -D IDF_TARGET=esp32 -D SDKCONFIG=build-diagnostic/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' build
```

`build-diagnostic/sdkconfig` içinde `# CONFIG_DIAG_ALLOW_DM_ACTIVATION is not set`
ve `# CONFIG_DIAG_ALLOW_HALT_RESUME is not set` olmalıdır. Bu ortamda `idf.py`
reconfigure ardından build `ninja -C build-diagnostic -j 2` ile tamamlandı. Aynı
SDK ortamında `idf.py build` komutu da kullanılabilir. Eski `build-esp32` dosyalarını yeni diagnostic diye
karıştırmayın. Adaptörü güncellemek gerekirse hedef sinyalleri ayrılmış halde:

```bash
export ADAPTER_PORT=/dev/ttyUSB0
idf.py -B build-diagnostic -D SDKCONFIG=build-diagnostic/sdkconfig -p "$ADAPTER_PORT" -b 115200 flash
```

Bu `flash` komutu yalnızca ESP32 adaptörü içindir; burada çalıştırılmadı. Adaptör
USB konsolu 115200 8N1, GPIO1 TX / GPIO3 RX; RVSWD hedef hatlarından ayrıdır.

Sinyaller bağlı değilken adaptör bilgisi alınabilir:

```bash
export ADAPTER_PORT=/dev/ttyUSB0
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-info-001.json info
```

`protocol=3`, `build=read-only-diagnostic`, `dm_activation_enabled=false`,
`halt_resume_enabled=false`,
`board_verified=true`, `initialized=true`, `gpio_clk=25`, `gpio_dio=26` beklenir.
Bunlar beklenen alanlardır; yeni donanım capture'ı değildir.

Bağlantı ve taze gerilim ölçümü tamamlandıktan sonra ilk hedef testi **yalnızca
DMI read** olsun:

```bash
read -r -p 'Hedef header 4–1 gerilimi (mV): ' TARGET_MV
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-status-001.json capabilities --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
```

`result=ok`, stable preflight ve `pins_released=true` görülürse, yalnızca DM zaten
aktifse açıkça seçilmiş salt okunur snapshot testlerine geçin. Her komut ayrı
bounded oturumdur; hata sonrası otomatik devam yapılmaz. Her seferinde taze
gerilim ölçün ve yeni çıktı adı kullanın. Bu komutlar DM'yi etkinleştirmez ve
CPU'yu durdurmaz:

```bash
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-sram-001.json capabilities --sram --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-peripherals-001.json capabilities --gpio --usart --spi --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
python tools/diag.py --port "$ADAPTER_PORT" --output captures/diagnostic-registers-001.json capabilities --registers --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
```

Son komut running hart'ta beklendiği gibi `hart_running_no_implicit_halt` ile durur.
Register almak için bu aşamada CPU'yu durdurmayın. SBCS desteği yoksa snapshot
komutu yazmadan `unsupported_debug_capability` verir. DM inactive ise
`debug_module_inactive` ile durun; bu salt okunur test dizisine activation eklemeyin.
DM inactive ise sadece kontrollü aktivasyon adımlarını gerçekten seçtiyseniz üstteki
`activate-dm` prosedürüne geçin; aksi halde burada durun. Protection, parity veya
timeout sonucunda reset, unprotect, option-byte işlemi ya da eski dump komutuyla
devam etmeyin. `--help` veya `info` ile devam etmek hedefe erişim gerektirmez.

## Başvurulan birincil kaynaklar

- [RISC-V External Debug Support v0.13.2](https://riscv.org/wp-content/uploads/2024/12/riscv-debug-release.pdf):
  DMSTATUS, write-only halt/resume istekleri, Access Register, ABSTRACTAUTO ve SBA
  tanımları. Bunlar WCH üzerindeki opsiyonel komut desteğinin kanıtı değildir.
- [WCH referans el kitabı sayfası](https://www.wch-ic.com/downloads/CH32FV2x_V3xRM_PDF.html):
  üretici yazarlı v2.5 PDF'nin analiz reposundaki mevcut kopyası okundu:
  `references/wch-dump-feasibility/CH32FV2x_V3xRM_EN.PDF`.
  Bölüm 10.3.1 GPIO, 18.10 USART, 20.3.7 ve 20.4 SPI, Flash OBR/RDPRT açıklaması.
  Özellikle SPI durum okumaları flag temizleyebildiğinden kapsam dışındadır.
- Analiz reposundaki sabitlenmiş WCH `ch32v30x.h`: register taban/ofsetleri ve
  RCC clock bitleri çapraz kontrol edildi. `docs/FIRMWARE_DUMP_FEASIBILITY.md`
  mevcut RDP ve hafıza haritası bağlamı için okundu; değiştirilmedi.

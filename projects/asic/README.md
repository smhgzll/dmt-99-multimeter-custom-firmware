# DMT99 ASIC UART — pasif kayıt ve analiz

Birleşik depoda kaynaklar `projects/asic/`, tam kayıtlar
`artifacts/captures/asic-captures/` içindedir. Bu sayfadaki eski edinim ve test
sonuçları tarihsel kanıttır. [Güncel ortak derleme](../../docs/BUILDING.md),
[kanıt özeti](../../docs/RESEARCH.md), [elektriksel riskler](../../docs/LEGAL_AND_SAFETY.md).

Çalışma dizini: `projects/asic`.
Eski analiz yalnızca referans olarak okunur. Python 3 standart kütüphanesi,
`sigrok-cli` ve FX2 analyzer runtime firmware'i yeterlidir.

## Donanımın mevcut durumu

2026-10-18, Europe/Istanbul: USB `0925:3881` bulundu. `fx2lafw` sürücüsü
Saleae Logic / 8 kanal olarak tanıdı. Eksik firmware Nobara deposundan
`sigrok-firmware-fx2lafw-0.1.7.git9fe40f8-1.fc44.noarch.rpm` olarak indirildi;
`rpm -K` imza/digest kontrolü geçti. Paket **proje içine** açıldı.
Analyzer runtime firmware yüklemesi sonrası USB adresi `1.62` → `1.63` oldu.
MCU/ASIC/Winbond üzerinde programlama veya UART gönderimi yapılmadı.

PulseView kapatıldıktan sonra analyzer erişimi doğrulandı; gerçek AUTO/açılış,
ST2 referans kartı ile direnç ve açık uç mod kayıtları alındı. Direnç sonuçları
[RESISTANCE_VALIDATION.md](docs/RESISTANCE_VALIDATION.md), wire formatı
[ASIC_UART_SPEC.md](docs/ASIC_UART_SPEC.md) içinde. Güncel, zaman damgalı sonuçlar
`evidence/probe_*.json` içinde tutulur. Gerçek kayıtlardan kaynak hash'leriyle
izlenebilir regression kesitleri `tests/fixtures/` içindedir; sentetik testler
ayrıca etiketlidir.

**Gerçek bağlantı D0/D2:** kullanıcı D0 ve D2 kullandığını doğruladı.
AUTO ölçüm burst'leri iki örnekleme hızında **12 bayt**; açılıştaki ASIC event
adayı `AA 55 FD ...` ise **10 bayt** gözlendi. Eski 13 bayt DMA tamponu bilgisi
wire paket uzunluğu olarak genellenmez. Ayrıntı: [donanım bulguları](docs/HARDWARE_FINDINGS.md).

Uzun oturumlarda yaklaşık 30 s civarında erken bitiş iki kez gözlendi; nedeni
çözülmüş değildir. CLI hedef sample sayısını karşılaştırıp bunları eksik olarak
işaretler. Şu oturumda ayrı 10 s kayıtlar kullanıldı; aralarındaki boşluklar
kesintisiz kayıt gibi birleştirilmez. `evidence/acquisition_limit_observation.json`.

Kullanıcının son kapsamı: yalnız açık uç testleri; yeni komponent/kaynak bağlama
testleri ertelendi. Kondansatör, pil ve kapalı devre ile fiziksel doğrulama yoktur.

## Bağlantı ve ilk kayıt

- Analyzer GND → DMT99 GND.
- D0 → MCU PA9 / pin 42 → ASIC (`MCU_TX`).
- D2 → MCU PA10 / pin 43 ← ASIC (`ASIC_TX`), kullanıcının mevcut bağlantısı.
- Analyzer VCC bağlanmaz. Tüm kanallar pasif logic input'tur.
- DMT99 orijinal firmware ile çalışır; ölçüm girişleri ilk kayıtta boş olur.
- Analyzer bağlıyken şebeke ölçümü yapılmaz.

```bash
cd projects/asic
python3 tools/dmt99.py probe
python3 tools/dmt99.py scenarios
```

`probe` yalnızca cihazı açıp yeteneklerini okur; örnekleme başlatmaz.
FX2 sürücüsünün normal açılışı analyzer'ın RAM'ine runtime firmware yükleyebilir.
Fiziksel bağlantı hazır olduktan sonra:

```bash
python3 tools/dmt99.py capture --test startup_idle --seconds 30 --connected \
  --rx-channel 2 --rx-frame-bytes 12 \
  --notes 'Ölçüm girişleri boş; kayıt başladıktan sonra açıldı'
```

Açılışı yakalamak için cihazı önce kapalı tutun; araç `Ham veri dosyasına
yazılıyor; kayıt etkin` mesajını verdikten sonra açın. Bu mesaj USB'den ilk
verinin dosyaya gelmesini gösterir; işletim sistemi gecikmesi içerir. Varsayılan
1 MHz; `--samplerate 2000000` ile 2 MHz seçilir. Kayıt süresi yapılandırılabilir.
30 saniye / 2 MHz için yaklaşık 60 MB ham logic verisi ve türetilmiş çıktılar
gerekir. PulseView `.sr` dosyasını kayıt tamamlandıktan sonra açabilir.

Eksik paketlerin sistem kurulumu için Nobara komutu:

```bash
sudo dnf install sigrok-cli pulseview sigrok-firmware-fx2lafw
```

Root gerektirmeyen, burada kullanılan proje içi kurulum:

```bash
bash tools/bootstrap_analyzer.sh
```

Firmware arama yolu sigrok'un `XDG_DATA_HOME/sigrok-firmware` mekanizmasıyla
proje içine yönlendirilir. [Sigrok firmware dokümanı](https://www.sigrok.org/wiki/Firmware).

## Etiketler ve ölçüm metadata'sı

Her fiziksel bağlantı değişiminde önce donanımı hazırlayın, sonra ayrı kayıt alın.
`scenarios` tüm istenen direnç, kapasitans, pil, süreklilik ve AUTO testlerini listeler.
Örnek komut gerçek kayıt **oluşturmaz**, çalıştırıldığında bağlı cihazdan örnekler:

```bash
python3 tools/dmt99.py capture --test resistance_1k --connected --seconds 10 \
  --rx-channel 2 --rx-frame-bytes 12 \
  --actual '1 kohm, %1 nominal; referans ölçümü henüz yok' --mode resistance \
  --display '0.999 kΩ' --display-numeric 0.999 --display-unit kohm --tolerance 0.002
```

Kayıt sonrasında ekrandaki değeri ve komponenti eklemek/düzeltmek için:

```bash
python3 tools/dmt99.py annotate captures/OTURUM \
  --display '1.000 kΩ' --display-numeric 1.000 --display-unit kohm \
  --actual '1 kohm, %1' --notes 'Kararlı bölge; ekran dalgalanması ±0.002 kΩ'
```

Önceki metadata değerleri `annotation_history` içinde korunur. `display_numeric`
ekrandaki **gösterilen birimdedir**, SI'ya sessiz dönüştürülmez. `--actual` nominal
ile referans cihazda ölçülmüş değeri ayırt edebileceğiniz serbest metindir.
AUTO geçişlerinin fiziksel sırası ve kayıt içindeki yaklaşık zamanlarını `--notes`
ile belirtin; tek bir ekran değeri bütün geçiş kaydını temsil etmez.

## Saklanan çıktılar

Her kayıt benzersiz, yerel zamanlı bir klasöre yazılır:

| Dosya | İçerik |
|---|---|
| `raw.sr` | Orijinal sigrok logic kaydı; yeniden analizde değişmez |
| `sigrok.log` | Driver ve USB günlükleri; hatalar dahil |
| `metadata.json` | Test, ekran/komponent, bağlantı, komut, süre ve sonuç |
| `logic.bin` | `.sr` chunk'larının sıralı, birleştirilmiş ham kopyası |
| `decoded.json` | Her UART adayı, bitler, zamanlar, paketler, hatalar |
| `bytes.csv` | Her bayt/adaya ait yön, zaman, değer ve hata bilgisi |
| `packets.csv` | Paket adayları, eksikler, başlıksız parçalar ve alanlar |
| `decoded.txt` | Okunabilir TX/RX zaman çizelgesi |
| `analysis.json` | Tekrarlar, değişen offsetler, gruplar ve checksum hipotezleri |

```bash
python3 tools/dmt99.py analyze captures/OTURUM --gap-chars 4
python3 tools/dmt99.py compare captures/OTURUM1 captures/OTURUM2
python3 tools/dmt99.py import-sr /gercek/kayit.sr --hardware --test resistance_open
```

Kayıt ve import CLI varsayılanı kullanıcının doğruladığı D0/D2 ve gerçek
kayıtlarda gözlenen 12 bayt profilidir. `--rx-channel` / `--rx-frame-bytes` ile
alternatif seçilebilir. Seçim metadata'da saklanır. Eski D0/D1 + 13 hipotezi için
`--rx-channel 1 --rx-frame-bytes 13` açıkça verilir.
İçe aktarmada seçilen kanal metadata'sı ve tek bayt/sample kontrol edilir; bilinmeyen
kanal eşlemesi tahmin edilmez. İlk kayıtta D2 seçili kanallar arasında değildi;
FX2'nin ham 8-bit verisinde korunan bit2, kullanıcının D0/D2 doğrulamasıyla
açık bir profil revizyonuyla geri kazanıldı. Önceki çıktılar `revisions/` içinde
saklıdır; orijinal `.sr` değişmedi. `--hardware` verilmemiş içe aktarımlar fiziksel
karşılaştırmaya kabul edilmez. Karşılaştırma sonuçları `evidence/comparison.json`
dosyasına yazılır; `--output` ile değiştirilebilir.

## Veri ve kanıt politikası

9600 8N1, LSB-first, bit merkezi örneklemesi kullanılır. Örnek indeksi asıl
zaman damgasıdır; `t_s` kayıt başlangıcına göredir. Host saati ayrı metadata'dır;
USB başlatma zamanı baytın mutlak duvar saati olarak sunulmaz.

Başlangıçta düşük seviye, kısa start/glitch, düşük stop biti ve kayıt sonunda
yarım UART frame'i işaretlenir. Tamamlanmayan değer `null`, bilinen bitler
`bits_lsb_first` ve `partial_value` alanlarında kalır. Ham örneklerin tamamı
saklanır; aktif frame içindeki kenarlar ayrı UART baytı diye çoğaltılmaz.
Uzun düşük seviyeler `signal` istatistiklerinde ve ham veride görünür.

Paket sınırı açık profil seçimine bağlıdır: MCU AA55/10, ASIC AA55FD/10,
ASIC 5AA5/12 (AUTO'da gözlenen) veya tarihsel 13 bayt hipotezi.
Varsayılan dört karakterlik boşluk eşiği **fiziksel doğrulanmış zamanlama değildir**.
Eksik paket boşlukla bölünür; başlıksız kalan veri korunur. Adayın içindeki yeni
başlıklar `embedded_header_offsets` içinde raporlanır; sessiz yeniden senkronizasyon
yapılmaz. Her UART adayı aynı yöndeki tam bir parçaya atanır; hiçbir aday atılmaz.

TX check-byte kontrolü yalnızca firmware'den çıkarılmış 19 literal form ile
karşılaştırılır. Legacy `00/.../11` aynen kabul edilir. Genel payload checksum
kuralı doğrulanmış değildir. RX checksum varlığı, konumu ve algoritması bilinmez.
Toplama/XOR taraması yalnızca hipotez üretir; sabit alanların tesadüfi eşleşmesi,
tekrarlanan aynı paket ve UART hatalı paketler doğrulama kanıtı sayılmaz.

Ölçüm görüntüsü firmware'deki packed LCD segmentlerinden çözülür; BCD/ham ADC
varsayılmaz. Bilinmeyen semboller sayıya çevrilmez. Stock numeric parser'ın
bilinmeyen sembolü sayıya zorlayan davranışı bilinçli olarak kullanılmaz.
Firmware yorumu UART hatalı bir aday üzerinde de görünür fakat hatalar açık
işaretlidir; checksum hipotezi ve sayısal ekran karşılaştırmasına bu adaylar girmez.

Önceki TX ile zaman yakınlığı ACK veya nedensel cevap olarak yorumlanmaz.
`compare` ekran sayısı/birimi eşleşmelerini sayar; alanları otomatik “onaylı”
yapmaz. Donanımda görülen wire formları [donanım bulgularında](docs/HARDWARE_FINDINGS.md)
ayrı tutulur; analog alanlar fiziksel ölçüm karşılaştırmalarını bekler.

## Kontroller ve sonraki aşama

```bash
python3 -m unittest discover -s tests -v
```

Testler UART'ın 256 değerini iki örnekleme hızı/iki kanalda, chunk sınırlarını,
framing/truncation/glitch hatalarını, paket kayıpsızlığını, 19 literal TX formunu,
sigrok UART decoder karşılaştırmasını ve salt okunur eski reconstructed C'ye
karşı 4096 alan kombinasyonunu kapsar. Bunlar analog/ASIC doğrulaması değildir.

25 gerçek kayıt ve 24 gerçek sample kesiti korunuyor. [Kayıt indeksi](docs/CAPTURE_INDEX.md)
tam `.sr`, metadata ve analiz dosyalarını listeler. 31 test ve kesitlerin iki
yönü için 48 bağımsız sigrok karşılaştırması geçti. DC/AC düşük akım ve A,
NCV/LIVE boşta ve harici prob olmadan sıcaklık kayıtları da dahildir.
Bağımsız C parser/serializer/stream modülü `firmware/dmt99_asic.c/.h` içinde,
entegrasyon sınırı `firmware/INTEGRATION.md` içinde bulunur. Gerçek paketler,
literal serializer, sentetik numeric yorumlama ve 100000 bayt stream testleri
ile kontrol edilir. Host ASan/UBSan runtime paketleri eksikse sistem kurulumu:
`sudo dnf install libasan libubsan`; bu oturumda paketler proje içine açılarak
kullanıldı. C modülü UI birimleri için V/A/Ω/F/Hz SI prefix dönüşümü de sunar;
analog kalibrasyon, denenmeyen mod/prefix'ler ve custom firmware binding'i
ayrıca doğrulama bekler. `make -C firmware` RV32 arşivi üretir; hedefe bağlanmaz.
Mevcut projelerde dosya değiştirilmez.

Regresyon için bitleri değiştirmeden kesit çıkarmak:

```bash
python3 tools/export_fixture.py captures/OTURUM tests/fixtures/ISIM \
  --start-sample 100000 --end-sample 1000000
python3 tools/validate_capture.py captures/OTURUM
python3 tools/validate_fixtures.py
python3 tools/update_inventory.py
```

Kesit sample aralığı, parent hash ve metadata snapshot'ı ile saklanır. Kesme
sınırındaki UART eksikleri/hataları kaybolmaz. Yaklaşık ekran bildirimi için
`--display-approximate` kullanılabilir; tolerans verilmediyse otomatik tam sayı
eşleşmesi kanıtı üretilmez. `--display-exact` ile açıkça geri değiştirilebilir.

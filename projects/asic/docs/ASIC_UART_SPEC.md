# DMT99 ASIC UART — deneysel, donanım kanıtına bağlı spesifikasyon

Kapsam: orijinal firmware ile AUTO/açılış, direnç referans kartı ve açık uç mod kayıtları.
Kalibrasyon algoritması, ASIC register protokolü, tüm ölçüm modları veya genel
checksum çözüldüğü iddia edilmez. Kaynaklar `evidence/sources.json`, gerçek
kayıtlar `captures/`, taşınabilir uygulama `firmware/dmt99_asic.c/.h` içindedir.

## Transport ve sınırlar

9600 baud, 8 data, parity yok, 1 stop, LSB-first. D0=PA9 MCU_TX,
D2=PA10 ASIC_TX. Analyzer pasif input'tur. 1 ve 2 MHz AUTO kayıtları bağımsız
sigrok UART decoder'ıyla aynı baytları üretir. Direnç kayıtları 1 MHz'dedir.
Zaman damgasının aslı capture içindeki sample indeksidir.

Donanımda üç wire formu görüldü:

| Yön / aile | Başlangıç | Gözlenen uzunluk | Kapsam |
|---|---|---|---|
| MCU komutu | `AA 55 05` | 10 bayt | Açılışta 14; mod geçişinde 0C/0D/0B ve 0A |
| ASIC görüntü | `5A A5` | 12 bayt | AUTO, direnç ve açık uç V/akım/C/Hz/NCV/LIVE/sıcaklık kayıtları |
| ASIC event adayı | `AA 55 FD` | 10 bayt | Açılışta event 14 |

MCU firmware'indeki DMA uzunluğu **13**; bu bir DMA tampon ayarıdır. Gerçek
görüntü mesajının bu kayıtlarda 12 bayt olmasıyla karıştırılmaz. 13 baytlık
5AA5 formu sürücüde tarihsel uyumluluk seçeneği olarak bulunur, donanımda
onaylanmış varsayılan değildir. Hattaki eksik bayt tamamlanmaz.

Görüntü burst'leri yaklaşık 6/s. Zamanlamalar oturumun `analysis.json` içindeki
bağımsız idle-burst istatistiklerinde min/median/max olarak saklanır. Yazılım
ayırma eşiği 4 UART karakteridir; ASIC timing gereksinimi olduğu iddia edilmez.
Boşluklar, UART hataları, bilinmeyen baytlar ve eksik paketler saklanır.

## MCU komutları

Gözlenen AUTO açılış komutu:
`AA 55 05 14 00 00 00 00 00 19`.

Eski firmware'de 19 literal TX formu bulunmuştur. Ordinary ID'ler
`01,03,08,09..16(hex)` için sıfır payload formunda b9=ID+5; legacy
`AA 55 05 00 00 00 00 00 00 11` istisnası korunur. A0 diagnostic literalinin
anlamı bilinmez. Serializer diagnostic A0 üretmez. Bu, arbitrary payload
checksum'u değildir; serializer çağrısı hiçbir yere UART verisi göndermez.

Sabit direnç kayıtlarında MCU TX sessizdi. Ayrı AUTO→direnç geçiş kaydında
`0C → 0D → 0B` komutları ve aynı ID'li event adayları görüldü; son durumda
açık uç direnç deseni geldi. Böylece 0B'nin direnç UI geçişiyle ilişkisi
gerçek donanımda da gözlendi. 0C/0D arasındaki düşük gerilim gösterimleri
AC/DC doğruluğu veya kalibrasyon testi sayılmaz.

## ASIC görüntü mesajı, 12 bayt

| Offset | Bilgi | Kanıt / sınır |
|---|---|---|
| 0..1 | `5A A5` | Gerçek AUTO/direnç burst'leri |
| 2..6 | Dört packed LCD sembolü | Firmware formülü ve gerçek ekran karşılaştırmaları |
| 2 bit4 | Eksi işareti | Firmware kaynaklı; negatif fiziksel test yok |
| 3/4/5 bit4 | Decimal sonrası hane 1/2/3 | Bu konumlar gerçek direnç kayıtlarında karşılaştırıldı; çakışma önceliği firmware kaynaklı |
| 2 bit0/1 | Host AC/DC kodu | V, düşük akım ve A açık uçta 2=DC, 1=AC ile ilişkilendi; bit1 önceliği firmware kaynaklı |
| 6..8 | Unit priority ağacı | Ω/kΩ/MΩ örnekleri gerçek direnç kayıtlarında görüldü |
| 7 bit7 | İsimsiz status flag | Fiziksel anlamı bilinmiyor |
| 9..11 | Opaque | Kaydedilir; checksum/ADC/kalibrasyon alanı sayılmaz |

Semboller:
`(b2&F0)|(b3&0F)`, `(b3&F0)|(b4&0F)`,
`(b4&F0)|(b5&0F)`, `(b5&F0)|(b6&0F)`.
Segment matching öncesi `&EF`. Bunlar BCD değildir. Tüm segment kodları
eski reconstructed C ile 256 girişte karşılaştırıldı; tüm sembollerin ayrı
fiziksel kayıtla doğrulandığı anlamına gelmez.

Sayısal dönüşüm sadece rakamlar ve baştaki boş LCD hanelerini kabul eder.
Ham `text` değişmez. Örnek gerçek `"  1.1"` → mantissa 11 / divisor 10,
unit code 11 → Ω. İç/son boşluk, harf, bilinmeyen sembol ve AUTO sayıya zorlanmaz.

Direnç unit ağacı (normal priority koşullarında):

| Koşul | Host unit code | Birim |
|---|---|---|
| b7&20, b6&40 yok, b7&04 yok | 11 | Ω |
| b7&20, b6&40 | 12 | kΩ |
| b7&20, b6&40 yok, b7&04 | 13 | MΩ |

Çakışan diğer unit flag'leri için tam öncelik eski firmware'den korunur;
tüm çakışmalar donanımda denenmedi. `dmt99_resistance_ohms` numeric değeri
gösterilen birimden Ω'a çevirir (×1/×1000/×1000000); nominal komponent değerine
zorlamaz. Fiziksel kademe/range alanı ayrıca çözülmüş değildir.

Açık uç testinde sembol kodları `[17,0,14,17]`, decimal_after=2 ve unit code=13
gözlendi; ham metin `" 0.L "`. `resistance_open_pattern` bu tam desene verilen
isimdir. Tüm overload/hata durumlarını ayırt ettiği iddia edilmez. Numeric=false,
değer `None`; C tarafında divisor=0 ve Ω dönüşümü başarısız döner.

## ASIC event adayı

`AA 55 FD 14 00 00 00 AA 01 BB` — 10 bayt. Ek mod geçişinde
`AA 55 FD 0C 00 00 00 AA 01 B3`,
`AA 55 FD 0D 00 00 00 AA 01 B4`,
`AA 55 FD 0B 00 00 00 AA 01 B2` gözlendi.
Kapasitans geçişinde `AA 55 FD 0A 00 00 00 AA 01 B1` de gözlendi.
FD offset2, event ID offset3. İki açılış gözleminde AUTO TX'ten yaklaşık
164–166 ms sonra geldi. Correlated ACK/kabul/red garantisi yoktur. Orijinal
IDLE ISR FD kontrolünde header guard içermez; yalnızca 5AA5 kabul eden bir
parser bu ayrı aileyi kaçırır. b4..9 anlamları ve integrity bilinmiyor.

## Integrity ve kanıt seviyeleri

RX checksum varlığı/offset/kuralı bilinmiyor. Temiz AUTO kayıtlarda aynı
b0..10 ile farklı b11 görüldü; b11'in yalnızca bu prefix'in deterministik
checksum'u olması bu gözlemlere uymaz. Genel checksum yoktur sonucu çıkarılmaz.
Toplama/XOR taraması hipotez üretir; otomatik onay vermez.
10 bayt event ailesinde bu beş ID için son bayt `sum(b0..8)&FF` ile
eşleşir. Diğer event alanları değişmediği için bu hâlâ sınırlı bir checksum
hipotezidir; arbitrary event payload kuralı olarak kullanılmaz.

Gerçek ekran karşılaştırmaları protokol/gösterim uyumunu test eder. ST2 kartı
dirençlerinin burada kullanılan değerleri **nominaldir**; bağımsız referans
ölçümü veya tolerans bilgisi bildirilmedi. Bu kayıtlar analog doğruluk ya da
kalibrasyon sertifikası değildir. Sayısal eşleşme sayıları
`evidence/resistance_comparison.json` içinde; ham `.sr`, tüm geçişler ve kullanıcı
bildirimleri oturum metadata'sında korunur.
Açık uç kapasitans/buzzer/DC/AC/Hz/akım/NCV/LIVE/sıcaklık kapsamı
[OPEN_PROBE_VALIDATION.md](OPEN_PROBE_VALIDATION.md) içindedir.

NCV ve LIVE sayısal ölçüm olarak yorumlanmaz. Çağıranın host mode=09/13
verdiği `dmt99_host_ui_status`, orijinal screen.c'deki NCV `-` sayısını veya
LIVE ilk segmentin `L` olup olmama kararını döndürür. Mod wire mesajından
otomatik tahmin edilmez. Açık girişte ham ` E? ` ve `L1UE` değişmeden saklanır;
aktif kaynakla seviyeler/eşikler test edilmedi. Sıcaklık code=21 ve kullanıcı
°C seçimi ayrı kanıtlardır; 32 ekranı ile 32 paket değeri eşleşti. Genel SI
fonksiyonu code=21'i C/F belirsizliğinde reddeder.

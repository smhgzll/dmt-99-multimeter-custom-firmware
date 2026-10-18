# Direnç kayıtları — 2026-10-18

ST2 kitinden çıkan 9 dirençli referans kartı kullanıldı. Kart değerleri kullanıcı
tarafından nominal olarak bildirildi; tolerans/bağımsız referans ölçümü yoktur.
DMT99 orijinal firmware, analyzer pasif, D0=MCU_TX / D2=ASIC_TX, 1 MHz / 9600 8N1.

| Fiziksel durum | Kullanıcı ekran bildirimi | Kayıtta çözülen gösterim | Tam RX paketleri |
|---|---|---|---|
| Nominal 1 Ω, ilk | 1.1 Ω | 0.8–1.1 Ω ve açık uç/boş geçişleri | 59 |
| Nominal 1 Ω, tekrar | 1.1 Ω | 1.0 Ω (17), 1.1 Ω (42) | 59 |
| Nominal 1 kΩ | 1.001 kΩ | 1.000–1.001 kΩ (45), açık uç/boş geçişleri (13) | 58 |
| Nominal 10 kΩ | 9.970 kΩ | ` 9.97` kΩ, tüm paketlerde | 59 |
| Nominal 100 kΩ | yaklaşık 99.80 kΩ | ` 99.8` kΩ, tüm paketlerde | 60 |
| Nominal 1 MΩ | 0.996, MΩ bağlamında | 0.996 MΩ, tüm paketlerde | 60 |
| Nominal 10 MΩ | 9.970 MΩ | 9.96 MΩ (39), 9.97 MΩ (20) | 59 |
| Açık uç | Kullanıcı “boşta”; ekran metni bildirilmedi | Ham ` 0.L `, sayısal değil | 60 |

Her kayıt 10 saniyedir. Tam görüntü mesajları **12 bayt**, `5A A5` başlıklıdır.
İlk yedi direnç/açık uç oturumunda UART hatası yoktur. 10 MΩ kaydında başlangıç
ve bitişteki iki eksik/hatalı UART adayı, 7+9 baytlık parçalarla birlikte korunur;
59 tam paket UART hatasızdır. Temiz kesitin bağımsız sigrok karşılaştırması geçti.
Diğer temiz direnç kayıtlarının tam byte akışları sigrok ile karşılaştırıldı.

Hane metinleri sessizce yeniden biçimlenmez: kullanıcının 9.970 bildirimi ile
UART'ın ` 9.97` metni metadata ve çözümleme çıktılarında ayrıdır. Sayısal değer
aynıdır. 1 MΩ bildiriminin birimi bağlamdan seçildiği metadata'da belirtilmiştir;
wire flag'leri firmware'in MΩ unit kodunu üretir.

## Uygulanan düzeltmeler

- Gerçek 1Ω kaydındaki baştaki boş LCD hücreleri `"  1.1"` sayısal dönüşümü
  engelliyordu. Python/C sayısal dönüşümünde yalnızca baştaki boş hücreler
  atlanır; ham metin korunur. Harfler, iç/son boşluklar ve bilinmeyen semboller
  sayıya zorlanmaz.
- Ω/kΩ/MΩ unit priority örnekleri gerçek kayıtlarla eşleştirildi. C dönüşümü
  gösterilen birimden Ω'a ×1/×1000/×1000000 uygular; nominal değere düzeltmez.
- Açık uçtaki `[17,0,14,17]` sembolleri / decimal_after=2 / unit=13 tam deseni
  ayrı tanınır. Sayısal 0 veya 0 Ω üretilmez. Bu desen tüm overload/hata
  koşullarının çözüldüğü anlamına gelmez.

## Komut ilişkisi

`20261018T011808_auto_to_resistance_open_fa59a550` kaydı, 20 s / 1 MHz,
UART hatasız. Kullanıcı AUTO'dan direnç moduna geçti. TX sırası:

| t (s) | MCU komutu | ASIC event ID | Event başlama gecikmesi |
|---|---|---|---|
| 13.073980 | `AA 55 05 0C 00 00 00 00 00 11` | 0C | 181.435 ms |
| 15.091084 | `AA 55 05 0D 00 00 00 00 00 12` | 0D | 162.763 ms |
| 15.892405 | `AA 55 05 0B 00 00 00 00 00 10` | 0B | 164.707 ms |

Event'ler 10 bayt `AA 55 FD` ailesindedir. Sonrasında direnç açık uç deseni
görüldü. Bu, 0B'nin direnç UI geçişiyle ilişkisini destekler. Event'lerin
garantili ACK olduğu ya da analog fonksiyonun tüm yan etkilerinin çözüldüğü
iddia edilmez. Bağımsız iki yön UART decoder karşılaştırması geçti.

## Dosyalar ve kapsam

Tam ham kayıtlar `captures/*resistance*`, `*1Mohm*`, `*10Mohm*` klasörlerinde;
CSV/JSON/text, metadata, hatalar ve sigrok logları birlikte saklanır.
Sayısal ekran/birim eşleşme sayıları `evidence/resistance_comparison.json` içinde.
Gerçek, bitleri değiştirilmemiş regresyon kesitleri `tests/fixtures/` içinde;
parent hash ve sample aralığı her kesitin JSON'unda saklanır.

Bağımsız C modülü: `firmware/dmt99_asic.c/.h`. Hiçbir UART/MMIO gönderimi veya
target flash değişikliği yapmaz. Firmware binding'i için
`firmware/INTEGRATION.md` kullanılır. Açıklanamayan b9..11, ASIC kimliği,
kalibrasyon, fiziksel range kodu, RX integrity ve test edilmeyen analog modlar
bilinmeyen olarak kalır. Son kullanıcı kapsamı: bundan sonra yalnızca açık uç
modları; yeni komponent/kaynak bağlanması istenmez.

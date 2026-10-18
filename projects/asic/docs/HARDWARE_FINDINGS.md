# Gerçek donanım bulguları — 2026-10-18

Pasif GND/D0/D2 bağlantısı, orijinal firmware, kullanıcı tarafından AUTO modu
bildirildi. ASIC hattı D2, MCU hattı D0 olarak izlendi. MCU'ya komut gönderilmedi.

| Kayıt | Örnekleme / alınan süre | Gözlem |
|---|---|---|
| `20261018T003908_idle_already_powered_81d808b6` | 1 MHz / 20 s | 120 × 12-byte 5AA5 burst; 1440 RX byte; UART hatası yok |
| `20261018T004101_auto_idle_d0_d2_2mhz_9eb57894` | 2 MHz / 10 s | 61 × 12-byte 5AA5 burst; 732 RX byte; UART hatası yok |
| `20261018T004155_startup_auto_d0_d2_2mhz_998d0648` | 2 MHz / 29.429760 s | İki AUTO TX komutu ve iki AA55FD event adayı; güç geçişlerinde hatalar korunmuş |

İlk iki kaydın 2172 RX baytı libsigrokdecode UART çözümlemesiyle birebir
eşleşti (`evidence/real_capture_decoder_validation.json`). Bunlar 9600 8N1
yorumunu ve gözlenen burst uzunluğunu destekler; tüm modları doğrulamaz.

## Ölçüm/görüntü ailesi

`5A A5 E4 2E 63 25 07 00 00 00 01 3F` — toplam **12 bayt**.
Firmware segment çözümlemesi `AUTO` üretir. İlk iki kayıtta b0..10 sabitken
b11 `3E/3F/40` değerleri aldı. Dolayısıyla b11'in önceki 11 baytın deterministik
bir checksum'u olduğu varsayımı bu temiz kayıtlara uymaz. b11'in fiziksel anlamı
henüz bilinmiyor. Başka checksum konumları/kuralları doğrulanmış değildir.

13 baytlık DMA tamponu eski firmware kodunda vardır. Bu gözlem 12 bayt wire
burst ile birlikte raporlanır; bir eksik bayt eklenmez ve nedenine dair
tamponun eski veri içerdiği gibi olasılıklar doğrulanmış sayılmaz.

## Açılışta farklı mesaj ailesi

MCU TX: `AA 55 05 14 00 00 00 00 00 19` — **10 bayt**.

ASIC RX: `AA 55 FD 14 00 00 00 AA 01 BB` — **10 bayt**, header `5AA5` değil.

İki gözlemde RX başlangıcı TX başlangıcından 165.855 ms ve 163.391 ms sonra
geldi. `FD/14` firmware event dispatch yorumu ve AUTO ilişkisiyle uyumlu;
ACK, kabul/red veya garanti edilmiş request/response semantiği henüz ispatlı değil.
İlk kayıttaki sabit `5AA5` guard'ı bu event'i kaçırabilirdi; parser artık ayrı
event ailesini kayıpsız korur.

Açılışta 60,000,000 sample hedefi için 58,859,520 sample geldi. `sigrok-cli`
return code 0 olsa da oturum `partial_or_failed` olarak işaretlendi. Logda
USB transfer timeout'ları vardır; eksik sayının nedeni ayrıca doğrulanmadı.
Kayıt başında düşük seviye ve güç geçişlerindeki framing hataları saklıdır.

## Doğrulama sınırı

Donanımda gözlenen: bu AUTO kayıtlarında UART byte dizileri, iki mesaj ailesi,
12/10 bayt burst uzunlukları, 14 komutunun literal formu ve zamansal ilişki.
Firmware'den yorumlanan: segmentler, işaret/decimal/unit bit öncelikleri.
Sonraki oturumlarda ST2 direnç kartıyla 1Ω/1kΩ/10kΩ/100kΩ/1MΩ/10MΩ ve açık
uç testleri tamamlandı; [RESISTANCE_VALIDATION.md](RESISTANCE_VALIDATION.md).
Yeni komponent istemeyen kullanıcı kapsamıyla kapasitans/buzzer/DC/AC/Hz açık
uç kayıtları da alındı; [OPEN_PROBE_VALIDATION.md](OPEN_PROBE_VALIDATION.md).
UI birim prefix'leri C dönüşümünde uygulanır; analog kalibrasyon, fiziksel
kademe, b9..11 anlamları, RX checksum ve ACK garantisi doğrulanmış değildir.

Regresyon kesitleri tam ham sample kesitleridir; yeniden üretilmiş wave şekilleri
değildir. Yanlarındaki JSON dosyalarında parent capture hash'leri, sample aralığı
ve beklenen bayt/sample konumları bulunur. Eski analiz dizini değişmedi.

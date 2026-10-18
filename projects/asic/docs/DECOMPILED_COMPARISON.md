# Orijinal decompile kodla karşılaştırma — 2026-10-18

Komut şablonları eşleşti. Kaynak proje `.`
salt okunur kullanıldı. Tekrar çalıştırılabilir denetim:

```bash
python3 -B tools/audit_decompiled.py
```

Sonuçlar: [decompiled_audit.json](../evidence/decompiled_audit.json).
Bu araç orijinal projedeki `rv32_trace` ile korunmuş talimat envanterini ve ROM
verisini Python RAM modelinde çalıştırır. USART ve UI yaprak çağrıları yakalanır;
hedefe erişim veya UART gönderimi yoktur. Bu, tam MCU/ASIC emülasyonu değildir.
Import sırasında eski dizine bytecode yazılması kapalıdır. Okunan kaynakların
başlangıç/bitiş SHA-256 değerleri eşleşti.

## Komut baytları

Decompile `0xB5C8` düzenli görünüm tuş işleyicisinde:

| Mod | CC (hex) | Son bayt (hex) | Doğrudan kaynak satırı |
|---|---|---|---|
| DC düşük akım / mA UI grubu | 11 | 16 | B5C8:32–33 |
| DC A | 10 | 15 | B5C8:36–37 |
| AC düşük akım / mA UI grubu | 16 | 1B | B5C8:40–41 |
| AC A | 15 | 1A | B5C8:44–45 |
| Sıcaklık | 12 | 17 | B5C8:104–105 |
| NCV | 09 | 0E | B5C8:108–109 |
| LIVE | 13 | 18 | B5C8:113–114 |

Tam ordinary şablon: `AA 55 05 CC 00 00 00 00 00 SON`.
`0x97C4` tam 10 bayt gönderir. `0xA910` event/UI kaynak yolları DC A/mA,
AC A/mA ve sıcaklık ilişkilerini destekler; `0xA7A4` 09/13 modlarını NCV/LIVE
çizim dallarına yönlendirir. ID bir ASIC register tanımı veya genel payload
checksum kuralı olarak yorumlanmaz.

24 denetim bağlamından 19 farklı literal form elde edildi. Bunlar hem eski
`analysis/asic_protocol_commands.json` hem yeni komut kataloğuyla aynı.
C serializer'ın 18 çıktısı orijinal baytlarla birebir eşleşti. Diagnostic
`AA 55 02 A0 ... A2` kaynakta var, serializer bilerek üretmiyor. Legacy
`AA 55 05 00 ... 11` formu da kaynakla aynı; son baytı `00+05` diye düzeltilmez.

25 gerçek oturumda toplam **6 tam, UART hatasız MCU TX paketi** bulundu:
14 iki kez; 0C, 0D, 0B ve 0A birer kez. Altısının tamamı orijinal constructor
çıktısıyla aynı. Son yedi açık giriş kaydı mod seçildikten sonra başladı;
yukarıdaki yedi modun seçim komutları bu kayıtlarda yoktur. Bu satırlar için
kanıt orijinal kod ve UI bağlamıdır, yakalanmış seçim TX'i değildir.

## ASIC cevapları ve yorum farkları

256 segment girişinin tamamı orijinal `0x98AA` ile eşleşti. Gerçek corpus'taki
74 farklı, temiz `5AA5` cevabında orijinal `0x997E` / `0xBE78` çalıştırılarak
şunlar karşılaştırıldı: dört segment kodu, işaret, ondalık konumu, birim,
AC/DC kodu ve b7 bit7. Hepsi eşleşti. Yeni parser'ın sayısal kabul ettiği
cevaplarda değer de orijinal float sonucuyla eşleşti (rel_tol=1e-6,
abs_tol=1e-9; orijinal float32 yuvarlaması gözetildi).

NCV kaydındaki iki farklı ham cevap için orijinal `0xA74E` seviye=0 verdi;
LIVE kaydındaki üç farklı cevap için `0xA782` active-render=0 verdi. Yeni
context gerektiren durum çözücüsüyle aynı. Aktif alan/kablo eşikleri denenmedi.

`0x17F92`, unit code=8 için `units/ua.jpg`, 9 için `units/ma.jpg`, 10 için
`units/a.jpg` seçer. Dolayısıyla mA **mod başlığı** altında kaydedilen code=8
cevabını µA yorumlamamız decompile kodla uyumludur. Unit code=21 için C/F
resmi host ayarından seçilir; `0x9CAA` Fahrenheit tercihinde gösterim dönüşümü
yapar. Paket code=21 tek başına host C/F tercihini taşıyor sayılmaz.

Yeni sürücünün bazı çıktıları bilinçli olarak stock UI metniyle aynı değildir:

- Stock parser işaret/boşluk/ondalık düzenleyerek metin üretir; yeni `text`
  segmentlerin ham sembol metnidir. Örneğin stock mode=0 metni ` L1UE ` iken
  ham sembol metni `L1UE` kalır.
- NCV'nin E4 segmenti orijinal sözlükte de 255/bilinmeyendir. Generic stock
  parser signed-char davranışıyla `/` üretebilir; yeni parser `?` ve numeric=None
  tutar. NCV UI bu metin yerine ayrı seviye çizimini kullanır.
- Stock, sıcaklık/düşük akım modlarında baştaki boş LCD hücrelerini `0` yapabilir;
  yeni ham metin boşlukları korur. Sayısal dönüşüm ayrı yapılır.
- `0xC350` ve `0xC468` DMA sayacı/kopyası 13'tür; hat kayıtlarında görüntü
  burst'leri 12, `AA55FD` event'leri 10 bayttır. Tampon uzunluğu wire uzunluğu
  olarak uygulanmaz ve ham kayda bayt eklenmez.

RX checksum, opaque kuyruk, fiziksel kademe ve analog kalibrasyon bu
karşılaştırmayla çözülmüş sayılmaz.

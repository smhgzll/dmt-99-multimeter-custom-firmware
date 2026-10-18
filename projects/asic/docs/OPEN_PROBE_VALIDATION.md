# Açık uç mod kayıtları — 2026-10-18

Komutlar ve cevap alanlarının orijinal decompile kodla doğrudan denetimi:
[DECOMPILED_COMPARISON.md](DECOMPILED_COMPARISON.md).

Kullanıcı yeni kondansatör, pil veya başka malzeme bağlamak istemediğini
belirtti. Sonraki tüm kayıtlar problar açıkken, harici kaynak olmadan alındı.
Sonuçlar ekran/protokol durumunu gösterir; analog kalibrasyon veya bilinen
sinyal/komponentle doğrulama sayılmaz.

| Kullanıcının seçtiği mod | Kayıtta çözülen gösterim | Tam paket | UART sınır hatası |
|---|---|---:|---:|
| Kapasitans | 0.014 nF | 59 | 0 |
| Buzzer/süreklilik | Ham ` .0L `; numeric=None; firmware unit code=6/V | 61 | 0 |
| DC V | −0.003..0.000 V | 60 | 0 |
| AC V | 0.272..0.273 V | 58 | 1, kayıt sonunda eksik aday |
| Hz | 48.40..51.57 Hz, çoğunluk 50.03..50.05 | 61 | 0 |
| DC düşük akım, kullanıcı mA modu | Ham `   0`, unit code=8/µA; ac_dc_code=2 | 60 | 0 |
| AC düşük akım, kullanıcı mA modu | Ham `   3`, unit code=8/µA; ac_dc_code=1 | 59 | 0 |
| DC A | 0.000 A; ac_dc_code=2 | 61 | 0 |
| AC A | 0.001 A; ac_dc_code=1 | 60 | 1, kayıt başında eksik parça |
| NCV ile uyumlu dalgalı kablo ekranı | Ham ` E? `; sayısal değil, host NCV seviye hesabı=0 | 61 | 0 |
| LIVE, yalnız kablo ekranı | Ham `L1UE`; sayısal değil, host LIVE active-render=false | 61 | 0 |
| Sıcaklık, harici prob yok, °C seçili | Ham `  32`, numeric=32, unit code=21; kullanıcı ekranı 32 °C | 58 | 0 |

Her kayıt 10 s / 1 MHz, D0=MCU_TX, D2=ASIC_TX. Görüntü mesajları 12 bayt
`5AA5` ailesindedir. Kullanıcı kapasitans ekranını önce 0.015, sonra nF
birimiyle doğruladı; 0.014'e sessizce eşitlenmedi. DC için dalgalanma, Hz için
yaklaşık 50 Hz bildirdi. AC/Buzzer ekran metni ayrıca bildirilmedi; decoder
çıktıları kullanıcı bildirimiymiş gibi metadata'ya yazılmaz.

DC V açık uç kaydında tüm 60 pakette `ac_dc_code=2`; AC V kaydında tüm 58
tam pakette `ac_dc_code=1`. Orijinal formül b2 bit1 öncelikli, sonra bit0'dır.
DC/AC düşük akım ve A kayıtlarında da sırasıyla 2/1 gözlendi.
Bu, bu V ve akım modlarında 2→DC ve 1→AC ilişkisini destekler. Diğer ölçüm modları,
fiziksel sinyal tipi/kalibrasyonu ve çakışan flag'ler ayrıca doğrulanmadı.

Direnç açık ucunda `" 0.L "`, unit code=13, decimal_after=2; buzzer diye
bildirilen modda `" .0L "`, unit code=6, decimal_after=1 gözlendi. İkisi de
numeric=None. Firmware unit adının V olması kullanıcıya sorulmamış bir Ω
birimine dönüştürülmez. Buzzer/diode ayrımı ve kapalı devre davranışı tam
çözülmüş sayılmaz.

AC açık uçta b9..11 sıfır. Diğer pek çok modda b9=0, b10..11'in BE16 yorumu
yaklaşık 300'lü sayılar üretir. Tamsayı/ölçek/yardımcı ölçüm veya status anlamı
doğrulanmadı. Kullanıcı AC ekranında ikinci/küçük okuma olmadığını bildirdi.
Sıcaklık/frekans alanı olduğu iddia edilmez; adaylar
`evidence/opaque_tail_hypotheses.json` içindedir.

Temiz byte akışları bağımsız sigrok UART decoder'ıyla karşılaştırıldı.
AC'nin son eksik adayı korunur; temiz, kesintisiz regresyon kesiti ayrıca
karşılaştırılır. RX checksum hâlâ bilinmiyor.

60 s hedefli tarama 29.532160 s / 29,532,160 sample ile bitti ve eksik
etiketlendi. Sonunda MCU command 0A, ASIC event 0A ve ilk 0.000 nF paketleri
yakalandı; 0A'nın kapasitans UI ilişkisi gözlendi. Sonraki modlar ayrı 10 s
kayıtlara alındı; aralarındaki boşluklar birleştirilmedi. Erken bitiş nedeni
çözülmüş değil; USB power/control=on ve runtime suspend süresi=0 olarak okundu.

Regresyon kesitleri `tests/fixtures/real_*open*` ve `real_capacitance_mode_event*`
dosyalarıdır. Parent `.sr` hash'leri, sample aralığı ve metadata snapshot'ı
her JSON'da bulunur. Tam ham kayıtlar `captures/` altında korunur.

## NCV, LIVE ve sıcaklık için yorum sınırları

İlk NCV isteğinde kullanıcı mod adını bilmediğini, kablo simgesinin üstünde/altında
dalga bulunduğunu söyledi. Tek tuş sonrası yalnız kablo ekranını bildirdi.
[FNIRSI kılavuzu, basılı s.16–17](https://www.docs.beti.com.tr/FNIRSI/DMT-99/DMT-99_ing.pdf)
bu görünümleri sırasıyla NCV/LIVE olarak gösterir. İlk kayıt etiketi
`cable_wave_icon_idle_unconfirmed` korunur; sonradan desteklenen mod ilişkisi
metadata geçmişine eklenmiştir. Bu sabit mod kayıtları seçim komutlarını yakalamadı.
09/13 komutlarının wire üzerinde gözlendiği iddia edilmez.

Orijinal `reconstructed/ui/screen.c`, mode=09 için dört segment arasında code=16
(`-`) sayısını NCV çizim seviyesine verir. Mode=13 için ilk segment code=14 (`L`)
ise LIVE aktif görünümü çizilmez. `dmt99_host_ui_status` ve Python `host_ui_status`
bu işlemleri yalnız çağıranın açıkça verdiği host mode ile uygular. Açıkta NCV=0
ve LIVE=false gözlendi. Enerjili kaynak, eşikler veya alan şiddeti denenmedi.
NCV'deki ham segment E4 sözlükte yoktur; `?` korunur. LIVE ham `L1UE` metni
`LIVE` diye değiştirilmez. Bu kayıtların sayısal değeri None'dır.

Kullanıcının mA mod adı/başlığı ile paket birim kodu=8/µA ayrı tutulur. Düşük
akım açıkta yalnız 0/3 seviyelerinde gözlendi; mA kademesi veya ölçek geçişi bu
kayıtlarla doğrulanmaz. A açıkta 0.000/0.001 verisi gerçek akım kalibrasyonu değildir.

Sıcaklıkta kullanıcı hem °C tercihini hem 32 ekran değerini doğruladı. Birim
kodu=21 tek başına C/F seçimini kanıtlamaz; `dmt99_value_si` bu kodu reddetmeye
devam eder. Orijinal `ui/reading.c` mode=12 için Fahrenheit tercihi açıksa
gösterimde ×1.8+32 uygular; bu firmware kaynaklı bilgi, fiziksel Fahrenheit
karşılaştırması değildir. Sıcaklık kaydındaki b10..11=0143..0146, BE16=323..326;
ana okuma 32 ile yakınlık, opaque kuyruğun anlamını veya ölçeğini doğrulamaz.

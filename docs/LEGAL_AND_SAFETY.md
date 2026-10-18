# Bağımsızlık, kullanım sınırları ve sorumluluk

Bu proje FNIRSI ile bağlantılı değildir; FNIRSI tarafından hazırlanmış,
onaylanmış, sertifikalandırılmış veya desteklenen bir firmware/araç değildir.
Ürün ve marka adları yalnız incelenen cihazı ve kaynakları tanımlamak içindir.
FNIRSI, WCH, Espressif ve id Software dâhil hak sahiplerinin hakları saklıdır.

## Garanti ve sorumluluk

Kaynak, doküman, BIN, tema ve araçlar deneysel olarak, **olduğu gibi** sunulur.
Uygulanabilir hukukun izin verdiği ölçüde; çalışabilirlik, belirli amaca
uygunluk, ölçüm doğruluğu, veri bütünlüğü veya ihlâlsizlik garantisi verilmez.
Yazarlar/katkıcılar; cihazın çalışmaz hâle gelmesi, bootloader/kalibrasyon/ayar
ve veri kaybı, hatalı ölçüm, maddi zarar, iş kaybı veya kullanım sonucu oluşan
doğrudan/dolaylı zararlardan sorumluluk kabul etmez. Kullanma/değiştirme/yükleme
kararı ve gerekli yedekleme kullanıcıya aittir.

Bu ifade kanunen kaldırılamayan hakları veya sorumlulukları ortadan
kaldırdığı iddiası taşımaz. Üçüncü tarafların kendi lisanslarındaki garanti
hükümleri ayrıca geçerlidir. Bu doküman hukukî görüş değildir.

## Elektriksel ve cihaz riskleri

- Özel firmware, cihazın üretici güvenlik sınıfını veya kalibrasyonunu
  yeniden onaylamaz. Güvenlik/kritik/şebeke ölçümlerinde kullanılmamalıdır.
- Logic analyzer, USB-UART ve ESP32 GND bağlantısı bilgisayarın toprağını
  ölçüm devresine bağlayabilir. Açık cihaz/debug kablosuyla enerjili veya
  şebekeye bağlı bir devrede ölçüm yapmayın; probları ayırın.
- Hedef pinlere 5 V veya ESP32 besleme çıkışı bağlamayın. Analyzer pasif giriş
  olarak kullanılır, VCC'si bağlanmaz. RVSWD için kart raporundaki gerilim,
  açık drenaj ve pull-up koşulları geçerlidir.
- Yanlış imaj/adres, güç kesintisi, yanlış Flash geometrisi veya FAT yazması
  kaynakları/ayarları/bootloader'ı bozabilir. Emulator testi bunu engellemez.
- RDP kaldırma, korunmuş Flash'ın mass erase ile silinmesine yol açabilir.
  Elimizde özgün ilk 25 KiB'ın yedeği bulunmadığından bu işlem geri
  döndürülebilir bir güncelleme yöntemi değildir.
- Bu depodaki geçmiş lab komutları güvenli kullanım için genel bir onay veya
  tekrarlama önerisi değildir. Default RVSWD build'i deney kapılarını kapalı tutar.

## Araştırma ve yeniden dağıtım

Yalnız sahibi olduğunuz veya açıkça yetkili olduğunuz cihaz/veriler üzerinde
çalışın. Tersine mühendislik, koruma mekanizmaları, telif ve yeniden dağıtım
kuralları yargı alanına ve sözleşmelere göre değişebilir; bu depo genel bir
izin oluşturmaz. Kalibrasyon/sahiplik/kimlik verilerini halka açık paylaşmayın.

Özgün firmware, Flash'tan çıkarılmış JPEG/font, dekompilasyon, oyun verisi ve
patentli/telifli bileşenlere ilişkin haklar açık kaynak kod lisansından
ayrıdır. Dosyanın bu çalışma alanında bulunması onu serbestçe yeniden
lisanslama veya dağıtma hakkı vermez. [Lisans kapsamı](../LICENSE.md) ve
[üçüncü taraf bildirimleri](../THIRD_PARTY_NOTICES.md) esas alınmalıdır.

DOOM uyarlama kodu AGPL-3.0-or-later kapsamındadır. Dağıtım ve gerekiyorsa
ağ üzerinden kullanım için karşılık kaynak yükümlülükleri
[GNU AGPL metninde](https://www.gnu.org/licenses/agpl-3.0.en.html) açıklanır.
Bu kod lisansı id Software WAD/harita/doku/sprite verilerine izin sağlamaz.

Varsayılan `.gitignore`; üretici imajı, ham Flash dump, tam oturumlar, DOOM
verisi ve binary/debug çıktıları Git dışında tutar. Yerel dosyalar silinmez.
Bu filtre tek başına tüm hukukî veya kişisel veri denetiminin yerine geçmez.

## Kanıtların sınırı

Fiziksel UART kayıtları ve RVSWD teşhis oturumları vardır. Bunlar özel
firmware'in bütün sürümlerinin fiziksel doğrulaması değildir. BIN'in doğru
adrese linklenmesi updater'ın erase davranışını kanıtlamaz. VM'de bir başarılı
kopyalama evrensel kurtarma garantisi değildir. RDP bypass/Flash recovery
başarısı rapor edilmemektedir.

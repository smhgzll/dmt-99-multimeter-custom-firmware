# USB BIN yükleme, geri dönüş ve sınırlar

Bu işlem cihazın program belleğini değiştirir. [Riskleri okuyun](LEGAL_AND_SAFETY.md).
Bootloader geri okunamadığından, kesintide kurtarma veya her kart revizyonunda
kabul garantisi yoktur. RDP kaldırma/MCU erase normal yükleme adımı değildir.

## Hangi dosya?

| İstenen kullanım | Uygun dosya / yer |
|---|---|
| Ölçü aleti: POTAT-99 | `artifacts/firmware/potat-99/v0.6.0/DMT-99_V0.6.0_20261018.bin` veya yeni doğrulanmış USB uygulama sürümü |
| DOOM | `artifacts/firmware/doom/v0.4.0/DMT-99_Doom_V0.4.0_20261018.bin` |
| Üretici uygulamasına dönüş | `artifacts/firmware/original/DMT-99_V2.4_20240306.bin` |
| Tema | `THEME.PKG`; BIN değildir, [ayrı süreç](THEMES.md) |

Bu yollar yerel arşivi gösterir; dosyalar Git klonuyla otomatik gelmez.
Kaynağı ve hash'i doğrulanmış dosyanız yoksa indirme yerine rastgele `.bin`
seçmeyin. Yayın klasöründeki `release.json`, `SHA256SUMS` ve
[katalog](../artifacts/catalog.json) karşılaştırılmalıdır. Windows PowerShell:

```powershell
Get-FileHash .\DMT-99_V0.6.0_20261018.bin -Algorithm SHA256
```

Mevcut POTAT-99 0.6.0 SHA-256:
`e7381d64205047d3e43102e690acc0be5663532424f6103665579fd576c50ed0`.
DOOM 0.4.0:
`2622d1bc35b678d74234178b79c922b0523e00d8e490df89d79b1dc253ee5d61`.

**Kullanılmayacak dosyalar:** `firmware/development/` standalone/restore
imajları, ESP32 adaptör BIN'leri, 8 MiB W25Q64 dump'ı, `.elf`, `.map`, `.hex`
ve `debug/` dosyaları. 8 MiB dump MCU update imajı değildir.

## Windows 10/11 ve sanal makine

FNIRSI'nin [resmî güncelleme sayfası](https://www.fnirsi.com/pages/manuals-firmwares)
Windows 10/11 desteği bildiriyor ve macOS ile güncelleme yapılmamasını istiyor.
Bu projede **yükleme için şimdilik Windows şartı** uygulanır; Linux üzerinden
BOOT hacmine kopyalama desteklenen yol değildir.

Kullanıcı **sanal Windows 11 ile BIN kopyalamanın başarılı olduğunu** bildirdi.
VM kullanırken USB aygıtını bütünüyle Windows misafire aktarın; ana sistemde
diski aynı anda bağlamayın. VM paylaşımlı klasörü, USB aygıt aktarımının yerine
geçmez. Bu kullanıcı gözlemi bootloader'ın tüm doğrulama/erase davranışının
çözüldüğü veya her VM'nin çalışacağı anlamına gelmez.

## Yükleme

1. Bataryayı doldurun. Probları, ESP32 ve logic analyzer kablolarını ayırın.
   Cihazın özgün Flash çipi, paneli ve bootloader'ı yerinde olmalıdır.
2. Geri dönebileceğiniz çalışan POTAT-99/V2.4 BIN'ini bilgisayarda saklayın.
   Harici Flash yedeğinin aynı zamanda MCU bootloader yedeği olmadığını unutmayın.
3. Cihaz kapalıyken **POWER + MENU** tuşlarıyla USB güncelleme moduna girin ve
   USB'yi Windows bilgisayara bağlayın. Üretici yönergesini esas alın.
4. Görünen diski **biçimlendirmeyin**, bölüm tablosunu değiştirmeyin. Yalnız
   seçilmiş uygulama BIN'ini kopyalayın; aynı anda birden fazla firmware veya
   tema paketi bırakmayın. Dosya adını gereksiz değiştirmeyin.
5. Windows kopyalamayı bitirsin; diski güvenli çıkarın. Üreticinin tamamlanma
   yönergesine göre USB'yi ayırın ve normal açın. İşlem sırasında gücü kesmeyin.
6. POTAT-99 için About sürümünü, ekran/tuş/güç işlevlerini, ardından güvenli
   düşük enerjili referansla ölçüm davranışını kontrol edin. DOOM ölçüm yapmaz.

## Geri dönüş

DOOM'u kaldırmak için **aynı Windows POWER+MENU yöntemiyle** çalıştığı bilinen
POTAT-99 uygulama BIN'ini kopyalayın. Orijinal uygulama için V2.4 BIN'ini kullanın.
Bootloader ve kaynak hacimleri sağlam kalmışsa aynı yol kullanılabilir;
geri dönüşün her hata/kesinti durumunda çalışacağı garanti edilmez.

Dosya kabul edilmezse veya açılış başarısızsa diski formatlamak, RDP açmak,
mass erase yapmak veya `0x0` standalone imajı yazmak otomatik çözüm değildir.
Mevcut hata mesajını ve dosya durumunu kaydedin, doğru çalışan BIN/hash ve USB
yöntemini tekrar kontrol edin. Bootloader yok olmuşsa bu projenin elinde
doğrulanmış bir orijinal bootloader restore imajı yoktur.

## Adres güvencesinin sınırı

USB uygulama BIN'leri **dosya ofseti 0 → `0x08006400`**, vektörler `+0x38`
düzeniyle kontrol edilir. İlk 25 KiB dosyaya dahil değildir. Bu, üretilmiş
dosyanın doğru bölge için linklendiğinin kanıtıdır; okunamayan bootloader'ın
hangi sayfaları sildiğini tek başına kanıtlamaz. Ayar sayfasına ilişkin
uygulama davranışı ayrıca incelenmiştir; tam dahili Flash dump'ı yoktur.

[Mühendislik açıklaması](FIRMWARE_RELEASE.md) · [dump fizibilitesi](FIRMWARE_DUMP_FEASIBILITY.md).

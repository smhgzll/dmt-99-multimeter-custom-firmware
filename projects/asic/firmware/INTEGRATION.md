# CH32V303 entegrasyon sınırı

`dmt99_asic.c/.h` C11, libc `memcpy/memmove/memset` ve sabit boyutlu tamponlar
kullanır. HAL, MMIO, kesme, gecikme, UART gönderimi veya kalibrasyon erişimi yoktur.
Bu aşama transport ve firmware kaynaklı görüntü yorumlama modülüdür; analog
doğruluğu ve tüm modlarda çalışması henüz doğrulanmış sürüm değildir.

```c
dmt99_stream rx;
/* on_record tüm FRAME/UNFRAMED/INCOMPLETE kayıtlarını kopyalayarak saklamalı. */
dmt99_stream_init(&rx, 12, on_record, context);
/* UART RX ring buffer'dan HER alınan bayt için: */
dmt99_stream_feed(&rx, received_byte, uart_error_flags);
/* Gerçek UART IDLE'da, o ana kadarki tüm baytları besledikten sonra: */
dmt99_stream_idle(&rx);
```

DMA sayacının 13 olması 13 yeni bayt alındığını göstermez. Gerçek DMA yazma
indeksini/RXNE akışını kullanın; IDLE'da bayt sayısı yerine sabit 13 kopyalamayın.
Circular DMA wrap/TC/IDLE sıralamasını firmware binding'i ayrıca yönetmelidir.
Hatalar callback'e aynen aktarılır; hatalı kaydın sayısal çıktısını güvenilir
ölçüm olarak kullanma kararı uygulamaya aittir. RX checksum doğrulanmış değildir.

`parsed.numeric` doğruysa gösterilen birimdeki değer `mantissa/divisor`'dır;
tam sayı bölmesi yerine uygun float/rasyonel hesap kullanın. Unit code ve
`dmt99_unit_resource_name` firmware UI ilişkisidir. `dmt99_value_si` bilinen
unit kodlarından V/A/Ω/F/Hz cinsine matematiksel prefix dönüşümü yapar; bilinmeyen
ve C/F ayrımı belirsiz sıcaklık kodunu reddeder. Gerçek corpus V/µA/A/Ω/kΩ/MΩ/nF/Hz
ve code=21 sıcaklık örneklerini kapsar; diğer prefix'ler firmware kaynak ilişkisidir. V ve akım açık uç
kayıtlarında ac_dc_code=2 DC, =1 AC ile ilişkilendi. Fiziksel kademe, tüm overload
koşulları ve analog kalibrasyon ayrıca doğrulanmış değildir.
Direnç için Ω/kΩ/MΩ örnekleri gerçek referans kartı kayıtlarında karşılaştırıldı;
`dmt99_resistance_ohms` bunları Ω'a dönüştürür. `dmt99_is_resistance_open_pattern`
açık problarda gözlenen tam deseni tanır; tüm overload/fault türlerini sınıflamaz.
`parsed.numeric=false` durumunda divisor=0; bölme yapmayın. AUTO/OL/bilinmeyen
semboller otomatik sıfır ölçümüne dönüştürülmez.
Baştaki boş LCD haneleri sayısal değerde atlanır (gerçek 1Ω kaydındaki
`"  1.1"` → 11/10 Ω); `text` alanı boşlukları aynen korur. İç/son boşluklar ve
harfler sayısala zorlanmaz. Nominal 1Ω komponent ile ekrandaki 1.1Ω aynı
metadata alanı değildir; bu karşılaştırma mutlak analog doğruluğu ispatlamaz.

`dmt99_host_ui_status(&parsed, host_mode, &state)` için çağıran seçili modu
bilmelidir: 0x09 NCV'de dört segmentin `-` sayısı (0..4), 0x13 LIVE'da ilk
segment `L` değilse 1, aksi durumda 0 döner. Bu, orijinal screen.c çizim
kararının uygulamasıdır; fiziksel güvenlik/enerjili kablo dedektörü değildir.
Gerçek açık giriş kayıtlarında NCV=0, LIVE=0 görüldü; aktif seviyeler denenmedi.
Desteklenmeyen mod veya display dışı frame için false ve değişmeyen output vardır.
Ham NCV bilinmeyen sembolü ve LIVE `L1UE` metni parser'da aynen korunur.

`dmt99_build_firmware_template` sadece onaylı firmware literal formunu üretir;
hiçbir yere göndermez. Normal komutların çoğu henüz donanımda test edilmedi.
Legacy 00/check11 korunur; diagnostic A0 serializer'da sunulmaz. Kullanıcının
pasif analiz kuralı gereğince bu çalışma sırasında üretilen komutlar gönderilmez.

Custom firmware ağacına bu oturumda dosya kopyalanmadı ve MCU flash edilmedi.
Mevcut custom firmware'in `rv32imac_zicsr_zifencei / ilp32` ayarlarıyla obje ve
statik arşiv derlendi. Yeniden üretmek için proje kökünden `make -C firmware`;
farklı toolchain için `make -C firmware CROSS=/path/to/riscv-prefix-`.
`build/libdmt99_asic_rv32.a` yalnızca host'ta oluşur; Makefile hedefe bağlanmaz.
Standart `mem*` ve soft double işlemleri için firmware'in libc/libgcc link'i
gereklidir; RV32 obje derlemesi tam uygulama link/RAM/stack doğrulaması değildir.
Gerçek ölçüm corpus'u büyüdükten sonra binding ve fiziksel dönüşümler ayrıca
doğrulanacak. Eski 13 bayt formu ancak açık profil tercihiyle stream'de seçilir;
bu seçeneğin bulunması wire uzunluğunun donanımda doğrulandığı anlamına gelmez.

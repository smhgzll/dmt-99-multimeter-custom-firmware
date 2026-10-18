# ASIC protokolü — firmware kaynakları ve donanım kanıt durumu

Bu belge tam/doğrulanmış bir wire spesifikasyonu değildir.
Gerçek AUTO/açılış kayıtları alındı; kapsam ve byte dizileri
[HARDWARE_FINDINGS.md](HARDWARE_FINDINGS.md) içindedir. `evidence/sources.json` okunan kaynakların
mutlak yollarını ve SHA-256 değerlerini içerir; literal komutlar
`evidence/firmware_commands.json` içinde kaynak metadata'sıyla korunmuştur.

| Alan | Firmware kaynaklı bilgi | Fiziksel doğrulama |
|---|---|---|
| UART | `0xC350`: USART1 PA9/PA10; main 9600; 8N1 | 1/2 MHz AUTO kayıtları 9600 8N1 çözümlemesiyle tutarlı; bağımsız decoder karşılaştırması geçti |
| RX DMA | DMA1 channel 5, circular, byte, 13; IDLE ISR `0xC468` resetler | Bekleniyor |
| TX | `0x97C4`: tam 10 bayt, her baytta TC bekler | Açılışta 14 komutunun 10 bayt literal formu görüldü |
| TX b0..1 | `AA 55` | Açılış 14 komutunda görüldü |
| TX b2 | Çoğunlukla `05`; diagnostic A0 formunda `02` | Bekleniyor |
| TX b3 | 17 ordinary command ID; ayrıca legacy 00 ve A0 literal | ASIC etkileri ayrı doğrulanacak |
| TX b4..8 | Elde edilen tüm literal formlarda sıfır | Genel payload bilinmiyor |
| TX b9 | Listelenen command+5 formları, legacy 00/11, A0/A2 | Genel checksum bilinmiyor |
| RX b0..1 | `0xBE78` görüntü parser guard: `5A A5` | AUTO görüntü mesajı 5AA5/12; event adayı AA55FD/10 |
| RX b2..6 | Dört packed LCD segment sembolü; BCD değil | Direnç ve açık uç ekranlarıyla karşılaştırıldı; tüm semboller fiziksel test edilmiş değil |
| RX b2 bit4 | Negatif işaret | Bekleniyor |
| RX b3/b4/b5 bit4 | Decimal sonrası 1/2/3, bu öncelikle | Üç konum direnç kayıtlarında gözlendi; çakışma önceliği firmware kaynaklı |
| RX b2 bit0/1 | Host kodu 1/2; bit1 öncelikli | V, düşük akım ve A açık uçta 1=AC, 2=DC ile ilişkilendi |
| RX b6..8 | Host unit priority tree, UI kaynak ilişkileri | Ω/kΩ/MΩ direnç ve V/µA/A/nF/Hz/sıcaklık açık uç örnekleri var; fiziksel kademe bilinmiyor |
| RX NCV/LIVE | screen.c: NCV '-' sayısı; LIVE ilk sembol L değilse aktif çizim | Açıkta NCV=0, LIVE=false; aktif giriş denenmedi, host mode açıkça gerekli |
| RX b7 bit7 | İsimsiz status flag | Anlamı bilinmiyor |
| RX b2=FD | Event snapshot, b3 event ID; ISR header guard içermez | ACK korelasyonu kanıtsız |
| RX b9..12 | Eski 13-byte DMA görünümünde opaque | Gerçek 12-byte mesajda b9..11 var; anlamları bilinmiyor, b12 wire'da gözlenmedi |
| RX integrity | Firmware'de doğrulanmış checksum kontrolü bulunmuyor | Var/yok, offset ve algoritma bilinmiyor |

Packed segmentler:
`(b2&F0)|(b3&0F)`, `(b3&F0)|(b4&0F)`,
`(b4&F0)|(b5&0F)`, `(b5&F0)|(b6&0F)`.
`0x98AA` karşılaştırma öncesi bit `0x10` temizler. Kodlar
`EB 0A AD 8F 4E C7 E7 8A EF CF` → `0..9`.
Diğer semboller ve unit priority uygulaması eski reconstructed C ile
differential test edilir; eski reconstruction'ın doğruluğu burada yeniden
makine kodu emülasyonuyla ispatlanmış sayılmaz.

Üç kanıt katmanı korunur: **donanımda gözlenen**, **firmware kaynaklı/olası**,
**bilinmeyen**. Gözlenen wire şekilleri analog anlamların doğrulandığı şeklinde
yorumlanmaz. Güncel donanım erişim sonuçları
`evidence/probe_*.json` dosyalarındadır; USB tanınması UART formatı doğrulaması
olarak sayılmaz.

Planlanan fiziksel sıra: startup/idle, resistance open/1k/10k/100k,
capacitance 100n/1u/10u, DC 1.5V battery, continuity open/closed, AUTO transitions.
Her aşamada bağlantı kullanıcı tarafından hazırlanıp bildirildikten sonra kayıt
başlatılır. Şebeke testi yapılmaz. Ham dosyalar ve metadata kesinlikle korunur.

# POTAT-99 DOOM doğrulaması

## 0.4.0 — tuş düzeltmesi, kademeli hareket ve görünürlük

18 Ekim 2026. Orta PC8 artık debounce sonrası basışta ateş eder. Tek başına
uzun basmak pause üretmez. PC8 + aşağı PC10 birlikte 2 saniye tutulunca menü
bir kez açılır; iki tuş bırakılana kadar ateş/geri hareket bastırılır.
Menü onayı tuş tekrarından etkilenmez. Bootta basılı tuşlar bırakılana kadar
arm edilmez; POWER ve 32-bit saat sarımı korunur.

- Dört C grubu ve dört ASan/UBSan grubu geçti: core, sound, events/AI, controls.
  Önceki 5.920 filtreli/tam ray, 2.000 karma action ve 6.000 upstream saf
  collision/ray karşılaştırması korunur. Altı Python host testi geçti.
- Yeni controls testleri: kısa/uzun ateş, 2.000 ms chord eşiği, erken bırakma,
  debounce, menüde tek onay, chord sonrası bırakma, boot guard, saat sarımı,
  kısa basışta küçük hareket/dönüş, ivme, sabit üst hız, bırakınca durma,
  yön değişiminde ramp reset ve zıt tuşların iptali.
- Hareket 40→160 birim/s, ivme 240 birim/s²; dönüş 16→80 derece/s,
  ivme 128 derece/s². Q8 kesirler saklanır. Tek servis aralığı 100 ms ile,
  birikmiş dünya hareketi 64 birim / 45 derece ile sınırlanır. Çizim sırasında
  tuş servisi sadece girdi toplar; dünya değişiklikleri güvenli noktada uygulanır.
- Ateş sesi artık duvar arkasındaki IDLE düşmanı uyandırmaz. Görüş kaybında
  en fazla 50 AI tick son gözlenen konuma takip; güncel gizli oyuncu konumu
  hedefe aktarılmaz. Haritanın 404 duvarı için çapraz görüş engeli kontrol edildi.
  Duvar arkasındaki windup atışı, imp projectile ve splash hasarı da engellenir;
  görünmeyen hedef unutulur. Global rota arama eklenmedi.
- GOD MODE oyuncunun tüm `hurt_player` yollarını engeller. SOUND kapalıyken
  target PWM başlamaz. Seçenekler New Game ile korunur; app init/power cycle
  god kapalı, ses açık ve tabanca varsayılanlarını geri getirir. Flash yazılmaz.
- Ölüm animasyonu bitince ceset 80 tick (oyun saatiyle 8 s) görünür, sonra
  REMOVED olur; yeni oyunda tekrar kurulur. Diziler sabittir, heap yoktur:
  kaldırma RAM iadesi değil sprite çizim işi tasarrufudur.

Gerçek BIN için dört RV32 senaryosu geçti: **134 tamamlanmış kare**, bunların
**60'ı native C ile RGB565 düzeyinde aynı**. Action, Q8 motion ve tick gözlemcileri
native oracle'a replay eder; hedef talimatları değiştirilmez. Fiziksel GPIO
modelinde dört yön, 2,3 s solo fire, 2 s chord, pause clock, resume, weapons,
god/sound menüleri ve POWER kontrol edildi. God fixture'ında düşman saldırısı
sağlığı düşürmedi; muted-fire sırasında yeni TIM2 tonu başlamadı. Actor konumu
ve ceset süresinin hızlandırıldığı SRAM fixture'ları piksel eşitliği sayılmaz.
Legacy `collision_during_detour` alanı bu sürümde duvarın arkasında uyanmadan
kalan aktörün altı konum kontrolüdür; ek bir hareketli detour kanıtı sayılmaz.

BIN **163.444 B**, fiziksel **0x08006400–0x0802E273**; statik RAM **8.568 B**,
stack **6.144 B**, boşluk **18.056 B**. Statik stack bound **3.274 B**; gözlenen
en yüksek emülatör kullanımı **848 B**. Emülatörde Flash/option-byte/NOR yazımı,
ASIC TX, beklenmeyen IRQ/fault veya panel sınır ihlali yoktur. Ana projedeki
5.017 dosya ve önceki üç DOOM BIN'i hash ile korundu. Tam object/asset rebuild
aynı BIN'i üretir. Güncel kanıt `docs/validation/v0.4.0/` ve ekranlar
`docs/screenshots/v0.4.0/` içindedir. Yeni sürüm fiziksel cihazda denenmedi.
Çizim üst sınırı hâlâ 5 FPS'tir; donanım hız/yükleme/ses ölçümü yapılmadı.

## 0.3.0 — zamanlı AI, animasyonlar ve silahlar

18 Ekim 2026. AI artık tuşlardan bağımsız 100 ms adımlarla çalışır. Yeni davranış
bilerek eski turn-based AI'dan ayrılır; 0.2.0'ın 3.000 action/AI eşitlik sonucu
bu sürüme taşınmaz. Aynı upstream ray intersection ve saf duvar/circle collision
karşılaştırmalarının **6.000** vakası korunur. Önceki test kaynakları/sonuçları
Git geçmişinde ve eski rapor klasörlerinde durur; ana proje değişmedi.

- Core C: menü/resume/new game, silah menüsü, **5.920** filtreli/tam ray
  karşılaştırması, **2.000** zamanlı walk/combat işlemi ve renderer sınırları.
- Event/AI fixture: girdi anında NPC hareket etmemesi; tuşsuz clock ile hareket,
  görüş/hearing, duvar arkası görüş/atış engeli, duvar çarpışması/detour, zamanlı
  saldırı ve ölüm/respawn, HP/pain/death/corpse, küçük pickup ölçeği.
- Altı silahın ayrı hasarı, shotgun saçmaları, seri atış, kısa menzil, roket
  çarpışma/alan hasarı ve silah animasyonunun clock ile değişmesi.
- Sound C testi; üç C grubunun ASan + fail-fast UBSan çalışması. Ptrace nedeniyle
  LeakSanitizer kapalı; address/UB kontrolleri açıktır.
- Dört Python host testi: deterministik CLI, pause/clock, silah seçimi ve PNG.

Kısa GPIO girişlerinin ağır raycast içinde kaybolduğu emülatörde yakalandı.
Tuş/ses servisi ray grupları, actor güncellemeleri ve çizim şeritleri arasında
çalışır. Oyun durumuna girişler güvenli güncelleme noktasında uygulanır; servis
callback'i doğrudan NPC/world değiştirmez. Sekiz girişlik kuyruk sabittir;
dolduğunda pause son girdiye öncelik verir. Yeni IRQ veya heap eklenmedi.

Sprite metadata'sı gerçek WAD en/boylarından gelir; pickup yüksekliği 8–24
birim, aktör yüksekliği 56 birimdir. Animasyon kareleri gerçek WAD lump'larıdır.
Harita tek yükseklikli kalır; global pathfinding, tam envanter ve tüm yönler için
sprite rotasyonları uygulanmadı. AI yerel engel dolaşması kullanır.

Yeni BIN **162.096 B**, `0x08006400–0x0802DD2F`; statik RAM **8.028 B**,
stack **6.144 B**, boşluk **18.596 B**. Statik stack bound **3.274 B**.
Hedef çizim üst limiti 5 FPS'tir; fiziksel performans ölçümü yoktur.

Dört emülatör senaryosu geçti: toplam 77 tamamlanmış kare, bunların **40'ı**
native C ile RGB565 düzeyinde aynı. Kalan 37 kare kontrollü target fixture
akışındadır. Fixture'da NPC tuşsuz görüp yaklaşır ve hasar verir; atış öldürür,
ceset kalır; duvar görüş/atışı engeller, detour sırasında duvar geçilmez.
Gözlenen stack 852 B; statik bound 3.274 B korunur. Yeniden tam derleme
byte-identical; ana projenin 5.017 dosyası ve önceki iki oyun BIN'i değişmedi.

Güncel makine raporları `docs/validation/v0.3.0/` içindedir. Gerçek RV32 ekran
karşılaştırmaları GPIO/action ve simulation-tick gözlemcileriyle aynı zaman
adımlarını native oracle'a replay eder; target talimatları değiştirilmez.
Kontrollü SRAM actor fixture'ları ayrı işaretlenir ve native piksel eşitliği
iddiasına katılmaz. Bunlar fiziksel cihaz deneyi değildir.

## 0.2.0 — yön tuşları ve buzzer

18 Ekim 2026. Yukarı PC11=ileri, aşağı PC10=geri, sol PC12=sola dön,
sağ PC9=sağa dön; PC8 kısa/uzun ve POWER korunur. Bu fiziksel yön eşlemesi
kullanıcının 0.1.0 gözleminden alınmıştır; 0.2.0 üzerinde fiziksel test yapılmadı.

Yeni BIN **81.452 B**, `0x08006400–0x0801A22B`; statik RAM **6.160 B**,
stack **6.144 B**, boşluk **20.464 B**. Stack bound yine **2.650 B**.
0.1.0 yayın dosyaları ve ana POTAT-99 projesi korunur.

- C core, üç Python host testi ve **9.000** upstream karşılaştırması PASS.
- Yeni event fixture testi: gerçek adım, duvara takılma, dönüş sessizliği,
  isabet/isabetsiz atış, bump kill, hasar ve ölüm/respawn olayı PASS.
- Yeni sound testi: dört desen, ateş/öldürme sırası, hasar önceliği, sınırlı
  bekleyen olaylar, pause/stop, geciken servis ve 32-bit zaman sarımı PASS.
- Üç C testinin Clang ASan/UBSan çalışması PASS; ptrace istisnası aynı.
- Dört RV32 emülatör senaryosu PASS. Düzeltilmiş yönlerle normal oyun
  ekranları native C ile aynı; kill ve ölüm/respawn fixture'larında dört ses
  olayının tamamı görüldü. PWM frekansları nominal değerlere %1 içinde,
  duty yaklaşık %50; süre sonunda, pause ve kapanmada buzzer kapalı.
- Yeniden tam derleme byte-identical; ana projedeki 5.017 dosya ve önceki
  0.1.0 BIN içeriği korunmuş durumda. Emülatörde gözlenen stack 864 B.

Olaylar oyun kodunun gerçek karar noktalarında üretilir. Sağlık snapshot'ı
karşılaştırılarak ölümde 100'e dönen hasar olayı kaybedilmez. Blocked movement
ve dönüş walk sesi üretmez. Gameplay matematiği upstream karşılaştırmasında
korunmuştur.

Tam RV32 BIN testinde düzeltilen dört GPIO yönü native ekranla karşılaştırılır.
SRAM'deki **izole test fixture'ları**, gerçek BIN kodunda öldürücü atış ve
ölüm/respawn dallarını tetikler; üretim firmware'inde fixture setter yoktur.
Bu kısım doğal oynanış iddiası değildir. PA1 AF push-pull, TIM2 CH2 enable,
PSC/ARR, yaklaşık %50 duty, ayrı frekanslar ve susma kontrol edilir.

Efekt servisinin raycast sırasında gecikebildiği testte görüldü. Sonraki notayı
atlamak yerine tam süresiyle başlatan politika kullanıldı; notalar uzayabilir,
oyun ses bitene kadar beklemez. Emülatör elektrik/akustik model değildir;
gerçek buzzer duyulabilirliği ve ses seviyesi henüz sınanmadı.

Güncel raporlar: `docs/validation/v0.2.0/`. Aşağıdaki 0.1.0 sonuçları tarihsel
kanıt olarak korunur.

## 0.1.0 doğrulaması (korunan önceki sonuçlar)

Tarih: **18 Ekim 2026**. Bu rapor yalnız ayrı `dmt_99_doom` projesi içindir.
Ana POTAT-99 projesinde build/test çalıştırılmadı ve kaynak/yayın değiştirilmedi.

## Derleme ve bellek

- RV32IMAC + Zicsr/Zifencei, ILP32, yazılımsal float; ESP-IDF GCC 14.2.0,
  `-Os -ffreestanding -fno-builtin`, `-Wall -Wextra -Werror`.
- Ham BIN **79.872 bayt**, yükleme `0x08006400`, son adres hariç `0x08019C00`.
  Alias entry `0x6400`, vektör `0x6438`; V2.4 başlık sözcükleri aynı.
- Kod üst sınırı alias `0x30000`; ayar sayfası `0x08040000` imajın dışında.
- Statik RAM **6.136 B**, stack rezervi **6.144 B**, aradaki boşluk **20.488 B**.
  `size` çıktısının BSS toplamı ayrılmış stack bölümünü de kapsar.
- Statik stack bound **2.650 B**: foreground 2.634 B + trap yolu 16 B.
  Yerel callback ailesi tanımları ve GCC `.su`/objdump verisi kullanıldı.
- Emülatörde gözlenen stack kullanımı oyun/ateş dahil **860 B**; yalnız boot
  senaryolarında 712 B. Bu, bütün olası oyun durumları için stack ispatı değildir.
- Bütün objeler ve WAD alt kümesi zorla yeniden üretildi; BIN byte-identical.
- BIN SHA-256:
  `c6c3bfed28a7fa6eba0718b9121155be706a30c8829ea06deadb1937b4a41005`.

## Çalıştırılan testler

| Test | Sonuç / kapsam |
|---|---|
| C core testi | PASS; menü/help/resume/new game, çarpışma, sınır kontrolü |
| Filtreli ışınlar / bütün duvarları tarama | **5.920** karşılaştırma PASS; 24 yön ve 50 yürüyüş konumu |
| Deterministik C walk/combat | **2.000** işlem; renderer 50 konumda, son tam ekran kapsaması |
| Clang ASan + UBSan | Aynı C testi PASS; LeakSanitizer ptrace ortamı nedeniyle kapalı |
| Python host suite | **3 test PASS**; tekrar üretim, pause/restart, PPM/PNG |
| Pinned upstream gameplay | **3.000 işlem** sonunda konum/açı/sağlık/kill/pickup eşit |
| Pinned upstream collision | **3.000 nokta** eşit |
| Pinned upstream raycast | **3.000 ışın** için wall/distance/fraction eşit |
| RV32 update format / ELF | PASS; header, vektörler, yükleme alanı, RAM ve undefined symbol kontrolü |
| Tam RV32 BIN + panel emülasyonu | **4 senaryo PASS**, aşağıda |
| Ana proje hash koruması | **5.017 mevcut dosya** eşit; yeni/değişmiş/silinmiş dosya yok |

Upstream karşılaştırması, orijinal `level1.c` fonksiyonlarının değiştirilmeden
alınıp AVR bellek/UART erişimlerinin host shim ile derlenmesine karşı yapılır.
AVR makine kodu veya orijinal id Software DOOM motoru ile karşılaştırma değildir.
Yeni kamera düzlemi ve yerel renderer ayrıca aşağıdaki RV32/host testleriyle
kontrol edilir. Upstream'in 140 duvarla kesilen candidate listesi burada tam
404 eleman kapasitesine sahiptir; filtreleme görünür duvarı sessizce kaybetmez.

## Gerçek RV32 BIN'in offline çalışması

Ana projeden **kopyalanmış** `Ch32Emulator` kullanıldı. Testte orijinal 8 MiB
Flash dump yalnız okunur ve emülatörün RAM'deki özel kopyasına alınır.
CPU kodu native renderer ile değiştirilmedi. Unicorn **2.1.0**, Pillow **13.0.0**
ile çalıştırıldı; emülatör zamanı fiziksel FPS ölçümü değildir.

1. **Temiz başlangıç:** DOOM menüsü → PC8 ile New Game → PC11/PC10 dönme →
   PC9/PC12 hareket → PC8 ateş → PC8 uzun ile pause → PC2 ile kapanma.
   Sekiz tamamlanmış ekran native C framebuffer ile **piksel piksel aynı**.
2. **Bootloader artıkları:** SysTick/OTG IRQ enable/pending ve VTF artıklarıyla
   başlangıç. Başlangıç kodu temizliyor; menü temiz başlangıçla aynı.
3. **HSE hazır değil:** mevcut POTAT-99 HSI×18 fallback yolu ile aynı menü.
4. **Açılışta POWER basılı:** oyun açılıyor, hemen kapanmıyor. POWER bırakılıp
   yeniden basılınca latch bırakılıyor ve CPU halt ediyor.

Her senaryoda fault/stray IRQ ve panel sınır aşımı **0**; ASIC TX **0**;
Flash erase/program ve unlock dizileri **0**; ayarlar ve NOR hash'i değişmiyor.
Oyun firmware'inde Flash/SPI yazıcı veya heap fonksiyonu bağlı değil.

Bir polling düzeltmesi test sırasında yapıldı: bırakılmakta olan bir hareket
tuşu debounce süresinde tekrar hareket üretmiyor. Uzun PC8 testi tamamlanmış
pause ekranını bekliyor; önceki tamamlanmamış/pending oyun ekranını saymıyor.

## Ana proje korunması

Başlangıç hash envanteri `docs/reference_hashes.json`; sonuç
`docs/validation/reference_preservation.json`. `.cache` ve `.history` envanter
dışındadır; bu dizinlerde build veya Git işlemi çalıştırılmadı. Mevcut Python
ortamı yalnız yorumlayıcı/kurulu bağımlılık olarak, bytecode yazımı kapalı
kullanıldı. Ana proje kaynakları, orijinal BIN, `flashdump/`, analiz kanıtları,
build ürünleri ve yayınlanmış `final/vX.Y.Z` dosyaları korundu.

## Donanımda ilk deneme

Fiziksel DMT99'da **hiçbir işlem yapılmadı**. Gerçek panel/tuş/power/backlight
zamanları, performans, USB updater kabulü ve geri dönüş henüz doğrulanmış değil.

1. Hâlen çalışan firmware dosyasını ve SHA-256'sını ayrı tutun; şarjı yeterli
   cihazın normal USB güncelleme moduna girdiğini önce kontrol edin.
2. [README](README.md) içindeki tek BIN'i mevcut USB yöntemiyle kurun.
   Uploader dosya/CRC/sürüm kuralları bilinmediği için kabulü burada varsaymayın.
3. Menü, New Game, beş tuş, kısa/uzun PC8, POWER bırakma/tekrar basma denenir.
   PA2 115200 konsol varsa banner veya trap mesajı kaydedilir.
4. Tepki süresi ve ekran yönü/renkleri kaydedilir. Oyun ölçüm, şarj ekranı,
   V2.4 düşük-pil akışı veya otomatik kapanma sağlamaz; kullanıcı POWER ile kapatır.
5. USB modunda önceki çalışan POTAT-99 BIN'i tekrar kurulur; ölçüm ve mevcut
   ayarlar kontrol edilir. Geri dönüşün çalıştığı cihaz gözlemi ayrı kaydedilir.

RDP, option byte veya debug programlayıcı işlemine ihtiyaç duyan bir adım yok.
Bootloader bulunmadığı için updater'ın erase aralığını veya kesinti sonrası
geri dönüşü emulator testlerinden garanti etmek mümkün değil.

Makine tarafından okunabilir build, stack, upstream, emulator ve koruma
raporları `docs/validation/` ve yerel `dist/v0.1.0/` içindedir.

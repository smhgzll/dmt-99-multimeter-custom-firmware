# POTAT-99 DOOM 0.4.0

DMT99 / CH32V303RCT6 için küçük E1M1 oyunu. Birleşik depoda kaynaklar
`projects/doom/`, yayınlar `artifacts/firmware/doom/` içindedir. Önceki bağımsız
çalışmanın test raporları korunur. [Birleşik derleme](../../docs/BUILDING.md),
[Windows yükleme ve geri dönüş](../../docs/UPDATING.md),
[riskler](../../docs/LEGAL_AND_SAFETY.md) güncel kullanım rehberidir.

**[Güncel USB BIN: DMT-99_Doom_V0.4.0_20261018.bin](dist/v0.4.0/DMT-99_Doom_V0.4.0_20261018.bin)**

0.4.0'da orta tuş basıldığı anda ateş eder; tek başına uzun basma menü açmaz.
**Orta PC8 + aşağı PC10 birlikte 2 saniye** pause menüsünü açar. Hareket ve
dönüş süreden hesaplanır; kısa basış küçük adım/dönüş, uzun basış sınırlı hız verir.
Düşmanlar duvar arkasındaki silah sesiyle uyanmaz. Görüş kaybında oyuncunun canlı
konumunu takip etmek yerine en fazla 5 saniye son gördükleri konuma yönelir.

![Menü seçenekleri](docs/screenshots/v0.4.0/sound-off.png)
![Oyun](docs/screenshots/v0.4.0/game.png)

[Yeni BIN’in emülatör görüntüleri](docs/screenshots/v0.4.0/README.md).

## Yenilikler

- Duvarla engellenen görüş/atış/ses; yalnız son gözlenen konuma sınırlı takip.
- GOD MODE ve SOUND menü seçenekleri. God tüm oyuncu hasarını engeller;
  ses kapalıyken PWM efekti başlamaz. Güç döngüsünde god kapalı, ses açıktır.
- Debounce sonrası basışta ateş, basılı tutunca silah cooldown’una göre tekrar;
  yalnız PC8+PC10 birlikte 2 saniye tutulunca pause. Bırakmadan tekrar açılmaz.
- Hareket 40→160 birim/s (ivme 240 birim/s²), dönüş 16→80 derece/s
  (ivme 128 derece/s²). Tuş bırakılınca durur, yön değişince ivme sıfırlanır.
  Zıt tuşlar birbirini iptal eder; alt derece hassasiyeti Q8 ile tutulur.
- Bekleme, takip, saldırı hazırlığı, saldırı, yaralanma ve ölüm durumları.
- İnsan düşmanlarda menzilli atış, imp'te kaçınılabilir ateş topu, demon'da yakın
  saldırı; saldırı bekleme süreleri ve birden fazla vuruş gerektiren sağlık.
- WAD'dan gerçek yürüme, saldırı, yaralanma ve ölüm kareleri; ölüm animasyonu sonrası 8 saniye duran, sonra kaldırılan cesetler.
  Aktör dizileri sabittir; heap yoktur. Silme RAM boyutunu değil çizim işini azaltır.
- Nesneler kendi en/boy oranlarında, 8–24 dünya birimi yüksekliğinde çizilir.
  Sağlık nesneleri sağlık verir; diğerleri toplama sayacına eklenir.
- Silah menüsü, farklı hasar/menzil/hızlar, namlu animasyonu, geri tepme ve
  yürürken silah salınımı. Roket/ateş topu hareketi ve duvar/aktör çarpışması.
- Çizim ve uzun hesaplamalar sırasında da tuş/ses servisi; girişler sekiz
  elemanlık kuyrukta tutulup güvenli oyun güncelleme noktasında uygulanır.

Bu, [doom-avr](https://github.com/Artfical/doom-avr/) tabanlı geliştirilmiş bir
raycaster'dır. Klasik DOOM motorunun tamamı değildir. Harita tek yüksekliklidir;
gerçek sektör/kapı/anahtar sistemi, müzik, tam envanter veya kalıcı kayıt yok.
Düşmanlar yerel engel dolaşma/duvar kayması kullanır; genel rota araması yoktur.
Sprite'lar kameraya bakar; tüm yönlerin görselleri bu küçük ROM'a alınmamıştır.

**Yeni sürüm offline/emülatör testlidir; fiziksel FPS, ses ve yükleme henüz
bu sürümle doğrulanmadı.** Önceki sürüme ait tuş gözlemleri kullanıcıdan alındı.

## Tuşlar ve silah seçimi

| Tuş | Oyunda | Menüde |
|---|---|---|
| Yukarı / PC11 | İleri | Önceki satır |
| Aşağı / PC10 | Geri | Sonraki satır |
| Sol / PC12 | Sola dön | Geri / Resume |
| Sağ / PC9 | Sağa dön | — |
| PC8 | Basışta ateş, tutulunca tekrar | Seç / onayla |
| PC8 + aşağı, 2 saniye | Pause menüsü | Geri / Resume |
| POWER / PC2 | Kapat | Kapat |

Ana menüde **WEAPONS** seçilir. Yukarı/aşağı ile silah, PC8 ile seçim yapılır;
sol ile seçmeden geri dönülür. Oyun sırasında PC8+aşağı 2 saniye → **WEAPONS** → seçim →
**RESUME**. Menü, silah ekranı ve pause sırasında dünya donar. Silah seçimi
RAM'dedir; başlangıçta tabanca gelir. Bu küçük oyun sürümünde altı silah baştan
seçilebilir ve cephane sınırsızdır. GOD MODE, SOUND ve silah seçimi New Game
ile korunur; tam başlangıçta varsayılanlar geri gelir. Bu seçenekler Flash’a yazılmaz.

| Silah | Davranış |
|---|---|
| FIST | Kısa menzil, 24 hasar |
| PISTOL | Tek mermi, 20 hasar, 400 ms bekleme |
| SHOTGUN | Beş saçma ×12 hasar, dağılım, 800 ms bekleme |
| CHAINGUN | Üçlü seri ×12 hasar, 400 ms bekleme |
| CHAINSAW | Kısa menzilli üçlü vuruş ×12 hasar |
| ROCKET | Hareket eden roket, merkezde 100/çevrede 50 alan hasarı |

Menzil, duvar ve hedefe isabet hasarı etkiler. Yakın roket patlaması oyuncuya
30 hasar verir. Tuşa basmak düşmanları bir adım ilerletmez; AI saati bağımsızdır.

## Sesler

PA1 / TIM2 CH2 üzerinden yaklaşık %50 PWM: adım 800 Hz; ateş 2600→1300 Hz;
hasar 330→220 Hz; öldürme 1200→1800→2400 Hz. Duvara takılma veya yalnız dönme
adım sesi üretmez. Hasar önceliklidir; öldürücü atışta ateşten sonra öldürme
sesi gelir. Menü ve kapanmada buzzer susar. Müzik/ses örneği yoktur; bunlar kısa
buzzer efektleridir. Masaüstü simülatör ses çalmaz.

## İmaj ve RAM

| Ölçü | Değer |
|---|---:|
| BIN | **163.444 bayt / yaklaşık 159,6 KiB** |
| Fiziksel yükleme aralığı | **0x08006400–0x0802E273** |
| Entry / vektör alias | **0x6400 / 0x6438** |
| İmaj üst sınırı | 0x30000; uygulamaya 167 KiB |
| Statik RAM | **8.568 B** |
| Stack rezervi / statik bound | **6.144 B / 3.274 B** |
| Statik RAM + stack dışında boş | **18.056 B** |
| Ekran | 320×200 sahne + 40 piksel HUD, 80 ışın |
| Çizim şeridi | 1.600 B; cihazda tam framebuffer yok |
| ROM görselleri/metadata | 129.200 B; 75 sprite karesi |

Dünya adımı 100 ms, çizim üst limiti 5 FPS'tir; gerçek donanım FPS ölçümü yoktur.
Uzun duraklamalarda güncelleme başına en fazla 500 ms işlenerek catch-up yükü
sınırlanır. Çizim/hız düşük kalırsa gerçek zamanın bir kısmı düşürülebilir.

Görseller küçültülmüş WAD alt kümesi olarak BIN içindedir; cihaza ayrıca WAD
veya paket kopyalanmaz. Yaklaşık 1,6 MB USB/BOOT hacmi MCU kod alanı değildir.
Oyun, harici Flash veya 0x08040000 ayar sayfasını yazmaz; ASIC'e komut göndermez.

## Kurulum ve ölçüm firmware'ine dönüş

1. Oyundan önce çalışan POTAT-99 BIN'ini bilgisayarda tutun. Ana projenin
   `../dmt_99_analysis/final/vX.Y.Z/` yayınları değiştirilmedi.
2. `cd dist/v0.4.0 && sha256sum -c SHA256SUMS` ile dosyayı doğrulayın.
3. Mevcut **POWER+MENU USB güncelleme** yöntemiyle yalnız güncel oyun BIN'ini
   kopyalayın. `debug/`, JSON, WAD veya görseller kopyalanmaz.
4. Kaldırmak için aynı yöntemle önceki çalışan POTAT-99 BIN'ini tekrar kurun.
   Orijinale dönüş seçeneği `../dmt_99_analysis/DMT-99_V2.4_20240306.bin`.

Bu uygulamada ölçüm modu, V2.4 şarj ekranı, düşük-pil akışı veya otomatik kapanma
bulunmaz; POWER ile kapatılır. BIN bootloader bölgesini içermez ve kendisi
Flash programlamaz. Bootloader okunmadığı için gerçek erase/kabul kuralları
ve kesinti sonrası geri dönüş garanti edilemez. RDP/option byte/programlayıcı
adımı veya cihazınıza bağlanan bir araç yoktur.

## Derleme ve simülatör

```bash
git submodule update --init
python3 tools/fetch_wad.py        # hash kontrollü yerel shareware giriş
make -j4 all
make test sanitize
make emulate
make verify                      # ana projedeki 5.017 dosya korunmalı
make package
python3 tools/simulator.py
python3 tools/simulator.py --script 'fwwwaaf..........' --png build/scene.png
```

PC'de W/S veya oklar hareket/seçim, A/D veya sol/sağ dönme, Space/Enter ateş/onay,
Space+aşağı 2 saniye veya Esc pause, F12 PNG. Pencere aynı C tuş/ivme kodunu
kullanır; tuş bırakma ve odak kaybı hareketi durdurur. Ana döngü zamanı ilerletir. Başsız `--script`
içinde her `.` 100 ms ilerletir; diğer karakterler zamanı doğrudan ilerletmez.

GCC/make/Python 3 ve RV32IMAC ILP32 GCC gerekir. Makefile ESP-IDF toolchain'ini
bulur; `CROSS=` ile değiştirilebilir. Python Tk pencere için, Unicorn/Pillow
emülasyon için gereklidir. Varsayılan emülatör Python'u ana projenin mevcut
ortamını yalnız okur; ayrı ortam için:

```bash
python3 -m venv .venv
.venv/bin/pip install -r tools/requirements-emulation.txt
make emulate EMU_PYTHON=.venv/bin/python
```

## Kaynaklar ve kanıt

- `src/game.c`: zamanlı dünya, aktörler, görünürlük/çarpışma, silahlar, roketler.
- `src/render.c`, `app.c`: sprite/şerit çizimi, HUD, menüler ve silah ekranı.
- `src/controls.c`: ortak debounce, iki tuşlu pause ve süreye bağlı ivme.
- `src/sound.c`: sınırlı kısa efektler; `board/board.c`: tuş, PWM, ekran, güç.
- `board/reference/`, `third_party/wch/`: ana projeden ayrı kaynak kopyaları.
- `tools/build_assets.py`: pinned WAD'dan animasyon/silah/nesne ROM verisi.
- `tools/test_emulator.py`: gerçek RV32 kodu, GPIO girişleri, panel framebuffer.

Güncel sonuçlar [TEST_RESULTS.md](TEST_RESULTS.md) ve
`docs/validation/v0.4.0/`; önceki raporlar ve yayın dosyaları korunmuştur.
Kaynak/lisans kökeni [NOTICE.md](NOTICE.md), dosya hash'leri
`docs/copied_sources.json` içindedir. WAD/görsellerin ayrı id Software koşulları
vardır; yerel BIN ve WAD Git'e eklenmez.

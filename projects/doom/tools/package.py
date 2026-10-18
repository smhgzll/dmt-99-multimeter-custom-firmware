#!/usr/bin/env python3
"""Publish only inside this project. Never touches the reference release tree."""
import argparse,hashlib,json,re,shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--date',default='20261018');a=ap.parse_args()
    if len(a.date)!=8 or not a.date.isascii() or not a.date.isdigit():raise SystemExit('date must be YYYYMMDD')
    checks=json.loads((ROOT/'build/image_checks.json').read_text())
    version=checks['format']['version'].removesuffix('-doom')
    if not re.fullmatch(r'\d+\.\d+\.\d+',version):raise SystemExit('invalid embedded version')
    emu=json.loads((ROOT/'build/emulator/results.json').read_text())
    if not checks['pass'] or not emu['pass']:raise SystemExit('validation failed')
    image=ROOT/'build/dmt99-doom.bin'
    if hashlib.sha256(image.read_bytes()).hexdigest()!=checks['sha256']:raise SystemExit('BIN changed since layout verification')
    if emu.get('image_sha256')!=checks['sha256']:raise SystemExit('BIN differs from the emulated image')
    out=ROOT.parents[1]/'artifacts/firmware/doom'/('v'+version);out.mkdir(parents=True,exist_ok=True)
    name=f'DMT-99_Doom_V{version}_{a.date}.bin'
    dest=out/name
    if dest.exists() and dest.read_bytes()!=image.read_bytes():raise SystemExit('refusing to replace an existing different release')
    shutil.copyfile(image,dest)
    debug=out/'debug';debug.mkdir(exist_ok=True)
    for suffix in ('elf','map','size.txt'):shutil.copyfile(ROOT/f'build/dmt99-doom.{suffix}',debug/f'dmt99-doom.{suffix}')
    for file in ('image_checks.json','stack.txt','upstream_differential.json','reference_preservation.json','reproducibility.json'):
        shutil.copyfile(ROOT/'build'/file,out/file)
    shutil.copyfile(ROOT/'build/emulator/results.json',out/'emulator_results.json')
    shutil.copyfile(ROOT/'generated/assets.json',out/'assets.json')
    (out/'SHA256SUMS').write_text(f'{checks["sha256"]}  {name}\n')
    info={'project':'POTAT-99 DOOM','version':version,'date':a.date,'file':name,
          'load_address':'0x08006400','execution_alias':'0x6400','vector_alias':'0x6438',
          'flash_end_exclusive':hex(checks['format']['flash_end']),'image_bytes':image.stat().st_size,
          'sha256':checks['sha256'],'static_ram_bytes':checks['ram_static_bytes'],
          'reserved_stack_bytes':checks['stack_reserved_bytes'],'hardware_verified':False,
          'bootloader_acceptance_verified':False,'external_asset_file_needed':False,
          'sound_effects':['step','fire','hurt','kill'],
          'weapons':['fist','pistol','shotgun','chaingun','chainsaw','rocket'],
          'world_tick_ms':100,'timed_ai':True,'enemy_wall_collision':True,'animated_sprites':True,
          'wall_gated_hearing':True,'last_seen_target_timeout_ms':5000,'corpse_display_ms':8000,
          'god_mode_menu':True,'sound_toggle_menu':True,'options_persisted':False,
          'movement_max_units_per_second':160,'turn_max_degrees_per_second':80,
          'controls':{'up_PC11':'ramped_forward','down_PC10':'ramped_back','left_PC12':'ramped_turn_left','right_PC9':'ramped_turn_right','PC8':'fire_on_press_repeat','PC8_plus_PC10_2_seconds':'pause_menu'},
          'writes_internal_or_external_flash_at_runtime':False}
    (out/'release.json').write_text(json.dumps(info,indent=2)+'\n')
    (out/'README_TR.md').write_text(f'''# POTAT-99 DOOM {version}

USB güncelleme diski için tek dosya: `{name}`.
`sha256sum -c SHA256SUMS` ile kontrol edin. `debug/` dosyalarını veya raporları
cihaza kopyalamayın. WAD/paket gerekmez; oyun görselleri BIN içindedir.

Mevcut POWER+MENU USB yöntemi kullanılır. Kaldırmak için aynı yöntemle önceki
çalışan POTAT-99 BIN'ini geri yükleyin. Ayrıntılar: [proje README](../../README.md).

Yukarı PC11 / aşağı PC10: ileri/geri; sol PC12 / sağ PC9: dönme.
PC8: ateş/onay; PC8 + aşağı PC10 birlikte 2 saniye: pause menüsü; PC2: kapatma.
Orta tuş tek başına uzun tutulunca menü açılmaz, silah bekleme süresine göre ateş sürer.
Hareket ve dönüş basılı tutulunca kademeli hızlanır; hız sınırı vardır, bırakınca durur.
Ateş, adım, hasar ve düşman öldürme için farklı kısa buzzer efektleri var.
Menüde WEAPONS ile altı silah, GOD MODE ve SOUND ile hasarsızlık/ses seçilir.
Ayarlar RAM'de kalır; güç döngüsünde god kapalı, ses açık olur. Menüde dünya donar.
Düşmanlar duvar arkasındaki atışla uyanmaz; görüş kaybında yalnız son görülen
konuma en fazla 5 saniye yönelir. Cesetler ölüm animasyonundan 8 saniye sonra silinir.
Düşmanlarda duvar çarpışması ve
yürüme/saldırı/ölüm animasyonları vardır. Harita hâlâ tek yükseklikli raycaster'dır.
Ölçüm modu bu oyun firmware'inde yok.

BIN {image.stat().st_size} bayt, fiziksel 0x08006400, vektör alias 0x6438, 32 KiB RAM sınırı.
Emülatör testleri geçti; fiziksel cihaz ve USB kabulü henüz doğrulanmadı.
Updater'ın gerçek erase davranışı ve kesinti sonrası dönüş garantisi bilinmiyor.
''')
    print(dest)
if __name__=='__main__':main()

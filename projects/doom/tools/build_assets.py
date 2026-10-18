#!/usr/bin/env python3
# SPDX-License-Identifier: AGPL-3.0-or-later
"""Build a small, ROM-only E1M1 subset; never accesses the meter's NOR.

WAD decoding and geometry conventions come from the pinned doom-avr submodule.
The game data retains id Software's separate terms; it is not AGPL code.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.dont_write_bytecode = True
sys.path.insert(0, str(ROOT / 'upstream/doom-avr/host'))
from wad import Wad, load_palette, read_texture_defs, read_pnames, compose_texture, decode_patch
from wad import read_vertexes, read_linedefs, read_sidedefs, read_things
import map_data as m


def array(name, data, ctype='uint8_t'):
    return f'const {ctype} {name}[{len(data)}] = {{\n' + '\n'.join(
        ','.join(str(v) for v in data[i:i+24])+',' for i in range(0, len(data), 24)) + '\n};\n'


def generate(path, out):
    digest=hashlib.sha256(path.read_bytes()).hexdigest()
    if digest!='1d7d43be501e67d927e415e0b8f3e29c3bf33075e859721816f652a526cac771':
        raise ValueError('this E1M1 build requires the pinned DOOM shareware 1.9 IWAD')
    wad = Wad(path)
    verts = read_vertexes(wad, 'E1M1')
    sides = read_sidedefs(wad, 'E1M1')
    walls = [(*verts[l['v1']], *verts[l['v2']], sides[l['side0']]['mid'] or 'STARTAN3')
             for l in read_linedefs(wad, 'E1M1') if l['side1'] is None]
    if walls != m.WALLS:
        raise ValueError('E1M1 geometry/textures differ from the pinned doom-avr map')
    things = read_things(wad, 'E1M1')
    start = next(t for t in things if t['type'] == 1)
    if (start['x'], start['y'], start['angle']) != m.START:
        raise ValueError('player start differs')
    palette = load_palette(wad)
    lookup = {tuple(rgb): i for i, rgb in enumerate(palette)}
    pal565 = [(r >> 3) << 11 | (g >> 2) << 5 | (b >> 3) for r,g,b in palette]
    names = sorted({w[4] for w in walls})
    items = sorted({e[2] for e in m.ITEMS if e[2]})
    enemy_names = [
        ['POSS'+f for f in ['A1','B1','C1','D1','E1','F1','G1','H0','J0','L0']],
        ['SPOS'+f for f in ['A1','B1','C1','D1','E1','F1','G1','H0','J0','L0']],
        ['TROO'+f for f in ['A1','B1','C1','D1','E1','G1','H1','I0','K0','M0']],
        ['SARG'+f for f in ['A1','B1','C1','D1','E1','G1','H1','I0','K0','N0']],
    ]
    weapon_names=[['PUNGA0','PUNGB0','PUNGC0','PUNGA0'],
                  ['PISGA0','PISGB0','PISGC0','PISGA0'],
                  ['SHTGA0','SHTGB0','SHTGC0','SHTGD0'],
                  ['CHGGA0','CHGGB0','CHGGA0','CHGGB0'],
                  ['SAWGA0','SAWGB0','SAWGC0','SAWGD0'],
                  ['MISGA0','MISGB0','MISGA0','MISGA0']]
    projectile_names=['MISLA1','BAL1A0','BAL1B0']
    sprites=sorted(set(items+sum(enemy_names,[])+sum(weapon_names,[])+projectile_names))
    if len(sprites)>255: raise ValueError('sprite index overflow')
    pnames, defs = read_pnames(wad), read_texture_defs(wad)
    pixels, masks = [], []
    for name in names:
        w,h,img = compose_texture(wad, defs[name], pnames, palette)
        for y in range(32):
            for x in range(32):
                pixels.append(lookup[tuple(img[y*h//32][x*w//32][:3])])
    sprite_data=[]; widths=[]; heights=[]
    for name in sprites:
        w,h,_,_,img = decode_patch(wad.read(name), palette)
        widths.append(w);heights.append(h)
        bits=[]
        for y in range(40):
            for x in range(32):
                rgba=img[y*h//40][x*w//32]
                sprite_data.append(lookup[tuple(rgba[:3])]);bits.append(bool(rgba[3]))
        masks.extend(sum(int(v)<<j for j,v in enumerate(bits[i:i+8])) for i in range(0,len(bits),8))
    def ids(seq):return [sprites.index(n) for n in seq]
    # Preserve actual pickup proportions, with a deliberately small world scale.
    item_heights=[min(24,max(8,heights[sprites.index(e[2])]//2)) if e[2] else 12 for e in m.ITEMS]
    classes={'POSS':0,'SPOS':1,'TROO':2,'SARG':3}
    enemy_classes=[classes[e[2][:4]] for e in m.ENEMIES]
    enemy_things=[t for t in things if t['type'] in (3001,3002,3004,9,58)]
    assert [(t['x'],t['y']) for t in enemy_things]==[e[:2] for e in m.ENEMIES]
    item_things=[t for t in things if t['type']!=2035 and (2000<=t['type']<=2999 or t['type'] in (5,6,13,8))]
    assert [(t['x'],t['y']) for t in item_things]==[e[:2] for e in m.ITEMS]
    healing={2011:10,2012:25,2013:100,2014:1}
    # Keep a genuine DOOM logo, reduced to 128x40 for the local menu.
    w,h,_,_,img = decode_patch(wad.read('M_DOOM'), palette)
    logo = [lookup[tuple(img[y*h//40][x*w//128][:3])] if img[y*h//40][x*w//128][3] else 0
            for y in range(40) for x in range(128)]
    header = f'''/* Generated from the user's WAD. Game data: id Software, separate terms. */
#ifndef DOOM_ASSETS_H
#define DOOM_ASSETS_H
#include <stdint.h>
#define TEXTURE_COUNT {len(names)}
#define SPRITE_COUNT {len(sprites)}
#define SPRITE_W 32
#define SPRITE_H 40
#define SPRITE_PIXELS 1280
#define SPRITE_MASK_BYTES 160
extern const uint16_t doom_palette[256];
extern const uint8_t doom_textures[TEXTURE_COUNT*1024];
extern const uint8_t doom_sprites[SPRITE_COUNT*SPRITE_PIXELS], doom_masks[SPRITE_COUNT*SPRITE_MASK_BYTES];
extern const uint8_t doom_logo[128*40];
extern const uint8_t doom_wall_texture[{len(walls)}];
extern const uint8_t doom_enemy_class[{len(m.ENEMIES)}], doom_enemy_facing[{len(m.ENEMIES)}];
extern const uint8_t doom_enemy_frames[40], doom_weapon_frames[24], doom_projectile_frames[3];
extern const uint8_t doom_item_sprite[{len(m.ITEMS)}], doom_item_height[{len(m.ITEMS)}], doom_item_heal[{len(m.ITEMS)}];
extern const uint16_t doom_sprite_width[SPRITE_COUNT], doom_sprite_height[SPRITE_COUNT];
#endif
'''
    source = '#include "assets.h"\n' + array('doom_palette',pal565,'uint16_t')
    source += array('doom_textures',pixels) + array('doom_sprites',sprite_data)
    source += array('doom_masks',masks) + array('doom_logo',logo)
    source += array('doom_wall_texture',[names.index(w[4]) for w in walls])
    source += array('doom_enemy_class',enemy_classes)
    source += array('doom_enemy_facing',[(t['angle']//45)&7 for t in enemy_things])
    source += array('doom_enemy_frames',ids(sum(enemy_names,[])))
    source += array('doom_weapon_frames',ids(sum(weapon_names,[])))
    source += array('doom_projectile_frames',ids(projectile_names))
    source += array('doom_sprite_width',widths,'uint16_t')+array('doom_sprite_height',heights,'uint16_t')
    source += array('doom_item_height',item_heights)
    source += array('doom_item_heal',[healing.get(t['type'],0) for t in item_things])
    source += array('doom_item_sprite',[sprites.index(e[2]) if e[2] else 255 for e in m.ITEMS])
    map_header = (ROOT/'upstream/doom-avr/avr/src/map_data.h').read_text()
    map_header = map_header.replace('#include <avr/pgmspace.h>', '#include <stdint.h>').replace(' PROGMEM', '')
    out.mkdir(parents=True,exist_ok=True)
    (out/'assets.h').write_text(header)
    (out/'assets.c').write_text(source)
    (out/'map_data.h').write_text(map_header)
    summary = {'wad_sha256':digest, 'wad_bytes':path.stat().st_size,
               'map':'E1M1','walls':len(walls),'enemies':len(m.ENEMIES),'items':len(m.ITEMS),
               'textures':names,'sprites':sprites,
               'asset_rom_bytes':len(pixels)+len(sprite_data)+len(masks)+len(logo)+512+len(walls)+len(m.ENEMIES)*2+len(m.ITEMS)*3+40+24+3+len(sprites)*4,
               'generated_sha256':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in out.glob('*') if p.suffix in ('.c','.h')}}
    (out/'assets.json').write_text(json.dumps(summary,indent=2)+'\n')
    print(json.dumps({k:v for k,v in summary.items() if k not in ('textures','sprites','generated_sha256')}))


if __name__ == '__main__':
    ap=argparse.ArgumentParser(); ap.add_argument('wad',type=Path); ap.add_argument('--out',type=Path,default=ROOT/'generated')
    args=ap.parse_args(); generate(args.wad,args.out)

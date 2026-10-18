#!/usr/bin/env python3
"""Extract only newly used UI ROM ranges; reuse the existing numeric glyph bank."""
import argparse,hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def main():
 parser=argparse.ArgumentParser();parser.add_argument('--write',action='store_true');args=parser.parse_args()
 image=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes();rows=[]
 text='#include "gauge.h"\n/* Exact original ROM; see analysis/scene_assets.json. Numeric glyphs are reused. */\n'
 for name,start,end in [('dmt_scene_prefix',0x247c0,0x254c8),('dmt_scene_suffix',0x26338,0x27194)]:
  data=image[start-0x6400:end-0x6400];text+=f'const uint8_t {name}[0x{end-start:x}]={{\n'
  text+='\n'.join(' '+','.join(f'0x{x:02x}' for x in data[i:i+24])+',' for i in range(0,len(data),24))+'\n};\n'
  rows.append({'symbol':name,'start':hex(start),'end_exclusive':hex(end),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()})
 report=json.dumps({'firmware_sha256':hashlib.sha256(image).hexdigest(),'ranges':rows,'reused':{'source':'reconstructed/ui/bitmap_images.c','start':'0x254c8','end_exclusive':'0x26338','bytes':3696}},indent=2)+'\n'
 for path,content in [('reconstructed/ui/scene_rom.c',text),('analysis/scene_assets.json',report)]:
  p=ROOT/path
  if args.write:p.write_text(content)
  else:assert p.read_text()==content,path
 print('Scene ROM bytes verified' if not args.write else 'Scene ROM fragments extracted')
if __name__=='__main__':main()

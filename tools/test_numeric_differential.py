#!/usr/bin/env python3
import hashlib
import ctypes as C,json,random
from pathlib import Path
from rv32_trace import TraceMachine
ROOT=Path(__file__).resolve().parents[1]
PIX=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_uint16,C.c_uint16)
IMG=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_char_p,C.c_uint16,C.c_uint16)
class Canvas(C.Structure):_fields_=[('ctx',C.c_void_p),('pixel',PIX),('width',C.c_uint16),('height',C.c_uint16),('fill',C.c_void_p)]
class View(C.Structure):_fields_=[('ctx',C.c_void_p),('image',IMG),('delay',C.c_void_p),('canvas',C.POINTER(Canvas))]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));lib.numeric_jpeg_text.argtypes=[C.POINTER(View),C.c_char_p,C.c_uint8,C.c_uint8,C.c_uint8]
 rng=random.Random(109004);cases=0
 for view in [1,2,3]:
  for text in ['', ' 12.34','-0.12',' 0.L','OATUEL','/.0',*[''.join(rng.choice('0123456789.- OATUEL/') for _ in range(7)) for i in range(12)]]:
   for mode,unit in [(0x12,21),(0x11,8),(0x16,8),(0xc,6)]:
    expected=[];actual=[];m=TraceMachine()
    m.write(0x200001de,1,view);m.write(0x20000206,1,mode);m.write(0x20000289,1,unit)
    m.ram[0x6000:0x6000+len(text)+1]=text.encode()+b'\0'
    def image(m,p,x,y,*_):expected.append(('image',m.string(p),x&65535,y&65535));return 0
    def fill(m,x1,y1,x2,y2,c,*_):
     for y in range(y1,y2+1):
      for x in range(x1,x2+1):expected.append(('pixel',x,y,c))
     return 0
    # graphics_fill's valid original contract uses inclusive right/bottom endpoints.
    m.hooks[0x678c]=image;m.hooks[0x8a02]=fill
    m.call(0x109e4,0x20006000,limit=100000)
    @PIX
    def pixel(_,x,y,c):actual.append(('pixel',x,y,c))
    @IMG
    def img(_,p,x,y):actual.append(('image',p.decode(),x,y));return True
    canvas=Canvas(None,pixel,65535,65535);v=View(None,img,None,C.pointer(canvas))
    lib.numeric_jpeg_text(C.byref(v),text.encode(),view,mode,unit)
    assert actual==expected,(view,text,mode,unit,actual[:5],expected[:5],len(actual),len(expected));cases+=1
 (ROOT/'analysis/numeric_differential.json').write_text(json.dumps({'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'entry':'0x000109e4','cases':cases,'scope':'Ordered resource filenames/coordinates and fill pixels versus original instructions, resource decoder and rectangle primitive isolated.'},indent=2)+'\n')
 print(cases,'numeric renderer cases passed')
if __name__=='__main__':main()

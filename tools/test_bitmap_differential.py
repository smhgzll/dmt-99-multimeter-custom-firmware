#!/usr/bin/env python3
import hashlib
import ctypes as C,json
from test_numeric_differential import PIX,IMG,Canvas,View,ROOT
from rv32_trace import TraceMachine

def expand(data,x,y,fg,bg):
 width=int.from_bytes(data[2:4],'little');height=int.from_bytes(data[4:6],'little');stride=(width+7)//8
 return [('pixel',(x+c)&65535,(y+r)&65535,fg if data[6+r*stride+c//8]&(128>>(c&7)) else bg) for r in range(height) for c in range(width)]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));lib.numeric_bitmap_text.argtypes=[C.POINTER(View),C.c_char_p,C.c_uint8,C.c_uint8,C.c_uint8];lib.graphics_bitmap.argtypes=[C.POINTER(Canvas),C.c_uint16,C.c_uint16,C.c_void_p,C.c_size_t,C.c_uint16,C.c_uint16];lib.graphics_bitmap.restype=None
 glyphs=C.string_at(C.addressof((C.c_ubyte*(14*264)).in_dll(lib,'numeric_bitmap_glyphs')),14*264);rom=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes();assert glyphs==rom[0x254c8-0x6400:0x26338-0x6400]
 count=0
 for b in [glyphs[i*264:(i+1)*264] for i in range(14)]+[bytes([0,1,w,0,3,0])+bytes([0xa5])*(((w+7)//8)*3) for w in [0,1,7,8,9,15,16,17,31,38]]:
  m=TraceMachine();m.ram[0x6000:0x6000+len(b)]=b;expected=[];actual=[]
  m.hooks[0x7f00]=lambda m,x,y,c,*_:expected.append(('pixel',x&65535,y&65535,c&65535)) or 0
  m.call(0x1a360,0x20006000,3,4,0xfd40,0,limit=1000000)
  @PIX
  def pixel(_,x,y,c):actual.append(('pixel',x,y,c))
  canvas=Canvas(None,pixel,65535,65535);lib.graphics_bitmap(C.byref(canvas),3,4,b,len(b),0xfd40,0);assert actual==expected;count+=1
 texts=['',' 12.34','-0.12',' 0.L','OATUEL','/.0','1234567','-9.9',' 99',' A',' E',' 0',' 1.2','0.0.0','/---','L.T','UUU',' 9E']
 for view in [1,2,3]:
  for text in texts:
   for mode,unit in [(0x12,21),(0x11,8),(0x16,8),(0xc,6)]:
    m=TraceMachine();m.write(0x200001de,1,view);m.write(0x20000206,1,mode);m.write(0x20000289,1,unit);m.ram[0x6000:0x6000+len(text)+1]=text.encode()+b'\0';expected=[];actual=[]
    def bitmap(m,p,x,y,fg,bg,*_):expected.extend(expand(m.image[p-0x6400:p-0x6400+264],x,y,fg,bg));return 0
    def fill(m,x,y,x1,y1,c,*_):expected.extend(('pixel',xx,yy,c) for yy in range(y,y1+1) for xx in range(x,x1+1));return 0
    m.hooks[0x1a360]=bitmap;m.hooks[0x8a02]=fill;m.hooks[0x678c]=lambda m,p,x,y,*_:expected.append(('image',m.string(p),x,y)) or 0
    m.call(0x10bd0,0x20006000,limit=1000000)
    @PIX
    def pixel(_,x,y,c):actual.append(('pixel',x,y,c))
    @IMG
    def img(_,p,x,y):actual.append(('image',p.decode(),x,y));return True
    canvas=Canvas(None,pixel,65535,65535);v=View(None,img,None,C.pointer(canvas));lib.numeric_bitmap_text(C.byref(v),text.encode(),view,mode,unit)
    assert actual==expected,(view,text,mode,unit,len(actual),len(expected));count+=1
 (ROOT/'analysis/bitmap_differential.json').write_text(json.dumps({'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'entries':['0x00010bd0','0x0001a360'],'cases':count,'scope':'Original packed bitmap pixels, alternate numeric JPEG/bitmap calls and clears; exact3696-byte internal glyph bank. Valid NUL-terminated strings and bitmap dimensions.'},indent=2)+'\n');print(count,'bitmap renderer cases passed')
if __name__=='__main__':main()

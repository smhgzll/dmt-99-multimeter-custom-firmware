#!/usr/bin/env python3
import ctypes as C,json,random,struct,hashlib
from rv32_trace import TraceMachine
from test_numeric_differential import Canvas,PIX,ROOT
class Graph(C.Structure):_fields_=[('leading',C.c_float),('samples',C.c_float*196),('index',C.c_uint16),('count',C.c_uint16),('strip',C.c_uint8),('scale',C.c_double),('pixels',C.c_uint16*3920)]
def bits(f):return struct.unpack('<I',struct.pack('<f',f))[0]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));lib.graph_strip_pixel.argtypes=[C.POINTER(Graph),C.c_uint32,C.c_uint32,C.c_uint16];lib.graph_strip_line.argtypes=[C.POINTER(Graph)]+[C.c_uint16]*5;lib.graph_update.argtypes=[C.POINTER(Graph),C.c_float,C.c_uint8,C.c_uint8,C.c_uint32,C.c_uint32,C.POINTER(Canvas),C.c_void_p]
 rng=random.Random(174);count=0
 for strip in range(11):
  for x,y in [(0,0),(195,strip*20),(196,strip*20),(5,strip*20+19),(5,strip*20+20),(5,65536+strip*20)]:
   m=TraceMachine();m.write(0x20000254,1,strip);g=Graph();g.strip=strip
   m.call(0x17ab2,x,y,0xbeef);lib.graph_strip_pixel(C.byref(g),x,y,0xbeef)
   assert bytes(g.pixels)==bytes(m.ram[0x3634:0x3634+7840]),(strip,x,y);count+=1
 for _ in range(32):
  m=TraceMachine();g=Graph();g.strip=rng.randrange(8);m.write(0x20000254,1,g.strip);args=[rng.randrange(210),rng.randrange(180),rng.randrange(210),rng.randrange(180),0xffff]
  m.call(0x17c06,*args,limit=200000);lib.graph_strip_line(C.byref(g),*args);assert bytes(g.pixels)==bytes(m.ram[0x3634:0x3634+7840]);count+=1
 # Execute original17c8c itself, isolating only the display window/data wire.
 lib.graph_strip_blit.argtypes=[C.POINTER(Graph),C.POINTER(Canvas),C.c_void_p,C.c_uint16,C.c_uint16]
 for x,y in [(40,55),(40,195),(0,0)]:
  m=TraceMachine();g=Graph();g.pixels[:]=[(i*53)&65535 for i in range(3920)];m.ram[0x3634:0x3634+7840]=bytes(g.pixels);stream=[];windows=[];native=[]
  m.hooks[0x86f6]=lambda m,*p:windows.append(tuple(p[:4])) or 0
  m.hooks[0x7e20]=lambda *_:0
  m.hooks[0x7dba]=lambda m,value,*_:stream.append(value&65535) or 0
  m.call(0x17c8c,x,y,limit=200000)
  @PIX
  def pixel(_,xx,yy,c):native.append((xx,yy,c))
  canvas=Canvas(None,pixel,320,240);lib.graph_strip_blit(C.byref(g),C.byref(canvas),None,x,y)
  assert windows==[(x,y,196,20)]
  assert native==[(x+col,y+row,stream[row*196+col]) for row in range(20) for col in range(196)];count+=1
 # Full graph includes original soft-float and line code; only window blits isolated.
 for index,n,reset,mode,unit in [(0,0,1,0xc,6),(1,1,0,0xb,12),(196,195,0,0x11,8),(0,196,0,0xf,20)]:
  m=TraceMachine();g=Graph();g.index=index;g.count=n;values=[float((i*7)%37-18) for i in range(196)];g.samples[:]=values
  m.ram[0x3324:0x3324+784]=struct.pack('<196f',*values);m.write(0x20000256,2,index);m.write(0x20000260,2,n);m.write(0x20000206,1,mode);m.write(0x20000289,1,unit);m.write(0x20000284,4,bits(12.5));m.r[21]=bits(-20.)
  original=[];native=[]
  def blit(m,x,y,*_):
   raw=bytes(m.ram[0x3634:0x3634+7840]);data=struct.unpack('<3920H',raw)
   original.extend((x+col,y+row,data[col*20+row]) for row in range(20) for col in range(196));return 0
  def clear(m,x,y,x1,y1,c,*_):original.extend((xx,yy,c) for yy in range(y,y1+1) for xx in range(x,x1+1));return 0
  m.hooks[0x17c8c]=blit;m.hooks[0x8a02]=clear
  m.call(0x17cce,0,reset,limit=15000000)
  @PIX
  def pixel(_,x,y,c):native.append((x,y,c))
  canvas=Canvas(None,pixel,320,240);lib.graph_update(C.byref(g),12.5,mode,unit,reset,bits(-20.),C.byref(canvas),None)
  assert original==native,(index,n,len(original),len(native),next(((i,a,b) for i,(a,b) in enumerate(zip(original,native)) if a!=b),None))
  assert bytes(g.samples)==bytes(m.ram[0x3324:0x3324+784]);assert (g.index,g.count)==(m.read(0x20000256,2),m.read(0x20000260,2));count+=1;print('graph',index,n,'pass',flush=True)
 (ROOT/'analysis/graph_differential.json').write_text(json.dumps({'cases':count,'entries':['0x00017ab2','0x00017c06','0x00017c8c','0x00017cce'],'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'Strip pixel/line and full original graph arithmetic/buffer/state against C; blit callback isolated. Inherited s5 explicitly injected. Reset/index underflow modeled as separate leading slot.'},indent=2)+'\n')
if __name__=='__main__':main()

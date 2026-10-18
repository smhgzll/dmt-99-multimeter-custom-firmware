#!/usr/bin/env python3
"""Bounded pixel/format/alarm/history comparisons to original V2.4 entries."""
import ctypes as C, hashlib, json, struct
from rv32_trace import TraceMachine, ROOT
from test_numeric_differential import PIX, Canvas, View
from test_reading_differential import Render, TRACE
from test_ui_differential import Trace

def bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
class Record(C.Structure):_fields_=[('meta',C.c_uint16),('value',C.c_uint16)]
class History(C.Structure):_fields_=[('count',C.c_uint16),('records',Record*30)]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));counts={}
 def done(a):counts[f'0x{a:08x}']=counts.get(f'0x{a:08x}',0)+1
 # Original soft-float and formatter execute; pixel IO alone is intercepted.
 for pc,name in [(0x10198,'gauge_lower'),(0x10404,'gauge_upper'),(0x1066c,'gauge_legacy')]:
  fn=getattr(lib,name);fn.argtypes=[C.POINTER(Render),C.c_uint32]
  for n in ([0,1,2,3,4,5,6,255] if pc!=0x1066c else [0,1,2,10,20,39,40,49,50,60,79,80,89,90,98,99,100,255]):
   m=TraceMachine();actual=[];expected=[]
   m.hooks[0x7f00]=lambda m,x,y,c,*_:expected.append((x&65535,y&65535,c&65535)) or 0
   m.write(0x20000270,2,65535);m.write(0x20000272,2,65535)
   m.call(pc,n,limit=3000000)
   @PIX
   def pixel(_,x,y,c):actual.append((x,y,c))
   canvas=Canvas(None,pixel,65535,65535);status=View(None,View._fields_[1][1](),None,C.pointer(canvas))
   v=Render(C.cast(C.pointer(status),C.c_void_p),None,TRACE(),None);fn(C.byref(v),n)
   assert actual==expected,(name,n,len(actual),len(expected),next(((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),None));done(pc)
 print('gauges',sum(counts.values()),flush=True)
 # Opaque bitmap backgrounds are preserved only for set bits in this gauge.
 lib.gauge_bitmap.argtypes=[C.POINTER(Render),C.c_void_p,C.c_size_t,C.c_uint32]+[C.c_uint16]*4+[C.c_float]
 rom=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes()
 for a in [0x263ec,0x269a4]:
  p=rom[a-0x6400:];w,h=struct.unpack_from('<HH',p,2);p=p[:6+((w+7)//8)*h]
  for value in [0.,1.,9.9,50.,99.,100.]:
   m=TraceMachine();actual=[];expected=[];m.hooks[0x7f00]=lambda m,x,y,c,*_:expected.append((x&65535,y&65535,c&65535)) or 0
   m.call(0x1a468,a,12,23,0x1c9f,0x4228,bits(value),limit=3000000)
   @PIX
   def pixel(_,x,y,c):actual.append((x,y,c))
   canvas=Canvas(None,pixel,65535,65535);status=View(None,View._fields_[1][1](),None,C.pointer(canvas));v=Render(C.cast(C.pointer(status),C.c_void_p),None,TRACE(),None)
   lib.gauge_bitmap(C.byref(v),p,len(p),a,12,23,0x1c9f,0x4228,value)
   assert actual==expected,('gauge-bitmap',a,value,len(actual),len(expected),next(((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),None));done(0x1a468)
 lib.reading_reset_extrema.argtypes=[C.c_void_p]
 from test_reading_differential import Reading
 m=TraceMachine();r=Reading();r.maximum=17;r.minimum=-20;m.call(0x96c0);lib.reading_reset_extrema(C.byref(r));assert bits(r.maximum)==m.read(0x200001b0,4) and bits(r.minimum)==m.read(0x200001b4,4);done(0x96c0)
 lib.extrema_format.argtypes=[C.c_void_p,C.c_size_t,C.c_float]
 for x in [-9999.,-1000.,-999.95,-100.,-99.995,-10.,-9.9995,-0.,0.,9.999,10.,99.99,100.,999.9,1000.,9999.]:
  m=TraceMachine();m.call(0x13504,bits(x),0x20006000,limit=1000000);out=C.create_string_buffer(32);lib.extrema_format(out,32,x);assert out.value==m.string(0x20006000).encode();done(0x13504)
 lib.monitor_threshold_render.argtypes=[C.POINTER(Render),C.c_void_p,C.c_uint8,C.c_uint8,C.c_bool,C.c_bool]
 lib.monitor_alarm_render.argtypes=[C.POINTER(Render),C.c_void_p,C.c_uint8,C.c_float,C.c_bool,C.c_uint8,C.POINTER(C.c_uint8)]
 BUZZ=C.CFUNCTYPE(None,C.c_void_p,C.c_bool)
 words=(C.c_uint16*6)(0x8005,220,0x8064,100,0x800a,100)
 for layout in [0,1]:
  for en in [0,1]:
   for f in [0,1]:
    for mode in [11,12,13,16,18,21]:
     m=TraceMachine();m.ram[0x32:0x3e]=bytes(words);m.write(0x20000205,1,layout);m.write(0x2000003e,2,en|(32 if f else 0));trace=Trace(m)
     m.call(0x12d5a,mode,limit=1000000);lib.monitor_threshold_render(C.byref(trace.view),words,mode,layout,en,f);trace.check(('monitor-threshold',layout,en,f,mode));done(0x12d5a)
  for f in [0,1]:
   for mode in [11,12,13,16,18,21]:
    for value in [-20.,-5.,0.,1.,100.,220.,300.]:
     for old in [0,1,2,255]:
      m=TraceMachine();m.ram[0x32:0x3e]=bytes(words);m.write(0x20000205,1,layout);m.write(0x2000003e,2,32 if f else 0);m.write(0x20000206,1,mode);m.write(0x20000284,4,bits(value));m.write(0x200001db,1,old)
      trace=Trace(m);expected_b=[];actual_b=[];m.hooks[0x78d0]=lambda m,on,*_:expected_b.append(bool(on)) or 0
      @BUZZ
      def buzz(_,on):actual_b.append(on)
      trace.view.buzzer=C.cast(buzz,C.c_void_p);alarm=C.c_uint8(old);m.call(0x13264,limit=2000000)
      lib.monitor_alarm_render(C.byref(trace.view),words,mode,value,f,layout,C.byref(alarm));trace.check(('alarm',mode,value,old,f,layout));assert alarm.value==m.read(0x200001db,1) and actual_b==expected_b;done(0x13264)
 print('monitor',sum(counts.values()),flush=True)
 lib.compact_unit_draw.argtypes=[C.POINTER(Render),C.c_uint8,C.c_uint16,C.c_uint16,C.c_bool]
 for unit in range(256):
  for f in [0,1]:
   m=TraceMachine();m.write(0x2000003e,2,32 if f else 0);trace=Trace(m);m.call(0x12b00,unit,93,0xfd40,limit=1000000)
   lib.compact_unit_draw(C.byref(trace.view),unit,93,0xfd40,f);trace.check(('compact-unit',unit,f));done(0x12b00)
 lib.history_page_render.argtypes=[C.POINTER(Render),C.POINTER(History)]+[C.c_uint8]*3+[C.c_bool]*3
 modes=[10,11,12,13,14,15,16,17,18,21,22]
 for layout in [0,1]:
  for en in [0,1]:
   for f in [0,1]:
    for mode in modes:
     for unit in range(6,22):
      m=TraceMachine();m.write(0x2000003e,2,en|(32 if f else 0));m.write(0x20000205,1,layout);h=History();h.count=30
      for i in range(30):h.records[i]=Record(mode|(unit<<6)|((i%4)<<13)|(0x1000 if i&1 else 0),[1234,61166,65535,0,12345][i%5])
      m.ram[0x40:0x40+C.sizeof(h)]=bytes(h);trace=Trace(m);m.hooks[0x14408]=lambda *_:0
      m.call(0x14578,1,5,1,limit=3000000);lib.history_page_render(C.byref(trace.view),C.byref(h),1,5,layout,en,f,False);trace.check(('history',layout,en,f,mode,unit));done(0x14578)
 report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'Original arithmetic/formatter, gauge pixel streams, monitor threshold/alarm contracts and packed-history labels. Resource and text primitives are isolated in contract cases. History indices are bounded to valid records.'}
 (ROOT/'analysis/render_differential.json').write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'render cases passed')
if __name__=='__main__':main()

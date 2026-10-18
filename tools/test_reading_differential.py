#!/usr/bin/env python3
import ctypes as C,json,struct,hashlib
from test_application_differential import Control,FIELDS
from test_graph_differential import Graph,bits
from rv32_trace import TraceMachine,ROOT
TRACE=C.CFUNCTYPE(None,C.c_void_p,C.c_uint32,C.POINTER(C.c_uint32),C.c_char_p)
class Render(C.Structure):_fields_=[('status',C.c_void_p),('ctx',C.c_void_p),('trace',TRACE),('buzzer',C.c_void_p),('monitor',C.c_void_p)]
class Reading(C.Structure):_fields_=[('maximum',C.c_float),('minimum',C.c_float),('maximum_unit',C.c_int32),('minimum_unit',C.c_int32),('previous_mode',C.c_uint8),('guard',C.c_uint8),('scratch',C.c_char*4),('graph',Graph)]
def normalize(a,p,s):
 if a==0x85cc:return (a,tuple(p[:6]),s)
 if a in [0x109e4,0x10bd0]:return (a,s)
 if a==0x8a02:return(a,tuple(x&65535 for x in p[:5]))
 if a==0x1066c:return(a,p[0]&255)
 if a==0x1a468:return(a,tuple(p[:6]))
 if a==0x17cce:return(a,p[1]==1,p[2])
 return(a,)
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));lib.reading_render.argtypes=[C.POINTER(Reading),C.POINTER(Control),C.c_void_p,C.c_uint16,C.POINTER(Render)]
 count=0
 for view in [1,2,3]:
  for mode,unit in [(0xc,6),(0xd,7),(0xb,11),(0xb,12),(0xb,13),(0xa,15),(0xa,16),(0xa,17),(0x11,8),(0x16,9),(0xf,18),(0xf,19),(0xf,20),(0xe,14),(0x12,21)]:
   for value in [-12.5,0.,12.5,123.75]:
    for layout in [0,1]:
     s=Control();s.view=view;s.mode=mode;s.unit=unit;s.numeric=1;s.layout=layout;s.value=value;s.relative=(count%3==0);s.relative_value=1.25
     r=Reading();r.minimum=100;r.maximum=-100;r.maximum_unit=7;r.minimum_unit=20;r.previous_mode=mode if count%2 else 0
     m=TraceMachine();frame=bytes([0x5a,0xa5,0,0,0,0,0,0,0,0,0,0,0]);m.ram[0x2ac:0x2ac+13]=frame
     for n,a in FIELDS.items():m.write(0x20000000+a,1,getattr(s,n))
     m.write(0x20000284,4,bits(value));m.write(0x200001f8,4,bits(1.25));settings=32 if count%4 else 0;m.write(0x2000003e,2,settings)
     for a,val in [(0x1b0,bits(r.maximum)),(0x1b4,bits(r.minimum)),(0x1bc,r.maximum_unit),(0x1c0,r.minimum_unit)]:m.write(0x20000000+a,4,val)
     m.write(0x200001b8,1,r.previous_mode);expected=[];actual=[]
     def hook(address):
      def call(m,*p):
       p=list(p);text=None
       if address==0x85cc:text=m.string(p[6]).encode()
       if address in [0x109e4,0x10bd0]:text=m.string(0x2000028c).encode()
       if address==0x17cce:p[2]=m.r[21]
       expected.append(normalize(address,p,text));return 0
      return call
     for a in [0x85cc,0x8a02,0x1066c,0x109e4,0x10bd0,0x13264,0x17cce,0x1a468]:m.hooks[a]=hook(a)
     m.call(0x9caa,limit=2000000)
     @TRACE
     def trace(_,a,p,text):actual.append(normalize(a,list(p[:8]),text))
     v=Render(None,None,trace,None);lib.reading_render(C.byref(r),C.byref(s),frame,settings,C.byref(v))
     assert actual==expected,(view,hex(mode),unit,value,layout,actual,expected)
     assert bytes(s.text)==m.string(0x2000028c).encode(),('text',s.text,m.string(0x2000028c))
     assert (bits(r.minimum),bits(r.maximum),r.minimum_unit,r.maximum_unit,r.previous_mode)==(m.read(0x200001b4,4),m.read(0x200001b0,4),C.c_int32(m.read(0x200001c0,4)).value,C.c_int32(m.read(0x200001bc,4)).value,m.read(0x200001b8,1)),(view,hex(mode),value,layout,bits(r.minimum),bits(r.maximum),r.minimum_unit,r.maximum_unit,[hex(m.read(a,4)) for a in [0x200001b4,0x200001b0,0x200001bc,0x200001c0]])
     count+=1
 (ROOT/'analysis/reading_differential.json').write_text(json.dumps({'cases':count,'entries':['0x00009caa'],'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'All views/layouts, unit-aware extrema and formatting; graphics and graph leaf contracts isolated. Canonical numeric value injected independently of packed text. Stack scratch inputs zero.'},indent=2)+'\n');print(count,'reading cases passed')
if __name__=='__main__':main()

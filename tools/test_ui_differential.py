#!/usr/bin/env python3
"""Ordered leaf contracts from original instructions, not decompiler evaluation."""
import ctypes as C
import hashlib
import json
from rv32_trace import TraceMachine, ROOT
from test_application_differential import Control
from test_reading_differential import Render, TRACE

LEAVES={0x678c:3,0x8a02:5,0x8050:5,0x8194:5,0x7f00:3,0x8266:6,0x85cc:7,0x8588:7,0x8e46:7,0x8b5c:4,0x8c64:4,0x80d4:6,0x81ce:6,0x83d6:7,0x1a360:5,0x14408:1,0x9caa:0,0x14e48:1,0x14d62:1}
TEXT={0x678c:0,0x85cc:6,0x8588:2,0x8e46:2,0x8b5c:2,0x8c64:2}
def normalized(a,p,text):
 p=list(p[:LEAVES[a]])
 if a in TEXT:p[TEXT[a]]=0
 # Image names and character strings are bytes, including original GBK.
 return a,tuple(p),text
class Trace:
 def __init__(self,m):
  self.expected=[];self.actual=[]
  for a in LEAVES:
   def hook(m,*p,a=a):
    t=m.string(p[TEXT[a]]).encode('latin1') if a in TEXT else None
    self.expected.append(normalized(a,p,t));return 0
   m.hooks[a]=hook
  @TRACE
  def native(_,a,p,t):self.actual.append(normalized(a,p,t))
  self.callback=native;self.view=Render(None,None,native,None)
 def check(self,context):
  if self.actual!=self.expected:
   mismatch=next(((i,a,b) for i,(a,b) in enumerate(zip(self.actual,self.expected)) if a!=b),None)
   raise AssertionError((context,len(self.actual),len(self.expected),mismatch,self.actual[-2:],self.expected[-2:]))
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));counts={}
 def done(pc):counts[f'0x{pc:08x}']=counts.get(f'0x{pc:08x}',0)+1
 for pc,name,compact in [(0x18ad2,'menu_category_compact',True),(0x19366,'menu_category_large',False),(0x19ba8,'menu_choice_compact',True),(0x19954,'menu_choice_large',False)]:
  function=getattr(lib,name);function.argtypes=[C.POINTER(Render)]+([C.c_bool] if compact else [])+[C.c_int]*3
  for en in ([0,1] if compact else [0]):
   for initial in [0,1]:
    for selected in range(1,7 if 'category' in name else 4):
     for previous in [0,1,3,6 if 'category' in name else 2]:
      m=TraceMachine();m.write(0x2000003e,2,en);trace=Trace(m)
      m.call(pc,initial,selected,previous,limit=1000000)
      function(C.byref(trace.view),*([en] if compact else []),initial,selected,previous)
      trace.check((name,en,initial,selected,previous));done(pc)
 print('category/choice',sum(counts.values()),flush=True)
 lib.menu_threshold_hint.argtypes=[C.POINTER(Render),C.c_bool,C.c_bool,C.c_int,C.c_uint]
 for compact,pc in [(0,0x19f20),(1,0x1a124)]:
  for en in [0,1]:
   for selected in [0,1,2]:
    for row in range(1,7):
     m=TraceMachine();m.write(0x2000003e,2,en);trace=Trace(m);m.call(pc,selected,row,limit=1000000)
     lib.menu_threshold_hint(C.byref(trace.view),compact,en,selected,row);trace.check(('threshold-hint',compact,en,selected,row));done(pc)
 lib.menu_threshold_images.argtypes=[C.POINTER(Render),C.c_bool,C.c_int,C.c_uint]
 for en in [0,1]:
  for selected in [0,1,2]:
   for row in range(1,8):
    m=TraceMachine();m.write(0x2000003e,2,en);trace=Trace(m);m.call(0x10e86,selected,row,limit=1000000);lib.menu_threshold_images(C.byref(trace.view),en,selected,row);trace.check(('threshold-images',en,selected,row));done(0x10e86)
 print('threshold hints',sum(counts.values()),flush=True)
 for pc,name,view in [(0x17f92,'units_regular',1),(0x18354,'units_recording',2),(0x18974,'units_monitor',3)]:
  function=getattr(lib,name);function.argtypes=[C.POINTER(Render),C.c_uint8,C.c_uint8,C.c_uint8,C.c_bool,C.POINTER(C.c_uint8)]
  for mode in range(9,23):
   for unit in range(6,23):
    for cache in [0,7,16,22,25]:
     for layout in [0,1]:
      m=TraceMachine();m.write(0x20000206,1,mode);m.write(0x20000289,1,unit);m.write(0x20000205,1,layout);m.write(0x200001da,1,cache);m.write(0x2000003e,2,32 if cache&1 else 0);m.hooks[0xa7a4]=lambda *_:0
      trace=Trace(m);m.call(pc,limit=1000000);native=C.c_uint8(cache)
      function(C.byref(trace.view),mode,unit,layout,bool(cache&1),C.byref(native));trace.check((name,mode,unit,cache,layout));assert native.value==m.read(0x200001da,1);done(pc)
 print('units',sum(counts.values()),flush=True)
 # Static settings pages execute original nested primitives and helpers.
 for pc,name in [(0x153d0,'settings_scene_theme'),(0x15e9e,'settings_scene_measurement'),(0x16242,'settings_scene_auto_off')]:
  fn=getattr(lib,name);fn.argtypes=[C.POINTER(Render),C.c_uint8,C.c_void_p if pc==0x15e9e else C.c_uint16]
  for layout in [0,1]:
   for word in [0,1,8,16,24,32,64,128,192,1024,1025,0xffff]:
    m=TraceMachine();m.write(0x20000205,1,layout);m.write(0x2000003e,2,word);trace=Trace(m);m.call(pc,limit=1000000);native=C.c_uint16(word)
    fn(C.byref(trace.view),layout,C.byref(native) if pc==0x15e9e else word);trace.check((name,layout,word));assert native.value==m.read(0x2000003e,2);done(pc)
 lib.settings_brightness_image.argtypes=[C.POINTER(Render),C.c_int,C.c_int,C.c_uint,C.POINTER(C.c_uint16)]
 lib.settings_brightness_bar.argtypes=[C.POINTER(Render),C.c_uint,C.POINTER(C.c_uint16)]
 lib.settings_reset_choices.argtypes=[C.POINTER(Render),C.c_int,C.c_bool]
 for level in [0,1,2,3,255]:
  m=TraceMachine();trace=Trace(m);word=C.c_uint16(0x118);m.write(0x2000003e,2,word.value);m.call(0x1432e,level);lib.settings_brightness_bar(C.byref(trace.view),level,C.byref(word));trace.check(('brightness-bar',level));assert word.value==m.read(0x2000003e,2);done(0x1432e)
  for selected in [0,1,2]:
   for page in [1,2,3]:
    m=TraceMachine();trace=Trace(m);word=C.c_uint16(0x118);m.write(0x2000003e,2,word.value);m.call(0x142a4,selected,page,level);lib.settings_brightness_image(C.byref(trace.view),selected,page,level,C.byref(word));trace.check(('brightness-image',selected,page,level));assert word.value==m.read(0x2000003e,2);done(0x142a4)
 for selected in range(4):
  for en in [0,1]:
   m=TraceMachine();m.write(0x2000003e,2,en);trace=Trace(m);m.call(0x152a8,selected);lib.settings_reset_choices(C.byref(trace.view),selected,en);trace.check(('reset',selected,en));done(0x152a8)
 lib.settings_select_render.argtypes=[C.POINTER(Render),C.c_bool,C.c_int,C.c_int,C.POINTER(C.c_uint16)]
 for classic,pc in [(0,0x13c02),(1,0x135b0)]:
  for page in range(0,5):
   for option in range(0,6):
    for original in [0,1,32,192,1024,1025,0xffff]:
     m=TraceMachine();m.write(0x2000003e,2,original);trace=Trace(m);m.call(pc,page,option,limit=1000000);word=C.c_uint16(original)
     lib.settings_select_render(C.byref(trace.view),classic,page,option,C.byref(word));trace.check(('settings-select',classic,page,option,original));assert word.value==m.read(0x2000003e,2),(classic,page,option,original,word.value,m.read(0x2000003e,2));done(pc)
 print('settings scenes',sum(counts.values()),flush=True)
 report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'Original instruction leaf-call contracts: menu scene selection/erasure, threshold hints and shared unit cache. Common reading dispatcher isolated for unit renderers; return-register residue is outside void adapter contracts.'}
 (ROOT/'analysis/ui_differential.json').write_text(json.dumps(report,indent=2)+'\n')
 print(report['total'],'UI contract cases passed')
if __name__=='__main__':main()

#!/usr/bin/env python3
import ctypes as C,hashlib,json
from rv32_trace import TraceMachine,ROOT
from test_application_differential import Control,FIELDS
from test_reading_differential import Render,TRACE,Reading
from test_ui_differential import Trace
class Screen(C.Structure):
 _fields_=[('view',Render),('reading',Reading),('splash',C.c_bool),('normal',C.c_bool),('seen',C.c_uint8),('unit',C.c_uint8),('auto',C.c_uint8),('history_unit',C.c_uint8),('history_y',C.c_uint16),('ctx',C.c_void_p),('logo',C.c_void_p),('delay',C.c_void_p),('backlight',C.c_void_p),('debug',C.c_void_p)]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));counts={}
 lib.screen_base.argtypes=[C.POINTER(Screen),C.POINTER(Control),C.c_void_p,C.c_uint16,C.c_uint8]
 lib.screen_event.argtypes=[C.POINTER(Screen),C.POINTER(Control),C.c_uint16,C.c_int,C.c_bool]
 # enum order from control.h is checked by script rather than guessed below.
 header=(ROOT/'reconstructed/application/control.h').read_text();enum=header.split('enum dmt_effect {')[1].split('}')[0];effects={name.strip():i for i,name in enumerate(enum.split(','))}
 for view in [1,2,3]:
  for layout in [0,1]:
   for en in [0,1]:
    m=TraceMachine();c=Control();c.view=view;c.layout=layout;c.mode=12
    for name,address in FIELDS.items():m.write(0x20000000+address,1,getattr(c,name))
    m.write(0x2000003e,2,en);trace=Trace(m);m.hooks[0x9caa]=lambda m,*p:trace.expected.append((0x9caa,(),None)) or 0
    # Original samples ADC through a leaf, which is explicitly injected here.
    m.hooks[0xc19e]=lambda *_:87
    # Find battery_percentage_estimate from the preserved function list.
    functions=json.loads((ROOT/'analysis/functions.json').read_text());battery=next(int(f['address'],16) for f in functions if f['name']=='battery_percentage_estimate');m.hooks[battery]=lambda *_:87
    screen=Screen();screen.view=trace.view
    prefix=b'menu/english/' if en else b'menu/chinese/'
    # Application language initialization normally supplies this global.
    m.ram[0x298:0x298+len(prefix)+1]=prefix+b'\0'
    m.call(0x14ef2,limit=1000000)
    lib.screen_base(C.byref(screen),C.byref(c),bytes(13),en,87);trace.check(('base',view,layout,en));counts['0x00014ef2']=counts.get('0x00014ef2',0)+1
 # FD mode/hold graphics, with state effects supplied by separately tested control_event.
 modes={1:list(range(9,23)),2:list(range(10,19))+[21,22],3:[11,12,13,16,18,21]}
 for view,pc in [(1,0xa910),(2,0xaed0),(3,0xb29e)]:
  for layout in [0,1]:
   for en in [0,1]:
    for event in modes[view]+[3]:
     for held in [0,1]:
      m=TraceMachine();c=Control();c.view=view;c.layout=layout;c.mode=12;c.hold=held
      m.write(0x20000205,1,layout);m.write(0x20000206,1,12);m.write(0x2000023e,1,held);m.write(0x2000003e,2,en);m.write(0x200002ae,1,0xfd);m.write(0x200002af,1,event);m.hooks[0x12d5a]=lambda *_:0
      trace=Trace(m);m.stub(0x1aede);m.call(pc,limit=1000000);screen=Screen();screen.view=trace.view
      if event!=3 and held:c.hold=0;lib.screen_event(C.byref(screen),C.byref(c),en,effects['DMT_EFFECT_HOLD_OFF'],False)
      if event==3:c.hold=not held;lib.screen_event(C.byref(screen),C.byref(c),en,effects['DMT_EFFECT_HOLD_ON' if c.hold else 'DMT_EFFECT_HOLD_OFF'],True)
      else:c.mode=event;lib.screen_event(C.byref(screen),C.byref(c),en,effects['DMT_EFFECT_MODE_CHANGED'],False)
      trace.check(('event',view,layout,en,event,held));key=f'0x{pc:08x}';counts[key]=counts.get(key,0)+1
 report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'Base scene leaf-call ordering and normal FD mode/HOLD image contracts. Reading dispatch, ADC and monitor threshold calls are isolated. Live/NCV exit scene ordering is outside these FD cases.'};(ROOT/'analysis/scenes_differential.json').write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'scene cases passed')
if __name__=='__main__':main()

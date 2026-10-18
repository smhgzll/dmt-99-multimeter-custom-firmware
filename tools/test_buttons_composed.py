#!/usr/bin/env python3
"""Original complete button entries vs connected C application + real menu.

The original169d4 body runs inside the button call, with isolated qualified base
and history callees. Native application scheduling uses two public steps. This
compares ordered calls and entry/exit state, not the intervening yield's timing.
"""
import argparse,ctypes as C,hashlib,json,random,struct
from rv32_trace import ROOT,TraceMachine
from test_application_differential import Control,FIELDS,machine,check
from test_ui_differential import LEAVES,TEXT,normalized

LEAVES.update({0x876a:1,0x14ef2:0,0x126d8:0,0x14578:3,0x143e0:0,
               0x78ba:0,0x78d0:1,0x1a64c:1,0x7b08:1,0x12c78:1})
TEXT[0x12c78]=0
TRACE=C.CFUNCTYPE(None,C.c_void_p,C.c_uint32,C.POINTER(C.c_uint32),C.c_void_p)
def nested(m,pc):
    registers=m.r[:];old_pc=m.pc
    try:return m.call(pc,limit=2000000)
    finally:m.r=registers;m.pc=old_pc
def main():
    p=argparse.ArgumentParser();p.add_argument('--probe',default='reconstructed/build/asic_application_probe.so')
    p.add_argument('--report',default='analysis/buttons_composed_differential.json');args=p.parse_args()
    lib=C.CDLL(str(ROOT/args.probe));lib.probe_buttons.argtypes=[C.POINTER(Control),C.c_uint16,C.c_bool,C.c_uint8,TRACE,C.c_void_p,C.POINTER(C.c_float)]
    rng=random.Random(0x8ed4);counts={};packets={}
    for view,pc in [(1,0xb5c8),(2,0xb9e0),(3,0xbc90)]:
      for event in range(9):
       for mode in range(24):
        for layout in [0,1]:
         for language in [0,1]:
          for variant in (range(3) if event==6 else range(2)):
            is_long=bool(variant);menu_view=variant+1 if event==6 else view
            s=Control();s.view=view;s.mode=mode;s.layout=layout;s.event=event;s.menu_last=7
            s.unit=6;s.value=-12.5;s.relative_value=3.25;s.numeric=1;s.text=b'-12.50'
            s.regular_selector[:]=[rng.randrange(6),rng.randrange(5),rng.randrange(5),rng.randrange(13)]
            s.record_selector[:]=[rng.randrange(6),rng.randrange(5),rng.randrange(3)]
            s.monitor_selector=rng.randrange(2);s.monitor_state=1
            s.relative=rng.randrange(2);s.relative_mode=mode if variant else 0;s.relative_unit=6
            word=0x100|(layout<<10)|language|(32 if variant else 0)
            m=machine(s,pc);m.hooks={};expected=[];actual=[]
            m.ram[0x28c:0x28c+16]=bytes(s.text).ljust(16,b'\0');m.write(0x2000003e,2,word)
            m.write(0x200001d8,1,1);m.write(0x200001dd,1,1)
            m.write(0x200001b0,4,int.from_bytes(struct.pack('<f',123.),'little'));m.write(0x200001b4,4,int.from_bytes(struct.pack('<f',-456.),'little'))
            prefix=b'menu/english/' if language else b'menu/chinese/';m.ram[0x298:0x298+len(prefix)+1]=prefix+b'\0'
            for a in LEAVES:
                def hook(machine,*params,a=a):
                    text=machine.string(params[TEXT[a]]).encode('latin1') if a in TEXT else None
                    expected.append(normalized(a,params,text));return int(is_long) if a==0x7b08 else 0
                m.hooks[a]=hook
            def tx(machine,ptr,*params):
                data=bytes(machine.read(ptr+i,1) for i in range(10));expected.append((0x97c4,data))
                key=data.hex();packets.setdefault(key,{'hex':data.hex(' '),'sources':[]})
                context={'entry':f'0x{pc:08x}','view':view,'event':event,'mode':mode,'layout':layout,'language':language,'variant':variant}
                if not any(x['entry']==context['entry'] and x['event']==event for x in packets[key]['sources']):packets[key]['sources'].append(context)
                return 0
            m.hooks[0x97c4]=tx
            def menu(machine,*params):
                original=machine.hooks.pop(0x169d4)
                try:
                    assert nested(machine,0x169d4)==0
                    machine.write(0x200001de,1,menu_view);machine.write(0x200001ee,1,6)
                    assert nested(machine,0x169d4)==1
                finally:machine.hooks[0x169d4]=original
                return 1
            m.hooks[0x169d4]=menu
            @TRACE
            def trace(_,a,params,text):
                if a==0x97c4:actual.append((a,C.string_at(text,10)));return
                actual.append(normalized(a,params,C.string_at(text) if a in TEXT and text else None))
            extrema=(C.c_float*2)(123,-456);m.call(pc,limit=2000000)
            lib.probe_buttons(C.byref(s),word,is_long,menu_view,trace,None,extrema)
            label=(hex(pc),event,mode,layout,language,variant)
            if actual!=expected:
                mismatch=next(((i,x,y) for i,(x,y) in enumerate(zip(actual,expected)) if x!=y),None)
                raise AssertionError((label,len(actual),len(expected),mismatch,actual[-3:],expected[-3:]))
            check(s,m,FIELDS)
            if event==6 and view==2:assert tuple(extrema)==tuple(struct.unpack('<f',m.read(a,4).to_bytes(4,'little'))[0] for a in [0x200001b0,0x200001b4]),label
            if event!=6:assert s.text.decode('latin1')==m.string(0x2000028c),label
            key=f'0x{pc:08x}';counts[key]=counts.get(key,0)+1
      print(hex(pc),counts[f'0x{pc:08x}'],'composed cases passed',flush=True)
    report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),'probe_sha256':hashlib.sha256((ROOT/args.probe).read_bytes()).hexdigest(),
            'packets':list(packets.values()),'scope':'Full button call traces and selector/relative/return-event state, real original169d4 vs native application/menu composition; base/history calls isolated. Native two-step menu scheduling is not a cycle/interrupt equivalence claim.'}
    (ROOT/args.report).write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'composed button comparisons passed')
if __name__=='__main__':main()

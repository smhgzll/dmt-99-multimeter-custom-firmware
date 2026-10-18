#!/usr/bin/env python3
"""Original8ed4 entry/normal/diagnostic contracts and portable ASIC parsing.
No firmware analysis regeneration; the preserved instruction inventory executes.
"""
import argparse,ctypes as C,hashlib,json,random,struct
from rv32_trace import ROOT,TraceMachine
from test_application_differential import FIELDS
V=C.CFUNCTYPE(None,C.c_void_p)
U=C.CFUNCTYPE(None,C.c_void_p,C.c_uint32)
PIN=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_uint16)
PWM=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_uint16,C.c_uint16)
PARSE=C.CFUNCTYPE(None,C.c_void_p,C.c_void_p)
READ=C.CFUNCTYPE(C.c_uint8,C.c_void_p)
TEXT=C.CFUNCTYPE(C.c_void_p,C.c_void_p)
DRAW=C.CFUNCTYPE(None,C.c_void_p,*([C.c_uint16]*4),C.c_uint8,C.c_uint16,C.c_void_p)
UNIT=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_uint16)
TX=C.CFUNCTYPE(None,C.c_void_p,C.c_void_p)
WIRE_TX=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_void_p)
class WireIO(C.Structure):_fields_=[('ctx',C.c_void_p),('transmit',WIRE_TX),('delay',U)]
class Foreground(C.Structure):
    _fields_=[(n,C.c_uint8) for n in ['view','mode','layout','hold','rx','consumed','cache','due','battery']]+[('frame',C.c_uint8*16),('saved',C.c_uint8*16)]
class Entry(C.Structure):
    _fields_=[('ctx',C.c_void_p),('pin',PIN),('pwm',PWM),('delay',U),('diagnostic',V),('load',V),('initial',V)]
class Normal(C.Structure):
    _fields_=[('ctx',C.c_void_p)]+[(n,V) for n in ['b1','b2','b3']]+[('parse',PARSE)]+[(n,V) for n in ['e1','e2','e3','charge']]+[('read',READ),('battery0',V),('battery1',V)]
class Diagnostic(C.Structure):
    _fields_=[('ctx',C.c_void_p),('forward',V),('parse',PARSE),('text',TEXT),('draw',DRAW),('unit',UNIT)]
class Reading(C.Structure):
    _fields_=[(n,C.c_uint8) for n in ['mode','numeric','unit','ac_dc','high']]+[('value',C.c_float),('text',C.c_char*16)]
class Stop(Exception):pass
def until(m,entry,point):
    def stop(machine):
        if machine.pc==point:raise Stop()
    m.instruction_hook=stop
    try:m.call(entry,limit=2000000)
    except Stop:pass
    else:raise AssertionError('Original loop boundary was not reached')
    m.instruction_hook=None
def main():
    p=argparse.ArgumentParser();p.add_argument('--library',default='reconstructed/build/librecovered.so')
    p.add_argument('--report',default='analysis/asic_differential.json');args=p.parse_args()
    path=ROOT/args.library;lib=C.CDLL(str(path));counts={};rng=random.Random(0x7502)
    def done(pc,scope):key=f'0x{pc:08x}:{scope}';counts[key]=counts.get(key,0)+1
    # B56A: preserve full10-byte packets, order and delay. Randomized stack state
    # prevents a missing constructor byte from being hidden by zeroed test RAM.
    lib.measurement_diagnostic_entry.argtypes=[C.c_void_p,TX,U]
    diagnostics=[]
    for case in range(32):
        m=TraceMachine();m.ram[0xf000:]=rng.randbytes(4096);expected=[];actual=[]
        def tx_m(machine,ptr,*a):expected.append(('tx',bytes(machine.read(ptr+i,1) for i in range(10))));return 0
        m.hooks[0x97c4]=tx_m;m.hooks[0x1a64c]=lambda _,ms,*a:expected.append(('delay',ms))
        @TX
        def tx_c(_,ptr):actual.append(('tx',C.string_at(ptr,10)))
        @U
        def delay_c(_,ms):actual.append(('delay',ms))
        m.call(0xb56a);lib.measurement_diagnostic_entry(None,tx_c,delay_c)
        assert actual==expected,(actual,expected);done(0xb56a,'entry')
        if not case:diagnostics=[{'kind':kind,'value':value.hex(' ') if isinstance(value,bytes) else value} for kind,value in expected]
    # Diagnostic accepted-key constructors, independently of debounce timing.
    lib.asic_forward_key.argtypes=[C.POINTER(WireIO),C.c_uint8];lib.asic_forward_key.restype=C.c_bool
    key_packets=[]
    for key in range(1,5):
        m=TraceMachine();m.write(0x200001ee,1,key);m.hooks[0xcb40]=lambda *_:0;m.hooks[0x1a64c]=lambda *_:0
        expected=[];actual=[]
        m.hooks[0x97c4]=lambda machine,ptr,*a:expected.append(bytes(machine.read(ptr+i,1) for i in range(10)))
        @WIRE_TX
        def transmit_c(_,ptr):actual.append(C.string_at(ptr,10));return True
        io=WireIO(None,transmit_c,U())
        m.call(0x97f8);assert lib.asic_forward_key(C.byref(io),key) and actual==expected
        key_packets.append({'event':key,'packet':expected[0].hex(' '),'source':'0x000097f8'})
        done(0x97f8,'accepted_key_packet')
    lib.foreground_enter.argtypes=[C.POINTER(Foreground),C.POINTER(C.c_uint8),C.POINTER(C.c_uint8),C.POINTER(Entry)];lib.foreground_enter.restype=C.c_bool
    for high10 in [0,1]:
      for high11 in [0,1]:
       for view in [0,1,2,3,255]:
        m=TraceMachine();f=Foreground();f.view=1;event=C.c_uint8(7);normal=C.c_uint8(0);expected=[];actual=[]
        m.write(0x200001ee,1,7)
        def pin_m(_,port,mask,*a):expected.append(('pin',port,mask));return high10 if mask==0x400 else high11
        m.hooks[0xcb40]=pin_m
        def load_m(machine,*a):expected.append(('load',));machine.write(0x200001de,1,view);return 0
        m.hooks[0xc248]=load_m
        for pc,label,n in [(0x7f66,'pwm',3),(0x1a64c,'delay',1),(0xb56a,'diagnostic',0),(0x1511a,'initial',0)]:
            def hook(_, *values,label=label,n=n):expected.append((label,*values[:n]));return 0
            m.hooks[pc]=hook
        @PIN
        def pin_c(_,mask):actual.append(('pin',0x40011000,mask));return bool(high10 if mask==0x400 else high11)
        @PWM
        def pwm_c(_,a,b,c):actual.append(('pwm',a,b,c))
        @U
        def delay_c(_,ms):actual.append(('delay',ms))
        @V
        def diagnostic_c(_):actual.append(('diagnostic',))
        @V
        def load_c(_):actual.append(('load',));f.view=view
        @V
        def initial_c(_):actual.append(('initial',))
        io=Entry(None,pin_c,pwm_c,delay_c,diagnostic_c,load_c,initial_c)
        branch=not(high10 or high11);until(m,0x8ed4,0x8f1e if branch else 0x8f9e)
        assert lib.foreground_enter(C.byref(f),C.byref(event),C.byref(normal),C.byref(io))==branch
        assert actual==expected,(high10,high11,view,actual,expected)
        assert (f.view,f.mode,event.value,normal.value)==tuple(m.read(a,1) for a in [0x200001de,0x20000206,0x200001ee,0x20000251])
        done(0x8ed4,'boot_selection')
    lib.foreground_iteration.argtypes=[C.POINTER(Foreground),C.POINTER(Normal)]
    mapping={'view':(0x1de,1),'mode':(0x206,1),'layout':(0x205,1),'hold':(0x23e,1),'rx':(0x1c4,2),'consumed':(0x1c6,2),'cache':(0x1d9,1),'due':(0x1ec,1),'battery':(0x248,1)}
    for view in [0,1,2,3]:
     for rx in [0,1]:
      for consumed in [0,1]:
       for hold in [0,1]:
        for due in [0,1]:
         for layout in [0,1]:
          for mode in [9,12,19]:
            m=TraceMachine();m.stub(0xc248,0x1511a,0x1a64c);m.hooks[0xcb40]=lambda *_:1
            until(m,0x8ed4,0x8f9e)
            f=Foreground(view,mode,layout,hold,rx,consumed,7,due,17)
            f.frame[:]=rng.randbytes(16);f.saved[:]=rng.randbytes(16)
            for name,(a,n) in mapping.items():m.write(0x20000000+a,n,getattr(f,name))
            m.ram[0x2ac:0x2bc]=bytes(f.frame);m.ram[0x2bc:0x2cc]=bytes(f.saved)
            expected=[];actual=[];callbacks=[]
            for pc in [0xb5c8,0xb9e0,0xbc90,0xa910,0xaed0,0xb29e,0x14cd6,0x14e48,0x14d62]:
                def hook(_, *a,pc=pc):expected.append(pc);return 0
                m.hooks[pc]=hook
                def callback(_,pc=pc):actual.append(pc)
                callbacks.append(V(callback))
            def parse_m(machine,*a):expected.append(('parse',bytes(machine.ram[0x2ac:0x2bc])));return 0
            m.hooks[0xbe78]=parse_m;m.hooks[0x75f0]=lambda *_:expected.append(0x75f0) or 83
            @PARSE
            def parse_c(_,ptr):actual.append(('parse',C.string_at(ptr,16)))
            @READ
            def read_c(_):actual.append(0x75f0);return 83
            io=Normal(None,*callbacks[:3],parse_c,*callbacks[3:7],read_c,*callbacks[7:])
            # Execute one iteration, retaining the registers initialized by8ed4.
            started=False
            def stop(machine):
                nonlocal started
                if machine.pc==0x8f9e:
                    if started:raise Stop()
                    started=True
            m.instruction_hook=stop
            try:m.call(0x8f9e,limit=1000000)
            except Stop:pass
            lib.foreground_iteration(C.byref(f),C.byref(io));label=(view,rx,consumed,hold,due,layout,mode)
            assert actual==expected,(label,actual,expected)
            for name,(a,n) in mapping.items():assert getattr(f,name)==m.read(0x20000000+a,n),(label,name)
            assert bytes(f.frame)==bytes(m.ram[0x2ac:0x2bc]);done(0x8ed4,'normal_iteration')
    lib.foreground_diagnostic_iteration.argtypes=[C.POINTER(Foreground),C.POINTER(Diagnostic)];lib.foreground_diagnostic_iteration.restype=C.c_bool
    for rx in [0,1]:
     for consumed in [0,1]:
      for case in range(8):
        m=TraceMachine();m.stub(0x7f66,0x1a64c,0xb56a);m.hooks[0xcb40]=lambda *_:0;until(m,0x8ed4,0x8f1e)
        f=Foreground();f.rx=rx;f.consumed=consumed;f.frame[:]=rng.randbytes(16)
        m.ram[0x2ac:0x2bc]=bytes(f.frame);m.write(0x200001c4,2,rx);m.write(0x200001c6,2,consumed)
        m.ram[0x28c:0x293]=b' 12.34\0';text=C.create_string_buffer(b' 12.34');expected=[];actual=[]
        m.hooks[0x97f8]=lambda *_:expected.append('forward')
        m.hooks[0xbe78]=lambda machine,*a:expected.append(('parse',bytes(machine.ram[0x2ac:0x2bc])))
        m.hooks[0xbe24]=lambda *_:expected.append('text') or 0x2000028c
        m.hooks[0x8672]=lambda machine,*a:expected.append(('draw',*a[:6],machine.string(a[6])))
        m.hooks[0xc11a]=lambda _,*a:expected.append(('unit',*a[:2]))
        @V
        def forward_c(_):actual.append('forward')
        @PARSE
        def parse_c(_,ptr):actual.append(('parse',C.string_at(ptr,16)))
        @TEXT
        def text_c(_):actual.append('text');return C.addressof(text)
        @DRAW
        def draw_c(_,x,y,w,h,size,color,ptr):actual.append(('draw',x,y,w,h,size,color,C.string_at(ptr).decode()))
        @UNIT
        def unit_c(_,x,y):actual.append(('unit',x,y))
        io=Diagnostic(None,forward_c,parse_c,text_c,draw_c,unit_c);started=False
        def stop(machine):
            nonlocal started
            if machine.pc==0x8f1e:
                if started:raise Stop()
                started=True
        m.instruction_hook=stop
        try:m.call(0x8f1e,limit=1000000)
        except Stop:pass
        result=lib.foreground_diagnostic_iteration(C.byref(f),C.byref(io))
        assert actual==expected,(rx,consumed,actual,expected)
        assert result==(rx==0) and f.rx==m.read(0x200001c4,2) and f.consumed==m.read(0x200001c6,2)
        done(0x8ed4,'diagnostic_iteration')
    print('8ed4 branches passed',flush=True)
    lib.asic_reading_update.argtypes=[C.POINTER(Reading),C.c_void_p,C.c_size_t];lib.asic_reading_update.restype=C.c_bool
    for case in range(768):
        raw=bytearray(rng.randbytes(13));raw[:2]=b'\x5a\xa5'
        if case%4==0:raw[2]=0xfd
        if case%17==0:raw[0]=0
        r=Reading();r.mode=case%24;r.numeric=case%2;r.text=b'previous bytes'
        m=TraceMachine();m.ram[0x2ac:0x2b9]=raw;m.ram[0x28c:0x29c]=bytes(r.text).ljust(16,b'\0')
        m.write(0x20000206,1,r.mode);m.write(0x2000027c,1,r.numeric)
        m.write(0x20000289,1,r.unit);m.write(0x20000288,1,r.ac_dc);m.write(0x2000028a,1,r.high);m.write(0x20000284,4,0)
        m.call(0xbe78,limit=2000000);ok=lib.asic_reading_update(C.byref(r),bytes(raw),13)
        assert ok==(raw[0]==0x5a)
        assert (r.numeric,r.unit,r.ac_dc,r.high)==tuple(m.read(a,1) for a in [0x2000027c,0x20000289,0x20000288,0x2000028a]),case
        assert struct.pack('<f',r.value)==m.read(0x20000284,4).to_bytes(4,'little'),case
        assert C.string_at(C.addressof(r)+Reading.text.offset,16)==bytes(m.ram[0x28c:0x29c]),case
        done(0xbe78,'portable_reading')
    report={'cases':counts,'total':sum(counts.values()),'entries':['0x00008ed4','0x0000b56a','0x0000be78','0x000097f8'],'diagnostic_sequence':diagnostics,'accepted_key_packets':key_packets,
            'native_library_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'original_sha256':hashlib.sha256(m.image).hexdigest(),
            'scope':'Original complete B56A packets/delay;8ED4 boot selection and bounded normal/diagnostic iterations with explicit callee hooks; portable BE78 text/numeric/state including stale text bytes and FD parsing. No peer ACK/analog/timing or hardware validation.'}
    (ROOT/args.report).write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'ASIC/foreground comparisons passed')
if __name__=='__main__':main()

#!/usr/bin/env python3
"""Bounded original-entry comparisons for remaining startup/IO glue.

All hardware reads are declared inputs. Hooks compare call order and arguments;
the oracle still rejects unbound memory/MMIO and unsupported instructions.
"""
import argparse
import ctypes as C
import hashlib
import json
from rv32_trace import ROOT, TraceMachine

VOID=C.CFUNCTYPE(None,C.c_void_p)
WORD=C.CFUNCTYPE(None,C.c_void_p,C.c_uint32)
PAIR=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_uint16)
PIN=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_uint16)
DELAY16=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16)
TX=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint8))
SET=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_bool)
DATA=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16)
BOOL=C.CFUNCTYPE(None,C.c_void_p,C.c_bool)

class Startup(C.Structure):
    _fields_=[('context',C.c_void_p),('priority',WORD),('clock',VOID),('delay',VOID),
              ('debug',WORD),('measurement',WORD),('buttons',VOID),('battery',VOID),
              ('display',VOID),('resources',VOID),('timer',PAIR),('loop',VOID)]
class Buttons(C.Structure):
    _fields_=[('context',C.c_void_p),('pin',PIN),('delay',DELAY16),('clear',C.c_void_p),('power',C.c_void_p)]
class Bus(C.Structure):
    _fields_=[('context',C.c_void_p),('set',SET),('data',DATA)]
class Display(C.Structure):
    _fields_=[('bus',Bus),('width',C.c_uint16),('height',C.c_uint16),
              ('column',C.c_uint8),('row',C.c_uint8),('pixels',C.c_uint8),('orientation',C.c_uint8)]

def main():
    p=argparse.ArgumentParser();p.add_argument('--library',default='reconstructed/build/librecovered.so')
    p.add_argument('--report',default='analysis/glue_differential.json');args=p.parse_args()
    path=ROOT/args.library;lib=C.CDLL(str(path));counts={}
    def done(pc):key=f'0x{pc:08x}';counts[key]=counts.get(key,0)+1
    lib.application_startup.argtypes=[C.POINTER(Startup)];lib.application_startup.restype=C.c_bool
    m=TraceMachine();expected=[];actual=[]
    addresses=[0xcb8e,0xd2fc,0x1a626,0x1a682,0xc350,0x7944,0x770a,0x87aa,0xdd4e,0x7736,0x8ed4]
    for address,n in zip(addresses,[1,0,0,1,1,0,0,0,0,2,0]):
        def hook(_, *a, address=address,n=n):expected.append((address,tuple(a[:n])));return 0
        m.hooks[address]=hook
    callbacks=[]
    for address,kind in zip(addresses,[WORD,VOID,VOID,WORD,WORD,VOID,VOID,VOID,VOID,PAIR,VOID]):
        def call(_, *a, address=address):actual.append((address,tuple(a)))
        callbacks.append(kind(call))
    class EnterLoop(Exception):pass
    def enter_loop(_, *a):expected.append((0x8ed4,()));raise EnterLoop()
    m.hooks[0x8ed4]=enter_loop
    io=Startup(None,*callbacks)
    try:m.call(0x908a)
    except EnterLoop:pass
    else:raise AssertionError('Original startup failed to enter its nonreturning loop')
    assert lib.application_startup(C.byref(io))
    assert actual==expected,(actual,expected);done(0x908a)
    # Missing startup bindings fail before executing any initialized peripheral.
    for index in range(11):
        invalid=Startup.from_buffer_copy(io);setattr(invalid,Startup._fields_[index+1][0],Startup._fields_[index+1][1]())
        actual.clear();assert not lib.application_startup(C.byref(invalid)) and not actual
    lib.measurement_forward_button.argtypes=[C.POINTER(C.c_uint8),C.POINTER(Buttons),C.c_void_p,TX]
    for event in range(256):
        for high in [0,1]:
            for replacement in ([None,6] if event in [1,2,3,4] else [None]):
                m=TraceMachine();m.write(0x200001ee,1,event);expected=[];actual=[];native=C.c_uint8(event)
                def delay_m(machine,ms,*_):
                    expected.append(('delay',ms))
                    if replacement is not None:machine.write(0x200001ee,1,replacement)
                    return 0
                m.hooks[0x1a64c]=delay_m
                def pin_m(_,port,pin,*a):expected.append(('pin',port,pin));return high
                def tx_m(machine,ptr,*a):expected.append(('tx',bytes(machine.read(ptr+i,1) for i in range(10))));return 0
                m.hooks[0xcb40]=pin_m;m.hooks[0x97c4]=tx_m
                @DELAY16
                def delay_c(_,ms):
                    actual.append(('delay',ms))
                    if replacement is not None:native.value=replacement
                @PIN
                def pin_c(_,pin):actual.append(('pin',0x40011000,pin));return bool(high)
                @TX
                def tx_c(_,packet):actual.append(('tx',bytes(packet[:10])))
                buttons=Buttons(None,pin_c,delay_c,None,None)
                m.call(0x97f8);lib.measurement_forward_button(C.byref(native),C.byref(buttons),None,tx_c)
                assert actual==expected,(event,high,replacement,actual,expected)
                assert native.value==m.read(0x200001ee,1);done(0x97f8)
    print('startup / forwarding passed',flush=True)
    lib.settings_auto_off_access.argtypes=[C.POINTER(C.c_uint16),C.POINTER(C.c_uint16),C.c_uint32,C.c_int32]
    lib.settings_auto_off_access.restype=C.c_uint32
    for initial in [0,1,0x20,0x40,0x80,0xc0,0x1234,0xffff]:
        for flag in [0,1,2,0xffffffff]:
            for value in [-65537,-1,0,1,2,3,4,7,0x10000,0x7fffffff]:
                word=C.c_uint16(initial);timeout=C.c_uint16(0xa55a);m=TraceMachine()
                m.write(0x2000003e,2,initial);m.write(0x20000208,2,timeout.value)
                expected=m.call(0xda48,flag,value);actual=lib.settings_auto_off_access(C.byref(word),C.byref(timeout),flag,value)
                assert (actual,word.value,timeout.value)==(expected,m.read(0x2000003e,2),m.read(0x20000208,2)),(initial,flag,value)
                done(0xda48)
    lib.display_scan_direction.argtypes=[C.POINTER(Display),C.c_uint32]
    for width,height in [(320,240),(240,320),(1,1),(0,0),(256,257),(65535,65535)]:
        for direction in [*range(10),255,0xffffffff]:
            m=TraceMachine();expected=[];actual=[];is_data=False
            # Width/height and command globals are separate original inputs.
            m.write(0x20000270,2,width);m.write(0x20000272,2,height)
            m.write(0x20000278,2,0x2b2a)
            for address,label in [(0x7d6a,'command'),(0x7dba,'data')]:
                def hook(_,value,*a,label=label):expected.append((label,value & 65535));return 0
                m.hooks[address]=hook
            @SET
            def set_c(_,mask,high):
                nonlocal is_data
                if mask==0x20:is_data=high
            @DATA
            def data_c(_,value):actual.append(('data' if is_data else 'command',value))
            display=Display(Bus(None,set_c,data_c),width,height,0x2a,0x2b,0x2c,3)
            m.call(0x7e68,direction);lib.display_scan_direction(C.byref(display),direction)
            assert actual==expected,(width,height,direction,actual,expected);done(0x7e68)
    # 792e has exactly three calls. Reuse the application IO prefix without
    # binding any power/persistence or hardware interface.
    class ApplicationIO(C.Structure):
        _fields_=[('context',C.c_void_p),('send',C.c_void_p),('longpress',C.c_void_p),('delay',WORD),
                  ('charging',C.c_void_p),('battery',C.c_void_p),('buzzer',BOOL)]+[(f'rest{i}',C.c_void_p) for i in range(13)]
    m=TraceMachine();expected=[];actual=[]
    for address,label,n in [(0x78ba,'buzzer-on',0),(0x1a64c,'delay',1),(0x78d0,'buzzer',1)]:
        def hook(_, *a,label=label,n=n):expected.append((label,tuple(a[:n])));return 0
        m.hooks[address]=hook
    @WORD
    def delay_c(_,ms):actual.append(('delay',(ms,)))
    @BOOL
    def buzzer_c(_,on):actual.append(('buzzer-on',()) if on else ('buzzer',(0,)))
    io=ApplicationIO();io.delay=delay_c;io.buzzer=buzzer_c
    lib.application_beep_500.argtypes=[C.POINTER(ApplicationIO)]
    m.call(0x792e);lib.application_beep_500(C.byref(io));assert actual==expected,(actual,expected);done(0x792e)
    lib.settings_language_access.argtypes=[C.POINTER(C.c_uint16),C.c_void_p,C.c_uint32,C.c_uint32]
    lib.settings_language_access.restype=C.c_uint32
    for initial in [0,1,6,7,0x1234,0xffff]:
        for flag in [0,1,0xffffffff]:
            for value in [*range(9),0xffff,0x10000,0x10001,0xffffffff]:
                m=TraceMachine();word=C.c_uint16(initial);prefix=C.create_string_buffer(b'old/cache/',20)
                m.write(0x2000003e,2,initial);m.ram[0x298:0x298+20]=prefix.raw
                expected=m.call(0xd964,flag,value,limit=1000000)
                actual=lib.settings_language_access(C.byref(word),prefix,flag,value)
                assert (actual,word.value,prefix.raw)==(expected,m.read(0x2000003e,2),bytes(m.ram[0x298:0x298+20])),(initial,flag,value,actual,expected)
                done(0xd964)
    class Image(C.Structure):_fields_=[('words',C.c_uint16*128)]
    lib.settings_image_restore_defaults.argtypes=[C.POINTER(Image),C.POINTER(C.c_uint8),C.c_void_p,VOID]
    for flag in [0,1,0x55,0xff]:
        m=TraceMachine();source=bytes((i*13+flag)&255 for i in range(256))
        m.ram[0x20:0x120]=source;m.write(0x20000224,1,flag)
        image=Image.from_buffer_copy(source);loaded=C.c_uint8(flag);expected=[];actual=[]
        def remove_m(machine,*a):expected.append((bytes(machine.ram[0x20:0x120]),machine.read(0x20000224,1)));return 255
        m.hooks[0xde12]=remove_m
        @VOID
        def remove_c(_):actual.append((bytes(image),loaded.value))
        m.call(0xdc46);lib.settings_image_restore_defaults(C.byref(image),C.byref(loaded),None,remove_c)
        assert actual==expected and loaded.value==m.read(0x20000224,1) and bytes(image)==bytes(m.ram[0x20:0x120])
        done(0xdc46)
    report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),
            'native_library_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),
            'original_sha256':hashlib.sha256(m.image).hexdigest(),
            'additional_host_guards':{'missing_startup_bindings':11},
            'scope':'Original startup callee order/arguments and host-returning loop boundary; all 256 forwarding events with released/pressed GPIO and replacement during debounce; timeout write/read/spill effects; display command/data streams for eight directions/defaults; 500ms beep call contract. Hardware, peripheral initialization internals and interrupt timing remain unvalidated.'}
    (ROOT/args.report).write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'glue differential cases passed')
if __name__=='__main__':main()

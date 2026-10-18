#!/usr/bin/env python3
"""Original settings Flash instructions versus current C callback contract.

Records writes in PRIVATE Python memory. No MCU, physical Flash or RDP emulator.
CSR 0x800 is skipped and reported. STATR responses model ready/WRP only. Address
intent evidence does not establish successful programming on a protected chip.
"""
import ctypes as C
import hashlib
import json
import random
from pathlib import Path
from rv32_trace import ROOT, TraceMachine


class FlashTrace(TraceMachine):
    def __init__(self, data, status=4, fail=None):
        super().__init__();self.regs={0x40021004:0,0x4002200c:0,0x40022010:0x8080}
        self.data=bytearray(data);self.programs=[];self.erases=[];self.fail=fail
        self.write(0x200001d0,4,status)
        def csr(m):
            if m.pc==0xd88e:m.pc+=4
        self.instruction_hook=csr

    def read(self, a, n):
        if a in self.regs and n==4:return self.regs[a]
        if 0x08040000<=a and a+n<=0x08040100:
            return int.from_bytes(self.data[a-0x08040000:a-0x08040000+n],'little')
        return super().read(a,n)

    def write(self, a, n, v):
        if 0x08040000<=a and a+n<=0x08040100:
            assert n==2 and self.regs[0x40022010]&1
            self.programs.append((a,v&0xffff))
            if self.fail==len(self.programs)-1:self.regs[0x4002200c]=0x10
            return  # Record attempted stores, not physical programming success.
        if a in (0x40021004,0x40022004,0x4002200c,0x40022010,0x40022014,0x40022024):
            assert n==4
            if a==0x4002200c:  # EOP/error write-one-to-clear; BSY is read-only.
                self.regs[a]&=~(v&0x30);return
            if a==0x40022010 and v&0x20000 and v&0x40 and not self.regs.get(a,0)&0x40:
                self.erases.append(self.regs[0x40022014])
            self.regs[a]=v;return
        return super().write(a,n,v)


def main():
    libpath=ROOT/'reconstructed/build/librecovered.so';lib=C.CDLL(str(libpath))
    class Image(C.Structure):_fields_=[('halfwords',C.c_uint16*128)]
    READ=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_uint32,C.POINTER(C.c_uint16),C.c_uint16)
    BEGIN=C.CFUNCTYPE(C.c_bool,C.c_void_p)
    ERASE=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_uint32)
    PROGRAM=C.CFUNCTYPE(C.c_uint32,C.c_void_p,C.c_uint32,C.c_uint16)
    END_CB=C.CFUNCTYPE(None,C.c_void_p)
    class Backend(C.Structure):
        _fields_=[('ctx',C.c_void_p),('read',READ),('begin',BEGIN),('erase',ERASE),('program',PROGRAM),('end',END_CB)]
    lib.settings_image_commit.argtypes=[C.POINTER(Image),C.POINTER(Backend),C.c_uint32]
    lib.settings_image_commit.restype=C.c_uint32
    lib.settings_image_load.argtypes=[C.POINTER(Image),C.POINTER(Backend)];lib.settings_image_load.restype=C.c_bool
    rng=random.Random(0xd87c);cases=[]
    for status in (0,1,3,4,5):
      for fail in (None,0,1,63,127):
        source=rng.randbytes(256);m=FlashTrace(rng.randbytes(256),status,fail)
        m.ram[0x20:0x120]=source;m.call(0xd87c)
        programmed=[];erased=[];calls=[]
        @BEGIN
        def begin(_):calls.append('begin');return True
        @ERASE
        def erase(_,a):erased.append(a);return True
        @PROGRAM
        def program(_,a,v):programmed.append((a,v));return 3 if fail==len(programmed)-1 else 4
        @END_CB
        def end(_):calls.append('end')
        backend=Backend(None,READ(),begin,erase,program,end);image=Image.from_buffer_copy(source)
        result=lib.settings_image_commit(C.byref(image),C.byref(backend),status)
        assert programmed==m.programs and erased==m.erases==[0x08040000]
        assert calls==['begin','end'] and result==m.read(0x200001d0,4)
        assert m.regs[0x40022010]&0x80 and m.regs[0x40022010]&0x8000
        cases.append({'entry':'0xd87c','initial_status':status,'failure_after_store':fail,
                      'erase':hex(erased[0]),'attempted_halfwords':len(programmed),'final_status':result})
    for i in range(16):
        source=rng.randbytes(256);m=FlashTrace(source);m.call(0xd928);reads=[]
        @READ
        def read(_,address,out,n):
            reads.append((address,n));C.memmove(out,source,len(source));return True
        backend=Backend(None,read,BEGIN(),ERASE(),PROGRAM(),END_CB());image=Image()
        assert lib.settings_image_load(C.byref(image),C.byref(backend))
        assert bytes(image)==m.ram[0x20:0x120]==source and reads==[(0x08040000,128)]
        cases.append({'entry':'0xd928','read':'0x08040000','halfwords':128})
    report={'cases':len(cases),'entries':['0xd87c','0xd928'],'vectors':cases,
            'native_library_sha256':hashlib.sha256(libpath.read_bytes()).hexdigest(),
            'scope':'Original instructions including FLASH SDK bodies; private MMIO intent mock; CSR 0x800 skipped; no RDP/timing/erase success claim.',
            'hardware_accessed':False}
    (ROOT/'analysis/update_flash_trace.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f"{len(cases)} original settings entry comparisons passed; all writes target 0x08040000..0x080400ff")


if __name__=='__main__':main()

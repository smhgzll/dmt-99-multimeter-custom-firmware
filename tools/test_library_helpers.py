#!/usr/bin/env python3
import hashlib
import ctypes as C,random,json
from pathlib import Path
from rv32_trace import TraceMachine
ROOT=Path(__file__).resolve().parents[1]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/filesystem_oracle.so'));rng=random.Random(1402);counts={}
 def done(a):counts[a]=counts.get(a,0)+1
 lib.oracle_ld.argtypes=[C.c_void_p];lib.oracle_ld.restype=C.c_uint32
 lib.oracle_st.argtypes=[C.c_void_p,C.c_uint32]
 lib.oracle_checksum.argtypes=[C.c_void_p];lib.oracle_checksum.restype=C.c_uint8
 lib.oracle_cluster_sector.argtypes=[C.c_uint32,C.c_uint16,C.c_uint32,C.c_uint32];lib.oracle_cluster_sector.restype=C.c_uint32
 lib.ff_wtoupper.argtypes=[C.c_uint32];lib.ff_wtoupper.restype=C.c_uint32
 for _ in range(256):
  b=rng.randbytes(16);m=TraceMachine();m.ram[0x6000:0x6010]=b
  assert m.call(0xde5a,0x20006000)==lib.oracle_ld(b);done('0x0000de5a')
  value=rng.getrandbits(32);out=C.create_string_buffer(4);lib.oracle_st(out,value);m.call(0xde70,0x20006000,value)
  assert m.read(0x20006000,4)==int.from_bytes(out.raw,'little');done('0x0000de70')
  m.ram[0x6000:0x6010]=b;assert m.call(0xde9e,0x20006000)==lib.oracle_checksum(b);done('0x0000de9e')
  base=rng.randrange(2048);size=rng.choice([1,2,4,8,16,128]);count=rng.randrange(2,3000);c=rng.choice([0,1,2,count-1,count,0xffffffff])
  m.write(0x20006030,4,base);m.write(0x2000600a,2,size);m.write(0x2000601c,4,count)
  assert m.call(0xde84,0x20006000,c)==lib.oracle_cluster_sector(base,size,count,c);done('0x0000de84')
 for c in list(range(128))+[0xff,0x100,0x3a3,0x4e2d,0x10000,0xffffffff]+[rng.randrange(65536) for _ in range(512)]:
  m=TraceMachine();assert m.call(0xfd46,c)==lib.ff_wtoupper(c),(hex(c),m.call(0xfd46,c),lib.ff_wtoupper(c));done('0x0000fd46')
 # Compare both original partition entry points with authentic upstream fdisk.
 READ=C.CFUNCTYPE(C.c_int,C.c_void_p,C.c_uint32,C.c_void_p,C.c_uint32)
 class IO(C.Structure):_fields_=[('context',C.c_void_p),('read',READ),('write',READ),('gap',C.c_void_p)]
 native_written=[]
 @READ
 def read(_,p,b,n):return 0
 @READ
 def write(_,p,b,n):native_written.append((p,C.string_at(b,n)));return 0
 io=IO(None,read,write,None);lib.filesystem_bind(C.byref(io));lib.f_fdisk.argtypes=[C.c_uint8,C.c_void_p,C.c_void_p]
 for sizes in [(20,80,0,0),(400,0,0,0),(100,0,0,0),(200,300,400,500)]:
  values=(C.c_uint32*4)(*sizes);work=C.create_string_buffer(4096);native_written.clear();nr=lib.f_fdisk(0,values,work)
  for entry in [0xdf50,0xfc66]:
   m=TraceMachine();written=[]
   for i,n in enumerate(sizes):m.write(0x20006000+i*4,4,n)
   def ioctl(m,d,cmd,out,*_):
    if cmd==1:m.write(out,4,2048)
    return 0
   def write_original(m,d,p,sector,n,*_):written.append((sector*4096,bytes(m.ram[p-0x20000000:p-0x20000000+n*4096])));return 0
   m.hooks[0xdd00]=ioctl;m.hooks[0xdcd0]=write_original
   args=(0,0x20006000,7,0x20007000) if entry==0xdf50 else (0,0x20006000,0x20007000)
   result=m.call(entry,*args,limit=1000000)
   assert result==nr and written==native_written,(hex(entry),sizes,result,nr)
   done(f'0x{entry:08x}')
 lib.filesystem_bind(None)
 (ROOT/'analysis/library_helper_differential.json').write_text(json.dumps({'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'cases':counts,'total':sum(counts.values()),'scope':'Authentic FatFs R0.14b C versus original instruction execution. Pure helpers include unaligned LE access, SFN checksum, cluster bounds and Unicode uppercase.'},indent=2)+'\n')
 print(counts)
if __name__=='__main__':main()

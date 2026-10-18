#!/usr/bin/env python3
"""Additional upstream helpers, tested through explicit original ABI adapters."""
import ctypes as C,hashlib,json,random
from rv32_trace import TraceMachine,ROOT

def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/filesystem_oracle.so'));counts={};rng=random.Random(0xdec0)
 def done(pc):counts[f'0x{pc:08x}']=counts.get(f'0x{pc:08x}',0)+1
 lib.oracle_load_cluster.argtypes=[C.c_uint8,C.c_void_p];lib.oracle_load_cluster.restype=C.c_uint32
 lib.oracle_store_cluster.argtypes=[C.c_uint8,C.c_void_p,C.c_uint32]
 lib.oracle_drive.argtypes=[C.POINTER(C.c_char_p)];lib.oracle_drive.restype=C.c_int
 for _ in range(512):
  data=rng.randbytes(32);kind=rng.choice([0,1,2,3,4,255]);cluster=rng.getrandbits(32);m=TraceMachine();m.ram[0x6000:0x6020]=data
  # Original ld_clust has the scalarized fs_type parameter (compiler IPA);
  # the authentic source takes FATFS*. The exported adapter makes this explicit.
  assert m.call(0xe0f8,kind,0x20006000)==lib.oracle_load_cluster(kind,data);done(0xe0f8)
  out=C.create_string_buffer(data,32);m.write(0x20005000,1,kind);m.call(0xe116,0x20005000,0x20006000,cluster);lib.oracle_store_cluster(kind,out,cluster);assert out.raw==bytes(m.ram[0x6000:0x6020]);done(0xe116)
 paths=[None,b'',b'/',b'on.jpg',b'0:/on.jpg',b'1:on.jpg',b'2:/file',b'9:/file',b'00:/file',b':',b'a:/x',b'1:']
 paths+=[rng.randbytes(rng.randrange(1,40)).replace(b'\0',b'x')+b':/x' for _ in range(128)]
 for path in paths:
  m=TraceMachine();m.write(0x20005000,4,0 if path is None else 0x20006000)
  if path is not None:m.ram[0x6000:0x6001+len(path)]=path+b'\0'
  p=C.c_char_p(path);start=C.cast(p,C.c_void_p).value;r=lib.oracle_drive(C.byref(p));original=m.call(0xdec0,0x20005000)
  assert r==C.c_int32(original).value,(path,r,original)
  expected=m.read(0x20005000,4);actual=C.cast(p,C.c_void_p).value
  assert (0 if actual is None else actual-start)==(0 if not expected else expected-0x20006000),(path,actual,start,expected);done(0xdec0)
 report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'FatFs R0.14b get_ldnumber,ld_clust,st_clust versus original code, including directory bytes and path advancement. ld_clust original fs_type argument is compiler-scalarized; adapter supplies authentic FATFS*. No device writes.'}
 (ROOT/'analysis/library_extension_differential.json').write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'upstream helper extension cases passed')
if __name__=='__main__':main()

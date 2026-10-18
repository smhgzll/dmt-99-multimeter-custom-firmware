import ctypes as C,json,hashlib
from rv32_trace import TraceMachine,ROOT
class Result(C.Structure):_fields_=[('value',C.c_uint16),('status',C.c_int),('fault',C.c_uint32),('reads',C.c_uint8)]
def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));lib.codepage_stock_convert.argtypes=[C.c_uint32,C.c_uint16,C.c_int];lib.codepage_stock_convert.restype=Result;count=0
 for entry,to_oem in [(0xfc9e,1),(0xfcfc,0)]:
  for cp in [0,932,936]:
   for c in [*range(128),128,0x31a,0xd7e9,0x7ec4,65535,65536,0xffffffff]:
    if not to_oem and c>65535:continue
    m=TraceMachine();reads=[];original=m.read
    def read(a,n):
     if n==2 and (a==0x23fe4 or a==0x23fe6 or a==0x80023fe4):reads.append(a)
     return original(a,n)
    m.read=read;r=lib.codepage_stock_convert(c,cp,to_oem)
    try:value=m.call(entry,c,cp);assert r.status==0 and r.value==value
    except RuntimeError as e:assert '80023fe4' in str(e) and r.status==1 and r.fault==0x80023fe4
    if r.status:assert reads==[0x23fe4,0x23fe4,0x80023fe4],reads
    count+=1
 (ROOT/'analysis/codepage_differential.json').write_text(json.dumps({'cases':count,'entries':['0x0000fc9e','0x0000fcfc'],'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'All ASCII inputs, exceptional key031a, wrong pages and non-ASCII original ROM-fault path. Returns and first fault address preserved; unknown physical alias/bus behavior is not emulated.'},indent=2)+'\n');print(count,'codepage cases pass')
if __name__=='__main__':main()

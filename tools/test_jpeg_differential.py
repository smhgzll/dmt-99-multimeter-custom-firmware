#!/usr/bin/env python3
import ctypes as C,json,subprocess,hashlib
from pathlib import Path
from rv32_trace import TraceMachine
ROOT=Path(__file__).resolve().parents[1]

def compare(path):
 data=path.read_bytes();native=C.CDLL(str(ROOT/'reconstructed/build/jpeg_oracle.so'))
 native.phase3_jpeg_decode.argtypes=[C.c_void_p,C.c_uint,C.c_void_p,C.c_uint,C.c_void_p]
 pixels=(C.c_uint16*100000)();info=(C.c_uint*8)();native.phase3_jpeg_decode(data,len(data),pixels,len(pixels),info)
 m=TraceMachine();offset=0;original=[];rects=[]
 def inp(m,j,d,n,*_):
  nonlocal offset
  n=min(n,len(data)-offset)
  if d:m.ram[d-0x20000000:d-0x20000000+n]=data[offset:offset+n]
  offset+=n;return n
 def out(m,j,p,r,*_):
  rect=[m.read(r+2*i,2) for i in range(4)];rects.append(rect)
  n=(rect[1]-rect[0]+1)*(rect[3]-rect[2]+1)
  original.extend(m.read(p+2*i,2) for i in range(n));return 1
 m.hooks[0xffffff00]=inp;m.hooks[0xffffff02]=out
 r=m.call(0x696e,0x20006000,0xffffff00,0x20007000,0x1c00,0,limit=3000000)
 assert r==info[0],(path,r,list(info))
 if not r:
  assert [m.read(0x2000601c,4),m.read(0x20006020,4),0x1c00-m.read(0x20006070,4)]==list(info)[2:5],(path,list(info))
  r=m.call(0x6d9c,0x20006000,0xffffff02,0,limit=20000000)
  assert r==info[5] and original==list(pixels)[:info[6]],(path,r,list(info),len(original),next(((i,a,b) for i,(a,b) in enumerate(zip(original,pixels)) if a!=b),None))
 return {'file':str(path.relative_to(ROOT)),'prepare_result':info[0],'decode_result':r,'pixels':len(original),'rectangles':len(rects),'rgb565_mcu_stream_sha256':hashlib.sha256(bytes(pixels)[:len(original)*2]).hexdigest()}

def main():
 subprocess.run(['gcc','-std=c11','-O2','-shared','-fPIC','reconstructed/tests/jpeg_oracle.c','-o','reconstructed/build/jpeg_oracle.so'],cwd=ROOT,check=True)
 files=json.loads((ROOT/'analysis/flash_files.json').read_text())['active'];rows=[]
 for name in ['history/cap.jpg','mode/cap.jpg','num/radix.jpg']:
  p=next(ROOT/f['extracted_path'] for f in files if f['path'].casefold()==name)
  rows.append(compare(p));print(rows[-1],flush=True)
 (ROOT/'analysis/jpeg_differential.json').write_text(json.dumps({'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'cases':rows,'functions':['0x0000696e','0x00006d9c'],'scope':'Original RV32/Xwchc instructions execute, only input/output callbacks injected. Every emitted RGB565 MCU pixel compared with authentic R0.01b source.'},indent=2)+'\n')
if __name__=='__main__':main()

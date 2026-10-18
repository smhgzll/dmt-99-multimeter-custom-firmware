#!/usr/bin/env python3
import hashlib
import ctypes as C,json,subprocess,random
from pathlib import Path
from rv32_trace import TraceMachine
ROOT=Path(__file__).resolve().parents[1]

def original_fs():
 m=TraceMachine();image=(ROOT/'flashdump/dmt99_flash_1.bin').read_bytes()
 def read(m,drive,dst,lba,count,*_):
  assert drive<2 and lba+count<=2048
  data=image[lba*4096:(lba+count)*4096]
  m.ram[dst-0x20000000:dst-0x20000000+len(data)]=data;return 0
 def ioctl(m,drive,cmd,out,*_):
  if cmd==1:m.write(out,4,2048)
  elif cmd==2:m.write(out,2,4096)
  elif cmd==3:m.write(out,4,1)
  return 0
 m.hooks[0xdca8]=read;m.hooks[0xdd00]=ioctl
 m.hooks[0xdcd0]=lambda *_:(_ for _ in ()).throw(RuntimeError('unexpected original write'))
 return m

def main():
 # Original function comparisons use the same actual image, with only disk IO injected.
 m=original_fs();print('disk stubs',flush=True)
 fs=0x20006000;fil=0x20008000;name=0x2000a000;out=0x2000a200;br=0x2000c000
 def text(s):m.ram[name-0x20000000:name-0x20000000+len(s)+1]=s.encode()+b'\0'
 text('1:');r=m.call(0xf4f8,fs,name,1,limit=2000000);assert r==0,r
 files=json.loads((ROOT/'analysis/flash_files.json').read_text())['active'];rows=[]
 for path in ['mode/cap.jpg','history/cap.jpg','on.jpg','does_not_exist.jpg']:
  text('1:/'+path);r=m.call(0xf54a,fil,name,1,limit=2000000)
  if path.startswith('does_'):assert r==4;rows.append({'file':path,'open_result':r});continue
  assert r==0,(path,r)
  expected=(ROOT/next(f['extracted_path'] for f in files if f['path'].casefold()==path)).read_bytes()
  data=bytearray()
  while len(data)<len(expected):
   r=m.call(0xf732,fil,out,min(4096,len(expected)-len(data)),br,limit=1000000);assert r==0
   n=m.read(br,4);assert n;data.extend(m.ram[out-0x20000000:out-0x20000000+n])
  assert data==expected,path
  assert m.call(0xf962,fil,1,limit=1000000)==0
  assert m.call(0xf732,fil,out,17,br,limit=1000000)==0
  assert bytes(m.ram[out-0x20000000:out-0x20000000+17])==expected[1:18]
  assert m.call(0xf940,fil,limit=1000000)==0
  rows.append({'file':path,'original_open_read_seek_close':'pass','bytes':len(data)})
  print(rows[-1],flush=True)
 (ROOT/'analysis/filesystem_differential.json').write_text(json.dumps({'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'cases':rows,'entries':['0xf4f8','0xf54a','0xf732','0xf962','0xf940'],'scope':'Original mount/open/read/seek/close executes against acquired image. Extracted bytes independently compared; native upstream roundtrip tests separately cover the same files.'},indent=2)+'\n')
if __name__=='__main__':main()

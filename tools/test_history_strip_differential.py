#!/usr/bin/env python3
import ctypes as C,hashlib,json
from rv32_trace import TraceMachine,ROOT
from test_reading_differential import Render
from test_ui_differential import Trace

def main():
 lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'));counts={'0x00012c38':0,'0x00012c78':0}
 lib.history_strip_render.argtypes=[C.POINTER(Render),C.c_char_p,C.c_char_p,C.c_uint8,C.c_bool,C.c_uint8,C.POINTER(C.c_uint8),C.POINTER(C.c_uint16),C.c_bool]
 lib.history_unit_update.argtypes=[C.POINTER(Render),C.c_uint8,C.c_uint16,C.POINTER(C.c_uint8),C.POINTER(C.c_uint16),C.c_bool]
 for unit in [6,8,11,17,20,21]:
  for f in [0,1]:
   for old_y in [0,55,209]:
    m=TraceMachine();m.write(0x20000289,1,unit);m.write(0x2000024e,2,old_y);m.write(0x20000252,1,9);m.write(0x2000003e,2,32 if f else 0);trace=Trace(m)
    m.call(0x12c38,99,limit=1000000);old=C.c_uint8(9);y=C.c_uint16(old_y);lib.history_unit_update(C.byref(trace.view),unit,99,C.byref(old),C.byref(y),f);trace.check(('unit-update',unit,f,old_y));assert old.value==m.read(0x20000252,1) and y.value==m.read(0x2000024e,2);counts['0x00012c38']+=1
 for row in range(1,9):
  for wrapped in [0,1]:
   for unit in [6,8,17,21]:
    m=TraceMachine();m.write(0x200001dc,1,row);m.write(0x20000250,1,wrapped);m.write(0x20000289,1,unit);m.write(0x2000024e,2,55);m.write(0x20000252,1,7);m.ram[0x240:0x247]=b' 12.34\0';m.ram[0x6000:0x6007]=b' 24.68\0';trace=Trace(m);m.hooks[0xda9c]=lambda *_:0
    m.call(0x12c78,0x20006000,limit=1000000);old=C.c_uint8(7);y=C.c_uint16(55)
    lib.history_strip_render(C.byref(trace.view),b' 24.68',b' 12.34',row,wrapped,unit,C.byref(old),C.byref(y),False);trace.check(('history-strip',row,wrapped,unit));assert m.read(0x200001dc,1)==(row+1 if row<8 else 1);assert m.read(0x20000250,1)==(row==8);assert m.string(0x20000240)==' 24.68';counts['0x00012c78']+=1
 report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),'native_library_sha256':hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest(),'scope':'Original recording strip and unit-cache leaf calls, row/wrap/previous-string effects. Persistent append is isolated and separately covered by preserved history tests. C controller owns row/wrap/copy updates.'};(ROOT/'analysis/history_strip_differential.json').write_text(json.dumps(report,indent=2)+'\n');print(report['total'],'history strip cases passed')
if __name__=='__main__':main()

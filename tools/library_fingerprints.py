#!/usr/bin/env python3
"""Compile-derived tables, sizes, exact bytes and normalized operation sequences.

Normalization is evidence only, never the sole criterion for a strong match.
Xwchc operations come from the preserved validated Ghidra export.
"""
from pathlib import Path
import json,re,subprocess,difflib,hashlib
from validate import sections
from flash_forensics import ROOT,save,sha
from find_riscv_toolchain import find_prefix
MAPPING={'0x00006838':('tjpgd','alloc_pool'),'0x00006854':('tjpgd','bitext'),'0x000068d2':('tjpgd','huffext'),'0x0000696e':('tjpgd','jd_prepare'),'0x00006d9c':('tjpgd','jd_decomp'),
'0x0000de5a':('fatfs','ld_dword'),'0x0000de70':('fatfs','st_dword'),'0x0000de84':('fatfs','clst2sect'),'0x0000de9e':('fatfs','sum_sfn'),'0x0000df50':('fatfs','create_partition'),'0x0000f4f8':('fatfs','f_mount'),'0x0000f54a':('fatfs','f_open'),'0x0000f732':('fatfs','f_read'),'0x0000f940':('fatfs','f_close'),'0x0000f962':('fatfs','f_lseek'),'0x0000fc66':('fatfs','f_fdisk'),'0x0000fd46':('unicode_upper','ff_wtoupper')}

def op(s):
 s=s.split()[0].removeprefix('qk.').removeprefix('c.');return {'lbusp':'lbu','sbsp':'sb','lhusp':'lhu','shsp':'sh','lwsp':'lw','swsp':'sw','mv':'addi','li':'addi','j':'jal','ret':'jalr','jr':'jalr','addi16sp':'addi','addi4spn':'addi'}.get(s,s)
def main():
 fw=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes();functions={x['address']:x for x in json.loads((ROOT/'analysis/functions.json').read_text())};ins=json.loads((ROOT/'analysis/instructions.json').read_text())
 gcc=Path(find_prefix()+'gcc');objdump=str(gcc).removesuffix('gcc')+'objdump';objects={}
 for family,path in [('fatfs','vendor/fatfs/ff.c'),('tjpgd','vendor/tjpgd/tjpgd.c'),('unicode_upper','vendor/fatfs/unicode_upper.c')]:
  obj=ROOT/f'reconstructed/build/fingerprint_{family}.o'
  subprocess.run([str(gcc),'-march=rv32imac_zicsr_zifencei','-mabi=ilp32','-Os','-fno-inline','-fkeep-static-functions','-ffunction-sections','-fdata-sections','-Ireconstructed/vendor/fatfs','-Ireconstructed/vendor/tjpgd','-c','reconstructed/'+path,'-o',str(obj)],cwd=ROOT,check=True)
  _,ss=sections(obj);dis=subprocess.check_output([objdump,'-d',str(obj)],text=True);ops={};current=None
  for line in dis.splitlines():
   match=re.match(r'^0+ <([^>]+)>:',line)
   if match:current=match[1];ops[current]=[]
   match=re.match(r'^\s*[0-9a-f]+:\s+[0-9a-f]+\s+([^\s]+)',line)
   if match and current:ops[current].append(op(match[1]))
  objects[family]=(ss,ops)
 rows=[]
 for a,(family,symbol) in MAPPING.items():
  ss,ops=objects[family];section=ss.get('.text.'+symbol);f=functions[a];ranges=[(int(x,16),int(y,16)) for x,y in f['ranges']]
  original=b''.join(fw[x-0x6400:y-0x6400+1] for x,y in ranges)
  oo=[op(i['text']) for i in ins if any(x<=int(i['address'],16)<=y for x,y in ranges)];co=ops.get(symbol,[])
  exact=section is not None and len(ranges)==1 and original==section['data']
  rows.append({'address':a,'family':family,'symbol':symbol,'original_body_bytes':len(original),'compiled_body_bytes':section['size'] if section else None,'exact_body_bytes':exact,'operation_sequence_similarity':round(difflib.SequenceMatcher(None,oo,co,autojunk=False).ratio(),4),'original_operations':len(oo),'compiled_operations':len(co),'note':'Static helpers compiled with inlining disabled for review. Operation normalization omits operands and is a fingerprint, not proof. Calls, ABI, tables and differential evidence are assessed separately.'})
 tables=[]
 for family,(ss,_) in objects.items():
  for name,s in ss.items():
   if name.startswith('.rodata.') and s['size']>=64:
    p=fw.find(s['data'])
    if p>=0:tables.append({'family':family,'section':name,'size':s['size'],'sha256':sha(s['data']),'original_rom_address':p+0x6400,'classification':'Exact table bytes'})
 save('library_fingerprints.json',{'compiler':str(gcc),'flags':'-Os -fno-inline -fkeep-static-functions RV32IMAC ILP32','decoded_original':'preserved Ghidra RISC-V+Xwchc instruction inventory; unchanged','functions':rows,'exact_tables':tables,'limitations':'No identical compiler revision claim. Sequence similarity does not establish source/version identity; CFG matching beyond normalized opcode sequence is not implemented.'})
 sdk=json.loads((ROOT/'analysis/instruction_comparison.json').read_text());strong={'0x00006838','0x00006854','0x000068d2','0x0000696e','0x00006d9c','0x0000de5a','0x0000de70','0x0000de84','0x0000de9e','0x0000f4f8','0x0000f54a','0x0000f732','0x0000f940','0x0000f962','0x0000fd46','0x0000df50','0x0000fc66'}
 for r in rows:
  r['classification']='Exact' if r['exact_body_bytes'] else 'Strong semantic' if r['address'] in strong else 'Probable'
  r['evidence']=['analysis/jpeg_differential.json'] if r['family']=='tjpgd' else ['analysis/library_helper_differential.json'] if r['address'] in {'0x0000de5a','0x0000de70','0x0000de84','0x0000de9e','0x0000fd46'} else ['analysis/filesystem_differential.json','reconstructed/tests/test_resources.c']
  r['compiled_source']='reconstructed/vendor/'+('fatfs/'+('unicode_upper.c' if r['family']=='unicode_upper' else 'ff.c') if r['family']!='tjpgd' else 'tjpgd/tjpgd.c')
 save('library_matches.json',{'sdk_retained':{'compared':sdk['compared'],'exact':sdk['byte_identical'],'source':'references/wch-sdk-manifest.json'},'new_function_matches':rows,'upstream_components':[{'name':'TJpgDec','selected':'R0.01b','version_confidence':'Strong semantic branch match; original provenance unknown','rejected':['R0.01c/d: incompatible width/pool field layout','R0.02a: changed entropy/object layout'],'source_url':'https://elm-chan.org/fsw/tjpgd/arc/tjpgd1b.zip'},{'name':'FatFs','selected':'R0.14b','version_confidence':'Strong semantic configuration family; exact vendor provenance unknown','rejected':['R0.12c/R0.13c: incompatible partition builder and boot-sector validation','R0.14/R0.14a: boot validator lacks original reserved-sector/total-sector checks'],'source_url':'https://elm-chan.org/fsw/ff/arc/ff14b.zip'}],'unknown':['Original compiler build','USB MSC source and location','newlib/libgcc exact version','SPI/graphics upstream provenance','Bootloader/update middleware'],'vendor_boundaries':['fc9e/fcfc CP936 tables absent/invalid; native fails closed with address callback','block adapter bounds and capabilities; original disk_read narrowing remains separately preserved'],'classification_rule':'Exact requires whole compiled body bytes; Strong semantic requires matched algorithm/layout and bounded original-code behavioral evidence. Probable entries are compiled but excluded from complete counterpart coverage.'})
 print({'function_matches':len(rows),'exact':sum(r['classification']=='Exact' for r in rows),'strong':sum(r['classification']=='Strong semantic' for r in rows),'tables':len(tables)})
if __name__=='__main__':main()

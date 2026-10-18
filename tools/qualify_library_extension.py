#!/usr/bin/env python3
"""Incrementally qualify three reviewed FatFs helper mappings.

Normalized operations and CFGs are supplementary; direct original execution,
explicit ABI adaptation and reviewed object fields are required for credit.
Existing Phase 3 matches are retained without recompiling/reclassifying them.
"""
import difflib,hashlib,json,re,subprocess
from pathlib import Path
from library_fingerprints import ROOT,op
from library_cfg import graph
from validate import sections
from find_riscv_toolchain import find_prefix
MAPPING={'0x0000dec0':('get_ldnumber','Original const TCHAR** path contract; two logical volumes, FF_FS_RPATH=0'),
 '0x0000e0f8':('ld_clust','Original compiler scalarized FATFS.fs_type to an integer argument; oracle_load_cluster explicitly adapts this ABI'),
 '0x0000e116':('st_clust','Original FATFS.fs_type byte and SFN offsets20/26 match configured upstream')}
def main():
 report=json.loads((ROOT/'analysis/library_extension_differential.json').read_text());assert set(report['entries'])==set(MAPPING)
 digest=hashlib.sha256((ROOT/'reconstructed/build/librecovered.so').read_bytes()).hexdigest();assert report['native_library_sha256']==digest
 compiler=Path(find_prefix()+'gcc');obj=ROOT/'reconstructed/build/fingerprint_extension.o'
 subprocess.run([str(compiler),'-march=rv32imac_zicsr_zifencei','-mabi=ilp32','-Os','-fno-inline','-fkeep-static-functions','-ffunction-sections','-fdata-sections','-Ireconstructed/vendor/fatfs','-c','reconstructed/vendor/fatfs/ff.c','-o',str(obj)],cwd=ROOT,check=True)
 _,ss=sections(obj);code={};current=None
 dis=subprocess.check_output([str(compiler).removesuffix('gcc')+'objdump','-d',str(obj)],text=True)
 for line in dis.splitlines():
  m=re.match(r'^0+ <([^>]+)>:',line)
  if m:current=m[1];code[current]=[]
  m=re.match(r'^\s*([0-9a-f]+):\s+[0-9a-f]+\s+(.+)$',line)
  if m and current:code[current].append((int(m[1],16),m[2].strip()))
 functions={f['address']:f for f in json.loads((ROOT/'analysis/functions.json').read_text())};ins=json.loads((ROOT/'analysis/instructions.json').read_text());firmware=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes();rows=[]
 for address,(symbol,abi) in MAPPING.items():
  ranges=[(int(a,16),int(b,16)) for a,b in functions[address]['ranges']];old=[(int(i['address'],16),i['text']) for i in ins if any(a<=int(i['address'],16)<=b for a,b in ranges)]
  name=next((s for s in code if s==symbol),None) or next(s for s in code if s.startswith(symbol+'.'))
  new=code[name];section=ss['.text.'+name];body=b''.join(firmware[a-0x6400:b-0x6400+1] for a,b in ranges);exact=body==section['data']
  rows.append({'address':address,'family':'fatfs','symbol':symbol,'compiled_symbol':name,'classification':'Exact' if exact else 'Strong semantic','original_body_bytes':len(body),'compiled_body_bytes':section['size'],'exact_body_bytes':exact,'operation_sequence_similarity':round(difflib.SequenceMatcher(None,[op(t) for _,t in old],[op(t) for _,t in new],autojunk=False).ratio(),4),'original_cfg':graph(old),'compiled_object_cfg':graph(new),'abi_review':abi,'compiled_source':'reconstructed/vendor/fatfs/ff.c','evidence':['analysis/library_extension_differential.json','reconstructed/tests/filesystem_oracle.c'],'scope':'Documented valid inputs; direct return/memory/path effects tested. Fingerprints alone do not award credit. Object CFG may include unresolved relocations.'})
 p=ROOT/'analysis/library_matches.json';data=json.loads(p.read_text());prior=ROOT/'analysis/phase4/baseline_library_matches.json'
 if not prior.exists():prior.write_text(p.read_text())
 existing={r['address']:r for r in data['new_function_matches']}
 for r in rows:existing[r['address']]=r
 data['new_function_matches']=sorted(existing.values(),key=lambda r:r['address']);data['phase4_extension_evidence']='analysis/library_extension_matches.json'
 p.write_text(json.dumps(data,indent=2)+'\n');(ROOT/'analysis/library_extension_matches.json').write_text(json.dumps({'compiler':str(compiler),'functions':rows,'native_library_sha256':digest,'unqualified_compiled_helpers_credited':False},indent=2)+'\n');print(len(rows),'additional upstream helpers qualified')
if __name__=='__main__':main()

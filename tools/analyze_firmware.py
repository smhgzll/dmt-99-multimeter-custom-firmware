#!/usr/bin/env python3
"""Resumable, hardware-free DMT99 forensics, headless analysis, and verification."""
import argparse
from collections import Counter
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = '4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e'
BASE = 0x6400
BIN = ROOT / 'DMT-99_V2.4_20240306.bin'

def dump(path, value):
    (ROOT/path).write_text(json.dumps(value, indent=2, ensure_ascii=False)+'\n')

def jal_target(word, pc):
    v = ((word>>31)<<20)|(((word>>12)&255)<<12)|(((word>>20)&1)<<11)|(((word>>21)&1023)<<1)
    if v & (1<<20): v -= 1<<21
    return pc+v

def auipc_pair(blob, off, base):
    a,b = struct.unpack_from('<II',blob,off)
    imm = a & 0xfffff000
    if imm & 0x80000000: imm -= 1<<32
    low = b >> 20
    if low & 0x800: low -= 0x1000
    return (base+off+imm+low) & 0xffffffff

def peripherals():
    """Evaluate only SDK base-address macros, never arbitrary C code."""
    header = ROOT/'references/wch-sdk/EVT/EXAM/SRC/Peripheral/inc/ch32v30x.h'
    if not header.exists(): header = ROOT/'references/ch32v30x.h'
    names = {}
    for _ in range(5):
        for name, expr in re.findall(r'^#define\s+(\w+_BASE)\s+([^\n]+)',header.read_text(),re.M):
            expr = expr.split('/*')[0].replace('(uint32_t)','')
            for n,v in sorted(names.items(),key=lambda x:-len(x[0])): expr=re.sub(r'\b'+n+r'\b',str(v),expr)
            if re.fullmatch(r'[\dxa-fA-F()+\s]+',expr):
                names[name] = eval(expr,{'__builtins__':{}},{})
    regs={n.removesuffix('_BASE'):v for n,v in names.items() if 0x40000000<=v<0xf0000000 and 'PERIPH' not in n and n not in ['AHBPERIPH_BASE','APB1PERIPH_BASE','APB2PERIPH_BASE']}
    regs.update(PFIC=0xe000e000,SysTick=0xe000f000)
    dump('analysis/peripheral_bases.json',regs)
    (ROOT/'analysis/mmio_symbols.tsv').write_text(''.join(f'{v:08x}\t{n}\n' for n,v in sorted(regs.items(),key=lambda x:x[1])))

def forensics():
    b = BIN.read_bytes()
    sha = hashlib.sha256(b).hexdigest()
    if sha != EXPECTED: raise RuntimeError('Input hash differs from the supplied firmware; refusing stale analysis')
    # Preserve the original input without writing its contents or making any patched copy.
    BIN.chmod(0o444)
    reset = jal_target(struct.unpack_from('<I',b)[0],BASE)
    rows=[]
    for base in [0,0x6400,0x08000000,0x08006400]:
        gp=auipc_pair(b,0x6268,base); sp=auipc_pair(b,0x6270,base); data=auipc_pair(b,0x6280,base)
        valid=0; loops=0
        for off in range(0x38,0x1d8,4):
            target=struct.unpack_from('<I',b,off)[0]
            file=target-base
            if target and 0x200<=file<0x18420 and file%2==0:
                valid+=1;loops+=b[file:file+2]==b'\x01\xa0'
        rows.append({'base':hex(base),'gp':hex(gp),'sp':hex(sp),'data_vma':hex(data),'vectors_into_code':valid,'vectors_to_self_loops':loops})
    strings=[]
    for m in re.finditer(rb'[\x20-\x7e]{5,}\x00',b):
        if m.start()<0x18420: continue
        strings.append({'offset':hex(m.start()),'address':hex(BASE+m.start()),'value':m.group()[:-1].decode('ascii')})
    dump('analysis/strings.json',strings)
    entropy=[]
    for start in range(0,len(b),1024):
        block=b[start:start+1024];counts=Counter(block)
        entropy.append({'offset':hex(start),'size':len(block),'entropy':round(-sum((n/len(block))*math.log2(n/len(block)) for n in counts.values()),4)})
    dump('analysis/entropy.json',entropy)
    vectors=[]
    startup=ROOT/'references/wch-sdk/EVT/EXAM/SRC/Startup/startup_ch32v30x_D8.S'
    vector_names=[]
    if startup.exists():
        src=startup.read_text().split('_vector_base:')[1].split('.option rvc;')[0]
        vector_names=re.findall(r'^\s*\.word\s+(\w+)',src,re.M)
    for i,off in enumerate(range(0x38,0x1d8,4)):
        target=struct.unpack_from('<I',b,off)[0];f=target-BASE
        vectors.append({'index':i,'slot':hex(BASE+off),'target':hex(target),'sdk_name':vector_names[i] if i<len(vector_names) else None,'kind':'reserved' if not target else 'default_self_loop' if b[f:f+2]==b'\x01\xa0' else 'implemented'})
    dump('analysis/entrypoints.json',{'entry':hex(BASE),'reset':hex(reset),'main':'0x908a','system_init':'0xd212','mtvec':'0x643b','vectors':vectors})
    sections=[
      {'name':'entry_padding','offset':0,'size':0x38,'address':hex(BASE),'kind':'code entry, NOPs, EBREAK'},
      {'name':'vectors','offset':0x38,'size':0x1a0,'address':'0x6438','kind':'104 little-endian pointer slots'},
      {'name':'vector_padding','offset':0x1d8,'size':0x28,'address':'0x65d8','kind':'zero padding'},
      {'name':'text','offset':0x200,'size':0x18220,'address':'0x6600','kind':'code extent; embedded data and undiscovered gaps require coverage report'},
      {'name':'rodata','offset':0x18420,'size':0x20dd0-0x18420,'address':'0x1e820','kind':'jump tables, fonts, strings, constants'},
      {'name':'data_load','offset':0x20dd0,'size':0x1e8,'address':'0x271d0','vma':'0x20000000','kind':'initialized SRAM payload'}]
    assert sum(s['size'] for s in sections)==len(b)
    dump('analysis/firmware_sections.json',sections)
    dump('analysis/memory_map.json',{'sha256':sha,'file_size':len(b),'image_base':hex(BASE),'image_end_exclusive':hex(BASE+len(b)),'flash_alias_base':'0x08006400','candidate_load_addresses':rows,'gp':'0x200009b0','stack_top':'0x20010000','data_vma':'0x20000000','data_end':'0x200001e8','bss_end':'0x200054d8','missing_boot_region':['0x0','0x6400'],'external_flash':{'size_from_user':8388608,'dump_supplied':False}})
    peripherals()
    if shutil.which('binwalk'):
        with (ROOT/'logs/binwalk.log').open('w') as log:
            subprocess.run(['binwalk',str(BIN)],stdout=log,stderr=subprocess.STDOUT,timeout=30,check=False)
    table='\n'.join(f"| {r['base']} | {r['data_vma']} | {r['sp']} | {r['vectors_into_code']} | {r['vectors_to_self_loops']} |" for r in rows)
    report=f'''# Binary forensics

Input: `{BIN.name}`, {len(b)} bytes (`0x{len(b):x}`), SHA-256 `{sha}`.
The original is opened only for reading and marked read-only; no hardware is accessed.

## Verified execution layout

The first word `0x2680606f` is `jal x0,+0x6268`: application entry `0x6400` jumps to `0xc668`.
There is no observed update-container header: executable entry, vector pointers, startup code,
and SRAM initialization payload form a self-consistent raw application image. The lower
`0x6400` bytes of flash are absent; a resident bootloader there is a hypothesis, not a recovered component.

| Candidate base | Data destination from startup | Stack top | Vectors in code | Self-loop handlers |
|---|---|---|---:|---:|
{table}

At base `0x6400`, AUIPC+ADDI sets gp=`0x200009b0`, sp=`0x20010000`,
copies `[0x271d0,0x273b8)` to `[0x20000000,0x200001e8)`, and zeros
`[0x200001e8,0x200054d8)`. It sets vendor CSRs `0xbc0=31`, `0x804=11`,
sets mstatus bits `0x6088`, mtvec=`0x643b`, calls `0xd212`, and uses mepc/mret to enter `0x908a`.
The gp, stack, SRAM copy, and absolute vector pointers jointly reject the other tested bases.
The SDK documents the `0x08000000` flash alias; low-window linkage is demonstrated by this binary.

## Instruction decoding

RV32 little endian, compressed instructions, and integer multiply/divide are present.
No hardware floating-point instructions are observed in recovered code; software floating-point
calls pass values in integer registers, supporting the ilp32 soft-float ABI. WCH Xwchc byte/halfword loads/stores overlap standard RVC double
floating-point instructions. A project-local SLEIGH processor corrects all eight Xwchc forms,
uses the target's 32-bit FPR definitions and selects a soft-float compiler spec. The original installed
Ghidra and firmware bytes are not patched. The decoder definitions follow LLVM PR 97925.

## Data and resources

The code extent ends at `0x1e820` (file `0x18420`); this is an extent, not a count of decoded
executable bytes. The remainder contains lookup/function-pointer tables, bitmap/font-like data,
UI strings, JPEG resource paths, and the SRAM load payload. ASCII strings and 1 KiB entropy
windows are exported in JSON. Many `0:/...jpg` paths indicate resources outside this BIN;
the supplied image alone cannot reconstruct the 8 MiB flash filesystem. No global encryption
is compatible with the verified plaintext executable. No validated image-wide CRC/checksum field
or version/date string has been identified; the filename alone is not proof of embedded version.
Individual compressed resources and font formats still require separate carving/format analysis.

## Primary references

- [WCH SDK, pinned revision](https://github.com/openwch/ch32v307/tree/080af2e52055334eaa6af0f7ad7df527b4ef5a66)
- [LLVM WCH instruction implementation](https://github.com/llvm/llvm-project/pull/97925)
- Local `references/wch-sdk-manifest.json` records downloaded source hashes.
'''
    (ROOT/'analysis/binary_report.md').write_text(report)
    print('Forensics verified: image base 0x6400; reset 0xc668; SRAM data 488 bytes')

def discover_ghidra():
    if os.environ.get('GHIDRA_HOME'): return Path(os.environ['GHIDRA_HOME'])
    candidates=list((Path.home()/'Tools').glob('ghidra_*_PUBLIC'))+list(Path('/opt').glob('ghidra*'))
    return next((p for p in candidates if (p/'support/analyzeHeadless').exists()),None)

def ghidra(force=False, refresh=False):
    from prepare_ghidra import environment,prepare
    home=discover_ghidra()
    if home is None: raise RuntimeError('Ghidra missing: set GHIDRA_HOME to an installation')
    sources=[BIN,*sorted((ROOT/'tools/ghidra').glob('*.java')),ROOT/'tools/prepare_ghidra.py',ROOT/'analysis/semantic_names.tsv',ROOT/'analysis/global_names.tsv',ROOT/'analysis/function_signatures.tsv',ROOT/'analysis/recovered_types.h']
    fingerprint=hashlib.sha256(b''.join(p.read_bytes() for p in sources)).hexdigest()
    cache=ROOT/'.cache/ghidra-input.sha256'
    if not force and not refresh and cache.exists() and cache.read_text()==fingerprint and (ROOT/'analysis/functions.json').exists():
        print('Ghidra analysis unchanged: using cached project and exports');return
    lang_stamp=ROOT/'.cache/language-source.sha256'
    language_hash=hashlib.sha256((ROOT/'tools/prepare_ghidra.py').read_bytes()).hexdigest()
    if not lang_stamp.exists() or lang_stamp.read_text()!=language_hash:
        prepare(home);lang_stamp.write_text(language_hash)
    project=ROOT/'analysis/ghidra_projects';project.mkdir(exist_ok=True)
    cmd=[str(home/'support/analyzeHeadless'),str(project),'DMT99']
    if refresh and (project/'DMT99.gpr').exists(): cmd+=['-process',BIN.name,'-noanalysis']
    else: cmd+=['-import',str(BIN),'-overwrite','-loader','BinaryLoader','-loader-baseAddr',hex(BASE),'-processor','RISCV:LE:32:WCH','-cspec','gcc-soft','-preScript','DmtSetup.java',str(ROOT)]
    cmd+=['-scriptPath',str(ROOT/'tools/ghidra'),'-postScript','DmtRecover.java',str(ROOT),'-postScript','DmtTypes.java',str(ROOT),'-postScript','DmtExport.java',str(ROOT),'-analysisTimeoutPerFile','900','-max-cpu','4','-log',str(ROOT/'logs/ghidra.log'),'-scriptlog',str(ROOT/'logs/ghidra-script.log')]
    with (ROOT/'logs/headless.log').open('w') as log: subprocess.run(cmd,env=environment(),stdout=log,stderr=subprocess.STDOUT,check=True)
    if 'SCRIPT ERROR' in (ROOT/'logs/headless.log').read_text():
        raise RuntimeError('Ghidra script failed; refusing to accept stale/partial exports. Inspect logs/headless.log')
    if not (ROOT/'analysis/decompiler_status.json').exists(): raise RuntimeError('Ghidra did not export results; inspect logs/headless.log')
    cache.write_text(fingerprint)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--phase',choices=['forensics','ghidra','reports','validate','all'],default='all')
    parser.add_argument('--force',action='store_true')
    parser.add_argument('--refresh',action='store_true',help='Re-export existing project after semantic changes')
    args=parser.parse_args()
    for d in ['analysis','disassembly','decompiled/raw','decompiled/clean','docs','.cache','logs']: (ROOT/d).mkdir(parents=True,exist_ok=True)
    if args.phase in ['forensics','all']: forensics()
    if args.phase in ['ghidra','all']: ghidra(args.force,args.refresh)
    if args.phase in ['validate','all']: subprocess.run([sys.executable,str(ROOT/'tools/validate.py')],check=True)
    if args.phase in ['reports','all']:
        from enrich_analysis import enrich
        enrich()

if __name__=='__main__': main()

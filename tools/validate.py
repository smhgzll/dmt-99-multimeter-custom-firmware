#!/usr/bin/env python3
"""Build, test actual recovered behavior against BIN instructions, and audit artifacts."""
import ctypes
import hashlib
import json
from pathlib import Path
import random
import re
import struct
import subprocess
import sys
from analyze_firmware import ROOT,EXPECTED,BIN,discover_ghidra,dump
from prepare_ghidra import environment
from rv32_subset import Machine

def sections(path):
    b=Path(path).read_bytes()
    assert b[:6]==b'\x7fELF\x01\x01'
    assert struct.unpack_from('<H',b,18)[0]==243
    off=struct.unpack_from('<I',b,32)[0]
    size,count,strings=struct.unpack_from('<HHH',b,46)
    heads=[struct.unpack_from('<IIIIIIIIII',b,off+i*size) for i in range(count)]
    h=heads[strings];names=b[h[4]:h[4]+h[5]]
    result={}
    for h in heads:
        end=names.find(b'\0',h[0]);name=names[h[0]:end].decode()
        result[name]={'address':h[3],'size':h[5],'type':h[1],'data':b[h[4]:h[4]+h[5]] if h[1]!=8 else b''}
    return b,result

def differential():
    lib=ctypes.CDLL(str(ROOT/'reconstructed/build/librecovered.so'))
    decode=lib.measurement_segment_decode;decode.argtypes=[ctypes.c_uint8];decode.restype=ctypes.c_uint8
    firmware=BIN.read_bytes();count=0
    for value in range(256):
        original=Machine(firmware).run(0x98aa,value)
        assert decode(value)==original,(value,decode(value),original)
        count+=1
    access=lib.settings_access;access.argtypes=[ctypes.POINTER(ctypes.c_uint16),ctypes.c_int,ctypes.c_uint32,ctypes.c_int16];access.restype=ctypes.c_uint32
    rng=random.Random(20240306);settings_cases=0
    for field,entry in enumerate([0xd944,0xd9e6,0xda04,0xda28]):
        for _ in range(1024):
            word=rng.randrange(65536);write=rng.choice([0,1,2]);value=rng.randrange(4)
            machine=Machine(firmware);machine.write(0x2000003e,2,word)
            expected=machine.run(entry,write,value)
            native_word=ctypes.c_uint16(word);actual=access(ctypes.byref(native_word),field,write,value)
            assert actual==expected,(hex(entry),word,write,value,hex(actual),hex(expected))
            assert native_word.value==machine.read(0x2000003e,2),(hex(entry),word,value)
            settings_cases+=1
    result={'segment_function':'0x98aa','segment_inputs_exhaustive':count,'settings_functions':['0xd944','0xd9e6','0xda04','0xda28'],'settings_cases':settings_cases,'failed':0,'scope':'Original instructions interpreted by fail-closed RV32 subset versus native compiled reconstructed C; no device emulation.'}
    dump('analysis/differential_validation.json',result)
    return result

def decoder_test():
    maximums={'lbu':31,'lhu':62,'sb':31,'sh':62,'lbusp':15,'lhusp':30,'sbsp':15,'shsp':30}
    cases=[];lines=[]
    for name,maximum in maximums.items():
        width=2 if 'h' in name else 1
        offsets=sorted({0,maximum,*[1<<n for n in range(6) if width<=1<<n<=maximum]})
        for off in offsets:
            base='sp' if name.endswith('sp') else 'a1'
            cases.append({'mnemonic':'qk.c.'+name,'offset':off,'base':base,'width':2 if 'h' in name else 1})
            lines.append(f'qk.c.{name} a0, {off}({base})')
    dump('analysis/decoder_vectors.json',cases)
    asm=ROOT/'.cache/decoder-vectors.s';obj=ROOT/'.cache/decoder-vectors.o';raw=ROOT/'.cache/decoder-vectors.bin'
    asm.write_text('\n'.join(lines)+'\n')
    subprocess.run(['clang','--target=riscv32','-march=rv32imafc_xwchc','-c',str(asm),'-o',str(obj)],check=True)
    _,s=sections(obj);raw.write_bytes(s['.text']['data']);assert len(raw.read_bytes())==len(cases)*2
    project=ROOT/'analysis/ghidra_decoder_projects';project.mkdir(exist_ok=True)
    cmd=[str(discover_ghidra()/'support/analyzeHeadless'),str(project),'Decoder','-import',str(raw),'-overwrite','-processor','RISCV:LE:32:WCH','-cspec','gcc-soft','-noanalysis','-scriptPath',str(ROOT/'tools/ghidra'),'-postScript','DmtDecoderTest.java',str(ROOT)]
    status=ROOT/'analysis/decoder_validation.json'
    if status.exists():status.unlink()
    with (ROOT/'logs/decoder-test.log').open('w') as log:subprocess.run(cmd,env=environment(),stdout=log,stderr=subprocess.STDOUT,check=True)
    if not status.exists():raise RuntimeError('Ghidra decoder tests did not finish; see logs/decoder-test.log')
    return json.loads(status.read_text())

def compare_instructions():
    registry=json.loads((ROOT/'analysis/semantic_registry.json').read_text())
    functions={f['address']:f for f in json.loads((ROOT/'analysis/functions.json').read_text())}
    firmware=BIN.read_bytes();rows=[]
    for a,record in registry.items():
        name=record['name'];prefix=name.split('_')[0].lower()
        if prefix not in ['gpio','spi','dma','usart','tim','adc','flash','rcc','exti']:continue
        object_path=ROOT/f'reconstructed/build/ch32v30x_{prefix}.o'
        if not object_path.exists():continue
        _,ss=sections(object_path);section=ss.get('.text.'+name)
        if section is None:continue
        f=functions[a]
        if len(f['ranges'])!=1:continue
        off=int(a,16)-0x6400;original=firmware[off:off+f['size']]
        rows.append({'address':a,'name':name,'original_size':len(original),'sdk_gcc_size':section['size'],'byte_identical':original==section['data'],'scope':'Function-body bytes only; relocation-bearing and nonidentical functions require further analysis.'})
    dump('analysis/instruction_comparison.json',{'compiler':'GCC 14.2.0 Espressif RV32; standard RV32IMAC output, not vendor Xwchc generation','flags':'-Os -march=rv32imac_zicsr_zifencei -mabi=ilp32','compared':len(rows),'byte_identical':sum(r['byte_identical'] for r in rows),'functions':rows})
    return len(rows),sum(r['byte_identical'] for r in rows)

def main():
    assert hashlib.sha256(BIN.read_bytes()).hexdigest()==EXPECTED
    build=ROOT/'reconstructed/build'
    with (ROOT/'logs/build.log').open('w') as log:
        subprocess.run(['make','-C',str(ROOT/'reconstructed'),'-B','-j4','all','host-test'],stdout=log,stderr=subprocess.STDOUT,check=True)
    first=hashlib.sha256((build/'reconstruction.elf').read_bytes()).hexdigest()
    with (ROOT/'logs/reproducible-build.log').open('w') as log:
        subprocess.run(['make','-C',str(ROOT/'reconstructed'),'-B','-j4','all'],stdout=log,stderr=subprocess.STDOUT,check=True)
    second=hashlib.sha256((build/'reconstruction.elf').read_bytes()).hexdigest();assert first==second
    elf,sec=sections(build/'reconstruction.elf');entry=struct.unpack_from('<I',elf,24)[0]
    assert entry==0x6400 and sec['.vector']['address']==0x6438
    assert sec['.text']['address']==0x6600
    assert sec['.stack']['address']+sec['.stack']['size']==0x20010000
    assert sec['.bss']['address']+sec['.bss']['size']<=sec['.stack']['address']
    native=subprocess.run([str(build/'test_recovered')],capture_output=True,text=True,check=True)
    cases=len(re.findall(r'^PASS ',native.stdout,re.M));(ROOT/'logs/host-tests.log').write_text(native.stdout)
    dif=differential();decoder=decoder_test();compared,identical=compare_instructions()
    functions=json.loads((ROOT/'analysis/functions.json').read_text())
    instructions=json.loads((ROOT/'analysis/instructions.json').read_text())
    covered=set();duplicates=0
    for i in instructions:
        a=int(i['address'],16)
        for p in range(a,a+i['length']):
            if p in covered:duplicates+=1
            covered.add(p)
    assert duplicates==0,'Overlapping function bodies need review'
    assert all(int(f['address'],16)<0x1e820 for f in functions),'Data incorrectly classified as function'
    assert all((ROOT/f['pseudocode_file']).exists() for f in functions if f['decompiled'])
    result={'input_sha256':EXPECTED,'host_tests_passed':cases,'host_tests_failed':0,'differential_cases':dif['segment_inputs_exhaustive']+dif['settings_cases'],'decoder_vectors_passed':decoder['passed'],'sdk_functions_compared':compared,'sdk_functions_byte_identical':identical,'cross_build':'passed','linker_assertions':'passed','reproducible_elf':first==second,'elf_sha256':second,'entry':hex(entry),'vector':hex(sec['.vector']['address']),'firmware_equivalent':False,'hardware_validated':False,'compiled_reconstructed_modules':['measurement_asic','spi_flash','display_bus','buttons','history','settings','foreground','wch_platform','application_harness'],'sections':{n:{k:v for k,v in s.items() if k!='data'} for n,s in sec.items() if n in ['.init','.vector','.text','.data','.bss','.stack']}}
    dump('analysis/validation_results.json',result)
    print(json.dumps(result,indent=2))

if __name__=='__main__':main()

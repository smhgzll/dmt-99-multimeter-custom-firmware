#!/usr/bin/env python3
"""Build a project-local RV32IMAF+C+Xwchc SLEIGH variant without changing Ghidra."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def environment():
    env = os.environ.copy()
    env['XDG_CONFIG_HOME'] = str(ROOT / '.cache/config')
    env['XDG_CACHE_HOME'] = str(ROOT / '.cache/cache')
    # JAVA_HOME may be supplied by the caller; no user home is repurposed.
    if not env.get('JAVA_HOME'):
        for p in (Path.home()/'.local/share/JetBrains/Toolbox/apps').glob('*/jbr'):
            if (p / 'bin/javac').exists():
                env['JAVA_HOME'] = str(p)
                break
    env['JAVA_TOOL_OPTIONS'] = env.get('JAVA_TOOL_OPTIONS', '') + ' -Dghidra.external.modules=' + str(ROOT / 'tools/ghidra_wch')
    return env

def prepare(ghidra):
    source = ghidra / 'Ghidra/Processors/RISCV/data/languages'
    module = ROOT / 'tools/ghidra_wch'
    dest = module / 'data/languages'
    dest.mkdir(parents=True, exist_ok=True)
    (module / 'Module.manifest').write_text('')
    for p in source.iterdir():
        if p.suffix in ('.sinc', '.cspec', '.pspec'):
            shutil.copyfile(p, dest / p.name)
    # The upstream default spec assumes 64-bit FPRs; QingKe V4F has 32-bit FPRs.
    fp = dest / 'riscv32-fp.cspec'
    fp.write_text(fp.read_text().replace('minsize="4" maxsize="8" storage="float"', 'minsize="4" maxsize="4" storage="float"'))
    # GCC out-of-line prologues return through t0 and preserve all argument registers.
    # Model their actual stack stores, rather than treating them as ordinary C calls.
    fixups = ''
    for name, size, registers, initial in [
        ('wch_save_12',64,['s11','s10','s9','s8','s7','s6','s5','s4','s3','s2','s1','s0','ra'],12),
        ('wch_save_10',48,['s10','s9','s8','s7','s6','s5','s4','s3','s2','s1','s0','ra'],0),
        ('wch_save_6',32,['s6','s5','s4','s3','s2','s1','s0','ra'],0),
        ('wch_save_2',16,['s2','s1','s0','ra'],0)]:
        body = f'sp = sp - {size};\n'
        for i, reg in enumerate(registers):
            body += f'local saveaddr{i}:4 = sp + {initial+i*4}; *[ram]:4 saveaddr{i} = {reg};\n'
        fixups += f'<callfixup name="{name}"><pcode><body><![CDATA[{body}]]></body></pcode></callfixup>\n'
    fp.write_text(fp.read_text().replace('</compiler_spec>',fixups+'</compiler_spec>'))
    soft = dest / 'riscv32.cspec'
    soft.write_text(soft.read_text().replace('</compiler_spec>',fixups+'</compiler_spec>'))
    (dest / 'wch.ldefs').write_text('''<?xml version="1.0" encoding="UTF-8"?>
<language_definitions><language processor="RISCV" endian="little" size="32" variant="WCH" version="1.0" slafile="wch.sla" processorspec="RV32.pspec" id="RISCV:LE:32:WCH">
<description>WCH QingKe RV32 with Xwchc (firmware uses soft-float ABI)</description>
<compiler name="gcc" spec="riscv32-fp.cspec" id="gcc"/>
<compiler name="gcc-soft" spec="riscv32.cspec" id="gcc-soft"/>
</language></language_definitions>\n''')
    spec = (source / 'riscv.ilp32d.slaspec').read_text().replace('@define FLEN 8', '@define FLEN 4').replace('@define FPSIZE "64"', '@define FPSIZE "32"')
    spec += '\n@include "wch.sinc"\n'
    (dest / 'wch.slaspec').write_text(spec)
    # Encoding is independently documented by LLVM PR 97925; see references/llvm-xwchc-pr.json.
    body = '''# Xwchc unsigned offsets, matching LLVM RISCVInstrInfoXwch.td.
wchb: uimm is cop1212 & cop1011 & cop0506 [ uimm = cop1212 | (cop1011 << 3) | (cop0506 << 1); ] { local tmp:4 = uimm; export tmp; }
wchh: uimm is cop1012 & cop0506 [ uimm = (cop1012 << 3) | (cop0506 << 1); ] { local tmp:4 = uimm; export tmp; }
wchsb: uimm is cop0707 & cop0808 & cop0910 [ uimm = (cop0707 | (cop0808 << 1) | (cop0910 << 2)); ] { local tmp:4 = uimm; export tmp; }
wchsh: uimm is cop0707 & cop0808 & cop0910 [ uimm = (cop0707 << 4) | (cop0808 << 1) | (cop0910 << 2); ] { local tmp:4 = uimm; export tmp; }
'''
    for name, quad, kind, width in [('lbu', 0, 1, 1), ('sb', 0, 5, 1), ('lhu', 2, 1, 2), ('sh', 2, 5, 2)]:
        off = 'wchb' if width == 1 else 'wchh'
        action = f'cr0204s = zext(*[ram]:{width} ea);' if name.startswith('l') else f'*[ram]:{width} ea = cr0204s:{width};'
        body += f':qk.c.{name} cr0204s,{off}(cr0709s) is cr0204s & cr0709s & cop0001={quad} & cop1315={kind} & {off} {{ local ea:4 = cr0709s + {off}; {action} }}\n'
    for name, kind, width in [('lbusp', 0, 1), ('lhusp', 1, 2), ('sbsp', 2, 1), ('shsp', 3, 2)]:
        off = 'wchsb' if width == 1 else 'wchsh'
        action = f'cr0204s = zext(*[ram]:{width} ea);' if name.startswith('l') else f'*[ram]:{width} ea = cr0204s:{width};'
        body += f':qk.c.{name} cr0204s,{off}(sp) is cr0204s & sp & cop0001=0 & cop1112=0 & cop1315=4 & cop0506={kind} & {off} {{ local ea:4 = sp + {off}; {action} }}\n'
    (dest / 'wch.sinc').write_text(body)
    subprocess.run([str(ghidra / 'support/sleigh'), str(dest / 'wch.slaspec')], env=environment(), check=True)
    return environment()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('ghidra', type=Path)
    prepare(parser.parse_args().ghidra)

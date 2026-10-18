#!/usr/bin/env python3
"""Check the stock header convention and independently inspect ELF allocations."""
import argparse,hashlib,json,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.dont_write_bytecode=True
sys.path.insert(0,str(ROOT/'tools/reference'))
from update_image_format import check

def inspect(cross):
    image=(ROOT/'build/dmt99-doom.bin').read_bytes()
    original=(ROOT.parents[1]/'DMT-99_V2.4_20240306.bin').read_bytes()
    version=re.search(r'#define DOOM_VERSION "([^"]+)"',(ROOT/'src/version.h').read_text()).group(1)
    errors,fmt=check(image,original,version+'-doom')
    nm=subprocess.check_output([cross+'nm','-n',str(ROOT/'build/dmt99-doom.elf')],text=True)
    symbols={line.split()[2]:int(line.split()[0],16) for line in nm.splitlines() if len(line.split())==3}
    undefined=subprocess.check_output([cross+'nm','-u',str(ROOT/'build/dmt99-doom.elf')],text=True).strip()
    if undefined:errors.append('undefined symbols: '+undefined)
    for forbidden in ['FLASH_ProgramHalfWord','FLASH_ErasePage','FLASH_Unlock','SPI_I2S_SendData','malloc','printf','framebuffer']:
        if forbidden in symbols:errors.append('forbidden linked symbol '+forbidden)
    if symbols['_eusrstack']!=0x20008000:errors.append('stack top must be 32K SRAM')
    if symbols['_ebss']>symbols['_susrstack']:errors.append('static RAM/stack overlap')
    # All emitted load bytes are before 0x30000, not a 192K payload after 0x6400.
    if 0x6400+len(image)>0x30000:errors.append('alias code bound exceeded')
    report={'pass':not errors,'errors':errors,'format':fmt,'sha256':hashlib.sha256(image).hexdigest(),
            'ram_static_bytes':symbols['_ebss']-0x20000000,'stack_reserved_bytes':symbols['_eusrstack']-symbols['_susrstack'],
            'ram_gap_bytes':symbols['_susrstack']-symbols['_ebss'],
            'settings_page_written_by_application':False,'external_flash_used':False,
            'hardware_verified':False,'bootloader_acceptance_verified':False}
    (ROOT/'build/image_checks.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report));return not errors
if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--cross',required=True);a=ap.parse_args()
    raise SystemExit(0 if inspect(a.cross) else 1)

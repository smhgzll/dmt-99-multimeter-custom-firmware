#!/usr/bin/env python3
"""Incremental update-format audit using preserved exports; no Ghidra rerun.
No hardware access or mutation. Reports observations separately from unknown
updater policy. The firmware BIN remains a read-only input.
"""
import hashlib
import json
import struct
import zlib
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def main():
    p = ROOT/'DMT-99_V2.4_20240306.bin';b = p.read_bytes()
    assert sha(p) == '4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e'
    fs = json.loads((ROOT/'analysis/functions.json').read_text())
    ins = json.loads((ROOT/'analysis/instructions.json').read_text())
    selected = {0x6400, 0xc668, 0xc320, 0xd87c, 0xd908, 0xd928, 0xc982, 0xc99c,
                0xc9aa, 0xc9b2, 0xc9ca, 0xc9ee, 0xca26, 0xca44, 0xca52, 0x1a682, 0x1a6e2}
    rows = []
    for i in ins:
        if int(i.get('function', '0'), 16) in selected:
            a = int(i['address'], 16);raw = b[a-0x6400:a-0x6400+i['length']]
            rows.append({'address': i['address'], 'function': i['function'],
                         'bytes': raw.hex(), 'text': i['text']})
    word = struct.unpack_from('<I', b)[0]
    assert word & 0x7f == 0x6f and (word>>7)&31 == 0
    jump = ((word>>31)<<20)|((word>>12 & 255)<<12)|((word>>20 & 1)<<11)|((word>>21 & 1023)<<1)
    if jump & (1<<20):jump -= 1<<21
    assert 0x6400+jump == 0xc668
    assert 0x271d0+488 == 0x6400+len(b)
    areas = [('missing_prefix',0,0x6400,False),('entry_padding',0x6400,0x6438,True),
             ('vectors_padding',0x6438,0x6600,True),('text',0x6600,0x1e820,True),
             ('rodata',0x1e820,0x271d0,True),('initialized_SRAM_load',0x271d0,0x273b8,True)]
    regions = [{'name':n,'link_start':hex(s),'link_end_exclusive':hex(e),
                'physical_start':hex(0x08000000+s),'physical_end_exclusive':hex(0x08000000+e),
                'bytes':e-s,'present_in_update':present} for n,s,e,present in areas]
    fl = [{'entry':f['address'],'name':f['name'],'callers':f['callers'],
           'ranges':f['ranges']} for f in fs if f['name'].startswith('FLASH_') or int(f['address'],16) in (0xc320,0xd87c,0xd928,0xd908)]
    calls = [i for i in rows if i['text'].startswith(('jal ','j ')) and i['function'] in ('0x0000d87c','0x0000d928','0x0000c320')]
    source_files = ['analysis/functions.json','analysis/instructions.json','analysis/callgraph.json',
                    'analysis/hardware_interactions.json','analysis/usb_descriptor_report.json',
                    'analysis/firmware_differences.json','analysis/source_coverage.json']
    report = {'input':{'path':p.name,'bytes':len(b),'sha256':sha(p),'crc32':f'{zlib.crc32(b):08x}'},
        'regions':regions,'entry':{'offset':0,'bytes':b[:4].hex(),'jump_target_link':hex(0x6400+jump),
         'jump_target_physical':hex(0x08006400+jump),'no_prefix_container_header':True},
        'startup':{'gp_link_execution':'0x200009b0','gp_if_executed_at_high_alias':'0x280009b0',
          'stack_top':'0x20010000','data_source_link':'0x271d0','data_destination':'0x20000000',
          'data_bytes':488,'bss_start':'0x200001e8','bss_end':'0x200054d8','mtvec':'0x643b',
          'mepc':'0x908a','separate_trailing_bytes_after_data':0},
        'integrity_observations':{'last_32_bytes':b[-32:].hex(),
          'last_u32_matches_crc32_of_preceding_bytes':b[-4:]==struct.pack('<I',zlib.crc32(b[:-4])),
          'scope':'Only this explicit CRC32 trailer hypothesis checked; arbitrary embedded CRC/signature/version checks remain unproven without updater.',
          'version_string':{'address':'0x24ba0','value':'DMT-99 V2.4','caller':'0x143e0','role':'UI text, not demonstrated acceptance check'}},
        'internal_flash_functions':fl,'settings_call_sites':calls,
        'settings':{'read_entry':'0xd928','copy_entry':'0xd908','commit_entry':'0xd87c',
         'erase_call_site':'0xd8ac','erase_entry':'0xca52','erase_address':'0x08040000',
         'erase_bytes':256,'program_call_site':'0xd8f8','program_entry':'0xc9ee',
         'program_start':'0x08040000','program_end_exclusive':'0x08040100',
         'source':'0x20000020','max_halfwords':128,'continues_only_while_status':4,
         'caller':'power_down 0xc320','identified_as_firmware_updater':False},
        'update_policy':{'updater_identified':False,'first_25KiB_preserved_during_update':'unknown',
          'USB_MSC_identified':False,'bootloader_handoff_identified':False,
          'BOOT_directory':'0xdd4e makes directory 0:BOOT with FatFs 0xfb48; not a code handoff',
          'erase_geometry_counterexamples':[
             {'erase_bytes':256,'rounded_start':'0x08006400','prefix_overlap_bytes':0},
             {'erase_bytes':4096,'rounded_start':'0x08006000','prefix_overlap_bytes':1024},
             {'erase_bytes':32768,'rounded_start':'0x08000000','prefix_overlap_bytes':25600}],
          'warning':'Examples only; no actual updater erase policy recovered. RDP auto-write-protects first 4KiB, not first 25KiB.'},
        'instructions':rows,'sources':{s:sha(ROOT/s) for s in source_files},
        'no_hardware_access':True,'no_payload_or_patch_generated':True}
    out=ROOT/'analysis/firmware_update_evidence.json'
    out.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:report[k] for k in ['input','entry','settings','update_policy']},indent=2))


if __name__=='__main__':main()

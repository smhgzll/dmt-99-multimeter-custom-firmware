#!/usr/bin/env python3
"""Bounded raw carving; a carved extent is never called a live filesystem file."""
from pathlib import Path
import json,struct,io,collections
from flash_forensics import ROOT,sha,save,u16,u32

def jpeg_extent(b,start):
    p=start+2
    while p+2<=len(b):
        if b[p]!=255:raise ValueError('JPEG marker expected')
        while p<len(b) and b[p]==255:p+=1
        if p==len(b):raise ValueError('truncated marker')
        tag=b[p];p+=1
        if tag==0xd9:return p-start
        if tag==0 or tag==0xd8:raise ValueError('unexpected marker')
        if 0xd0<=tag<=0xd7:continue
        if p+2>len(b):raise ValueError('truncated size')
        n=int.from_bytes(b[p:p+2],'big')
        if n<2 or p+n>len(b):raise ValueError('invalid segment size')
        p+=n
        if tag==0xda:
            while p<len(b):
                q=b.find(b'\xff',p)
                if q<0 or q+1==len(b):raise ValueError('truncated entropy')
                if b[q+1]==0 or 0xd0<=b[q+1]<=0xd7:p=q+2;continue
                p=q;break
    raise ValueError('missing EOI')

def rv_imm(w):
    x=((w>>31)<<20)|(((w>>12)&255)<<12)|(((w>>20)&1)<<11)|(((w>>21)&1023)<<1)
    return x-(1<<21) if x&(1<<20) else x

def main():
    b=(ROOT/'flashdump/dmt99_flash_1.bin').read_bytes();o=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes();files=json.loads((ROOT/'analysis/flash_files.json').read_text())['active']
    by_offset={f['physical_extents'][0]['offset']:f for f in files if f['physical_extents']}
    carved=[];p=0
    while (p:=b.find(b'\xff\xd8\xff',p))>=0:
        try:
            size=jpeg_extent(b,p)
            from PIL import Image
            im=Image.open(io.BytesIO(b[p:p+size]));im.load()
            row={'offset':p,'size':size,'sha256':sha(b[p:p+size]),'width':im.width,'height':im.height,'live_file':by_offset.get(p,{}).get('path'),'status':'decoded raw extent; allocation status separate'}
            if not row['live_file']:
                target=ROOT/'extracted_flash/carved'/f'{p:08x}.jpg';target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(b[p:p+size]);row['extracted_path']=str(target.relative_to(ROOT))
            carved.append(row)
        except Exception:pass
        p+=3
    for x in carved:
        containers=[y for y in carved if y['offset']<x['offset'] and x['offset']+x['size']<=y['offset']+y['size']]
        x['nested_in_offset']=min(containers,key=lambda y:y['size'])['offset'] if containers else None
    save('flash_carved_images.json',carved)
    # The complete surviving application has entry/vector/reset consistency.
    # Reset AUIPC+ADDI at c66c loads ROM 27194; data length is 488; end274? =2737c.
    start=0x4d000
    reset=0x6400+rv_imm(u32(b,start))
    reset_off=start+reset-0x6400
    def pair(pc,off):
        hi=u32(b,off)&0xfffff000;lo=u32(b,off+4)>>20
        if lo&0x800:lo-=0x1000
        return (pc+hi+lo)&0xffffffff
    gp=pair(reset,reset_off)
    rom_data=pair(reset+16,reset_off+16)
    ram_start=pair(reset+24,reset_off+24)
    end_word=u32(b,reset_off+32);end_lo=end_word>>20
    if end_lo&0x800:end_lo-=0x1000
    ram_end=(gp+end_lo)&0xffffffff
    assert ram_start==0x20000000 and ram_end-ram_start==488
    size=rom_data+(ram_end-ram_start)-0x6400
    assert size==135036
    payload=b[start:start+size]
    target=ROOT/'extracted_flash/carved/application_0004d000.bin';target.write_bytes(payload)
    candidates=[{'id':'carved_rv32_application','offset':start,'size':size,'sha256':sha(payload),'path':str(target.relative_to(ROOT)),'format':'raw RV32 application with WCH Xwchc','load_address':0x6400,'entry_target':0x6400+rv_imm(u32(payload,0)),'vector_offset':0x38,'extent_evidence':['Reset at c65c copies 488 initialized bytes from ROM27194; derived logical end2737c, length135036.','135036 is also a deleted BIN directory size; no surviving allocation chain proves its filename.','Reset target, vector handler pointers and shared original application/library bodies are coherent.'],'extent_derivation':{'reset':reset,'gp':gp,'rom_data':rom_data,'ram_start':ram_start,'ram_end':ram_end,'logical_size':size},'live_file':False,'usb_stack_present':'no descriptors or USB MMIO identified in this carved application'}]
    # PE raw-file sizes derive from PE/COFF section table, not signature alone.
    p=0
    while (p:=b.find(b'MZ',p))>=0:
        try:
            pe=p+u32(b,p+0x3c)
            if b[pe:pe+4]!=b'PE\0\0':raise ValueError()
            machine=u16(b,pe+4);n=u16(b,pe+6);optional=u16(b,pe+20);table=pe+24+optional
            if not 0<n<96 or table+40*n>len(b):raise ValueError()
            sections=[];extent=u32(b,pe+24+60)
            for i in range(n):
                e=table+40*i;sz=u32(b,e+16);off=u32(b,e+20);extent=max(extent,off+sz)
                sections.append({'name':b[e:e+8].rstrip(b'\0').decode('ascii',errors='replace'),'raw_offset':off,'raw_size':sz})
            if extent<=0 or p+extent>len(b):raise ValueError()
            data=b[p:p+extent];path=ROOT/'extracted_flash/carved'/f'{p:08x}.pe';path.write_bytes(data)
            candidates.append({'id':f'pe_{p:08x}','offset':p,'size':extent,'sha256':sha(data),'path':str(path.relative_to(ROOT)),'machine':machine,'format':'PE/COFF','sections':sections,'extent_evidence':'Maximum section raw extent and SizeOfHeaders; overlays/signatures not inferred.','classification':'x86 host executable residue, not RISC-V device firmware','live_file':False})
        except (ValueError,struct.error):pass
        p+=2
    save('embedded_firmware_inventory.json',{'candidates':candidates,'carved_jpeg_count':len(carved),'non_live_jpeg_extents':sum(not x['live_file'] for x in carved),'nested_jpeg_extents':sum(x['nested_in_offset'] is not None for x in carved),'top_level_jpeg_extents':sum(x['nested_in_offset'] is None for x in carved),'deleted_metadata_is_not_live_files':True})
    # Exact whole-body searches distinguish code from embedded constants.
    matches=[]
    for f in json.loads((ROOT/'analysis/functions.json').read_text()):
        if len(f['ranges'])!=1 or f['size']<16:continue
        a=int(f['address'],16);end=int(f['ranges'][0][1],16) if isinstance(f['ranges'][0][1],str) else f['ranges'][0][1]
        sig=o[a-0x6400:end-0x6400+1];q=payload.find(sig)
        if q>=0:matches.append({'original_entry':f['address'],'candidate_address':0x6400+q,'bytes':len(sig),'evidence':'entire exported contiguous machine-code body is byte-identical; external targets still require relocation review'})
    prefix=[];fs=json.loads((ROOT/'analysis/functions.json').read_text())
    for f in fs:
        a=int(f['address'],16)
        if f['size']<32:continue
        q=payload.find(o[a-0x6400:a-0x6400+32])
        if q>=0:prefix.append({'original_entry':f['address'],'candidate_address':0x6400+q,'bytes':32})
    save('firmware_differences.json',{'original_sha256':sha(o),'candidate_sha256':sha(payload),'identical_files':False,'whole_contiguous_code_body_matches':matches,'prefix_leads_only':prefix,'original_initialized_data':{'rom_address':0x271d0,'size':488},'candidate_initialized_data':{'rom_address':0x27194,'size':488},'initialized_data_identical':o[0x271d0-0x6400:][:488]==payload[0x27194-0x6400:][:488],'version':'unknown; deleted filenames are not sufficient version attribution','equivalence':False,'limitations':'Only exact whole-body and prefix searches are reported. No CFG equivalence or firmware revision equivalence is inferred. Broad normalized candidate decoding remains future work.'})
    print({'images':len(carved),'candidates':len(candidates),'whole_body_code_matches':len(matches)})
if __name__=='__main__':main()

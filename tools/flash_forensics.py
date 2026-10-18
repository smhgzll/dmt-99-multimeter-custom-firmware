#!/usr/bin/env python3
"""Read-only FAT12/16 forensic inventory. Never opens evidence for writing.

Deleted entries are leads; cleared chains are never guessed as valid files.
All offsets are absolute byte offsets in the acquired SPI image.
"""
from pathlib import Path
import hashlib, json, math, struct, io

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = 'fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69'

def sha(b): return hashlib.sha256(b).hexdigest()
def u16(b, p): return struct.unpack_from('<H', b, p)[0]
def u32(b, p): return struct.unpack_from('<I', b, p)[0]
def save(name, obj):
    (ROOT/'analysis'/name).write_text(json.dumps(obj, indent=2, ensure_ascii=False)+'\n')

def sfn_checksum(b):
    s=0
    for c in b: s=(((s&1)<<7)+(s>>1)+c)&255
    return s

def lfn_text(entries):
    chars=[]
    for e in reversed(entries):
        chars.extend(u16(e,p) for p in [1,3,5,7,9,14,16,18,20,22,24,28,30])
    chars=chars[:chars.index(0)] if 0 in chars else chars
    return b''.join(struct.pack('<H',c) for c in chars if c!=65535).decode('utf-16le',errors='replace')

class FAT:
    def __init__(self, image, offset, length, index, extract=False):
        self.image=image;self.offset=offset;self.length=length;self.index=index;self.extract=extract
        b=image[offset:offset+512]
        if len(b)!=512 or b[510:]!=b'\x55\xaa': raise ValueError('boot signature')
        self.bps=u16(b,11);self.spc=b[13];self.reserved=u16(b,14);self.nfat=b[16]
        self.nroot=u16(b,17);self.total=u16(b,19) or u32(b,32);self.fatsz=u16(b,22)
        if self.bps not in [512,1024,2048,4096] or not self.spc or self.spc&(self.spc-1): raise ValueError('sector/cluster geometry')
        if not self.reserved or not self.nfat or not self.fatsz: raise ValueError('unsupported/invalid FAT geometry')
        self.root_sectors=(self.nroot*32+self.bps-1)//self.bps
        self.data_sector=self.reserved+self.nfat*self.fatsz+self.root_sectors
        if self.total<=self.data_sector or self.total*self.bps>length or offset+length>len(image):raise ValueError('volume bounds')
        self.clusters=(self.total-self.data_sector)//self.spc;self.bits=12 if self.clusters<4085 else 16
        if self.clusters>=65525:raise ValueError('FAT32 unsupported')
        self.cluster_bytes=self.bps*self.spc
        fat_offset=offset+self.reserved*self.bps
        self.fat=image[fat_offset:fat_offset+self.fatsz*self.bps]
        if len(self.fat)*8//self.bits<self.clusters+2:raise ValueError('FAT too small')
        self.copies_match=all(self.fat==image[fat_offset+i*len(self.fat):fat_offset+(i+1)*len(self.fat)] for i in range(self.nfat))
        self.root_offset=offset+(self.reserved+self.nfat*self.fatsz)*self.bps
        self.files=[];self.deleted=[];self.dirs=[];self.errors=[];self.owners={};self.visited=set()
    def value(self,c):
        if not 0<=c<self.clusters+2:raise ValueError('cluster outside FAT')
        p=c+c//2 if self.bits==12 else c*2
        v=u16(self.fat,p)
        return ((v>>4) if c&1 else v&4095) if self.bits==12 else v
    def cluster_offset(self,c):
        if not 2<=c<self.clusters+2:raise ValueError('cluster outside volume')
        return self.offset+self.data_sector*self.bps+(c-2)*self.cluster_bytes
    def chain(self,c):
        result=[];seen=set();eoc=0xff8 if self.bits==12 else 0xfff8
        if c==0:return [],None
        while True:
            if c in seen:return result,'cycle'
            if not 2<=c<self.clusters+2:return result,'out of range'
            seen.add(c);result.append(c);v=self.value(c)
            if v>=eoc:return result,None
            if v<2:return result,'free/reserved cluster in chain'
            if v==(0xff7 if self.bits==12 else 0xfff7):return result,'bad cluster'
            c=v
    def content(self,chain):
        return b''.join(self.image[self.cluster_offset(c):self.cluster_offset(c)+self.cluster_bytes] for c in chain)
    def claim(self,chain,path):
        for c in chain:self.owners.setdefault(c,[]).append(path)
    def walk(self,c=0,parent=''):
        if c:
            if c in self.visited:self.errors.append({'path':parent,'error':'directory cycle/alias'});return
            self.visited.add(c);chain,error=self.chain(c);self.claim(chain,parent+'/')
            if error:self.errors.append({'path':parent,'error':error});return
            data=self.content(chain)
            offsets=[self.cluster_offset(x)+i for x in chain for i in range(0,self.cluster_bytes,32)]
        else:
            data=self.image[self.root_offset:self.root_offset+self.nroot*32]
            offsets=list(range(self.root_offset,self.root_offset+len(data),32))
        lfn=[]
        for i,pos in enumerate(offsets):
            e=data[i*32:i*32+32]
            if e[0]==0:break
            if e[11]==15:lfn.append(e);continue
            deleted=e[0]==229
            raw=e[:11]; display_raw=(b'?'+raw[1:]) if deleted else ((b'\xe5'+raw[1:]) if raw[0]==5 else raw);base=display_raw[:8].decode('cp936',errors='replace').rstrip();ext=display_raw[8:].decode('cp936',errors='replace').rstrip()
            short=base+('.'+ext if ext else '')
            if deleted:short='?'+short[1:]
            valid_lfn=(lfn and not deleted and len(lfn)==(lfn[0][0]&31) and bool(lfn[0][0]&64)
                and all(x[0]==(len(lfn)-j | (64 if j==0 else 0)) and x[13]==sfn_checksum(raw) and x[12]==0 and u16(x,26)==0 for j,x in enumerate(lfn)))
            name=lfn_text(lfn) if valid_lfn else short
            old_name=lfn_text(lfn) if lfn and deleted else None
            lfn=[]
            if e[11]&8:continue
            if name in ['.','..']:continue
            path=parent+'/'+name if parent else name
            start=u16(e,26);size=u32(e,28)
            row={'volume':self.index,'path':path,'short_name':short,'directory_entry_offset':pos,'attributes':e[11], 'start_cluster':start,'size':size,'lfn_valid':bool(valid_lfn)}
            if deleted:
                row.update({'state':'deleted','lfn_fragment':old_name,'chain_status':self.chain(start)[1] if start else 'empty','recovery':'not extracted; metadata alone does not establish surviving contents'})
                self.deleted.append(row);continue
            if '/' in name or '\\' in name or name in ['', '.', '..']:
                self.errors.append({'path':path,'error':'unsafe name'});continue
            chain,error=self.chain(start)
            if not(e[11]&16) and len(chain)*self.cluster_bytes<size:error=error or 'chain shorter than file'
            row.update({'clusters':chain,'chain_error':error,'physical_extents':self.extents(chain,size if not(e[11]&16) else None)})
            if error:self.errors.append({'path':path,'error':error})
            if e[11]&16:
                self.dirs.append(row)
                if not error and start:self.walk(start,path)
                elif not start:self.errors.append({'path':path,'error':'zero directory cluster'})
            else:
                self.claim(chain,path)
                if not error:
                    payload=self.content(chain)[:size]
                    row['sha256']=sha(payload)
                    row['kind']='JPEG' if payload.startswith(b'\xff\xd8') else 'data'
                    if row['kind']=='JPEG':
                        from PIL import Image
                        try:
                            im=Image.open(io.BytesIO(payload));row['image']={'width':im.width,'height':im.height,'mode':im.mode};im.load();row['image']['decode']='pass'
                        except Exception as ex:row['image']={'decode':'error','error':str(ex)}
                    target=ROOT/'extracted_flash'/f'volume{self.index}'/path
                    
                    if self.extract:
                        target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(payload)
                    row['extracted_path']=str(target.relative_to(ROOT))
                self.files.append(row)
    def extents(self,chain,size=None):
        result=[];remaining=size if size is not None else len(chain)*self.cluster_bytes
        for c in chain:
            n=min(self.cluster_bytes,remaining);p=self.cluster_offset(c)
            if n<=0:break
            if result and result[-1]['offset']+result[-1]['size']==p:result[-1]['size']+=n
            else:result.append({'offset':p,'size':n})
            remaining-=n
        return result
    def report(self):
        self.walk()
        allocated=[c for c in range(2,self.clusters+2) if self.value(c)!=0]
        orphan=[c for c in allocated if c not in self.owners]
        return {'index':self.index,'offset':self.offset,'size':self.length,'fat_bits':self.bits,'bytes_per_sector':self.bps,'sectors_per_cluster':self.spc,'reserved_sectors':self.reserved,'fat_copies':self.nfat,'fat_sectors':self.fatsz,'root_entries':self.nroot,'root_offset':self.root_offset,'data_offset':self.offset+self.data_sector*self.bps,'total_sectors':self.total,'data_clusters':self.clusters,'fat_copies_identical':self.copies_match,'boot_signature_valid':True,'active_files':len(self.files),'active_directories':len(self.dirs),'deleted_entries':len(self.deleted),'allocated_clusters':len(allocated),'orphan_allocated_clusters':orphan,'crosslinks':{str(c):v for c,v in self.owners.items() if len(v)>1},'errors':self.errors}

def main():
    sources=[]
    for p in sorted((ROOT/'flashdump').glob('*')):
        if p.is_file():
            b=p.read_bytes();sources.append({'path':str(p.relative_to(ROOT)),'size':len(b),'sha256':sha(b),'expected_match':len(b)==8388608 and sha(b)==EXPECTED})
    p=ROOT/'flashdump/dmt99_flash_1.bin';b=p.read_bytes()
    if len(b)!=8388608 or sha(b)!=EXPECTED:raise ValueError('unexpected evidence image')
    firmware=ROOT/'DMT-99_V2.4_20240306.bin'
    save('flash_integrity.json',{'sources':sources,'expected_second_read_present':(ROOT/'flashdump/dmt99_flash_2.bin').exists(),'independent_read_agreement_verified':len(sources)==2 and all(x['expected_match'] for x in sources),'original_firmware_sha256':sha(firmware.read_bytes()),'originals_opened_read_only':True})
    partitions=[];volumes=[]
    for i in range(4):
        e=b[446+i*16:462+i*16];start=u32(e,8);count=u32(e,12)
        if not count:continue
        partitions.append({'slot':i,'type':e[4],'lba_start':start,'lba_count':count,'offset':start*4096,'size':count*4096})
        volumes.append(FAT(b,start*4096,count*4096,i,extract=True))
    reports=[v.report() for v in volumes]
    save('flash_layout.json',{'image_size':len(b),'mbr_signature_valid':b[510:512]==b'\x55\xaa','logical_sector_size':4096,'sector_size_evidence':'Both BPBs specify 4096, and partition bounds cover exactly the acquired device. 7-Zip independently detects 4096. Firmware disk_read uses sector<<12.','partition_type_warning':'Both MBR entries use 0x07 although their BPBs and cluster counts identify FAT12.','partitions':partitions,'volumes':reports})
    save('flash_files.json',{'active':[f for v in volumes for f in v.files],'directories':[f for v in volumes for f in v.dirs],'deleted_leads':[f for v in volumes for f in v.deleted]})
    regions=[]
    for s in range(len(b)//4096):
        p=s*4096;block=b[p:p+4096];role='outside partitions'
        for v in volumes:
            if v.offset<=p<v.offset+v.length:
                if p<v.offset+v.data_sector*v.bps:role=f'volume{v.index} metadata'
                else:
                    c=(p-v.offset-v.data_sector*v.bps)//v.cluster_bytes+2
                    role=f'volume{v.index} '+('active allocation' if c in v.owners else 'orphan allocation' if v.value(c) else 'free cluster')
        if s==0:role='MBR'
        regions.append({'offset':p,'size':4096,'role':role,'sha256':sha(block),'zero_bytes':block.count(0),'erased_bytes':block.count(255)})
    save('flash_regions.json',{'granularity':4096,'covers_entire_image':True,'regions':regions})
    print(json.dumps({'volumes':reports,'files':sum(len(v.files) for v in volumes)},indent=2))
if __name__=='__main__':main()

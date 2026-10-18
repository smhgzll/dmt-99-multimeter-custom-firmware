#!/usr/bin/env python3
"""Incremental, read-only comparison once a raw MCU dump is supplied.

Does not disassemble, interpret the preceding region or change existing evidence.
The application offset is explicit; the default comes from the supplied update BIN.
"""
import argparse,hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def compare(path,offset,page=256):
 image=path.read_bytes();known=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes()
 if offset<0 or offset+len(known)>len(image):raise ValueError('Dump does not contain the complete selected application interval')
 region=image[offset:offset+len(known)];changes=[]
 for at in range(0,len(known),page):
  a=known[at:at+page];b=region[at:at+page]
  if a!=b:changes.append({'dump_offset':offset+at,'update_offset':at,'bytes':len(a),'different_bytes':sum(x!=y for x,y in zip(a,b)),'update_sha256':hashlib.sha256(a).hexdigest(),'dump_sha256':hashlib.sha256(b).hexdigest()})
 return {'dump_path':str(path.resolve()),'dump_bytes':len(image),'dump_sha256':hashlib.sha256(image).hexdigest(),'application_offset_hypothesis':offset,'application_bytes_compared':len(known),'update_sha256':hashlib.sha256(known).hexdigest(),'identical_application':region==known,'changed_pages':changes,'preceding_region_bytes':offset,'preceding_region_sha256':hashlib.sha256(image[:offset]).hexdigest(),'preceding_region_interpreted':False,'evidence_modified':False,'hardware_accessed':False}
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('dump',type=Path);p.add_argument('--application-offset',type=lambda s:int(s,0),default=0x6400);p.add_argument('--report',type=Path);a=p.parse_args();text=json.dumps(compare(a.dump,a.application_offset),indent=2)+'\n'
 if a.report:
  # A report never replaces the supplied binary or existing baseline evidence.
  with a.report.open('x') as f:f.write(text)
 else:print(text,end='')
if __name__=='__main__':main()

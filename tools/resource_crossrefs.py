#!/usr/bin/env python3
import json,re
from pathlib import Path
from flash_forensics import ROOT,save

def main():
 files=json.loads((ROOT/'analysis/flash_files.json').read_text())['active'];strings=json.loads((ROOT/'analysis/strings.json').read_text());functions=json.loads((ROOT/'analysis/functions.json').read_text());coverage=json.loads((ROOT/'analysis/source_coverage.json').read_text())['functions'];native={x['address']:x for x in coverage};rows=[]
 for f in files:
  path=f['path'].casefold();names=[]
  for s in strings:
   value=s['value'];normalized=re.sub(r'^[01]:/?','',value).replace('\\','/').casefold()
   if normalized==path:names.append(s)
  addresses=set()
  for fn in functions:
   refs={int(x,16) for x in fn.get('data_references',[])}
   if any(int(s['address'],16) in refs for s in names):addresses.add(fn['address'])
   if any(re.search(r'"'+re.escape(s['value'])+r'"',(ROOT/fn['pseudocode_file']).read_text()) for s in names):addresses.add(fn['address'])
  rows.append({'file':f['path'],'sha256':f['sha256'],'physical_extents':f['physical_extents'],'firmware_strings':[{'address':s['address'],'value':s['value']} for s in names],'original_accessors':sorted(addresses),'maintained_sources':[native[a] for a in sorted(addresses) if a in native],'validation':['reconstructed/tests/test_resources.c'] if path.isascii() else ['tools/test_flash_forensics.py; native CP936 lookup unresolved'],'lookup':'Literal reference' if names else 'Dynamic/generated path or unreferenced asset; no direct reference established'})
 save('firmware_flash_crossrefs.json',{'path':['SPI NOR d656','disk_read dca8','FatFs mount f4f8 / open f54a / read f732','resource wrapper678c + callbacks6708/6734','TJpgDec696e/6d9c','display window86f6 / begin7e38 / data7e2e'],'files':rows,'dynamic_paths_note':'Language prefixes, selected/cached units, digit suffixes and menu page suffixes are composed at runtime. Absence of a literal crossref does not prove an asset unused.','shared_mount_object':'Original registers SRAM200012cc for both0: and1:. Remount changes the shared object; after remount1, accesses through0: can resolve the resource partition too.'})
 print('resources with literal firmware reference',sum(bool(x['firmware_strings']) for x in rows))
if __name__=='__main__':main()

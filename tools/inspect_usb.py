#!/usr/bin/env python3
"""Descriptor-shape scan; byte patterns alone never prove active USB support."""
from pathlib import Path
import json,struct,collections
from flash_forensics import ROOT,save,sha

def scan(b):
 devices=[];configs=[]
 for p in range(len(b)-18):
  if b[p:p+2]==b'\x12\x01':
   d=b[p:p+18];usb=int.from_bytes(d[2:4],'little');vid=int.from_bytes(d[8:10],'little');pid=int.from_bytes(d[10:12],'little')
   if usb in [0x100,0x110,0x200,0x210,0x300,0x310] and d[7] in [8,16,32,64] and 0<d[17]<=8 and vid not in [0,65535] and pid not in [0,65535]:devices.append({'offset':p,'bcdUSB':usb,'vid':vid,'pid':pid,'ep0_packet':d[7],'configurations':d[17],'status':'shape-only lead'})
  if b[p:p+2]!=b'\x09\x02':continue
  size=int.from_bytes(b[p+2:p+4],'little')
  if not 9<=size<=1024 or p+size>len(b) or not 1<=b[p+4]<=8 or not b[p+5] or not b[p+7]&128:continue
  q=p+9;interfaces=[];endpoints=[];valid=True
  while q<p+size:
   n=b[q]
   if n<2 or q+n>p+size:valid=False;break
   if b[q+1]==4:
    if n!=9:valid=False;break
    interfaces.append({'offset':q,'number':b[q+2],'alternate':b[q+3],'class':b[q+5],'subclass':b[q+6],'protocol':b[q+7]})
   if b[q+1]==5:
    if n<7:valid=False;break
    endpoints.append({'offset':q,'address':b[q+2],'attributes':b[q+3],'max_packet':int.from_bytes(b[q+4:q+6],'little')})
   q+=n
  if valid and len({i['number'] for i in interfaces})==b[p+4]:configs.append({'offset':p,'size':size,'interfaces':interfaces,'endpoints':endpoints,'status':'shape-only lead; no reference/activation proved'})
 return {'device_shape_leads':devices,'configuration_shape_leads':configs,'msc_interface_leads':[i for c in configs for i in c['interfaces'] if i['class']==8]}

def main():
 paths=['DMT-99_V2.4_20240306.bin','extracted_flash/carved/application_0004d000.bin','flashdump/dmt99_flash_1.bin'];rows=[]
 for p in paths:
  b=(ROOT/p).read_bytes();r=scan(b);rows.append({'path':p,'sha256':sha(b),**r})
 registry=json.loads((ROOT/'analysis/semantic_registry.json').read_text());traps=[{'address':a,'name':r['name']} for a,r in registry.items() if 'usb' in r['name'].lower()]
 save('usb_descriptor_report.json',{'images':rows,'confirmed_device_descriptors':0,'confirmed_active_msc_interfaces':0,'original_usb_roles':traps,'scope':'Static shape scan plus original semantic/MMIO inventory. USB hardware behavior not observed. Internal bootloader below6400 not supplied.'})
 commands={0:'TEST UNIT READY',3:'REQUEST SENSE',0x12:'INQUIRY',0x1b:'START STOP UNIT',0x1e:'PREVENT ALLOW MEDIUM REMOVAL',0x25:'READ CAPACITY10',0x28:'READ10',0x2a:'WRITE10',0x35:'SYNCHRONIZE CACHE10'}
 save('usb_msc_command_map.json',{'recovered_device_command_handlers':[],'implementation_location':'unknown; main application and carved application do not establish an MSC implementation','reference_model':'tools/usb_storage_reference.py','reference_only_commands':[{'opcode':k,'name':v,'original_address':None} for k,v in commands.items()],'read_only_simulation_geometry':{'block_size':4096,'block_count':2048,'evidence':'FatFs disk geometry only; USB-exported geometry remains unknown'},'device_interoperability_bugs_confirmed':[]})
 print([(x['path'],len(x['device_shape_leads']),len(x['configuration_shape_leads'])) for x in rows])
if __name__=='__main__':main()

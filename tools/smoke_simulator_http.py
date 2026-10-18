#!/usr/bin/env python3
"""Temporary loopback-only viewer/command/framebuffer check; no external IO.

Run separately where localhost sockets are available. The ordinary backend/PNG
tests require no socket permission. This is not JavaScript browser automation.
"""
import hashlib,json,re,signal,subprocess,sys
from pathlib import Path
from urllib.error import HTTPError
from urllib.request import build_opener,ProxyHandler,Request
ROOT=Path(__file__).resolve().parents[1]
def main():
 process=subprocess.Popen([sys.executable,str(ROOT/'tools/dmt99_simulator.py'),'--port','0'],cwd=ROOT,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
 try:
  line=process.stdout.readline();match=re.search(r'http://127\.0\.0\.1:\d+',line)
  if not match:raise RuntimeError(line+process.stderr.read())
  base=match[0];http=build_opener(ProxyHandler({}))
  assert b'DMT99 offline simulator' in http.open(base,timeout=10).read()
  def command(text):
   request=Request(base+'/command',data=json.dumps({'command':text}).encode(),headers={'Content-Type':'application/json'},method='POST')
   return json.loads(http.open(request,timeout=10).read())
  for text in ['ack 12','reading 12.34 6','button 6']:status=command(text)
  assert status['run']==1 and status['profile']=='STOCK_COMPATIBLE'
  try:command('reading 1.2.34 6')
  except HTTPError as error:assert error.code==400
  else:raise AssertionError('Malformed input accepted')
  assert command('status')['run']==1
  png=http.open(base+'/frame.png',timeout=10).read();assert png.startswith(bytes([137,80,78,71,13,10,26,10]))
  report={'status':'passed','profile':'STOCK_COMPATIBLE','requests':7,'html':True,'c_command_state':True,'bad_input_keeps_process_alive':True,'png_bytes':len(png),'binary_sha256':hashlib.sha256((ROOT/'reconstructed/build/dmt99-simulator').read_bytes()).hexdigest(),'viewer_sha256':hashlib.sha256((ROOT/'tools/dmt99_simulator.py').read_bytes()).hexdigest(),'scope':'Temporary loopback-only HTTP GET/POST and actual C framebuffer; JavaScript browser automation and physical hardware not claimed','reproduce':'python3 tools/smoke_simulator_http.py'}
  (ROOT/'analysis/simulator_http_validation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
 finally:
  if process.poll() is None:process.send_signal(signal.SIGINT)
  process.communicate(timeout=15)
if __name__=='__main__':main()

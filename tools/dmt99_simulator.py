#!/usr/bin/env python3
"""Local browser controls for the C DMT99 simulator. Standard library only.

The child process owns application logic, resources, framebuffer and PNG output.
No external service, device, or measurement-generation algorithm is involved.
"""
import argparse,json,re,subprocess,tempfile
from http.server import BaseHTTPRequestHandler,HTTPServer
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
HTML='''<!doctype html><meta charset="utf-8"><title>DMT99 offline simulator</title>
<style>body{font:16px system-ui;background:#17191d;color:#eee;max-width:900px;margin:30px auto}img{width:640px;max-width:100%;image-rendering:pixelated;border:1px solid #444}button,input,select{font:inherit;margin:4px;padding:8px;border-radius:5px}button{cursor:pointer}pre{white-space:pre-wrap;font-size:13px;color:#bcd}label{display:inline-block}small{display:block;color:#bbb}</style>
<h1>DMT99 offline simulator</h1><p id="profile"></p><img id="screen" width="640" height="480" alt="Recovered DMT99 framebuffer">
<div id="keys"></div><button onclick="send('button 2 long')">Long 2 / REL</button><button onclick="send('powercycle')">Power cycle</button><button onclick="send('poweroff')">Power off</button><button onclick="send('poweron')">Power on</button>
<p><label>Mode ACK <input id="mode" value="0x0c" size="5"></label><button onclick="send('ack '+value('mode'))">Deliver ACK</button></p>
<p><label>Reading <input id="digits" value="12.34" size="7"></label><label>Unit <select id="unit"><option value="6">V</option><option value="7">mV</option><option value="8">µA</option><option value="9">mA</option><option value="10">A</option><option value="11">Ω</option><option value="12">kΩ</option><option value="13">MΩ</option><option value="15">nF</option><option value="16">µF</option><option value="17">mF</option><option value="18">Hz</option><option value="19">kHz</option><option value="20">MHz</option><option value="21">°C</option></select></label><button onclick="send('reading '+value('digits')+' '+value('unit'))">Deliver packet</button></p>
<p><label>Raw 13-byte packet <input id="packet" size="28" maxlength="26"></label><button onclick="send('packet '+value('packet'))">Deliver</button><button onclick="send('tick 21')">21 TIM3 updates</button></p>
<p><label>View <select id="view"><option value="1">Regular</option><option value="2">Recording</option><option value="3">Monitor</option></select></label><label>Layout <select id="layout"><option value="0">Modern</option><option value="1">Classic</option></select></label><label>Language <select id="english"><option value="1">English</option><option value="0">Chinese</option></select></label><button onclick="send('configure '+value('view')+' '+value('layout')+' '+value('english'))">Boot fixture</button></p>
<p><a href="/frame.png" download="dmt99.png">Download PNG</a></p><pre id="status"></pre><small>ACKs are explicit inputs. Menu keys: 1 down, 2 up, 3 enter, 4 back, 5 select, 6 exit. Boot fixture changes simulated settings directly. Remaining scene gaps and scheduling differences are documented in the project.</small>
<script>const value=id=>document.getElementById(id).value;async function send(command='status'){try{const r=await fetch('/command',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({command})});const s=await r.json();if(!r.ok)throw Error(s.error);document.getElementById('profile').textContent=s.profile;document.getElementById('status').textContent=JSON.stringify(s,null,2);document.getElementById('screen').src='/frame.png?t='+Date.now();const names=s.run===1?['Down','Up','Enter','Back','Select','Exit']:['Other','HOLD','Primary','Current','Select / Record','Menu'];document.getElementById('keys').innerHTML=names.map((n,i)=>`<button onclick="send('button ${i+1}')">${i+1}: ${n}</button>`).join('')}catch(e){document.getElementById('status').textContent=e.message}}send();</script>'''
class Backend:
 def __init__(self,profile,flash,state=None):
  self.temp=tempfile.TemporaryDirectory(prefix='dmt99-simulator-');self.frame=Path(self.temp.name)/'frame.png'
  build=ROOT/'reconstructed/build'/('custom' if profile=='CUSTOM_DMT99' else '')
  binary=build/'dmt99-simulator'
  args=[str(binary),'--pipe','--flash',str(flash)]
  if state:args+=['--state',str(state)]
  self.process=subprocess.Popen(args,cwd=ROOT/'reconstructed',stdin=subprocess.PIPE,stdout=subprocess.PIPE,text=True,bufsize=1)
  self.last_status=None
 def command(self,command):
  if not isinstance(command,str):raise ValueError('Command must be text')
  parts=command.split()
  if not parts or parts[0] not in {'status','button','ack','reading','packet','tick','step','charge','battery','poweroff','poweron','powercycle','configure'} or '\n' in command or '\r' in command:raise ValueError('Unsupported simulator command')
  verb=parts[0]
  def integer(at,low,high):
   value=int(parts[at],0)
   if not low<=value<=high:raise ValueError('Value is outside the command range')
   return value
  try:
   if verb=='button':
    if len(parts) not in [2,3] or (len(parts)==3 and parts[2]!='long'):raise ValueError('Use button EVENT [long]')
    integer(1,1,7)
   elif verb=='reading':
    if len(parts)!=3 or not re.fullmatch(r'[+-]?(?:[0-9]{4}|[0-9]\.[0-9]{3}|[0-9]{2}\.[0-9]{2}|[0-9]{3}\.[0-9])',parts[1]):raise ValueError('Reading needs exactly four digits and an optional decimal point')
    if integer(2,6,21)==14:raise ValueError('Unsupported numeric unit')
   elif verb=='packet':
    if len(parts)!=2 or not re.fullmatch(r'[0-9a-fA-F]{26}',parts[1]):raise ValueError('Packet needs exactly 26 hexadecimal characters')
   elif verb=='configure':
    if len(parts)!=4:raise ValueError('Use configure VIEW LAYOUT ENGLISH')
    integer(1,1,3);integer(2,0,1);integer(3,0,1)
   elif verb in ['ack','tick','battery','charge']:
    if len(parts)!=2:raise ValueError('Command needs one integer')
    integer(1,0,{'ack':255,'tick':1000000,'battery':100,'charge':1}[verb])
   elif len(parts)!=1:raise ValueError('Command takes no arguments')
  except (IndexError,ValueError) as error:raise ValueError(str(error)) from None
  # A powered-off device does not consume ordinary key events. Keep the viewer
  # usable while its strict CLI child rejects unsupported script operations.
  if verb=='button' and self.last_status and not self.last_status['power']:return self.last_status
  assert self.process.stdin and self.process.stdout
  self.process.stdin.write(command+'\nsnapshot '+str(self.frame)+'\nstatus\n');self.process.stdin.flush()
  result=self.process.stdout.readline()
  if not result:raise RuntimeError('C simulator stopped; check input values')
  # A user status command emits its own response before the synchronized one.
  if parts[0]=='status':result=self.process.stdout.readline()
  self.last_status=json.loads(result);return self.last_status
 def close(self):
  if self.process.poll() is None:
   self.process.stdin.write('quit\n');self.process.stdin.flush();self.process.wait(timeout=10)
  self.process.stdin.close();self.process.stdout.close()
  self.temp.cleanup()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--profile',choices=['STOCK_COMPATIBLE','CUSTOM_DMT99'],default='STOCK_COMPATIBLE');p.add_argument('--flash',type=Path,default=ROOT/'flashdump/dmt99_flash_1.bin');p.add_argument('--state',type=Path);p.add_argument('--port',type=int,default=9999);a=p.parse_args()
 subprocess.run(['make','-s','-C',str(ROOT/'reconstructed'),'CONFIG='+a.profile,'simulator'],check=True)
 backend=Backend(a.profile,a.flash,a.state)
 class Handler(BaseHTTPRequestHandler):
  def reply(self,data,kind,status=200):
   self.send_response(status);self.send_header('Content-Type',kind);self.send_header('Cache-Control','no-store');self.send_header('Content-Length',str(len(data)));self.end_headers();self.wfile.write(data)
  def do_GET(self):
   if self.path=='/':self.reply(HTML.encode(),'text/html; charset=utf-8')
   elif self.path.split('?')[0]=='/frame.png' and backend.frame.exists():self.reply(backend.frame.read_bytes(),'image/png')
   else:self.reply(b'Not found','text/plain',404)
  def do_POST(self):
   if self.path!='/command' or self.headers.get('Content-Type')!='application/json':self.reply(b'{}','application/json',400);return
   try:
    length=int(self.headers.get('Content-Length','0'))
    if not 0<length<=1024:raise ValueError('Invalid request size')
    result=backend.command(json.loads(self.rfile.read(length))['command']);self.reply(json.dumps(result).encode(),'application/json')
   except (ValueError,KeyError,RuntimeError) as e:self.reply(json.dumps({'error':str(e)}).encode(),'application/json',400)
  def log_message(self,*args):pass
 try:server=HTTPServer(('127.0.0.1',a.port),Handler)
 except OSError as error:
  backend.close();raise SystemExit('Cannot bind the local viewer: '+str(error)+'. The C CLI remains available.')
 print(f'DMT99 {a.profile}: http://127.0.0.1:{server.server_port} (Ctrl-C to close)',flush=True)
 try:server.serve_forever()
 except KeyboardInterrupt:pass
 finally:server.server_close();backend.close()
if __name__=='__main__':main()

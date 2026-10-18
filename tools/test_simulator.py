import hashlib,json,subprocess,tempfile,unittest
from pathlib import Path
from PIL import Image
from dmt99_simulator import ROOT,Backend
FLASH=ROOT/'flashdump/dmt99_flash_1.bin'
class SimulatorTests(unittest.TestCase):
 def replay(self,text,custom=False,state=None):
  build=ROOT/'reconstructed/build'/('custom' if custom else '')
  with tempfile.TemporaryDirectory() as directory:
   script=Path(directory)/'replay.sim';script.write_text(text)
   cmd=[str(build/'dmt99-simulator'),'--flash',str(FLASH),'--script',str(script)]
   if state:cmd+=['--state',str(state)]
   result=subprocess.run(cmd,cwd=ROOT/'reconstructed',check=True,text=True,capture_output=True)
  return [json.loads(line) for line in result.stdout.splitlines() if line.startswith('{')]
 def test_stock_power_cycle(self):
  a,b=self.replay('ack 15\nreading 12.34 18\nstatus\npowercycle\nstatus\n')
  self.assertEqual(a['mode'],15);self.assertEqual(b['mode'],20);self.assertEqual(b['tx_mode'],20);self.assertEqual(b['last_reads'],0);self.assertEqual(b['last_writes'],0)
 def test_custom_waits_for_ack_and_persists(self):
  with tempfile.TemporaryDirectory() as directory:
   state=Path(directory)/'mock.state'
   a,b,c=self.replay('ack 15\nstatus\npowercycle\nstatus\nack 15\nstatus\n',True,state)
   self.assertEqual(a['mode'],15);self.assertEqual(b['mode'],20);self.assertEqual(b['tx_mode'],15);self.assertEqual(c['mode'],15)
   d=self.replay('status\n',True,state)[0];self.assertEqual(d['tx_mode'],15);self.assertEqual(state.stat().st_size,280)
   e=self.replay('status\n',False,state)[0];self.assertEqual(e['tx_mode'],20);self.assertEqual(e['last_reads'],0)
   self.assertEqual(self.replay('status\n',True,state)[0]['tx_mode'],15)
 def test_png_pixels_and_reproducibility(self):
  with tempfile.TemporaryDirectory() as directory:
   paths=[Path(directory)/f'{i}.png' for i in range(2)]
   for path in paths:self.replay(f'ack 12\nreading 12.34 6\nsnapshot {path}\n')
   self.assertEqual(paths[0].read_bytes(),paths[1].read_bytes())
   with Image.open(paths[0]) as image:
    image.load();self.assertEqual(image.size,(320,240));self.assertEqual(image.mode,'RGB');self.assertGreater(len(image.getcolors(1000000)),500)
 def test_menu_and_history(self):
  a,b=self.replay('button 6\nbutton 3\nbutton 1\nbutton 5\nbutton 6\nack 12\nreading 24.68 6\nbutton 5\nstatus\npowercycle\nstatus\n')
  self.assertEqual((a['view'],a['history']),(2,1));self.assertEqual((b['view'],b['history']),(2,1))
 def test_packet_encoder_units(self):
  text='ack 12\n'+''.join(f'reading -12.34 {unit}\nstatus\n' for unit in [6,7,8,9,10,11,12,13,15,16,17,18,19,20,21])
  results=self.replay(text)
  self.assertEqual([r['unit'] for r in results],[6,7,8,9,10,11,12,13,15,16,17,18,19,20,21])
  for r in results:self.assertAlmostEqual(r['value'],-12.34,places=4)
 def test_acquired_image_cannot_be_output(self):
  digest=hashlib.sha256(FLASH.read_bytes()).hexdigest()
  binary=ROOT/'reconstructed/build/dmt99-simulator'
  p=subprocess.run([str(binary),'--flash',str(FLASH),'--pipe'],input='snapshot '+str(FLASH)+'\n',text=True,capture_output=True)
  self.assertEqual(p.returncode,2);self.assertEqual(hashlib.sha256(FLASH.read_bytes()).hexdigest(),digest)
  p=subprocess.run([str(binary),'--flash',str(FLASH),'--state',str(FLASH),'--pipe'],input='quit\n',text=True,capture_output=True)
  self.assertEqual(p.returncode,2);self.assertEqual(hashlib.sha256(FLASH.read_bytes()).hexdigest(),digest)
 def test_browser_backend_uses_c(self):
  b=Backend('STOCK_COMPATIBLE',FLASH)
  try:
   b.command('ack 12');s=b.command('reading 12.34 6');self.assertAlmostEqual(s['value'],12.34,places=4);self.assertTrue(b.frame.exists())
   self.assertEqual(b.command('button 6')['run'],1)
  finally:b.close()
 def test_browser_bad_input_and_off_keys_keep_process_alive(self):
  b=Backend('STOCK_COMPATIBLE',FLASH)
  try:
   for command in ['reading 1.2.34 6','packet xyz','ack 999','button 0','tick -1','configure 4 0 1']:
    with self.assertRaises(ValueError):b.command(command)
   self.assertFalse(b.command('poweroff')['power']);self.assertFalse(b.command('button 2')['power'])
   self.assertTrue(b.command('poweron')['power']);self.assertEqual(b.command('ack 12')['mode'],12)
  finally:b.close()
if __name__=='__main__':unittest.main()

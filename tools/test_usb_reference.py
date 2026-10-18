import unittest,struct
from pathlib import Path
from usb_storage_reference import ReadOnlySCSI
class USBReferenceTests(unittest.TestCase):
 def setUp(self):self.image=(Path(__file__).resolve().parents[1]/'flashdump/dmt99_flash_1.bin').read_bytes();self.s=ReadOnlySCSI(self.image)
 def test_capacity_and_blocks(self):
  self.assertEqual(self.s.command(bytes([0x25])+bytes(9)),(struct.pack('>II',2047,4096),0))
  c=bytearray(10);c[0]=0x28;c[2:6]=(463).to_bytes(4,'big');c[7:9]=(2).to_bytes(2,'big')
  self.assertEqual(self.s.command(c),(self.image[463*4096:465*4096],0))
 def test_errors_and_sense(self):
  for c,k,a in [(b'\xff',5,0x20),(bytes([0x2a])+bytes(9),7,0x27),(b'\x28',5,0x24)]:
   self.assertEqual(self.s.command(c)[1],1);sense,status=self.s.command(bytes([3,0,0,0,18,0]));self.assertEqual((sense[2],sense[12]),(k,a));self.assertEqual(status,0)
  self.assertEqual(self.s.command(bytes([0])+bytes(5),1)[1],1)
 def test_out_of_bounds(self):
  c=bytearray(10);c[0]=0x28;c[2:6]=(2047).to_bytes(4,'big');c[7:9]=(2).to_bytes(2,'big');self.assertEqual(self.s.command(c)[1],1)
 def test_transport(self):
  cdb=bytes([0x25])+bytes(9);cbw=struct.pack('<IIIBBB',0x43425355,0x1234,8,128,0,10)+cdb+bytes(6)
  d,csw=self.s.bulk_in(cbw);self.assertEqual(len(d),8);self.assertEqual(struct.unpack('<IIIB',csw),(0x53425355,0x1234,0,0))
  with self.assertRaises(ValueError):self.s.bulk_in(cbw[:-1])
  bad=bytearray(cbw);bad[0]=0
  with self.assertRaises(ValueError):self.s.bulk_in(bad)
 def test_readonly_and_inquiry(self):
  self.assertEqual(self.s.command(bytes([0x12,0,0,0,36,0]))[0][8:16],b'OFFLINE ')
  self.assertEqual(self.s.image,self.image)
if __name__=='__main__':unittest.main()

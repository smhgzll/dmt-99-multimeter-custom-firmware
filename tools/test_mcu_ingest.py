import tempfile,unittest
from pathlib import Path
from ingest_mcu_dump import compare,ROOT
class IncrementalIngest(unittest.TestCase):
 def test_unchanged_and_changed_application(self):
  known=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes()
  with tempfile.TemporaryDirectory() as directory:
   path=Path(directory)/'synthetic.bin';data=bytearray(b'\xff'*0x6400+known);path.write_bytes(data);r=compare(path,0x6400);self.assertTrue(r['identical_application']);self.assertFalse(r['preceding_region_interpreted'])
   data[0x6500]^=1;path.write_bytes(data);r=compare(path,0x6400);self.assertEqual(len(r['changed_pages']),1);self.assertEqual(r['changed_pages'][0]['different_bytes'],1);self.assertEqual(path.read_bytes(),data)
 def test_short_input_rejected(self):
  with tempfile.TemporaryDirectory() as directory:
   path=Path(directory)/'short.bin';path.write_bytes(bytes(16))
   with self.assertRaises(ValueError):compare(path,0x6400)
if __name__=='__main__':unittest.main()

import unittest,json
from flash_forensics import FAT,ROOT,sha
from flash_candidates import jpeg_extent

class FlashTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.image=(ROOT/'flashdump/dmt99_flash_1.bin').read_bytes()
    def test_real_volume(self):
        v=FAT(self.image,463*4096,1585*4096,1);r=v.report()
        self.assertEqual((r['active_files'],r['active_directories']),(299,12));self.assertFalse(r['errors']);self.assertFalse(r['crosslinks']);self.assertFalse(r['orphan_allocated_clusters']);self.assertTrue(r['fat_copies_identical'])
        for f in v.files:
            self.assertEqual(sha((ROOT/f['extracted_path']).read_bytes()),f['sha256']);self.assertEqual(sum(e['size'] for e in f['physical_extents']),f['size'])
        self.assertEqual(sum(f['kind']=='JPEG' for f in v.files),297)
    def test_deleted_not_recovered(self):
        v=FAT(self.image,63*4096,400*4096,0);r=v.report()
        self.assertEqual(r['active_files'],0);self.assertEqual(len(v.deleted),57);self.assertTrue(r['errors'])
        self.assertTrue(any(x['lfn_fragment']=='CH32V303RBT61.bin' for x in v.deleted))
        self.assertTrue(all('extracted_path' not in x for x in v.deleted))
    def test_bad_signature_geometry_and_truncation(self):
        for pos,value in [(463*4096+510,0),(463*4096+12,0),(463*4096+13,3)]:
            b=bytearray(self.image);b[pos]=value
            with self.assertRaises(ValueError):FAT(b,463*4096,1585*4096,1)
        with self.assertRaises(ValueError):FAT(self.image[:2000000],463*4096,1585*4096,1)
    def test_chain_cycle_free_and_range(self):
        v=FAT(self.image,463*4096,1585*4096,1)
        for value,expected in [(2,'cycle'),(0,'free/reserved cluster in chain'),(2000,'out of range'),(0xff7,'bad cluster')]:
            fat=bytearray(v.fat);fat[3]=value&255;fat[4]=(fat[4]&240)|(value>>8);v.fat=bytes(fat)
            self.assertEqual(v.chain(2)[1],expected)
    def test_fat_disagreement(self):
        b=bytearray(self.image);b[(463+2)*4096+50]^=1
        self.assertFalse(FAT(b,463*4096,1585*4096,1).copies_match)
    def test_jpeg_extent_and_truncated_input(self):
        files=json.loads((ROOT/'analysis/flash_files.json').read_text())['active']
        for f in files:
            if f['kind']!='JPEG':continue
            data=(ROOT/f['extracted_path']).read_bytes();n=jpeg_extent(data,0)
            self.assertLessEqual(n,len(data));self.assertEqual(data[n-2:n],b'\xff\xd9')
            with self.assertRaises(ValueError):jpeg_extent(data[:n-1],0)
if __name__=='__main__':unittest.main()

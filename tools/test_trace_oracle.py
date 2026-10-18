import unittest
from rv32_trace import TraceMachine,INSTRUCTIONS
from rv32_subset import sx
class OracleEncodingTests(unittest.TestCase):
 def test_compressed_lui_sign_extension(self):
  class Finished(Exception):pass
  cases=0;negative=0
  for address,instruction in INSTRUCTIONS.items():
   if not instruction['text'].startswith('c.lui '):continue
   m=TraceMachine();raw=m.read(address,2);rd=(raw>>7)&31;imm=sx(((raw>>12)&1)<<5|((raw>>2)&31),6)
   def step(machine):
    if machine.pc!=address:raise Finished()
   m.instruction_hook=step
   with self.assertRaises(Finished):m.call(address,limit=2)
   self.assertEqual(m.r[rd],(imm<<12)&0xffffffff);cases+=1;negative+=imm<0
  self.assertGreater(cases,5);self.assertGreater(negative,0)
 def test_unmapped_memory_stays_an_error(self):
  with self.assertRaisesRegex(RuntimeError,'80023fe4'):TraceMachine().read(0x80023fe4,2)
if __name__=='__main__':unittest.main()

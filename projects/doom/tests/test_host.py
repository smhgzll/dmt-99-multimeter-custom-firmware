#!/usr/bin/env python3
"""Deterministic command/screenshot tests of the desktop build."""
import ctypes,hashlib,json,subprocess,tempfile,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

class Host(unittest.TestCase):
    def session(self,tmp,script):
        pic=Path(tmp)/'frame.ppm';state=Path(tmp)/'state.json'
        subprocess.run([str(ROOT/'build/doom-host'),'--script',script,'--ppm',str(pic),'--state',str(state)],check=True)
        raw=pic.read_bytes();self.assertEqual(raw[:15],b'P6\n320 240\n255\n');self.assertEqual(len(raw),15+320*240*3)
        return hashlib.sha256(raw).hexdigest(),json.loads(state.read_text())
    def test_repeat_and_controls(self):
        with tempfile.TemporaryDirectory() as tmp:
            first,state=self.session(tmp,'fwwadf')
            self.assertEqual((first,state),self.session(tmp,'fwwadf'))
            base,initial=self.session(tmp,'f')
            self.assertNotEqual(base,first);self.assertEqual(initial['x'],96)
    def test_pause_resume_and_restart(self):
        with tempfile.TemporaryDirectory() as tmp:
            game,state=self.session(tmp,'fww')
            self.assertEqual((game,state),self.session(tmp,'fwwpf'))
            start,initial=self.session(tmp,'f')
            self.assertEqual((start,initial),self.session(tmp,'fwwpsf'))
            menu,s=self.session(tmp,'');self.assertEqual(s['mode'],0)
            help_frame,s=self.session(tmp,'ssssf');self.assertEqual(s['mode'],2);self.assertNotEqual(menu,help_frame)
    def test_png_signature(self):
        with tempfile.TemporaryDirectory() as tmp:
            out=Path(tmp)/'frame.png'
            subprocess.run(['python3',str(ROOT/'tools/simulator.py'),'--script','f','--png',str(out)],check=True,capture_output=True)
            self.assertEqual(out.read_bytes()[:8],b'\x89PNG\r\n\x1a\n')
    def test_clock_pause_and_weapons(self):
        with tempfile.TemporaryDirectory() as tmp:
            _,active=self.session(tmp,'f..........');self.assertEqual(active['tick'],10)
            _,paused=self.session(tmp,'f..p........');self.assertEqual(paused['tick'],2)
            _,chosen=self.session(tmp,'sfsf');self.assertEqual(chosen['weapon'],2);self.assertEqual(chosen['mode'],0)
    def test_options(self):
        with tempfile.TemporaryDirectory() as tmp:
            _,state=self.session(tmp,'ssf');self.assertTrue(state['god']);self.assertEqual(state['mode'],0)
            _,state=self.session(tmp,'sssf');self.assertFalse(state['sound']);self.assertFalse(state['god'])
            _,state=self.session(tmp,'ssff');self.assertFalse(state['god'])
            _,state=self.session(tmp,'ssfwf');self.assertTrue(state['god']);self.assertEqual(state['mode'],3)
    def test_held_buttons(self):
        class State(ctypes.Structure):
            _fields_=[('x',ctypes.c_float),('y',ctypes.c_float),('angle',ctypes.c_int16),('health',ctypes.c_int16),
                      ('kills',ctypes.c_uint16),('pickups',ctypes.c_uint16),('complete',ctypes.c_bool),
                      ('shot',ctypes.c_uint8),('weapon',ctypes.c_uint8),('anim',ctypes.c_uint8)]
        lib=ctypes.CDLL(str(ROOT/'build/libdoom-host.so'))
        lib.doom_get_state.restype=State;lib.doom_host_input.argtypes=[ctypes.c_char]
        lib.doom_host_init();lib.doom_host_input(b'f')
        lib.doom_host_buttons(2);lib.doom_host_advance(100);small=lib.doom_get_state().x-96
        self.assertGreater(small,0);self.assertLess(small,5)
        lib.doom_host_advance(500);self.assertGreater(lib.doom_get_state().x-96,small*5)
        lib.doom_host_buttons(0);lib.doom_host_advance(50);stopped=lib.doom_get_state().x
        lib.doom_host_advance(100);self.assertEqual(stopped,lib.doom_get_state().x)
        lib.doom_host_buttons(16)
        for _ in range(6):lib.doom_host_advance(500)
        self.assertEqual(lib.doom_host_mode(),1) # solo middle never pauses
        lib.doom_host_buttons(0);lib.doom_host_advance(50)
        lib.doom_host_buttons(17)
        for _ in range(4):lib.doom_host_advance(500)
        self.assertEqual(lib.doom_host_mode(),1)
        lib.doom_host_advance(30);self.assertEqual(lib.doom_host_mode(),0)

if __name__=='__main__':unittest.main()

#!/usr/bin/env python3
"""Incremental menu reconstruction: execute preserved BIN instructions.

Compare ordered rendering leaves, state/cache/event/settings, and explicit
editor/history/restart call boundaries. No MMIO or fabricated callee behavior.
"""
import argparse
import ctypes as C
import hashlib
import json
from rv32_trace import ROOT, TraceMachine
from test_application_differential import Control, Menu, MenuIO, MENU_ACTION
from test_ui_differential import Trace, LEAVES

LEAVES.update({0x876a: 1, 0x14ef2: 0, 0x126d8: 0, 0x14578: 3,
               0x143e0: 0, 0x8016: 0, 0x14a92: 0, 0x11086: 1,
               0xdc46: 0, 0x8ed4: 0})

class Paint(C.Structure):
    _fields_ = [(n, C.c_void_p) for n in ['view', 'thresholds', 'history', 'context', 'base','alarm','auto_cache','unit_cache']]
class History(C.Structure):
    _fields_ = [('count',C.c_uint16),('records',C.c_uint16*60)]

class Restart(Exception):
    pass
BASE=C.CFUNCTYPE(None,C.c_void_p)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--library', default='reconstructed/build/librecovered.so')
    parser.add_argument('--report', default='analysis/menu_differential.json')
    args = parser.parse_args()
    path = ROOT / args.library
    lib = C.CDLL(str(path))
    counts = {}
    for pc, name in [(0x164d4, 'menu_navigate_rendered'), (0x1598c, 'menu_enter_back_rendered'),
                     (0x15af6, 'menu_select_rendered'), (0x169d4, 'menu_step_rendered')]:
        fn = getattr(lib, name)
        fn.argtypes = [C.POINTER(Menu), C.POINTER(Control), C.POINTER(C.c_uint16),
                       C.POINTER(MenuIO), C.POINTER(Paint)]
        fn.restype = C.c_bool if pc == 0x169d4 else None
        for layout in [0, 1]:
            for language in [0, 1]:
                for state in range(1, 33):
                    for event in range(7):
                        # Cache hit/stale paths, all three views, all brightness
                        # and timeout fields, and nonempty/clamped history rows.
                        for variant in range(4 if pc == 0x169d4 else 1):
                            initial = language | (layout << 10) | 0x100 | (variant << 3) | (variant << 6) | ((variant&1)<<5)
                            word = C.c_uint16(initial)
                            s = Control();s.view = 1 + (variant % 3);s.layout = layout;s.event = event
                            cache = state if variant&1 else 255 if variant==2 else 25
                            page = [1,2,3,1][variant]
                            menu = Menu(state, cache, page, True)
                            history=History();history.count=[0,5,6,30][variant]
                            alarm=C.c_uint8(0x47);auto_cache=C.c_uint8(0x43);unit_cache=C.c_uint8(cache)
                            m = TraceMachine()
                            for a, n, value in [(0x1d8,1,state),(0x1da,1,cache),(0x1dd,1,page),
                                                (0x1de,1,s.view),(0x205,1,layout),(0x1ee,1,event),(0x3e,2,initial),(0x40,1,history.count)]:
                                m.write(0x20000000+a,n,value)
                            m.write(0x200001db,1,alarm.value);m.write(0x200001d9,1,auto_cache.value)
                            prefix = b'menu/english/' if language else b'menu/chinese/'
                            m.ram[0x298:0x298+len(prefix)+1] = prefix+b'\0'
                            trace = Trace(m)
                            expected_base=[];actual_base=[]
                            original_base=m.hooks[0x14ef2]
                            def base_m(machine,*params):
                                expected_base.append(tuple(machine.read(0x20000000+a,1) for a in [0x1db,0x1d8,0x1dd,0x1ee,0x1da,0x1d9]))
                                return original_base(machine,*params)
                            m.hooks[0x14ef2]=base_m
                            @BASE
                            def base_c(_):actual_base.append((alarm.value,menu.state,menu.page,s.event,menu.last,auto_cache.value))
                            # Restart is a nonreturning original tail call. Stop
                            # at that boundary, after recording its ordered call.
                            def restart(machine, *p):
                                trace.expected.append((0x8ed4, (), None));raise Restart()
                            m.hooks[0x8ed4] = restart
                            @MENU_ACTION
                            def action(_, kind, value, _menu, _control):
                                if kind == 4: trace.actual.append((0x14a92, (), None))
                                if kind == 5: trace.actual.append((0x8016, (), None))
                                if kind == 8: trace.actual.append((0x11086, (value,), None))
                                if kind == 9:
                                    trace.actual.extend([(0xdc46, (), None), (0x8ed4, (), None)])
                            io = MenuIO(None, action)
                            paint = Paint(C.addressof(trace.view), None, C.addressof(history), None,C.cast(base_c,C.c_void_p),C.addressof(alarm),C.addressof(auto_cache),C.addressof(unit_cache))
                            try: result = m.call(pc, limit=2000000)
                            except Restart: result = 0
                            actual = fn(C.byref(menu), C.byref(s), C.byref(word), C.byref(io), C.byref(paint))
                            label = (hex(pc), layout, language, state, event, variant)
                            trace.check(label)
                            assert actual_base==expected_base,(label,actual_base,expected_base)
                            observed = (menu.state,menu.last,menu.page,s.event,s.view,s.layout,word.value)
                            expected = tuple(m.read(0x20000000+a,n) for a,n in
                                             [(0x1d8,1),(0x1da,1),(0x1dd,1),(0x1ee,1),(0x1de,1),(0x205,1),(0x3e,2)])
                            assert observed == expected, (label, observed, expected)
                            if pc == 0x169d4: assert actual == bool(result), (label,actual,result)
                            assert (alarm.value,auto_cache.value)==(m.read(0x200001db,1),m.read(0x200001d9,1)),label
                            if pc==0x169d4 and event==6:assert unit_cache.value==25,label
                            key = f'0x{pc:08x}';counts[key] = counts.get(key, 0)+1
        print(name, counts[f'0x{pc:08x}'], 'original-entry comparisons passed', flush=True)
    # 1a336 is a separate original entry, not a duplicate of each hinted row.
    lib.menu_threshold_all.argtypes = [C.c_void_p,C.c_uint8,C.c_bool]
    for layout in [0,1]:
        for language in [0,1]:
            m=TraceMachine();m.write(0x20000205,1,layout);m.write(0x2000003e,2,language)
            trace=Trace(m);m.call(0x1a336,limit=1000000)
            lib.menu_threshold_all(C.byref(trace.view),layout,language);trace.check(('1a336',layout,language))
    counts['0x0001a336']=4
    report={'cases':counts,'total':sum(counts.values()),'entries':list(counts),
            'native_library_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),
            'original_sha256':hashlib.sha256(m.image).hexdigest(),
            'scope':'Ordered display leaves and external-call contracts, complete menu ID/event matrix, both layouts/languages, cache hits/misses. Editor/history are synchronous isolated call boundaries; restart stops at nonreturning application-loop tail call. No interrupt/timing or hardware equivalence.'}
    (ROOT/args.report).write_text(json.dumps(report,indent=2)+'\n')
    print(report['total'],'menu differential cases passed')

if __name__ == '__main__': main()

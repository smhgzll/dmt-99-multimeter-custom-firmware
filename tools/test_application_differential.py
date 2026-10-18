#!/usr/bin/env python3
"""Compare maintained policy/state against original code, with IO callees isolated.
Run after make -C reconstructed host-test. No Ghidra invocation or output rewrite.
"""
import ctypes as C,json,random,struct
from pathlib import Path
from rv32_trace import TraceMachine,ROOT
class Control(C.Structure):
    _fields_=[(n,C.c_uint8) for n in ['view','mode','unit','layout','event','menu_last']]+[('regular_selector',C.c_uint8*4),('record_selector',C.c_uint8*3)]+[(n,C.c_uint8) for n in ['monitor_selector','monitor_state','relative','relative_mode','relative_unit','ac_dc_code','status_high','hold','numeric']]+[('value',C.c_float),('relative_value',C.c_float),('text',C.c_char*16)]
SEND=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint8));LONG=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_uint8);EFFECT=C.CFUNCTYPE(None,C.c_void_p,C.c_int,C.POINTER(Control))
class IO(C.Structure):_fields_=[('context',C.c_void_p),('send',SEND),('longpress',LONG),('effect',EFFECT)]
lib=C.CDLL(str(ROOT/'reconstructed/build/librecovered.so'))
lib.control_buttons.argtypes=[C.POINTER(Control),C.POINTER(IO)]
lib.control_event.argtypes=[C.POINTER(Control),C.POINTER(C.c_uint8),C.POINTER(IO)]
lib.control_parse.argtypes=[C.POINTER(Control),C.POINTER(C.c_uint8)];lib.control_parse.restype=C.c_bool
FIELDS={'view':0x1de,'mode':0x206,'unit':0x289,'layout':0x205,'event':0x1ee,'menu_last':0x1da,'monitor_selector':0x203,'monitor_state':0x204,'relative':0x1f4,'relative_mode':0x1f5,'relative_unit':0x1f6,'hold':0x23e,'numeric':0x27c,'ac_dc_code':0x288,'status_high':0x28a}
FUNCTIONS={int(f['address'],16):f for f in json.loads((ROOT/'analysis/functions.json').read_text())}
def machine(s,entry):
    m=TraceMachine()
    for n,a in FIELDS.items():m.write(0x20000000+a,1,getattr(s,n))
    for n,a in [('regular_selector',0x1fc),('record_selector',0x200)]:
        for i,v in enumerate(getattr(s,n)):m.write(0x20000000+a+i,1,v)
    for n,a in [('value',0x284),('relative_value',0x1f8)]:m.write(0x20000000+a,4,int.from_bytes(struct.pack('<f',getattr(s,n)),'little'))
    m.stub(*(int(a,16) for a in FUNCTIONS[entry]['callees'] if int(a,16)>0x6700 and int(a,16)!=0x1e59e))
    return m
def check(s,m,fields):
    for n in fields:
        actual=getattr(s,n);expected=m.read(0x20000000+FIELDS[n],1)
        assert actual==expected,(n,actual,expected)
    for n,a in [('regular_selector',0x1fc),('record_selector',0x200)]:
        assert list(getattr(s,n))==[m.read(0x20000000+a+i,1) for i in range(len(getattr(s,n)))],n
    assert struct.pack('<f',s.relative_value)==m.read(0x200001f8,4).to_bytes(4,'little'),'relative snapshot'
rng=random.Random(0xd099);counts={}
def button_cases():
    count=0
    for view in [1,2,3]:
      entry={1:0xb5c8,2:0xb9e0,3:0xbc90}[view]
      for event in [0,1,2,3,4,5,7,8]:
       for mode in range(0x18):
        for longpress in [False,True]:
          s=Control();s.view=view;s.event=event;s.mode=mode;s.unit=rng.randrange(1,22);s.layout=rng.randrange(2);s.menu_last=7
          s.relative=rng.randrange(2);s.relative_mode=mode if rng.randrange(2) else 0;s.relative_unit=s.unit if rng.randrange(2) else 0
          s.value=-12.375;s.relative_value=9.25
          s.regular_selector[:]=[rng.randrange(6),rng.randrange(5),rng.randrange(5),rng.randrange(13)]
          s.record_selector[:]=[rng.randrange(6),rng.randrange(5),rng.randrange(3)]
          s.monitor_selector=rng.randrange(2)
          m=machine(s,entry);expected=[];actual=[]
          def tx(m,a,*_):expected.append(bytes(m.read(a+i,1) for i in range(10)))
          m.hooks[0x97c4]=tx;m.hooks[0x7b08]=lambda *_:int(longpress)
          io=IO(None,SEND(lambda _,p:actual.append(bytes(p[:10]))),LONG(lambda *_:longpress),EFFECT(lambda *_:None))
          m.call(entry);lib.control_buttons(C.byref(s),C.byref(io))
          try:check(s,m,FIELDS);assert actual==expected,('TX',actual,expected)
          except AssertionError as e:raise AssertionError((view,event,mode,longpress,e))
          count+=1
    return count
def event_cases():
    count=0
    for view in [1,2,3]:
      entry={1:0xa910,2:0xaed0,3:0xb29e}[view]
      for old in range(0x18):
       for event in range(0x18):
        for hold in [0,1]:
          s=Control();s.view=view;s.mode=old;s.hold=hold;s.monitor_state=count%2
          b=(C.c_uint8*13)(0x5a,0xa5,0xfd,event);m=machine(s,entry)
          for i,v in enumerate(b):m.write(0x200002ac+i,1,v)
          m.call(entry);io=IO(None,SEND(lambda *_:None),LONG(lambda *_:False),EFFECT(lambda *_:None))
          lib.control_event(C.byref(s),b,C.byref(io))
          try:check(s,m,['view','mode','hold','monitor_state'])
          except AssertionError as e:raise AssertionError((view,old,event,hold,e))
          count+=1
    return count
def parse_cases():
    count=0
    for mode in [0x0c,0x11,0x12,0x16]:
      for _ in range(256):
        s=Control();s.mode=mode;s.numeric=rng.randrange(2)
        b=(C.c_uint8*13)(0x5a,0xa5,*(rng.randrange(256) for _ in range(11)))
        m=machine(s,0xbe78)
        # Only formatted characters are isolated. Float conversion and all integer
        # compiler helpers execute from the original image.
        m.hooks={}
        def fmt(m,dest,format_,*chars):
            for i,v in enumerate(chars[:6]):m.write(dest+i,1,v)
            m.write(dest+6,1,0);return 7
        m.hooks[0x969e]=fmt
        for i,v in enumerate(b):m.write(0x200002ac+i,1,v)
        m.call(0xbe78,limit=200000);lib.control_parse(C.byref(s),b)
        expected=m.read(0x20000284,4).to_bytes(4,'little')
        assert struct.pack('<f',s.value)==expected,(list(b),s.text,s.value,struct.unpack('<f',expected))
        assert s.text.decode('latin1')==m.string(0x2000028c),(list(b),s.text,m.string(0x2000028c))
        assert s.unit==m.read(0x20000289,1)
        assert s.numeric==m.read(0x2000027c,1)
        assert s.ac_dc_code==m.read(0x20000288,1)
        assert s.status_high==m.read(0x2000028a,1)
        count+=1
    return count
class Menu(C.Structure):_fields_=[('state',C.c_uint8),('last',C.c_uint8),('page',C.c_uint8),('active',C.c_bool)]
MENU_ACTION=C.CFUNCTYPE(None,C.c_void_p,C.c_int,C.c_uint8,C.POINTER(Menu),C.POINTER(Control))
class MenuIO(C.Structure):_fields_=[('context',C.c_void_p),('action',MENU_ACTION)]
def menu_cases():
    count=0
    for entry,name in [(0x164d4,'menu_navigate'),(0x1598c,'menu_enter_back'),(0x15af6,'menu_select')]:
     for state in range(1,33):
      if state==31 and entry==0x15af6:continue # nonreturning restart separately tested in C
      for event in range(1,6):
       for layout in [0,1]:
        for language in [0,1]:
          s=Control();s.view=1;s.event=event;s.layout=layout
          word=C.c_uint16(0x12+language);m=machine(s,entry);m.hooks={}
          m.stub(0x678c,0x8050,0x80d4,0x8194,0x81ce,0x8266,0x83d6,0x8588,0x85cc,0x8672,0x86f6,0x876a,0x8a02,0x8e46,0x1a360,0x1aede,0x969e,0x8016,0x14a92,0x11086,0x7e38,0x7e20,0x7dba,0x8a5a,0x8b5c,0x8c64,0x8d6c)
          m.write(0x200001d8,1,state);m.write(0x200001dd,1,1);m.write(0x2000003e,2,word.value)
          menu=Menu(state,0,1,True);io=MenuIO(None,MENU_ACTION(lambda *_:None))
          if entry==0x164d4:m.call(entry);lib.menu_navigate(C.byref(menu),C.byref(s),C.byref(io))
          else:m.call(entry);getattr(lib,name)(C.byref(menu),C.byref(s),C.byref(word),C.byref(io))
          try:
            assert menu.state==m.read(0x200001d8,1),('state',menu.state,m.read(0x200001d8,1))
            assert s.event==m.read(0x200001ee,1),('event',s.event,m.read(0x200001ee,1))
            assert word.value==m.read(0x2000003e,2),('settings',word.value,m.read(0x2000003e,2))
            assert s.view==m.read(0x200001de,1)
            assert s.layout==m.read(0x20000205,1)
          except AssertionError as exc:raise AssertionError((hex(entry),state,event,layout,language,exc))
          count+=1
    return count
PIXEL=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_uint16,C.c_uint16)
class Canvas(C.Structure):_fields_=[('context',C.c_void_p),('pixel',PIXEL),('width',C.c_uint16),('height',C.c_uint16),('fill',C.c_void_p)]
def graphics_cases():
    count=0
    for entry,name in [(0x8050,'graphics_line'),(0x8194,'graphics_rectangle'),(0x81ce,'graphics_cut_border')]:
      for _ in range(64):
        args=[rng.randrange(5,50) for _ in range(4)]
        if entry==0x81ce:args+=[2]
        args+=[0x1234];expected=[];actual=[];m=TraceMachine()
        m.hooks[0x7f00]=lambda _,x,y,col,*a:expected.append((x&65535,y&65535,col&65535))
        m.call(entry,*args)
        canvas=Canvas(None,PIXEL(lambda _,x,y,col:actual.append((x,y,col))),320,240)
        getattr(lib,name)(C.byref(canvas),*args)
        assert actual==expected,(hex(entry),args,actual[:8],expected[:8]);count+=1
    for entry,name in [(0x83d6,'graphics_ascii'),(0x8266,'graphics_measurement_char')]:
      for size in [12,16,24,32]:
       for ch in [32,46,48,57,65,74,88,89,90,126]:
        for transparent in [0,1]:
          expected=[];actual=[];m=TraceMachine();m.write(0x20000270,2,320);m.write(0x20000272,2,240)
          m.hooks[0x7f00]=lambda _,x,y,col,*a:expected.append((x&65535,y&65535,col&65535))
          args=[10,20,ch,size,0x1234]+([0x4321,transparent] if entry==0x83d6 else [transparent])
          m.call(entry,*args)
          canvas=Canvas(None,PIXEL(lambda _,x,y,col:actual.append((x,y,col))),320,240)
          getattr(lib,name)(C.byref(canvas),*args)
          assert actual==expected,(hex(entry),size,ch,transparent,actual[:8],expected[:8],len(actual),len(expected));count+=1
    return count

class Editor(C.Structure):_fields_=[('value',C.c_int32),('peer',C.c_int32),('index',C.c_uint8),('column',C.c_uint8),('active',C.c_bool)]
def threshold_cases():
    count=0;loops=[0x1111a,0x1149e,0x11828,0x11c1c,0x12016,0x1239c]
    lib.threshold_editor_begin.argtypes=[C.POINTER(Editor),C.POINTER(C.c_uint16),C.c_uint8]
    lib.threshold_editor_step.argtypes=[C.POINTER(Editor),C.POINTER(C.c_uint16),C.POINTER(C.c_uint8)]
    for index in range(6):
      for _ in range(64):
        raw=[rng.randrange(0,900),rng.randrange(901,1000)]*3
        if count%2:raw[0]|=0x8000;raw[2]|=0x8000;raw[4]=0x8000|50
        seq=[0]+[rng.choice([1,2,3,4]) for _ in range(12)]+[rng.choice([5,6])]
        m=TraceMachine();m.stub(0x8266,0x83d6);m.write(0x200001ee,1,5)
        for i,v in enumerate(raw):m.write(0x20000032+2*i,2,v)
        queue=list(seq)
        def feed(m):
            if m.pc==loops[index]:
                if not queue:raise AssertionError('unexpected extra editor iteration')
                m.write(0x200001ee,1,queue.pop(0))
        m.instruction_hook=feed;m.call(0x11086,index+6)
        w=(C.c_uint16*6)(*raw);e=Editor();lib.threshold_editor_begin(C.byref(e),w,index+6)
        for event in seq:
            ev=C.c_uint8(event);lib.threshold_editor_step(C.byref(e),w,C.byref(ev))
        assert list(w)==[m.read(0x20000032+2*i,2) for i in range(6)],(index,seq,list(w),[m.read(0x20000032+2*i,2) for i in range(6)])
        assert ev.value==m.read(0x200001ee,1);assert not queue
        count+=1
    lib.threshold_alarm.argtypes=[C.POINTER(C.c_uint16),C.c_uint8,C.c_float,C.c_bool,C.POINTER(C.c_uint8)]
    for mode in [0x0b,0x0c,0x0d,0x10,0x12,0x15]:
      for fahrenheit in [False,True]:
       for value in [-60,-10,0,1,10,100,1000]:
        for old in [0,1,2,255]:
          raw=[0x8000|50,100,0x8000|200,200,0x8000|55,100]
          m=TraceMachine();m.stub(0x678c,0x78d0)
          m.write(0x20000206,1,mode);m.write(0x200001db,1,old);m.write(0x2000003e,2,32 if fahrenheit else 0)
          for i,v in enumerate(raw):m.write(0x20000032+i*2,2,v)
          bits=int.from_bytes(struct.pack('<f',value),'little');m.hooks[0xbe2a]=lambda *_:bits
          # Lookup getter by inventory, so a stale address is an error.
          getter=next(a for a,f in FUNCTIONS.items() if f['name']=='measurement_get_value_bits');m.hooks[getter]=lambda *_:bits
          m.call(0x13264,limit=300000)
          alarm=C.c_uint8(old);lib.threshold_alarm((C.c_uint16*6)(*raw),mode,value,fahrenheit,C.byref(alarm))
          assert alarm.value==m.read(0x200001db,1),(mode,fahrenheit,value,old,alarm.value,m.read(0x200001db,1))
          count+=1
    return count

def format_cases():
    count=0;lib.measurement_format_value.argtypes=[C.c_char_p,C.c_size_t,C.c_float,C.c_uint8,C.c_uint8]
    values=[-1234,-999,-100,-99.99,-10,-9.999,-0.001,-0.0,0,0.001,1.234,9.9999,10,99.99,100,999.99,1000,1234]
    for mode,unit in [(0x0c,6),(0x12,21),(0x11,8),(0x16,9)]:
      for value in values:
        m=TraceMachine();m.write(0x20000206,1,mode);m.write(0x20000289,1,unit)
        bits=int.from_bytes(struct.pack('<f',value),'little');m.call(0x99cc,bits,0x20004000,limit=300000)
        expected=m.string(0x20004000);buf=C.create_string_buffer(32)
        lib.measurement_format_value(buf,32,value,mode,unit)
        assert buf.value.decode()==expected,(mode,unit,value,buf.value,expected)
        count+=1
    return count

IMAGE=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_char_p,C.c_uint16,C.c_uint16)
DELAY=C.CFUNCTYPE(None,C.c_void_p,C.c_uint32)
class StatusView(C.Structure):_fields_=[('context',C.c_void_p),('image',IMAGE),('delay',DELAY),('canvas',C.POINTER(Canvas))]
def status_cases():
    count=0
    for entry in [0x14e48,0x14d62]:
      for percent in range(101):
        expected=[];actual=[];m=TraceMachine()
        def rect(_,x,y,x1,y1,color,*args):
            for yy in range(y,y1+1):
                for xx in range(x,x1+1):expected.append(('pixel',xx,yy,color))
        m.hooks[0x8a02]=rect;m.hooks[0x678c]=lambda m,p,x,y,*args:expected.append(('image',m.string(p),x,y))
        m.call(entry,percent)
        canvas=Canvas(None,PIXEL(lambda _,x,y,col:actual.append(('pixel',x,y,col))),320,240)
        view=StatusView(None,IMAGE(lambda _,p,x,y:actual.append(('image',p.decode(),x,y)) or True),DELAY(lambda *_:None),C.pointer(canvas))
        lib.status_battery(C.byref(view),percent,int(entry==0x14d62));assert actual==expected,(entry,percent);count+=1
    for level in range(6):
      for layout in [0,1]:
        expected=[];actual=[];m=TraceMachine();m.write(0x20000205,1,layout)
        m.hooks[0x678c]=lambda m,p,x,y,*args:expected.append(('image',m.string(p),x,y))
        delay_entry=next(a for a,f in FUNCTIONS.items() if f['name']=='delay_ms');m.hooks[delay_entry]=lambda _,ms,*args:expected.append(('delay',ms))
        m.call(0xfe08,level)
        view=StatusView(None,IMAGE(lambda _,p,x,y:actual.append(('image',p.decode(),x,y)) or True),DELAY(lambda _,ms:actual.append(('delay',ms))),None)
        lib.status_ncv(C.byref(view),level,layout);assert actual==expected,(level,layout,actual,expected);count+=1
    return count
class HistoryRecord(C.Structure):_fields_=[('metadata',C.c_uint16),('magnitude',C.c_uint16)]
class History(C.Structure):_fields_=[('count',C.c_uint16),('records',HistoryRecord*30)]
def history_cases():
    count=0
    lib.history_encode.argtypes=[C.c_uint8,C.c_uint8,C.c_float,C.c_uint8,C.POINTER(HistoryRecord)]
    for value in [-9999.0,-1000,-999.9,-100,-99.99,-10,-9.999,-1.234,-0.0,0,1.234,9.999,10,99.99,100,999.9,1000,65535]:
      for overload in [0,1,2]:
       for oldcount in [0,29,30]:
        mode,unit=0xc,6;bits=int.from_bytes(struct.pack('<f',value),'little');m=TraceMachine()
        m.write(0x20000206,1,mode);m.write(0x20000040,2,oldcount)
        m.hooks[0xbe2a]=lambda *_:overload
        getvalue=next(a for a,f in FUNCTIONS.items() if f['name']=='measurement_get_value_bits');getunit=next(a for a,f in FUNCTIONS.items() if f['name']=='measurement_get_unit_code')
        m.hooks[getvalue]=lambda *_:bits;m.hooks[getunit]=lambda *_:unit
        h=History();h.count=oldcount
        for i in range(30):h.records[i]=HistoryRecord(i+1,i*31);m.write(0x20000042+i*4,4,h.records[i].metadata|(h.records[i].magnitude<<16))
        m.call(0xda9c,limit=200000)
        r=HistoryRecord();assert lib.history_encode(mode,unit,value,overload,C.byref(r));lib.history_push(C.byref(h),r)
        expected=m.read(0x20000040,122).to_bytes(122,'little')
        assert bytes(h)==expected,(value,overload,oldcount,bytes(h)[:10],expected[:10]);count+=1
    return count

class Browser(C.Structure):_fields_=[('page',C.c_uint8),('pages',C.c_uint8),('count',C.c_uint8),('active',C.c_bool),('dirty',C.c_bool)]
def browser_cases():
    lib.history_browser_rows.restype=C.c_uint8
    lib.history_browser_step.restype=C.c_bool
    count=0
    for records in range(31):
      for exit_event in [4,6]:
        seq=[0]+[1]*7+[2]*7+[exit_event];queue=list(seq);m=TraceMachine();expected=[];actual=[]
        m.write(0x20000040,2,records);m.stub(0x8a02,0x14408)
        m.hooks[0x14578]=lambda _,page,rows,*args:expected.append((page,rows))
        def feed(m):
            if m.pc==0x14b34:
                if not queue:raise AssertionError('history browser requested extra input')
                m.write(0x200001ee,1,queue.pop(0))
        m.instruction_hook=feed;m.call(0x14a92,limit=300000)
        b=Browser();lib.history_browser_begin(C.byref(b),records)
        for event in seq:
            e=C.c_uint8(event);done=lib.history_browser_step(C.byref(b),C.byref(e))
            if not done and b.dirty:actual.append((b.page,lib.history_browser_rows(C.byref(b))));b.dirty=False
        assert actual==expected,(records,actual,expected);assert not queue;count+=1
    return count

def scrollbar_cases():
    count=0
    for layout in [0,1]:
      for position in range(8):
        expected=[];actual=[];m=TraceMachine();m.write(0x20000205,1,layout)
        def rect(_,x,y,x1,y1,col,*args):
            for yy in range(y,y1+1):
                for xx in range(x,x1+1):expected.append((xx,yy,col))
        m.hooks[0x8a02]=rect;m.hooks[0x7f00]=lambda _,x,y,col,*args:expected.append((x,y,col))
        m.call(0x14408,position)
        canvas=Canvas(None,PIXEL(lambda _,x,y,col:actual.append((x,y,col))),320,240)
        view=StatusView(None,IMAGE(lambda *_:True),DELAY(lambda *_:None),C.pointer(canvas))
        lib.status_scrollbar(C.byref(view),position,layout)
        assert actual==expected,(layout,position,actual[:5],expected[:5]);count+=1
    return count

PC_SET=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16,C.c_bool)
PB_WRITE=C.CFUNCTYPE(None,C.c_void_p,C.c_uint16)
class DisplayBus(C.Structure):_fields_=[('context',C.c_void_p),('set_pc',PC_SET),('write_pb',PB_WRITE)]
class Display(C.Structure):_fields_=[('bus',DisplayBus),('width',C.c_uint16),('height',C.c_uint16),('column',C.c_uint8),('row',C.c_uint8),('pixels',C.c_uint8),('orientation',C.c_uint8)]
def chinese_cases():
    count=0
    lib.display_chinese_glyph.argtypes=[C.POINTER(Display),C.c_uint16,C.c_uint16,C.c_uint16,C.c_uint16,C.c_uint16,C.c_uint8,C.c_bool]
    lib.display_chinese_string.argtypes=[C.POINTER(Display),C.c_uint16,C.c_uint16,C.POINTER(C.c_uint8),C.c_uint16,C.c_uint16,C.c_uint8,C.c_bool]
    for size,entry,base,stride,records in [(12,0x8a5a,0x21dbc,26,12),(16,0x8b5c,0x21ef4,34,63),(24,0x8c64,0x22754,74,49),(32,0x8d6c,0x23580,130,1)]:
      for index in range(records):
       for transparent in [False,True]:
        m=TraceMachine();key=m.read(base+index*stride,2);m.write(0x20003000,2,key)
        m.write(0x20000278,2,0x2b2a);m.write(0x20000276,2,0x2c00)
        expected=[];actual=[];mode=['C']
        m.hooks[0x7d6a]=lambda _,v,*args:expected.append(('C',v&65535))
        m.hooks[0x7dba]=lambda _,v,*args:expected.append(('D',v&65535))
        m.call(entry,10,20,0x20003000,0x1234,0x4321,size,int(transparent),limit=500000)
        def pc(_,mask,high):
            if mask==32:mode[0]='D' if high else 'C'
        bus=DisplayBus(None,PC_SET(pc),PB_WRITE(lambda _,v:actual.append((mode[0],v))))
        d=Display(bus,320,240,0x2a,0x2b,0x2c,3)
        code=((key&255)<<8)|(key>>8)
        lib.display_chinese_glyph(C.byref(d),10,20,code,0x1234,0x4321,size,transparent)
        assert actual==expected,(size,index,transparent,len(actual),len(expected),actual[:12],expected[:12]);count+=1
      for transparent in [False,True]:
        m=TraceMachine();key=m.read(base,2);raw=key.to_bytes(2,'little')*2+b'\0';m.write(0x20003000,len(raw),int.from_bytes(raw,'little'))
        m.write(0x20000278,2,0x2b2a);m.write(0x20000276,2,0x2c00);expected=[];actual=[];mode=['C']
        m.hooks[0x7d6a]=lambda _,v,*args:expected.append(('C',v&65535));m.hooks[0x7dba]=lambda _,v,*args:expected.append(('D',v&65535))
        m.call(0x8e46,10,20,0x20003000,0x1234,0x4321,size,int(transparent),limit=1000000)
        def pc(_,mask,high):
            if mask==32:mode[0]='D' if high else 'C'
        d=Display(DisplayBus(None,PC_SET(pc),PB_WRITE(lambda _,v:actual.append((mode[0],v)))),320,240,0x2a,0x2b,0x2c,3)
        lib.display_chinese_string(C.byref(d),10,20,(C.c_uint8*len(raw)).from_buffer_copy(raw),0x1234,0x4321,size,transparent)
        assert actual==expected,(size,transparent,'string');count+=1
    return count

if __name__=='__main__':
    for name,fn in [('button_policies',button_cases),('event_state',event_cases),('frame_value',parse_cases),('menu_policy',menu_cases),('graphics',graphics_cases),('thresholds',threshold_cases),('format',format_cases),('status_rendering',status_cases),('history',history_cases),('history_browser',browser_cases),('scrollbar',scrollbar_cases),('chinese_glyphs',chinese_cases)]:
        counts[name]=fn();print(name,counts[name],'passed',flush=True)
    (ROOT/'analysis/application_differential.json').write_text(json.dumps({'cases':counts,'total':sum(counts.values()),'oracle':'Preserved original instructions; IO/render hooks; no hardware emulation','hardware_validated':False},indent=2)+'\n')

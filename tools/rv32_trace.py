"""Fail-closed integer execution over the preserved Ghidra instruction inventory.

This is a unit-test oracle, not an MCU emulator. Decoded operands come from
the existing Xwchc-validated export; the original image supplies ROM data. MMIO and missing instructions are errors. Hooks isolate
rendering/IO callees and record their arguments; no analysis is regenerated.
"""
import json,re,struct
from pathlib import Path
from rv32_subset import sx
ROOT=Path(__file__).resolve().parents[1]
REGS='zero ra sp gp tp t0 t1 t2 s0 s1 a0 a1 a2 a3 a4 a5 a6 a7 s2 s3 s4 s5 s6 s7 s8 s9 s10 s11 t3 t4 t5 t6'.split()
RI={n:i for i,n in enumerate(REGS)}
INSTRUCTIONS={int(i['address'],16):i for i in json.loads((ROOT/'analysis/instructions.json').read_text())}

# Incremental NCV export extends the old inventory without rewriting it.
GAP=ROOT/'analysis/ncv_gap_instructions.json'
if GAP.exists():
    firmware=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes()
    for row in json.loads(GAP.read_text()):
        address=int(row['address'],16)
        assert firmware[address-0x6400:address-0x6400+row['length']].hex()==row['bytes']
        if address in INSTRUCTIONS:assert INSTRUCTIONS[address]['text']==row['text']
        else:INSTRUCTIONS[address]=row

class TraceMachine:
    def __init__(self):
        self.image=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes()
        self.r=[0]*32;self.r[2]=0x2000fe00;self.r[3]=0x200009b0
        self.ram=bytearray(65536);self.ram[:488]=self.image[0x271d0-0x6400:0x271d0-0x6400+488]
        self.pc=0;self.instruction_hook=None;self.hooks={};self.trace=[];self.steps=0
    def read(self,a,n):
        a&=0xffffffff
        if 0x6400<=a and a+n<=0x6400+len(self.image):b=self.image[a-0x6400:a-0x6400+n]
        elif 0x20000000<=a and a+n<=0x20010000:b=self.ram[a-0x20000000:a-0x20000000+n]
        else:raise RuntimeError(f'Unmapped read {a:x} size {n}, PC {self.pc:x}')
        return int.from_bytes(b,'little')
    def write(self,a,n,v):
        a&=0xffffffff
        if not 0x20000000<=a or a+n>0x20010000:raise RuntimeError(f'Unmapped write {a:x}, PC {self.pc:x}')
        self.ram[a-0x20000000:a-0x20000000+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little')
    def reg(self,s):return self.r[RI[s]]
    def addr(self,s):
        m=re.fullmatch(r'(-?0x[0-9a-f]+)\((\w+)\)',s)
        if not m:raise RuntimeError(s)
        return (int(m[1],0)+self.reg(m[2]))&0xffffffff
    def call(self,entry,*args,limit=100000):
        self.pc=entry;self.r[1]=0xfffffffe
        for i,v in enumerate(args):self.r[10+i]=v&0xffffffff
        for self.steps in range(limit):
            if self.pc==0xfffffffe:return self.r[10]
            if self.pc in self.hooks:
                target=self.pc;args=tuple(self.r[10:18]);self.trace.append((target,args))
                result=self.hooks[target](self,*args)
                if result is not None:self.r[10]=result&0xffffffff
                self.pc=self.r[1];continue
            if self.instruction_hook:self.instruction_hook(self)
            ins=INSTRUCTIONS.get(self.pc)
            if not ins:raise RuntimeError(f'Missing instruction at {self.pc:x}')
            pc=self.pc;self.read(pc,ins['length']);self.pc+=ins['length']
            op,_,operands=ins['text'].partition(' ');a=operands.split(',') if operands else []
            op=op.removeprefix('qk.').removeprefix('c.');r=self.r
            def put(v):r[RI[a[0]]]=v&0xffffffff
            def val(s):return self.reg(s) if s in RI else int(s,0)
            if op in ('li','lui','auipc'):
                immediate=int(a[1],0)
                if ins['text'].startswith('c.lui '):
                    # Preserved text renders an unsigned6-bit immediate. RV32C
                    # sign-extends bit12; read raw evidence rather than treating
                    # e.g c.lui 0x3e as a positive full-width LUI.
                    raw=self.read(pc,2);immediate=sx(((raw>>12)&1)<<5 | ((raw>>2)&31),6)
                put(immediate*(4096 if op!='li' else 1)+(pc if op=='auipc' else 0))
            elif op in ('mv','not','neg'):put(val(a[1]) if op=='mv' else ~val(a[1]) if op=='not' else -val(a[1]))
            elif op in ('addi16sp','addi4spn'):put(sum(val(s) for s in (a if len(a)==2 else a[1:])))
            elif op in ('add','addi','sub','and','andi','or','ori','xor','xori','sll','slli','srl','srli','sra','srai','sltu','sltiu','mul','mulhu','div','divu','rem','remu'):
                x=val(a[0] if len(a)==2 else a[1]);y=val(a[-1]);base=op[:-1] if op.endswith('i') else op
                if base=='add':z=x+y
                elif base=='sub':z=x-y
                elif base=='and':z=x&y
                elif base=='or':z=x|y
                elif base=='xor':z=x^y
                elif base=='sll':z=x<<(y&31)
                elif base=='srl':z=x>>(y&31)
                elif base=='sra':z=sx(x,32)>>(y&31)
                elif base in ('sltu','sltiu'):z=int(x<(y&0xffffffff))
                elif base=='mul':z=x*y
                elif base=='mulhu':z=(x*y)>>32
                elif base in ('divu','remu'):z=(0xffffffff if base=='divu' else x) if y==0 else (x//y if base=='divu' else x%y)
                else:
                    xx,yy=sx(x,32),sx(y,32)
                    q= -1 if yy==0 else (abs(xx)//abs(yy))*(-1 if (xx<0)!=(yy<0) else 1)
                    z=q if base=='div' else xx if yy==0 else xx-q*yy
                put(z)
            elif op in ('lw','lwsp','lh','lhu','lb','lbu','lbusp'):
                n=4 if op.startswith('lw') else 2 if op.startswith('lh') else 1
                v=self.read(self.addr(a[1]),n);put(sx(v,n*8) if op in ('lb','lh') else v)
            elif op in ('sw','swsp','sh','shsp','sb','sbsp'):
                self.write(self.addr(a[1]),4 if op.startswith('sw') else 2 if op.startswith('sh') else 1,val(a[0]))
            elif op in ('beq','bne','bge','bgeu','blt','bltu','beqz','bnez'):
                x=val(a[0]);y=0 if op.endswith('z') else val(a[1])
                take={'beq':x==y,'bne':x!=y,'bge':sx(x,32)>=sx(y,32),'bgeu':x>=y,'blt':sx(x,32)<sx(y,32),'bltu':x<y,'beqz':x==0,'bnez':x!=0}[op]
                if take:self.pc=int(a[-1],0)
            elif op in ('j','jal','jr','jalr','ret'):
                rd=None
                if op=='ret':target=r[1]
                elif op=='jr':target=self.addr(a[0]) if '(' in a[0] else val(a[0])
                elif op=='jalr':
                    rd=RI[a[0]] if len(a)>1 else 1;target=val(a[-1]) if len(a)==1 else val(a[1])+int(a[2],0)
                else:
                    target=int(a[-1],0)
                    if op=='jal':rd=RI[a[0]] if len(a)>1 else 1
                if rd is not None:r[rd]=self.pc
                self.pc=target&0xfffffffe
            else:raise RuntimeError(f'Unsupported {ins["text"]} at {pc:x}')
            r[0]=0
        raise RuntimeError(f'Instruction budget exceeded at {self.pc:x}')
    def stub(self,*addresses):
        for a in addresses:self.hooks[a]=lambda *_:0
    def string(self,a):
        b=bytearray()
        while self.read(a,1):b.append(self.read(a,1));a+=1
        return b.decode('latin1')

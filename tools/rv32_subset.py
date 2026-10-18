"""Small fail-closed RV32 interpreter for differential tests, not a device emulator.

Implements only instructions needed by 0x98aa and four recovered settings routines.
Unsupported instructions fail, rather than inventing CPU behavior. No MMIO exists.
"""
import struct

def sx(value,bits):
    return value-(1<<bits) if value&(1<<(bits-1)) else value

class Machine:
    def __init__(self, firmware, base=0x6400):
        self.firmware=firmware;self.base=base;self.r=[0]*32;self.memory={};self.pc=0
        self.r[3]=0x200009b0
    def read(self,a,size):
        if self.base<=a and a+size<=self.base+len(self.firmware):
            return int.from_bytes(self.firmware[a-self.base:a-self.base+size],'little')
        return sum(self.memory.get(a+i,0)<<(8*i) for i in range(size))
    def write(self,a,size,value):
        if not 0x20000000<=a<0x20010000:raise RuntimeError(f'Write outside test RAM: {a:x}')
        for i in range(size):self.memory[a+i]=(value>>(8*i))&255
    def run(self,entry,*args):
        self.pc=entry;self.r[1]=0xfffffffe
        for i,v in enumerate(args):self.r[10+i]=v&0xffffffff
        for _ in range(1000):
            if self.pc==0xfffffffe:return self.r[10]
            pc=self.pc;h=self.read(pc,2);r=self.r;self.pc+=2
            if h&3!=3:
                q=h&3;f=h>>13
                if q==1 and f in (0,2):
                    rd=(h>>7)&31;imm=sx(((h>>2)&31)|((h>>12&1)<<5),6)
                    r[rd]=imm if f==2 else r[rd]+imm
                elif q==1 and f in (6,7):
                    rs=8+((h>>7)&7)
                    imm=((h>>12&1)<<8)|((h>>5&3)<<6)|((h>>2&1)<<5)|((h>>10&3)<<3)|((h>>3&3)<<1)
                    if (r[rs]==0)==(f==6):self.pc=pc+sx(imm,9)
                elif q==1 and f==4:
                    rd=8+((h>>7)&7);kind=(h>>10)&3
                    if kind==0:r[rd]>>=(h>>2)&31
                    elif kind==1:r[rd]=sx(r[rd],32)>>((h>>2)&31)
                    elif kind==2:r[rd]&=sx(((h>>2)&31)|((h>>12&1)<<5),6)
                    else:
                        rs=8+((h>>2)&7);op=(h>>5)&3
                        if op==0:r[rd]-=r[rs]
                        elif op==1:r[rd]^=r[rs]
                        elif op==2:r[rd]|=r[rs]
                        else:r[rd]&=r[rs]
                elif q==1 and f==3:
                    rd=(h>>7)&31;r[rd]=sx(((h>>2)&31)|((h>>12&1)<<5),6)<<12
                elif q==2 and f==0:
                    rd=(h>>7)&31;r[rd]<<=(h>>2)&31
                elif q==2 and f==4 and (h&0x7c)==0 and not h&0x1000:
                    self.pc=r[(h>>7)&31]&~1
                elif q==2 and f==4 and not h&0x1000:
                    r[(h>>7)&31]=r[(h>>2)&31]
                elif q==2 and f in (1,5):
                    rs=8+((h>>7)&7);rt=8+((h>>2)&7)
                    offset=((h>>10&7)<<3)|((h>>5&3)<<1)
                    addr=(r[rs]+offset)&0xffffffff
                    if f==1:r[rt]=self.read(addr,2)
                    else:self.write(addr,2,r[rt])
                else:raise RuntimeError(f'Unsupported compressed 0x{h:04x} at {pc:x}')
            else:
                w=self.read(pc,4);self.pc=pc+4;op=w&127;rd=(w>>7)&31;rs=(w>>15)&31;rt=(w>>20)&31;f=(w>>12)&7;imm=sx(w>>20,12)
                if op==0x13:
                    if f==0:r[rd]=r[rs]+imm
                    elif f==7:r[rd]=r[rs]&imm
                    elif f==6:r[rd]=r[rs]|imm
                    elif f==1:r[rd]=r[rs]<<((w>>20)&31)
                    elif f==5:r[rd]=sx(r[rs],32)>>((w>>20)&31) if w&(1<<30) else r[rs]>>((w>>20)&31)
                    else:raise RuntimeError(f'Unsupported OP-IMM {w:x}')
                elif op==0x37:r[rd]=w&0xfffff000
                elif op==0x63:
                    imm=((w>>31)<<12)|((w>>7&1)<<11)|((w>>25&63)<<5)|((w>>8&15)<<1)
                    if f not in (0,1):raise RuntimeError(f'Unsupported branch {w:x}')
                    if (r[rs]==r[rt])==(f==0):self.pc=pc+sx(imm,13)
                elif op==3 and f==5:r[rd]=self.read((r[rs]+imm)&0xffffffff,2)
                elif op==0x23 and f==1:
                    imm=sx(((w>>25)<<5)|((w>>7)&31),12);self.write((r[rs]+imm)&0xffffffff,2,r[rt])
                elif op==0x33 and f==6:r[rd]=r[rs]|r[rt]
                elif op==0x33 and f==7:r[rd]=r[rs]&r[rt]
                else:raise RuntimeError(f'Unsupported RV32 0x{w:x} at {pc:x}')
            for i in range(1,32):r[i]&=0xffffffff
            r[0]=0
        raise RuntimeError('Test interpreter instruction limit exceeded')

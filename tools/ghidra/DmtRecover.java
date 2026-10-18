// Repair GCC save-helper semantics and recover additional evidence-backed entrypoints.
//@category DMT99
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.data.*;
import java.util.*;

public class DmtRecover extends GhidraScript {
    long[] saves={0x6600,0x6608,0x6616,0x6630};
    boolean saveTarget(long a) {for(long t:saves)if(a==t)return true;return false;}
    long jal(long w,long pc) {
        long v=((w>>31)<<20)|(((w>>12)&255)<<12)|(((w>>20)&1)<<11)|(((w>>21)&1023)<<1);
        if((v&0x100000)!=0)v-=0x200000;
        return pc+v;
    }
    boolean prologue(long a) throws Exception {
        int w=currentProgram.getMemory().getInt(toAddr(a));
        long u=Integer.toUnsignedLong(w);
        if((w&0xfff)==0x2ef && saveTarget(jal(u,a)))return true;
        int half=w&0xffff;
        // Standard compressed negative SP adjustment, or ADDI sp,sp,negative.
        return ((half&0xef83)==0x6101 && (half&0x1000)!=0) ||
            ((half&0xef83)==0x0101 && (half&0x1000)!=0) ||
            ((w&0x000fffff)==0x10113 && w<0);
    }
    public void run() throws Exception {
        for(String name:new String[]{"apb_ahb_mmio","fsmc_mmio","pfic_systick"}) {
            currentProgram.getMemory().getBlock(name).setVolatile(true);
        }
        int added=0;
        // Retained weak stubs also exist for handlers overridden by the application's
        // vector table. Each is a verified two-byte jump to its own address.
        for(long a=0xc5be;a<0xc668;a+=2) {
            if(Short.toUnsignedInt(currentProgram.getMemory().getShort(toAddr(a)))==0xa001 && getFunctionAt(toAddr(a))==null) {
                disassemble(toAddr(a));if(createFunction(toAddr(a),"unused_default_trap_"+Long.toHexString(a))!=null)added++;
            }
        }
        // The memset tail is 15 four-byte byte stores plus the zero-length return.
        // Targets follow directly from a3=PC(0x6690)+4*(15-count), jr a3+10.
        Instruction fillJump=getInstructionAt(toAddr(0x6696));
        if(fillJump!=null)for(int n=0;n<16;n++) {
            Address target=toAddr(0x669a+4*n);
            fillJump.addMnemonicReference(target,RefType.COMPUTED_JUMP,SourceType.USER_DEFINED);
            disassemble(target);
        }
        Function fill=getFunctionAt(toAddr(0x6660));
        if(fill!=null)CreateFunctionCmd.fixupFunctionBody(currentProgram,fill,monitor);
        for(long a=0x6600;a<0x1e81e;a+=2) {
            int w=currentProgram.getMemory().getInt(toAddr(a));
            if((w&0xfff)==0x2ef && saveTarget(jal(Integer.toUnsignedLong(w),a)) && getFunctionContaining(toAddr(a))==null) {
                disassemble(toAddr(a)); if(createFunction(toAddr(a),null)!=null)added++;
            }
        }
        // Pointer tables can contain basic-block targets. Seed only plausible prologues
        // outside current function bodies, rather than treating every word as a function.
        for(long p=0x1e820;p<0x271d0;p+=4) {
            long a=Integer.toUnsignedLong(currentProgram.getMemory().getInt(toAddr(p)));
            if(a>=0x6660 && a<0x1e81e && (a&1)==0 && getFunctionContaining(toAddr(a))==null && prologue(a)) {
                disassemble(toAddr(a)); if(createFunction(toAddr(a),null)!=null)added++;
            }
        }
        if(added>0)analyzeChanges(currentProgram);
        String[] fixups={"wch_save_12","wch_save_10","wch_save_6","wch_save_2"};
        for(int n=0;n<saves.length;n++) {
            Function f=getFunctionAt(toAddr(saves[n]));
            if(f!=null) {f.setName(fixups[n],SourceType.USER_DEFINED);f.setCallFixup(fixups[n]);f.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);}
        }
        for(long a:new long[]{0x8ed4,0xc320}) {
            Function f=getFunctionAt(toAddr(a));if(f!=null)f.setNoReturn(true);
        }
        // 0x90d2 is a separate strlen immediately after the nonreturning main call.
        Function main=getFunctionAt(toAddr(0x908a));
        if(main!=null)currentProgram.getFunctionManager().removeFunction(main.getEntryPoint());
        createFunction(toAddr(0x908a),"main");
        if(getFunctionAt(toAddr(0x90d2))==null) {disassemble(toAddr(0x90d2));createFunction(toAddr(0x90d2),"strlen");}
        // Re-run switch/constant propagation after fixing helper argument semantics.
        analyzeAll(currentProgram);
        println("DMT recovered additional prologue/table functions: "+added+"; installed four save-helper fixups");
    }
}

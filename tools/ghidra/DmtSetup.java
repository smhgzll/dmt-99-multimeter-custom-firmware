// Configure the verified application image; no device access.
//@category DMT99
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.math.BigInteger;
import java.nio.file.*;

public class DmtSetup extends GhidraScript {
    private void seed(long address, String name) throws Exception {
        Address a = toAddr(address);
        disassemble(a);
        Function f = getFunctionAt(a);
        if (f == null) f = createFunction(a, name);
        if (f != null && name != null) f.setName(name, SourceType.USER_DEFINED);
    }
    public void run() throws Exception {
        Memory mem = currentProgram.getMemory();
        mem.getBlock(toAddr(0x6400)).setName("application_flash");
        mem.getBlock(toAddr(0x6400)).setWrite(false);
        mem.getBlock(toAddr(0x6400)).setExecute(true);
        mem.split(mem.getBlock(toAddr(0x6400)), toAddr(0x6438));
        mem.split(mem.getBlock(toAddr(0x6438)), toAddr(0x6600));
        mem.getBlock(toAddr(0x6438)).setName("vectors_and_padding");
        mem.getBlock(toAddr(0x6438)).setExecute(false);
        mem.split(mem.getBlock(toAddr(0x6600)), toAddr(0x1e820));
        mem.getBlock(toAddr(0x6600)).setName("text");
        mem.getBlock(toAddr(0x1e820)).setName("rodata_and_data_load");
        mem.getBlock(toAddr(0x1e820)).setExecute(false);
        byte[] data = new byte[0x1e8];
        mem.getBytes(toAddr(0x271d0), data);
        mem.createInitializedBlock("sram_data", toAddr(0x20000000), new ByteArrayInputStream(data), data.length, monitor, false).setWrite(true);
        mem.createUninitializedBlock("sram_bss_stack", toAddr(0x200001e8), 0x10000 - 0x1e8, false).setWrite(true);
        mem.createUninitializedBlock("apb_ahb_mmio", toAddr(0x40000000), 0x30000, false).setWrite(true);
        mem.createUninitializedBlock("fsmc_mmio", toAddr(0xa0000000L), 0x1000, false).setWrite(true);
        mem.createUninitializedBlock("pfic_systick", toAddr(0xe000e000L), 0x2000, false).setWrite(true);
        currentProgram.getProgramContext().setValue(currentProgram.getRegister("gp"), toAddr(0x6400), toAddr(0x273b7), BigInteger.valueOf(0x200009b0L));
        Path root = Paths.get(getScriptArgs()[0]);
        Path mmio = root.resolve("analysis/mmio_symbols.tsv");
        if (Files.exists(mmio)) for (String line : Files.readAllLines(mmio)) {
            String[] x = line.split("\t");
            if (x.length >= 2) createLabel(toAddr(Long.parseLong(x[0],16)), x[1], true);
        }
        for (int i=0; i<104; i++) {
            Address slot = toAddr(0x6438L + i*4);
            long target = Integer.toUnsignedLong(mem.getInt(slot));
            createData(slot, Pointer32DataType.dataType);
            if (target >= 0x6600 && target < 0x1e800 && (target & 1) == 0) seed(target, "vector_" + i);
        }
        createLabel(toAddr(0x6438), "interrupt_vector_table", true);
        createLabel(toAddr(0x200009b0), "__global_pointer", true);
        seed(0x6400, "application_entry");
        seed(0xc668, "handle_reset");
        seed(0xd212, "SystemInit");
        seed(0x908a, "main");
        println("DMT setup complete: base=0x6400, gp=0x200009b0, data=0x1e8, bss end=0x200054d8");
    }
}

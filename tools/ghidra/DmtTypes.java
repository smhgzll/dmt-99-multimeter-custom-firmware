//@category DMT99
import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.*;
import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.services.DataTypeManagerService;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;
import java.io.*;
import java.nio.file.*;

public class DmtTypes extends GhidraScript {
    void typed(long addr,DataType type) throws Exception {
        clearListing(toAddr(addr),toAddr(addr+type.getLength()-1));createData(toAddr(addr),type);
    }
    public void run() throws Exception {
        Path root=Paths.get(getScriptArgs()[0]);
        CParser p=new CParser(currentProgram.getDataTypeManager(),true,null);
        p.parse(Files.newInputStream(root.resolve("analysis/recovered_types.h")));
        DataTypeManager dtm=currentProgram.getDataTypeManager();
        for(long a:new long[]{0x40010800,0x40010c00,0x40011000})typed(a,dtm.getDataType("/GPIO_Registers"));
        typed(0x40021000,dtm.getDataType("/RCC_Registers"));
        typed(0x40013000,dtm.getDataType("/SPI_Registers"));
        for(long a:new long[]{0x40013800,0x40004400})typed(a,dtm.getDataType("/USART_Registers"));
        typed(0x40020058,dtm.getDataType("/DMA_Channel_Registers"));
        typed(0x200002ac,dtm.getDataType("/Measurement_Wire13"));
        typed(0x20000042,new ArrayDataType(dtm.getDataType("/History_Record4"),30,4));
        Path sigs=root.resolve("analysis/function_signatures.tsv");
        if(Files.exists(sigs))for(String line:Files.readAllLines(sigs)){
            if(line.isBlank()||line.startsWith("#"))continue;
            String[] x=line.split("\t",2);
            FunctionDefinitionDataType sig=CParserUtils.parseSignature((DataTypeManagerService)null,currentProgram,x[1],false);
            new ApplyFunctionSignatureCmd(toAddr(Long.parseLong(x[0],16)),sig,SourceType.USER_DEFINED).applyTo(currentProgram,monitor);
            getFunctionAt(toAddr(Long.parseLong(x[0],16))).setCallingConvention(currentProgram.getCompilerSpec().getDefaultCallingConvention().getName());
        }
        println("Applied recovered protocol/history/MMIO types and curated function signatures");
    }
}

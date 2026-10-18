//@category DMT99
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.lang.Register;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.pcode.PcodeOp;
import com.google.gson.*;
import java.nio.file.*;

public class DmtDecoderTest extends GhidraScript {
    public void run() throws Exception {
        Path root=Paths.get(getScriptArgs()[0]);
        JsonArray cases=JsonParser.parseString(Files.readString(root.resolve("analysis/decoder_vectors.json"))).getAsJsonArray();
        int n=0;
        for(JsonElement element:cases){
            JsonObject c=element.getAsJsonObject();
            disassemble(toAddr(n*2));Instruction i=getInstructionAt(toAddr(n*2));
            String name=c.get("mnemonic").getAsString();int offset=c.get("offset").getAsInt(),width=c.get("width").getAsInt();
            if(i==null || i.getLength()!=2 || !i.getMnemonicString().equals(name))throw new Exception("Mnemonic mismatch case "+n+": "+i);
            if(!i.getRegister(0).getName().equals("a0"))throw new Exception("Destination/source register mismatch "+n);
            boolean foundOffset=false,foundBase=false,foundAccess=false;
            for(Object o:i.getOpObjects(1)){
                if(o instanceof Scalar && ((Scalar)o).getUnsignedValue()==offset)foundOffset=true;
                if(o instanceof Register && ((Register)o).getName().equals(c.get("base").getAsString()))foundBase=true;
            }
            for(PcodeOp op:i.getPcode()){
                if(name.contains(".l") && op.getOpcode()==PcodeOp.LOAD && op.getOutput().getSize()==width)foundAccess=true;
                if(name.contains(".s") && op.getOpcode()==PcodeOp.STORE && op.getInput(2).getSize()==width)foundAccess=true;
            }
            String expected="0x"+Integer.toHexString(offset)+"("+c.get("base").getAsString()+")";
            boolean operandText=i.toString().replace(" ","").equals(name+"a0,"+expected);
            if(!operandText||!foundAccess)throw new Exception("Operand/pcode mismatch case "+n+": "+i+" operand="+operandText+" access="+foundAccess);
            n++;
        }
        Files.writeString(root.resolve("analysis/decoder_validation.json"),"{\"passed\":"+n+",\"failed\":0,\"producer\":\"LLVM clang integrated assembler\",\"consumer\":\"Ghidra local Xwchc SLEIGH; operands and pcode memory widths\"}\n");
        println("Validated "+n+" LLVM-generated Xwchc instructions against local Ghidra decoder");
    }
}

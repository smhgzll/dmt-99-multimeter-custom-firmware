// Incremental read of the omitted NCV interval; does not rename or decompile.
//@category DMT99
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import com.google.gson.*;
import java.nio.file.*;
import java.util.*;
public class DmtExportGap extends GhidraScript {
 public void run() throws Exception {
  var records=new ArrayList<Map<String,Object>>();
  clearListing(toAddr(0xff48),toAddr(0x1014d));
  for(long pc=0xff48;pc<0x1014e;){
   var address=toAddr(pc);Instruction i=getInstructionAt(address);
   if(i==null){disassemble(address);i=getInstructionAt(address);}
   if(i==null)throw new Exception("Cannot decode gap at "+address);
   var row=new LinkedHashMap<String,Object>();row.put("address",String.format("0x%08x",pc));row.put("length",i.getLength());row.put("mnemonic",i.getMnemonicString());row.put("text",i.toString());
   row.put("bytes",HexFormat.of().formatHex(i.getBytes()));row.put("provenance","NCV omitted interval, existing saved project; read-only headless run");records.add(row);pc+=i.getLength();
  }
  Files.writeString(Path.of(getScriptArgs()[0]),new GsonBuilder().setPrettyPrinting().create().toJson(records)+"\n");
  println("Exported "+records.size()+" previously omitted NCV instructions");
 }
}

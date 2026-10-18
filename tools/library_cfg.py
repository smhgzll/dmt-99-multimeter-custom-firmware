"""Supplementary CFG and memory-reference fingerprints; never equivalence proof."""
import json,re,collections,subprocess
from pathlib import Path
from library_fingerprints import ROOT,op

def graph(code):
 if not code:return {'blocks':0,'edges':0,'back_edges':0}
 addresses={x[0] for x in code};leaders={code[0][0]};edges=[];memory=collections.Counter()
 for i,(pc,text) in enumerate(code):
  name=op(text);target=None;nextpc=code[i+1][0] if i+1<len(code) else None
  for off in re.findall(r'(-?(?:0x[\da-f]+|\d+))\(\w+\)',text):memory[str(int(off,0))]+=1
  conditional=name.startswith('b') and name not in ['bitext'];unconditional=text.split()[0] in ['j','c.j','ret','c.jr','jr','jalr','c.jalr']
  if conditional or text.split()[0] in ['j','c.j']:
   m=re.search(r'(?:,|\s)(0x[\da-f]+|[\da-f]+)(?:\s*<[^>]+>)?$',text)
   if m:
    value=m[1];target=int(value,16)
    if target in addresses:leaders.add(target);edges.append((pc,target))
  if conditional and nextpc is not None:leaders.add(nextpc);edges.append((pc,nextpc))
  elif not unconditional and nextpc is not None:edges.append((pc,nextpc))
 # Collapse instruction edges into basic blocks, retaining self-loop branches.
 starts=sorted(leaders)
 def block(pc):return max(x for x in starts if x<=pc)
 be={(block(a),block(b)) for a,b in edges if block(a)!=block(b) or b in leaders}
 return {'blocks':len(starts),'edges':len(be),'back_edges':sum(b<=a for a,b in be),'memory_operand_offset_counts':dict(sorted(memory.items(),key=lambda x:int(x[0])))}
def main():
 p=ROOT/'analysis/library_fingerprints.json';report=json.loads(p.read_text());ins=json.loads((ROOT/'analysis/instructions.json').read_text());functions={x['address']:x for x in json.loads((ROOT/'analysis/functions.json').read_text())}
 dis=(ROOT/'reconstructed/build/recovered_modules.disasm').read_text();compiled={};current=None
 for line in dis.splitlines():
  m=re.match(r'^[\da-f]+ <([^>]+)>:',line)
  if m:current=m[1];compiled[current]=[]
  m=re.match(r'^\s*([\da-f]+):\s+[\da-f]+\s+(.+)$',line)
  if m and current:compiled[current].append((int(m[1],16),m[2].strip().split('#')[0].strip()))
 for r in report['functions']:
  ranges=[(int(a,16),int(b,16)) for a,b in functions[r['address']]['ranges']]
  code=[(int(i['address'],16),i['text']) for i in ins if any(a<=int(i['address'],16)<=b for a,b in ranges)]
  r['original_cfg']=graph(code);r['linked_counterpart_cfg']=graph(compiled.get(r['symbol'],[]));r['cfg_note']='Compiled graph uses linked, relocated code; missing static symbols were inlined. Basic-block topology and memory offsets are supplementary fingerprints. Differences/absence are retained, not scored as equivalence.'
 p.write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()

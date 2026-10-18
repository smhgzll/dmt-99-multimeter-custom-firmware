#!/usr/bin/env python3
"""Rank the140 Phase4 incomplete bodies; preserve the original analysis files.

Priority is an explicit application-first review policy, not an assertion that
static call-graph reachability proves a path executable on the device.
"""
import hashlib
import json
from collections import deque
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def read(path):return json.loads((ROOT/path).read_text())
def main():
    functions=read('analysis/functions.json');by_address={f['address']:f for f in functions}
    baseline_path='analysis/phase4_continuation/baseline_source_coverage.json'
    baseline={f['address']:f for f in read(baseline_path)['functions']}
    current={f['address']:f for f in read('analysis/source_coverage.json')['functions']}
    sdk={f['address'] for f in read('analysis/instruction_comparison.json')['functions']}
    ins={i['address']:i for i in read('analysis/instructions.json')}
    traps={f['address'] for f in functions if f['size']==2 and ins.get(f['address'],{}).get('flows')==[f['address']]}
    old_full={a for a,f in baseline.items() if f['extent']=='full'}
    full={a for a,f in current.items() if f['extent']=='full'}
    original_remaining=set(by_address)-old_full-sdk-traps
    remaining=set(by_address)-full-sdk-traps
    assert len(original_remaining)==140 and len(traps)==86
    semantic=read('analysis/semantic_registry.json')
    names={a:f.get('name',by_address[a]['name']) for a,f in semantic.items() if isinstance(f,dict) and a in by_address}
    names.update({f['address']:f['name'] for f in read('analysis/semantic_continuation.json')['functions']})
    first=[0x8ed4,0x908a,0xc248,0x169d4,0x164d4,0x1598c,0x15af6,0x15168,
           0x14cd6,0xb5c8,0xb9e0,0xbc90,0xa910,0x97f8,0xda48,0xd964,0xdc46]
    first={f'0x{a:08x}':i for i,a in enumerate(first)}
    boot={0x6400,0xc668,0xd212,0xd2fc,0xcb8e,0xcb94,0x7562,0x7598,0x75f0,0x767a,
          0x770a,0x780c,0x792e,0x7f66,0x8016,0x87aa,0x1a626,0x1a64c,0x1a682,0xdd4e}
    roots=[f['address'] for f in functions if 'IRQHandler' in f['name']]+['0x0000908a','0x00008ed4','0x0000c668','0x000169d4']
    reached=set();queue=deque(roots)
    while queue:
        address=queue.popleft()
        if address in reached or address not in by_address:continue
        reached.add(address);queue.extend(by_address[address]['callees'])
    def tier(address):
        pc=int(address,16)
        if address in first:return 0,'application control / menu / scheduling'
        if pc in boot:return 1,'startup dependency / timer / power IO'
        if 0xa000<=pc<0xc248 or pc==0x1a336:return 2,'application display / measurement glue'
        if 0x6600<=pc<0x6640 or pc>=0x1af02:return 5,'compiler / arithmetic / libc runtime'
        if 0x1a6e2<=pc<0x1af02 or pc in [0x90d2,0x910e,0x969e]:return 4,'format / debug library'
        return 3,'display / storage / platform dependency'
    ordered=sorted(original_remaining,key=lambda a:(tier(a)[0],first.get(a,1000),a not in reached,-len(by_address[a]['callers']),int(a,16)))
    rows=[]
    for rank,a in enumerate(ordered,1):
        f=by_address[a];entry=current.get(a,{})
        rows.append({'rank':rank,'address':a,'name':names.get(a,f['name']),
                     'priority_tier':tier(a)[0],'priority_reason':tier(a)[1],
                     'baseline_extent':baseline.get(a,{}).get('extent','untranslated'),
                     'current_extent':entry.get('extent','untranslated'),
                     'static_callgraph_reachable':a in reached,'body_bytes':f['size'],
                     'callers':f['callers'],'callees':f['callees'],
                     'remaining_direct_dependencies':[c for c in f['callees'] if c in remaining],
                     'current_boundary':entry.get('scope','No complete native or qualified upstream counterpart'),
                     'evidence':f['clean_pseudocode_file']})
    report={'baseline_incomplete_nontrap':140,'current_incomplete_nontrap':len(remaining),
            'completed_from_ranked_baseline':len(original_remaining-remaining),
            'baseline_partial':29,'current_partial':sum(f['extent']=='partial' for f in current.values()),
            'baseline_coverage_sha256':hashlib.sha256((ROOT/baseline_path).read_bytes()).hexdigest(),
            'definition':'Application-first priority. Dependencies are recorded direct calls, not a topological execution order; indirect calls/tail transfers may be absent. Full source credit is separate from physical validation. No bootloader/USB/layout assumptions.',
            'functions':rows}
    (ROOT/'analysis/incomplete_priority.json').write_text(json.dumps(report,indent=2)+'\n')
    lines=['# Phase4 continuation function priorities','',
           f'The original140 incomplete non-trap bodies are ranked below. **{len(remaining)} remain incomplete**; {len(original_remaining-remaining)} have been completed in this continuation. Current partial entries remain included in the incomplete total.','',
           'Startup/application/menu/scheduling are first. The remaining-dependencies column uses preserved direct call edges; it excludes completed native/SDK/qualified-library counterparts. Static reachability is not a hardware execution claim. Source gaps and valid domains remain in `source_coverage.json`.','',
           '| Rank | Original entry | Role | Current | Remaining direct dependencies |','|---:|---|---|---|---|']
    for r in rows:
        deps=', '.join(c.removeprefix('0x000') for c in r['remaining_direct_dependencies']) or '—'
        lines.append(f"| {r['rank']} | [{r['address']}](../{r['evidence']}) | {r['name']} | {r['current_extent']} | {deps} |")
    lines+=['','Machine-readable roles, body sizes, callers, callees, reachability, tiers and current barriers: `analysis/incomplete_priority.json`.','',
            'Bootloader, actual USB MSC and internal Flash layout confirmation are deferred. RDP does not block reconstruction of application instructions already present in V2.4. Unknown CP936 physical reads, target timing and calibration remain unresolved.','']
    (ROOT/'docs/FUNCTION_PRIORITIES.md').write_text('\n'.join(lines))
    print(f'{len(original_remaining)} ranked; {len(remaining)} incomplete; {report["current_partial"]} partial')
if __name__=='__main__':main()

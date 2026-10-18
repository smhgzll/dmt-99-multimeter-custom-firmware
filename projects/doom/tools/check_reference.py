#!/usr/bin/env python3
"""Check immutable evidence after monorepo migration; parent source may change.

The 5017-file reference_hashes.json is historical. It has not been rebaselined.
"""
import hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
REF=ROOT.parents[1]
if __name__=='__main__':
    before=json.loads((REF/'docs/integration/evidence_hashes.json').read_text())
    changed=[n for n,h in before.items() if not (REF/n).is_file() or hashlib.sha256((REF/n).read_bytes()).hexdigest()!=h]
    result={'scope':'immutable BIN/NOR/disassembly/decompilation, not the maintained parent source',
            'baseline_files':len(before),'changed_or_missing':changed,'pass':not changed}
    (ROOT/'build').mkdir(exist_ok=True)
    (ROOT/'build/reference_preservation.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result));raise SystemExit(0 if result['pass'] else 1)

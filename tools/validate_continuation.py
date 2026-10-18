#!/usr/bin/env python3
"""Incremental continuation build/tests/report. Never imports/reanalyzes firmware.

The original inventory, decompilation and reports remain baseline evidence. New
names, source coverage and omitted instructions are overlays. Heavy differential
checks are reused only if their source/evidence digest is unchanged.
"""
import argparse,hashlib,json,re,subprocess,sys
from pathlib import Path
from validate import sections
ROOT=Path(__file__).resolve().parents[1]
def read(path):return json.loads((ROOT/path).read_text())
def write(path,value):
    p=ROOT/path;s=json.dumps(value,indent=2)+'\n'
    if not p.exists() or p.read_text()!=s:p.write_text(s)
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def command(args,log):
    with log.open('w') as out:subprocess.run(args,cwd=ROOT,stdout=out,stderr=subprocess.STDOUT,check=True)
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--reproducible',action='store_true');args=ap.parse_args()
    evidence=read('analysis/continuation_evidence_hashes.json')
    for path,digest in evidence.items():assert sha(ROOT/path)==digest,f'Preserved evidence changed: {path}'
    image=ROOT/'DMT-99_V2.4_20240306.bin';assert image.stat().st_mode&0o222==0,'Original BIN must remain read-only'
    logs=ROOT/'logs';logs.mkdir(exist_ok=True)
    command(['make','-C','reconstructed']+(['-B'] if args.reproducible else [])+['-j4','all','host-test'],logs/'continuation-build.log')
    elf=ROOT/'reconstructed/build/reconstruction.elf';first=sha(elf)
    retained=ROOT/'reconstructed/build/recovered_modules.elf';first_retained=sha(retained)
    previous=read('analysis/continuation_validation.json') if (ROOT/'analysis/continuation_validation.json').exists() else {}
    reproducible=None;retained_reproducible=None
    if args.reproducible:
        command(['make','-C','reconstructed','-B','-j4','all'],logs/'continuation-rebuild.log');reproducible=first==sha(elf);retained_reproducible=first_retained==sha(retained);assert reproducible and retained_reproducible
    elif previous.get('elf_sha256')==first:
        reproducible=previous.get('reproducible_elf')
        if previous.get('retained_modules_elf_sha256')==first_retained:retained_reproducible=previous.get('retained_modules_reproducible')
    # Both source input and evidence changes invalidate the differential cache.
    inputs=sorted(p for p in (ROOT/'reconstructed').rglob('*') if p.suffix in ['.c','.h'] and not any(x in p.parts for x in ['build']) and not ('vendor' in p.parts and 'wch' in p.parts))
    inputs += [ROOT/'tools'/p for p in ['test_application_differential.py','rv32_trace.py','rv32_subset.py','validate.py']]
    inputs += [ROOT/'reconstructed/Makefile',ROOT/'analysis/instructions.json',ROOT/'analysis/ncv_gap_instructions.json',image]
    fingerprint=hashlib.sha256(b''.join(str(p.relative_to(ROOT)).encode()+p.read_bytes() for p in inputs)).hexdigest()
    if previous.get('differential_inputs_sha256')!=fingerprint:
        command([sys.executable,'tools/test_application_differential.py'],logs/'continuation-differential.log')
        # Reuse the baseline tests without regenerating their evidence report.
        import validate
        validate.dump=lambda *_:None
        baseline=validate.differential()
    else:baseline=previous['baseline_differential']
    diff=read('analysis/application_differential.json')
    baseline_ins=read('analysis/instructions.json');by_pc={int(i['address'],16):i for i in baseline_ins}
    new_rows=[]
    for row in read('analysis/ncv_gap_instructions.json'):
        pc=int(row['address'],16);assert image.read_bytes()[pc-0x6400:pc-0x6400+row['length']].hex()==row['bytes']
        if pc not in by_pc:new_rows.append(row);by_pc[pc]=row
    added=sum(i['length'] for i in new_rows)
    write('analysis/ncv_inventory_delta.json',{'original_function':'0x0000fe08','original_classified_body_bytes':322,'additional_instruction_records':len(new_rows),'additional_instruction_bytes':added,'range':['0x0000ff48','0x0001014c'],'preserved_baseline':True,'note':'Read-only saved Ghidra project; cleared only omitted interval transiently. Changes discarded, original exports untouched.'})
    functions=read('analysis/functions.json');semantic=read('analysis/semantic_registry.json');overlay=read('analysis/semantic_continuation.json')['functions']
    addresses={f['address'] for f in functions};assert len(overlay)==186
    classified=set(semantic)|{f['address'] for f in overlay};assert classified==addresses
    coverage=read('analysis/source_coverage.json')['functions'];assert len(set(f['address'] for f in coverage))==len(coverage)
    for f in coverage:assert f['address'] in addresses and (ROOT/f['source']).exists()
    full={f['address'] for f in coverage if f['extent']=='full' and f.get('kind')!='upstream_library'};partial={f['address'] for f in coverage if f['extent']=='partial' and f.get('kind')!='upstream_library'}
    libraries={f['address'] for f in coverage if f.get('kind')=='upstream_library' and f['extent']=='full'}
    sdk=read('analysis/instruction_comparison.json');sdk_addresses={f['address'] for f in sdk['functions']}
    original_by_pc={i['address']:i for i in baseline_ins}
    traps={f['address'] for f in functions if f['size']==2 and original_by_pc.get(f['address'],{}).get('flows')==[f['address']]}
    assert len(traps)==86
    detailed=full|sdk_addresses|libraries|traps
    entry_checked={int(a,16) for a in ['b5c8','b9e0','bc90','a910','aed0','b29e','be78','164d4','1598c','15af6','8050','8194','81ce','83d6','8266','11086','13264','99cc','14e48','14d62','fe08','da9c','14a92','14408','98aa','d944','d9e6','da04','da28','8a5a','8b5c','8c64','8d6c','8e46']}
    _,sec=sections(elf);build=(logs/'continuation-build.log').read_text();passed=re.search(r'(\d+) tests passed',build);integration=re.search(r'(\d+) application/storage integration tests passed',build)
    assert passed and integration
    module_paths=[p for p in (ROOT/'reconstructed').rglob('*.c') if not any(x in p.parts for x in ['build','vendor','tests'])]
    result={'input_sha256':sha(image),'preserved_evidence_files':len(evidence),'identified_instruction_bytes':sum(i['length'] for i in by_pc.values()),'code_gap_bytes':534-added,'discovered_functions':len(functions),'decompiled_functions':len(functions),'role_classified_functions':len(classified),'previously_unresolved_role_reviews':len(overlay),'unclassified_roles':len(addresses-classified),'detailed_semantics_confirmed_lower_bound':len(detailed),'detailed_semantics_definition':'Full valid-domain native routines, pinned SDK and qualified upstream counterparts, and proven default loops. Role labels alone do not satisfy this stricter metric.','full_source_translated_functions':len(full),'partial_source_translated_functions':len(partial),'functions_without_complete_native_translation':len(addresses-full),'compiled_native_translations_full_or_partial':len(full|partial),'compiled_sdk_function_counterparts':len(sdk_addresses),'machine_code_checked_entry_functions':len(entry_checked),'machine_code_checked_entries':[f'0x{x:08x}' for x in sorted(entry_checked)],'native_c_translation_units':len(module_paths),'host_tests_passed':int(passed[1])+int(integration[1]),'host_tests_failed':0,'new_differential_cases':diff['total'],'baseline_differential':baseline,'total_differential_cases':diff['total']+baseline['segment_inputs_exhaustive']+baseline['settings_cases'],'differential_inputs_sha256':fingerprint,'cross_build':'passed','elf_sha256':sha(elf),'reproducible_elf':reproducible,'sections':{n:{k:v for k,v in s.items() if k!='data'} for n,s in sec.items() if n in ['.init','.vector','.text','.data','.bss','.stack']},'baseline_decoder_vectors':52,'baseline_sdk_byte_identical':sdk['byte_identical'],'firmware_equivalent':False,'hardware_validated':False,'complete_application':False,'remaining_boundaries':['Two-key diagnostic command0xa0 meaning','Remaining menu/base scenes and reading/graph/extrema orchestration','CP936 conversion boundary and one Chinese-named resource lookup','Device-specific internal flash clock/CSR/program backend','ADC/debug/delay startup binding and on-device interrupt timing']}
    assert result['sections']['.vector']['address']==0x6438 and result['sections']['.text']['address']==0x6600
    resource_tests=re.search(r'(\d+) resource/filesystem tests passed',build)
    if resource_tests:result['host_tests_passed']+=int(resource_tests[1])
    result['compiled_upstream_library_counterparts']=len(libraries)
    result['remaining_incomplete_nontrap_bodies']=len(addresses-full-sdk_addresses-libraries-traps)
    result['native_source_translation_definition']='Native totals exclude kind=upstream_library; imported counterparts are counted separately.'
    phase_files=['analysis/jpeg_differential.json','analysis/filesystem_differential.json','analysis/library_helper_differential.json','analysis/numeric_differential.json','analysis/bitmap_differential.json']
    phase_entries=set();phase_cases=0;phase_current=True
    native_digest=sha(ROOT/'reconstructed/build/librecovered.so')
    for path in phase_files:
        if not (ROOT/path).exists():continue
        record=read(path)
        if record.get('native_library_sha256')!=native_digest:
            phase_current=False;continue
        if 'jpeg_' in path:
            phase_entries.update(int(a,16) for a in record['functions']);phase_cases+=len(record['cases'])
        elif 'filesystem_' in path:
            phase_entries.update(int(a,16) for a in record['entries']);phase_cases+=len(record['cases'])
        elif 'library_helper_' in path:
            phase_entries.update(int(a,16) for a in record['cases']);phase_cases+=record['total']
        elif 'bitmap_' in path:
            phase_entries.update(int(a,16) for a in record['entries']);phase_cases+=record['cases']
        elif 'numeric_' in path:
            phase_entries.add(int(record['entry'],16));phase_cases+=record['cases']
    result['phase3_differential_status']='passed' if phase_current else 'not_current; run tools/validate_phase3.py'
    if not phase_current:phase_entries=set();phase_cases=0
    entry_checked|=phase_entries
    result['machine_code_checked_entry_functions']=len(entry_checked)
    result['machine_code_checked_entries']=[f'0x{x:08x}' for x in sorted(entry_checked)]
    result['phase3_differential_cases']=phase_cases
    result['preserved_regression_differential_cases']=result['total_differential_cases']
    result['total_differential_cases']+=phase_cases
    # Incremental Phase4 evidence, with the same current-binary requirement as
    # Phase3. New simulator/custom code earns no original-function comparisons.
    phase4_paths=['graph','reading','ui','render','scenes','history_strip','codepage','library_extension']
    phase4_entries=set();phase4_cases=0;phase4_current=True;phase4_evidence={}
    for name in phase4_paths:
        path='analysis/'+name+'_differential.json'
        if not (ROOT/path).exists():phase4_current=False;continue
        record=read(path)
        if record.get('native_library_sha256')!=native_digest:phase4_current=False;continue
        n=record.get('total',record.get('cases'))
        assert isinstance(n,int) and n>=0,path
        phase4_evidence[path]=n;phase4_cases+=n
        phase4_entries.update(int(a,16) for a in record['entries'])
    result['phase4_differential_status']='passed' if phase4_current else 'not_current; run tools/validate_phase4.py'
    if not phase4_current:phase4_entries=set();phase4_cases=0
    result['phase4_differential_cases']=phase4_cases
    result['phase4_differential_evidence']=phase4_evidence if phase4_current else {}
    result['total_differential_cases']+=phase4_cases
    entry_checked|=phase4_entries
    result['machine_code_checked_entry_functions']=len(entry_checked)
    result['machine_code_checked_entries']=[f'0x{x:08x}' for x in sorted(entry_checked)]
    result['host_simulator_c_units']=sum('simulator' in p.parts for p in module_paths)
    result['firmware_native_c_units']=len(module_paths)-result['host_simulator_c_units']
    result['remaining_boundaries']=['Full169d4 transition repaint/cache ordering and remaining legacy scene glue','CP936 first unmapped ROM read80023fe4 and physical alias behavior','Two-key diagnostic command0xa0 meaning','Device-specific internal flash clock/CSR/program binding; layout unconfirmed','Target startup/stack/interrupt timing and on-device validation','Bootloader/USB/update recovery deferred pending supplied MCU evidence']
    _,retained_sections=sections(retained)
    result['retained_modules_elf_sha256']=sha(retained)
    result['retained_modules_reproducible']=retained_reproducible
    result['retained_modules_size']={
        'text_and_rodata':sum(retained_sections[n]['size'] for n in ['.init','.vector','.text']),
        'initialized_data':retained_sections['.data']['size'],
        'bss':retained_sections['.bss']['size'],
        'reserved_stack':retained_sections['.stack']['size']}
    sanitizer=read('analysis/sanitizer_validation.json') if (ROOT/'analysis/sanitizer_validation.json').exists() else None
    result['host_sanitizers']=sanitizer if sanitizer and sanitizer.get('differential_inputs_sha256')==fingerprint else {'status':'not_current'}
    write('analysis/continuation_validation.json',result)
    print(json.dumps({k:v for k,v in result.items() if k not in ['sections','machine_code_checked_entries','baseline_differential']},indent=2))
if __name__=='__main__':main()

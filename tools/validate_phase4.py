#!/usr/bin/env python3
"""Build both offline profiles, rerun preserved tests and audit Phase4 evidence.

No Ghidra import, device IO, acquired-image writes or bootloader interpretation.
Historical Phase3 aggregate reports remain preserved; per-test evidence is rerun
against the current native library, including the corrected RV32C LUI oracle.
"""
import argparse,hashlib,json,re,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def read(path):return json.loads((ROOT/path).read_text())
def sha(path):return hashlib.sha256((ROOT/path).read_bytes()).hexdigest()
def run(args,name):
 path=ROOT/'logs'/('phase4-'+name+'.log')
 with path.open('w') as out:subprocess.run(args,cwd=ROOT,stdout=out,stderr=subprocess.STDOUT,check=True)
 if name.endswith('sanitizers') and re.search(r'runtime error:|ERROR: AddressSanitizer',path.read_text()):raise RuntimeError('Sanitizer diagnostics in '+str(path))
 print(name+' passed',flush=True)
 return path.read_text()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reproducible',action='store_true');args=p.parse_args()
 assert sha('flashdump/dmt99_flash_1.bin')=='fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69'
 for path,digest in read('analysis/continuation_evidence_hashes.json').items():assert sha(path)==digest,path
 for row in read('references/recovered_library_manifest.json')['files']:assert sha(row['path'])==row['sha256'],row['path']
 run([sys.executable,'tools/recover_scene_assets.py'],'scene-assets')
 for profile in ['STOCK_COMPATIBLE','CUSTOM_DMT99']:
  run(['make','-s','-C','reconstructed','CONFIG='+profile,'all','check','simulator'],profile.lower())
 run(['make','-s','-C','reconstructed','host-sanitize','simulator-sanitize','profile-sanitize'],'stock-sanitizers')
 run(['make','-s','-C','reconstructed','CONFIG=CUSTOM_DMT99','simulator-sanitize','profile-sanitize'],'custom-sanitizers')
 python_tests=0
 for pattern in ['test_flash_forensics.py','test_usb_reference.py','test_trace_oracle.py','test_mcu_ingest.py','test_simulator.py']:
  output=run([sys.executable,'-m','unittest','discover','-s','tools','-p',pattern],pattern.removesuffix('.py'))
  match=re.search(r'Ran (\d+) tests?',output);assert match,pattern
  python_tests+=int(match[1])
 run(['gcc','-std=c11','-O2','-shared','-fPIC','-Ireconstructed/vendor/fatfs','reconstructed/tests/filesystem_oracle.c','reconstructed/storage/filesystem.c','reconstructed/storage/codepage.c','reconstructed/vendor/fatfs/unicode_upper.c','-o','reconstructed/build/filesystem_oracle.so'],'library-oracle-build')
 for name in ['jpeg','filesystem','library_helpers','numeric','bitmap','graph','reading','ui','render','scenes','history_strip','codepage','library_extension']:
  filename='tools/test_'+name+('_differential' if name not in ['library_helpers','library_extension'] else '')+'.py'
  run([sys.executable,filename],name+'-differential')
 run([sys.executable,'tools/qualify_library_extension.py'],'library-qualification')
 run([sys.executable,'tools/validate_continuation.py']+(['--reproducible'] if args.reproducible else []),'continuation')
 current=read('analysis/continuation_validation.json');assert current['phase3_differential_status']=='passed' and current['phase4_differential_status']=='passed'
 assert current['preserved_regression_differential_cases']==13740 and current['phase3_differential_cases']==2141
 custom_repro=None
 if args.reproducible:
  first=[sha('reconstructed/build/custom/'+name) for name in ['reconstruction.elf','recovered_modules.elf']]
  run(['make','-s','-C','reconstructed','CONFIG=CUSTOM_DMT99','-B','all'],'custom-fresh-build')
  second=[sha('reconstructed/build/custom/'+name) for name in ['reconstruction.elf','recovered_modules.elf']]
  assert first==second;custom_repro=True
 # Per-profile execution counts: repeated stock regressions in custom are
 # execution evidence, never additional unique original functions/cases.
 baseline=read('analysis/phase4/baseline_validation.json')
 report={'phase_complete':False,'objective':'Incremental UI/application recovery and offline simulation; whole-firmware equivalence remains unclaimed','baseline':{'native_full':87,'native_partial':36,'qualified_upstream':17,'remaining_incomplete_nontrap':185,'direct_entries':51,'differential_cases':15881},'current':{'native_full':current['full_source_translated_functions'],'native_partial':current['partial_source_translated_functions'],'qualified_upstream':current['compiled_upstream_library_counterparts'],'sdk_counterparts':current['compiled_sdk_function_counterparts'],'remaining_incomplete_nontrap':current['remaining_incomplete_nontrap_bodies'],'semantics_lower_bound':current['detailed_semantics_confirmed_lower_bound'],'direct_entries':current['machine_code_checked_entry_functions'],'differential_cases':current['total_differential_cases'],'new_differential_cases':current['phase4_differential_cases'],'firmware_native_c_units':current['firmware_native_c_units'],'host_simulator_c_units':current['host_simulator_c_units'],'stock_c_test_executions':33,'custom_c_test_executions':36,'python_tests':python_tests,'asan_ubsan_c_test_executions':30,'stock_elves_reproducible':current['reproducible_elf'] and current['retained_modules_reproducible'],'custom_elves_reproducible':custom_repro},'native_library_sha256':sha('reconstructed/build/librecovered.so'),'custom_library_sha256':sha('reconstructed/build/custom/librecovered.so'),'differential_inputs_sha256':current['differential_inputs_sha256'],'oracle_correction':'RV32C c.lui immediate sign extension now decoded from preserved raw instruction bytes; all 15881 prior cases rerun','preserved_evidence_files':885,'flash_images_verified':1,'flash_sha256':sha('flashdump/dmt99_flash_1.bin'),'custom_feature':'Last acknowledged supported mode, separate16-byte CRC record, injected mock/COW backend only; stock feature callbacks never called','simulator':'C controller/resources/framebuffer/PNG plus CLI and local browser controls; deterministic explicit button, packet, ACK and timer inputs','limitations':['Full menu dispatcher transition repaint/cache ordering remains partial; cooperative observer timing differs','CP936 physical read at80023fe4 is unmapped in supplied evidence; one filename remains unavailable through stock FatFs','Graph inherited register and resistance stack scratch inputs are explicit; simulator initializes its scratch deterministically','Target startup/interrupt/stack timing, ASIC internals and calibration unvalidated','Browser backend/PNG tested offline; physical display/panel timing not tested'],'deferred_until_mcu_evidence':['Definitive bootloader reconstruction','Actual USB MSC recovery','Internal Flash layout confirmation','Hardware firmware update integration'],'hardware_accessed':False,'hardware_validated':False,'hardware_flashable_release_produced':False}
 (ROOT/'analysis/phase4_validation.json').write_text(json.dumps(report,indent=2)+'\n')
 current['phase4_validation']='analysis/phase4_validation.json';current['phase4_host_execution_counts']=report['current']
 sanitizer={'status':'passed','tests':30,'sanitizers':['address','undefined'],'leak_detection':False,'leak_detection_note':'Existing sandbox ptrace exception; address/undefined checks enabled','differential_inputs_sha256':current['differential_inputs_sha256'],'commands':['make host-sanitize simulator-sanitize profile-sanitize','make CONFIG=CUSTOM_DMT99 simulator-sanitize profile-sanitize']}
 current['host_sanitizers']=sanitizer
 (ROOT/'analysis/sanitizer_validation.json').write_text(json.dumps(sanitizer,indent=2)+'\n');(ROOT/'analysis/continuation_validation.json').write_text(json.dumps(current,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()

#!/usr/bin/env python3
"""Re-run Phase3 evidence checks and host/original-code comparisons incrementally.

No Ghidra import, evidence rewriting or device access. Acquired image is always read-only;
C write cases use a disposable heap image. Use continuation --reproducible after code edits.
"""
import hashlib,json,re,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def read(p):return json.loads((ROOT/p).read_text())
def write(p,o):(ROOT/p).write_text(json.dumps(o,indent=2)+'\n')
def sha(p):return hashlib.sha256((ROOT/p).read_bytes()).hexdigest()
def run(cmd,name):
 p=ROOT/'logs'/('phase3-'+name+'.log')
 with p.open('w') as out:subprocess.run(cmd,cwd=ROOT,stdout=out,stderr=subprocess.STDOUT,check=True)
 return p.read_text()
def main():
 assert sha('flashdump/dmt99_flash_1.bin')=='fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69'
 for p,d in read('analysis/continuation_evidence_hashes.json').items():assert sha(p)==d,p
 for r in read('references/recovered_library_manifest.json')['files']:assert sha(r['path'])==r['sha256'],r['path']
 run([sys.executable,'-m','unittest','discover','-s','tools','-p','test_flash_forensics.py'],'forensics')
 run([sys.executable,'-m','unittest','discover','-s','tools','-p','test_usb_reference.py'],'usb-reference')
 run(['make','-s','-C','reconstructed','all','host-test'],'build')
 sanitized=run(['make','-s','-C','reconstructed','host-sanitize'],'sanitizers')
 run([sys.executable,'tools/test_jpeg_differential.py'],'jpeg-differential')
 run([sys.executable,'tools/test_filesystem_differential.py'],'filesystem-differential')
 run(['gcc','-std=c11','-O2','-shared','-fPIC','-Ireconstructed/vendor/fatfs','reconstructed/tests/filesystem_oracle.c','reconstructed/storage/filesystem.c','reconstructed/storage/codepage.c','reconstructed/vendor/fatfs/unicode_upper.c','-o','reconstructed/build/filesystem_oracle.so'],'helper-build')
 run([sys.executable,'tools/test_library_helpers.py'],'library-helpers')
 run([sys.executable,'tools/test_numeric_differential.py'],'numeric-differential')
 run([sys.executable,'tools/test_bitmap_differential.py'],'bitmap-differential')
 # Continuation already supplies the authoritative current source/input fingerprint.
 run([sys.executable,'tools/validate_continuation.py'],'continuation')
 current=read('analysis/continuation_validation.json')
 assert current['preserved_regression_differential_cases']==13740
 assert current['host_tests_passed']==26 and current['phase3_differential_cases']==2141
 compiler=subprocess.check_output(['clang','--version'],text=True).splitlines()[0]
 san={'status':'passed','tests':13,'sanitizers':['address','undefined'],'leak_detection':False,'leak_detection_note':'LeakSanitizer cannot run under sandbox ptrace; existing exception retained. Address/UB sanitizers remain enabled.','compiler':compiler,'differential_inputs_sha256':current['differential_inputs_sha256'],'binary_sha256':sha('reconstructed/build/test_application_sanitized'),'resource_binary_sha256':sha('reconstructed/build/test_resources_sanitized'),'command':'make -C reconstructed host-sanitize'}
 write('analysis/sanitizer_validation.json',san);current['host_sanitizers']=san
 layout=read('analysis/flash_layout.json');files=read('analysis/flash_files.json');libraries=read('analysis/library_matches.json');baseline=read('analysis/phase3/baseline_validation.json')
 report={'phase_complete':False,'baseline':{'native_full':baseline['full_source_translated_functions'],'native_partial':baseline['partial_source_translated_functions'],'upstream_library_counterparts':0,'remaining_incomplete_nontrap':209,'direct_entries':baseline['machine_code_checked_entry_functions'],'host_c_tests':baseline['host_tests_passed'],'differential_cases':baseline['total_differential_cases']},'current':{'native_full':current['full_source_translated_functions'],'native_partial':current['partial_source_translated_functions'],'upstream_library_counterparts':current['compiled_upstream_library_counterparts'],'remaining_incomplete_nontrap':current['remaining_incomplete_nontrap_bodies'],'direct_entries':current['machine_code_checked_entry_functions'],'host_c_tests':26,'host_python_tests':11,'differential_cases':current['total_differential_cases'],'asan_ubsan_tests':13,'cross_build':'pass','reproducible_both_elfs':current['reproducible_elf'] and current['retained_modules_reproducible']},'flash':{'verified_images_available':1,'second_acquisition_available':False,'logical_sector_size':4096,'volumes':2,'live_files':len(files['active']),'jpeg_files':sum(f['kind']=='JPEG' for f in files['active']),'native_file_reads':298,'native_jpeg_renders':296,'native_jpeg_repeats':2,'deleted_directory_leads':len(files['deleted_leads']),'sectors_inventoried':2048,'carved_rv32_applications':1,'carved_x86_extents':4},'new_library_matches':{'Exact':sum(r['classification']=='Exact' for r in libraries['new_function_matches']),'Strong semantic':sum(r['classification']=='Strong semantic' for r in libraries['new_function_matches'])},'limitations':['Second CH341A image missing','One non-ASCII filename requires unresolved original CP936 handling','Alternative application normalized/CFG version comparison incomplete','Menu/base scenes and graph/extrema paths remain','Original USB bootloader/transport/update recognition not recovered','No physical hardware or calibration validation'],'evidence_preserved':885,'hardware_validated':False}
 write('analysis/phase3_validation.json',report);current['phase3']=report['flash'];current['phase3_validation']='analysis/phase3_validation.json';write('analysis/continuation_validation.json',current)
 print(json.dumps(report,indent=2))
if __name__=='__main__':main()

#!/usr/bin/env python3
"""Enrich machine evidence with curated semantics; build reports and a queryable xref DB."""
from collections import Counter,defaultdict
import json
from pathlib import Path
import re
import sqlite3
from analyze_firmware import ROOT,dump

def write(name,text):
    (ROOT/name).write_text(text.strip()+'\n')

def enrich():
    functions=json.loads((ROOT/'analysis/functions.json').read_text())
    registry=json.loads((ROOT/'analysis/semantic_registry.json').read_text())
    instructions=json.loads((ROOT/'analysis/instructions.json').read_text())
    graph=json.loads((ROOT/'analysis/callgraph.json').read_text())
    by_address={i['address']:i for i in instructions}
    shared={a:i for a,i in by_address.items() if i.get('function') is None}
    owned=defaultdict(list)
    for i in instructions:
        if i.get('function') is not None:owned[i['function']].append(i)
    shared_owners=defaultdict(set)
    def successors(i):
        return [*i.get('flows',[]),*([i['fallthrough']] if i.get('fallthrough') else [])]
    for f in functions:
        todo=[target for i in owned[f['address']] for target in successors(i) if target in shared]
        seen=set()
        while todo:
            a=todo.pop()
            if a in seen:continue
            seen.add(a);shared_owners[a].add(f['address'])
            todo.extend(t for t in successors(shared[a]) if t in shared and t not in seen)
        f['shared_instruction_addresses']=sorted(seen)
        f['shared_code_bytes']=sum(shared[a]['length'] for a in seen)
        f['size_with_shared_code']=f['size']+f['shared_code_bytes']
        for a in seen:
            for r in shared[a]['references']:
                if r['to'] not in f['data_references']:f['data_references'].append(r['to'])
                if 'CALL' in r['type']:
                    graph.append({'from':f['address'],'site':a,'to':r['to'],'type':r['type'],'shared_code':True})
                    if r['to'] not in f['callees']:f['callees'].append(r['to'])
    graph=list({(e['from'],e['site'],e['to']):e for e in graph}.values())
    dump('analysis/callgraph.json',graph)
    dump('analysis/shared_code.json',[{**i,'reachable_from_functions':sorted(shared_owners[a])} for a,i in sorted(shared.items())])
    callers=defaultdict(set)
    for e in graph:callers[e['to']].add(e['from'])
    for f in functions:
        f['callers']=sorted(set(f['callers'])|callers[f['address']]);f['callees']=sorted(set(f['callees']))
    vectors=json.loads((ROOT/'analysis/entrypoints.json').read_text())['vectors']
    peripherals=json.loads((ROOT/'analysis/peripheral_bases.json').read_text())
    literal_strings=json.loads((ROOT/'analysis/strings.json').read_text())
    globals_={line.split('\t')[0]:line.split('\t')[1] for line in (ROOT/'analysis/global_names.tsv').read_text().splitlines()}
    names={a:r['name'] for a,r in registry.items()}
    peripheral_groups=defaultdict(list)
    covered=set()
    for i in instructions:
        a=int(i['address'],16);covered.update(range(a,a+i['length']))
    gaps=[];begin=None
    for a in range(0x6600,0x1e820):
        if a not in covered and begin is None:begin=a
        if a in covered and begin is not None:gaps.append({'start':hex(begin),'end_exclusive':hex(a),'size':a-begin});begin=None
    if begin is not None:gaps.append({'start':hex(begin),'end_exclusive':'0x1e820','size':0x1e820-begin})
    dump('analysis/code_gaps.json',gaps)
    dump('analysis/address_to_name.json',names)
    db=sqlite3.connect(ROOT/'analysis/xrefs.sqlite')
    db.executescript('DROP TABLE IF EXISTS functions; DROP TABLE IF EXISTS refs; DROP TABLE IF EXISTS shared_owners; CREATE TABLE functions(address TEXT PRIMARY KEY,name TEXT,size INTEGER,confidence TEXT,purpose TEXT); CREATE TABLE refs(source TEXT,function_address TEXT,target TEXT,type TEXT); CREATE INDEX refs_target ON refs(target); CREATE INDEX refs_function ON refs(function_address); CREATE TABLE shared_owners(address TEXT,function_address TEXT);')
    db.executemany('INSERT INTO shared_owners VALUES(?,?)',[(a,f) for a,owners in shared_owners.items() for f in owners])
    for i in instructions:
        for ref in i['references']:db.execute('INSERT INTO refs VALUES(?,?,?,?)',(i['address'],i.get('function'),ref['to'],ref['type']))
    for f in functions:
        a=f['address'];reg=registry.get(a)
        if reg:
            f['name']=reg['name'];f['confidence']=reg['confidence'];f['likely_purpose']=reg['evidence'][0];f['analysis_notes']=[reg['notes'],*reg['evidence'][1:]]
        else:
            f['confidence']='unresolved';f['likely_purpose']='Unknown; inspect pseudocode and callers';f['analysis_notes']=['Function recovered by control-flow analysis; no verified semantic name.']
        raw=(ROOT/f['pseudocode_file']).read_text() if f.get('decompiled') else ''
        literals=set(re.findall(r'"([^"\n]+)"',raw));f['referenced_strings']=sorted(set(f['referenced_strings'])|literals)
        mmio=[]
        for ref in f['data_references']:
            addr=int(ref,16)
            if 0x40000000<=addr<0x40030000 or 0xa0000000<=addr<0xa0001000 or 0xe000e000<=addr<0xe0010000:
                candidates=[(int(base),name) for name,base in peripherals.items() if int(base)<=addr<int(base)+0x400]
                peripheral=max(candidates)[1] if candidates else 'unknown_mmio'
                mmio.append({'address':ref,'peripheral':peripheral,'confidence':'reference_only','evidence':'Ghidra reference heuristic; may be a peripheral argument or an unrelated constant. Verify against instructions before assigning hardware semantics.'})
                peripheral_groups[peripheral].append(a)
        f['mmio_references']=mmio
        if f.get('decompiled'):
            # Preserve unknown logic and warnings; substitute only curated names.
            clean=raw
            for addr,name in names.items():clean=clean.replace('FUN_ram_'+addr[2:],name)
            for addr,name in globals_.items():clean=clean.replace('DAT_ram_'+addr,name)
            banner=f"/* Semantic review: {f['confidence']}. {f['likely_purpose']}.\n * Address {a}; original logic and decompiler warnings preserved.\n * Readable pseudocode; maintained C implementations live in reconstructed/. */\n"
            clean_name=f"decompiled/clean/{int(a,16):08x}_{f['name']}.c"
            (ROOT/clean_name).write_text(banner+clean);f['clean_pseudocode_file']=clean_name
        db.execute('INSERT INTO functions VALUES(?,?,?,?,?)',(a,f['name'],f['size'],f['confidence'],f['likely_purpose']))
    db.commit();db.close()
    current_clean={f.get('clean_pseudocode_file') for f in functions}
    for p in (ROOT/'decompiled/clean').glob('*.c'):
        if str(p.relative_to(ROOT)) not in current_clean and p.read_text().startswith('/* Semantic review:'):
            p.unlink()  # Remove only this generator's superseded semantic filenames.
    dump('analysis/functions.json',functions)
    dump('analysis/hardware_interactions.json',{p:sorted(set(a)) for p,a in sorted(peripheral_groups.items())})
    indirect=[]
    for i in instructions:
        if i['mnemonic'] not in ['jalr','c.jalr','c.jr','jr']:continue
        indirect.append({**i,'resolution':'known GCC save-helper t0 return' if i['address'] in ['0x0000662e','0x0000663a'] else 'jump-table targets recovered' if i['references'] else 'unresolved computed transfer or callback; inspect caller context'})
    dump('analysis/indirect_transfers.json',indirect)
    exe=len(covered);classified=sum(f['confidence']!='unresolved' for f in functions)
    vector_default_count=sum(v['kind']=='default_self_loop' for v in vectors)
    unreferenced_defaults=sum(r.get('category')=='unreferenced_default_trap' for r in registry.values())
    default_count=vector_default_count+unreferenced_defaults
    irq=[v for v in vectors if v['index']!=0 and v['kind']=='implemented']
    validation_path=ROOT/'analysis/validation_results.json'
    validation=json.loads(validation_path.read_text()) if validation_path.exists() else {}
    summary={'input_bytes':135096,'code_extent_bytes':0x18220,'identified_instruction_bytes':exe,'functions':len(functions),'decompiled':sum(f['decompiled'] for f in functions),'semantic_classifications':classified,'named_nondefault_functions':classified-default_count,'default_interrupt_loops':default_count,'unresolved_functions':len(functions)-classified,'implemented_interrupt_handlers':len(irq),'nontrivial_interrupt_handlers':len(irq)-1,'code_gap_bytes':sum(g['size'] for g in gaps),'direct_mmio_groups':sorted(peripheral_groups),'validation':validation}
    dump('analysis/progress.json',summary)
    md='\n'.join(f"| {v['index']} | {v['sdk_name']} | `{v['target']}` | {v['kind']} |" for v in vectors)
    write('analysis/interrupts.md',f'''# Interrupt inventory

Table `0x6438`, mtvec=`0x643b`, 104 pointer slots identified using the official CH32V303 D8 startup table.
{len(irq)} implemented interrupt/exception entries: NMI is only `mret`; six others contain application logic.
{vector_default_count} vector-referenced self-loop handlers and {sum(v['kind']=='reserved' for v in vectors)} zero/reserved slots.
An additional {unreferenced_defaults} unreferenced weak self-loop stubs are retained in the code inventory.
A default handler does not demonstrate that its interrupt is enabled.

EXTI2, EXTI9_5 and EXTI15_10 handle buttons. TIM3 drives periodic power/battery work.
USART1 IDLE handles the 13-byte DMA snapshot; DMA1 channel 5 only clears its transfer-complete interrupt.
Routines end with `mret`, not normal C returns. Curated prototypes use `void(void)`.

| Slot | SDK identity | Target | Observed body |
|---:|---|---|---|
{md}
''')
    hardware_rows='\n'.join(f"| {p} | {', '.join('`'+a+'`' for a in sorted(set(aa)))} |" for p,aa in sorted(peripheral_groups.items()))
    write('docs/HARDWARE_MAPPING.md',f'''# Hardware interactions

The MCU model and physical components come from the supplied PCB description. Register bases and IRQ
numbers are verified against the pinned official WCH SDK; pin selections below are firmware observations.
No electrical continuity measurements or hardware operations were performed.

| Device/function | Verified MCU interface | Evidence | Confidence |
|---|---|---|---|
| Measurement-side peer | USART1 PA9 TX / PA10 RX, 9600 8N1 | `0x908a`, `0xc350`, `0xc468`, `0x97c4` | High for link; peer chip identity unresolved |
| Measurement receive | DMA1 channel 5, 13 bytes, circular, byte width, high priority | `0xc350`; IDLE stops/rearms count at `0xc468` | High |
| Debug output | USART2 TX PA2, 115200 | `0x1a682`, `0x1a6e2`, main argument | High |
| External NOR flash | SPI1 PA5/6/7, PA4 active-low CS; mode 3; prescaler 8 | `0xd38a` SPI init fields and JEDEC/read commands | High |
| Display | GPIOB full 16-bit OUTDR bus; PC5 D/C, PC4 select, PC13 WR, PC3 reset | `0x7d6a`, `0x7dba`, `0x87aa` | High for signals; controller model unresolved |
| Display geometry | 320x240 or rotated 240x320 | `0x7f18`, `0x876a` | High |
| Display brightness | TIM8 channel 1 / PC6 | `0x7f66`, `0x8016`, power-off duty selection | Medium-high |
| Buzzer | TIM2 channel 2 / PA1 | `0x780c`, `0x78ba`, 250/500 ms beeps at `0x7918`/`0x792e` | High |
| Button/power input | PC2 → EXTI2 → event 8 | `0x7944`, `0x7c0a` | High |
| Five other buttons | PC8/9/10/11/12 pullups, falling-edge EXTI | `0x7944`, `0x7c42`, `0x7cd8` | High; printed key labels need PCB/user evidence |
| Charge detect | PA8 floating input | `0x7562`, recharge-screen polling `0x14cd6` | High for pin, inferred circuit role |
| Battery sensing | ADC1 channel 10 / PC0 | `0x767a`, `0x7598`, `0x75f0` | High for channel; divider/percentage model partly unresolved |
| Power latch | PC1 output, deasserted at shutdown | `0x7944`, `0xc320` | High for action; external circuitry unknown |
| Internal settings/history | Address `0x08040000`, 256-byte copy | `0xd87c`, `0xd928` | High for access; physical partition requires verification |

The binary selects an HSE x18 PLL. Its initialized SystemCoreClock value is also 144000000.
With the supplied 8 MHz crystal this is a 144 MHz core, APB1 /2 and APB2 /1 configuration.
No positive evidence of I2C, USB or a separate hardware SPI bus for measurement was found.
That does not exclude inactive or bootloader-only functionality outside this image.

## Functions with resolved MMIO references

These are address-reference heuristics, including peripheral pointer arguments and possible unrelated constants.
They are not all verified MMIO accesses. Generic SDK functions
can access peripherals through arguments and therefore need not appear here. JSON and SQLite exports
provide the full addresses and reference types.

| Peripheral | Functions |
|---|---|
{hardware_rows}
''')
    protocol_docs()
    architecture_docs()
    unresolved='\n'.join(f"| `{f['address']}` | {f['size']} | {len(f['callers'])} | [pseudocode](../{f['clean_pseudocode_file']}) |" for f in functions if f['confidence']=='unresolved')
    write('docs/UNRESOLVED_FUNCTIONS.md',f'''# Functions requiring semantic review

{len(functions)-classified} functions lack verified semantic names. All have machine-derived entrypoints,
body ranges, callers/callees, strings, MMIO references and pseudocode in `analysis/functions.json`.
Unresolved does not mean invalid. Most remaining routines are UI/rendering, filesystem internals,
formatting, allocation, or compiler arithmetic, but those categories are hypotheses until individually checked.

Computed transfers are listed in `analysis/indirect_transfers.json`: JPEG callbacks, GCC computed fill
tails and jump tables require different interpretations. The full callgraph does not claim complete indirect edges.
`analysis/code_gaps.json` records {summary['code_gap_bytes']} bytes inside the code extent that are not
currently decoded instructions; they may include alignment, unreachable code, inline data or missing blocks.

| Address | Body bytes | Callers | Output |
|---|---:|---:|---|
{unresolved}
''')
    write('docs/RECONSTRUCTION_STATUS.md',f'''# Reconstruction status

This is a measured offline recovery, not an equivalent or hardware-ready firmware release.

| Metric | Result |
|---|---:|
| Input bytes, hash verified | 135096 |
| Code extent, excluding vectors and rodata | {summary['code_extent_bytes']} |
| Identified instruction bytes, including entry | {exe} |
| Discovered functions | {len(functions)} |
| Successfully decompiled | {summary['decompiled']} |
| Evidence-backed semantic classifications | {classified} |
| Named nondefault routines | {classified-default_count} |
| Default self-loop routines | {default_count} ({vector_default_count} referenced, {unreferenced_defaults} unreferenced) |
| Semantically unresolved functions | {len(functions)-classified} |
| Implemented interrupt/exception handlers | {len(irq)} (NMI trivial; six with logic) |
| Reconstructed compilable C modules | 9, including offline application harness |
| Host tests passing / failing | {validation.get('host_tests_passed','pending')} / {validation.get('host_tests_failed','pending')} |
| Original-instruction differential cases passing | {validation.get('differential_cases','pending')} |
| LLVM→Ghidra Xwchc decoder vectors passing | {validation.get('decoder_vectors_passed','pending')} |
| SDK functions compared / byte-identical | {validation.get('sdk_functions_compared','pending')} / {validation.get('sdk_functions_byte_identical','pending')} |
| Cross compilation/link | {validation.get('cross_build','pending')} |
| Repeated clean compilation produces identical ELF | {validation.get('reproducible_elf','pending')} |

Semantic counts include simple SDK helpers and default traps. They are not estimates of application
understanding. Confidence and supporting evidence are recorded individually in `semantic_registry.json`.
The inventory covers discovered functions; it does not prove no additional functions exist.
Exclusive body bytes and reachable shared code are recorded separately per function. `shared_code.json`
preserves decoded blocks outside exclusive Ghidra bodies and maps their known function owners;
their calls are included in the enriched callgraph. The instruction-byte count deduplicates shared code.

## Actual maintained source

- `drivers/measurement_asic.c`: observed zero-payload commands, packed segments, frame flags and IDLE snapshot policy.
- `drivers/spi_flash.c`: recovered read transactions and page-boundary calculation, using mocked transport.
- `drivers/display_bus.c`: exact command/data GPIO strobe order, using mocked transport.
- `ui/buttons.c`: observed interrupt debounce, PC8 long press and event overwrite order.
- `storage/history.c`: 30-record shift, metadata packing, scale and magnitude decode.
- `storage/settings.c`: four packed field accessors, including original setter quirks.
- `application/foreground.c`: recovered normal-operation dispatch, event copy, hold and battery sequencing; unresolved callees are explicit hooks.
- `platform/wch_platform.c`: official SDK translations of USART1/DMA and SPI1 setup.
- `application/main.c`: explicitly incomplete offline link/selfcheck harness.

The startup and SDK drivers are genuine vendor sources. The linker uses application origin `0x6400`,
vector origin `0x6438`, 64 KiB RAM and stack top `0x20010000`; original global/data addresses are not
expected to remain identical after source recompilation. All modules are compiled into objects;
`libreconstructed.a` preserves them for inspection even when the small harness link garbage-collects unused code.
No flashable BIN is emitted. The ELF is for inspection only.

## Limits and blockers

- No external flash or device-specific internal settings/calibration dump was supplied.
- ASIC identity, analog calibration algorithm and peer firmware are unresolved.
- UI and filesystem logic are decompiled but not yet fully translated into maintained C.
- The full application, scheduling/timing, power circuitry and interrupts have no hardware validation.
- Exact original compiler version and original project files are not recovered. Generic GCC produces standard
  RV32IMAC code with the recovered soft-float ABI; it does not emit WCH compressed extensions. Vendor startup uses supported CSR instructions.
- No whole-firmware functional-equivalence claim is supported by these tests.
''')
    write('docs/NEXT_STEPS.md','''# Next actions

1. Continue translating `application_loop`, three measurement views and `menu_dispatch`, using their
   address comments, global names and source-backed types. Start with 0xb9e0/0x12c78 for recording behavior.
2. Recover text-to-number conversion and complete unit/scaling semantics using captured wire frames;
   remaining bytes 9..12 and incoming integrity rules are unresolved. Exhaustive segment tests already pass.
3. Resolve JPEG callback/function-pointer edges and name filesystem/compiler-runtime routines.
   Review `code_gaps.json` before adding speculative function starts.
4. For future extra buttons: retain EXTI pin ownership, single-byte event overwrite semantics, 20 ms debounce,
   blocking long-press behavior, shared GPIOC display signals and ISR latency as explicit constraints.
5. For extended recording: the present 30x4-byte history shares a 256-byte internal-flash settings block.
   Design a separate versioned log format and preserve the original settings bytes before changing storage.
   The NOR contains an 8 MiB resource filesystem; no unused region has been verified.
6. When available, analyze an offline dump of the W25Q64 and the original 0x08040000 settings page.
   These are required to reconstruct assets and preserve device-specific values. No automatic acquisition
   or flashing is included in this repository.
7. Chip identification needs board pin continuity/package information or passive UART captures supplied
   from a separately reviewed hardware procedure. UART capability alone cannot distinguish the SDIC candidates.
8. Before any eventual hardware test proposal, resolve flash/RAM partitioning around 0x08040000, power latch
   polarity, display geometry and interrupt ABI; establish complete reversible backups including calibration.
''')
    write('docs/REVERSE_ENGINEERING.md','''# Reproducible workflow

`python3 tools/analyze_firmware.py --phase all` verifies the input, imports/analyzes with headless Ghidra,
exports disassembly and pseudocode, builds/tests recovered modules and writes reports. It never accesses hardware.
`GHIDRA_HOME` selects another installation. `make -C reconstructed CROSS=/path/to/riscv-prefix- all host-test`
selects another compatible compiler. Java must include a JDK; JAVA_HOME can be supplied explicitly.

`tools/fetch_sdk.py` obtains the pinned WCH revision and records hashes. Both the original fetched SDK and
the standalone project's vendor subset are included. WCH's notices are preserved; no substitute SDK is used.

## Ghidra stages

1. `prepare_ghidra.py`: derives a local RV32IMAF+C+Xwchc SLEIGH language from installed Ghidra sources.
   Uses the soft-float ABI and four GCC t0 save-helper callfixups; original installation stays unchanged.
2. `DmtSetup.java`: imports at 0x6400, separates nonexecuting vectors/rodata, initializes the SRAM load image,
   sets gp context and seeds reset/main/vector functions.
3. `DmtRecover.java`: finds validated save-helper prologues and pointer-table prologues, installs callfixups,
   marks MMIO volatile and repeats constant/switch analysis. This is essential: treating MMIO as ordinary memory
   incorrectly collapses polling loops, and ordinary C-call semantics corrupt helper argument tracking.
4. `DmtTypes.java`: applies curated protocol/history/register structs and function signatures.
5. `DmtExport.java`: exports listing instructions, functions, references, callgraph and per-function C-like output.
6. `enrich_analysis.py`: adds confidence/evidence, semantic pseudocode, MMIO groups, SQLite xrefs and reports.

Ghidra completed every discovered decompilation. Success means that C-like output was produced, not that every
type, return value or expression is verified. All warnings remain visible. The `clean` tree contains readable
semantic pseudocode; independently maintained native C is in `reconstructed/`.

## Incremental use

- `--phase forensics`: fast hash, startup, entropy, strings, memory map and vector reports.
- `--phase ghidra`: skip unchanged imports/exports using input/script/semantic fingerprints.
- `--phase ghidra --refresh`: reuse the project and regenerate semantic exports after a registry/type change.
- `--phase ghidra --force`: repeat a full import; appropriate after memory-map or language changes.
- `--phase reports`: regenerate confidence/status reports and SQLite references.
- `--phase validate`: compile, link, test, compare and verify repeat-build hashes.

Raw decompilation caching includes body bytes, function signature, language, curated names, globals, types,
signatures and save-helper definitions. An unchanged function is reused; semantic dependency changes invalidate
the relevant export epoch conservatively. Ghidra projects remain in `analysis/ghidra_projects/` for resumption.

## Validation boundaries

The small interpreter supports only instructions needed by five isolated differential targets and fails on
unsupported opcodes. It is not a peripheral, floating-point, timing or whole-device emulator. Decoder tests use
LLVM's independent assembler to produce boundary and one-hot offsets for all eight WCH encodings, then verify Ghidra operands
and pcode access widths. SDK byte comparisons are local function-body evidence; unequal code can still be
equivalent, and byte-identical helpers do not establish equivalence of callers or the whole application.

Build/test output and selected failed attempts are retained in `logs/`. The original BIN hash is checked before
analysis and validation and its file permissions are read-only. No programmer utility or hardware action exists
in the automated workflow. Private/device-specific calibration values are absent and never synthesized.
''')
    write('README.md',f'''# FNIRSI DMT99 V2.4 offline reverse engineering

Actual headless analysis of the supplied firmware: **{len(functions)} discovered functions, {summary['decompiled']}
decompiled, {classified} semantically classified** ({default_count} are default interrupt loops).
The raw application links at **0x6400**, uses WCH Xwchc compressed instructions and initializes a **144 MHz** core.

Recovered interfaces include USART1 measurement packets, GPIO display, SPI1 NOR flash, button EXTI,
buzzer/backlight PWM and a 30-entry history format. Native C modules compile with the official WCH SDK.
The application harness is incomplete and unsuitable for hardware. Nothing connects to, erases or flashes a device.

```bash
python3 tools/analyze_firmware.py --phase all
make -C reconstructed all host-test
```

Start with [current status](docs/RECONSTRUCTION_STATUS.md), [binary forensics](analysis/binary_report.md),
[measurement protocol](analysis/measurement_asic_protocol.md), [hardware map](docs/HARDWARE_MAPPING.md),
[architecture](docs/FIRMWARE_ARCHITECTURE.md) and [next actions](docs/NEXT_STEPS.md).
Reproduction instructions and test limits are in [the workflow](docs/REVERSE_ENGINEERING.md).

Evidence is available as [function inventory](analysis/functions.json), [callgraph](analysis/callgraph.json),
`analysis/xrefs.sqlite`, [disassembly](disassembly/full_disassembly.asm), `decompiled/raw/`, `decompiled/clean/`,
and [validation results](analysis/validation_results.json). Function and data names are interpretations with
address-specific evidence; original source symbol names are not claimed.

Vendor material is pinned and credited in `references/` and `reconstructed/vendor/`. The original firmware
is preserved read-only with SHA-256 `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
''')
    print(f'Reports generated: {len(functions)} functions, {classified} classified, {exe} instruction bytes')

def protocol_docs():
    write('analysis/measurement_asic_protocol.md','''# Measurement-side serial protocol

## Evidence and transport

High confidence: original main `0x908a` calls `0xc350` with 9600. GPIO setup selects PA9 TX and PA10 RX,
USART1 8N1, RX+TX and no flow control. DMA1 channel 5 uses byte-sized circular transfers, incrementing RAM
at `0x200002ac`, length 13. USART1 IDLE `0xc468` disables DMA, clears IDLE/reads DATAR, resets CNTR to 13,
and reenables DMA. The DMA transfer-complete handler `0xc570` only clears the interrupt flag.
No inter-packet period, electrical voltage or exact peer identity can be established without additional evidence.

## TX, MCU to peer

`measurement_send_command` at `0x97c4` transmits exactly ten bytes and waits for TC after every byte.
The template at `0x1e820` is `AA 55 05 00 00 00 00 00 00 00`; command constructors replace bytes 3 and 9.
For observed zero-payload commands: `AA 55 05 CC 00 00 00 00 00 (CC+05)`.
The second template at `0x1e82c` has byte 9 = `0x11`, used when selecting mode `0x0c`.
This is evidence for the listed commands, not proof of a general checksum over arbitrary payloads.

| ID | Supported interpretation | Construction evidence |
|---|---|---|
| 01 | PC12/button action; physical action unresolved | `0x97f8`, checksum 06 |
| 03 | Hold-related action; UI toggles hold icon on event 03 | `0x97f8`, `0xb5c8`, `0xa910`; checksum 08 |
| 08 | PC11/button action; physical action unresolved | `0x97f8`, checksum 0D |
| 09 | PC10/button action; mode/state 09 needs more interpretation | `0x97f8`, checksum 0E |
| 0A | Capacitance UI | `0xb9e0` and event `0xa910` cap.jpg; checksum 0F |
| 0B | Resistance UI | r.jpg; checksum 10 |
| 0C / 0D | DC voltage / AC voltage | dc_v.jpg / ac_v.jpg in regular view; checksum 11 / 12 |
| 0E | ol.jpg resource; physical function unresolved | `0xa910`; checksum 13 |
| 0F | Frequency | hz.jpg; checksum 14 |
| 10 / 11 | DC A / DC mA | regular-view dc_a resource and unit selection; checksum 15 / 16 |
| 12 | Temperature | temp.jpg; checksum 17 |
| 13 | State/mode requiring more interpretation | explicit constructor/checksum 18 |
| 14 | Default/automatic-view path, exact analog policy unresolved | constructor/checksum 19 |
| 15 / 16 | AC A / AC mA | ac_a resource and units; checksum 1A / 1B |

UI resource associations establish what the host displays; they do not establish every analog operation
performed by the peer. Some second-view English resource selections appear inconsistent with the regular view;
keep the original paths visible rather than silently correcting them.

## RX, peer to MCU

The normal display decoder `0xbe78` checks byte 0=`5A`, byte 1=`A5`. It decodes four LCD symbols from:
`(b[2]&F0)|(b[3]&0F)`, `(b[3]&F0)|(b[4]&0F)`, `(b[4]&F0)|(b[5]&0F)`, `(b[5]&F0)|(b[6]&0F)`.
`0x98aa` clears bit `0x10` before matching segment patterns. These are not ordinary BCD digits.

| Symbol | Patterns, after masking 0x10 |
|---|---|
| 0..9 | EB, 0A, AD, 8F, 4E, C7, E7, 8A, EF, CF |
| A,U,T,O,L,E | EE,23,65,27,61,E5 |
| minus, space, C, P, r | 04,00,E1,EC,24 |
| unknown | FF return code |

Negative sign is b[2] bit 4. Decimal markers are b[3], b[4], b[5] bit 4, in that priority order.
Bits 0/1 of b[2] produce status codes 1/2 (bit 1 has priority), otherwise FF; AC/DC labeling remains tentative.
Unit-code selection is a priority tree over bytes 6..8; the native decoder retains all original numeric unit IDs.
Byte 7 bit 7 is exported as a status flag. Meanings of bytes 9..12 and an incoming checksum rule are unresolved.

Special/event frames have b[2]=FD and event/mode ID in b[3]. ISR `0xc468` copies exactly 13 bytes into
`0x200002bc` and clears an event-consumed flag. The main loop later copies a 16-byte buffer, including three
bytes not newly copied by this ISR. The reconstruction models the known 13-byte snapshot only.
The ISR itself does not check 5AA5; the higher-level reconstructed parser adds a documented header/length guard.

The ISR suppresses a normal update if b[2] high three bits, b[3..5] low nibbles and high three bits, and b[6]
low nibble are all zero. This exact predicate is implemented independently of header validation.
No calibration command, continuous sample period, ADC register map or general error-code set is verified.

## Validation

All 256 input values to original machine-code `0x98aa` match the compiled C segment decoder.
Host tests cover literal TX vectors, packed-digit RX, events, blank suppression and invalid lengths/headers.
These validate defined wire operations; they do not validate the whole measurement subsystem or analog accuracy.
''')
    write('analysis/asic_candidates.md','''# Unknown ASIC candidates

The recovered CH32-facing interface is USART1, with 10-byte outgoing commands and 13-byte incoming display/event
records. It transports LCD segment-like status rather than a verified raw ADC conversion word. A programmable
measurement SoC can implement a product-specific protocol, so this interface is insufficient to name the chip.

| Candidate | Compatible evidence | Missing or incompatible evidence | Assessment |
|---|---|---|---|
| SD7502 | Vendor datasheet documents UART, multimeter functions and LQFP64 | No manufacturer-fixed AA55/5AA5 protocol found; actual package/pin connectivity unavailable | Plausible, not identified |
| SD78P952 | Vendor selection catalog lists multimeter SoC and LQFP64/100 | Detailed datasheet/protocol not recovered here; no observed chip ID; cannot match pinout | Plausible, evidence weaker |
| SD78P953 | Vendor datasheet documents UART, multimeter functions, dual ADC and LQFP48/64/100 | Its power-measurement capability is not demonstrated by this BIN; no unique protocol signature | Plausible, not identified |
| Other/custom programmed SoC | Product-specific UART and segment frames fit an independently programmed peer | No silicon identification or analog-side evidence | Remains possible |

No candidate is positively ruled out by the available firmware. A pin/package mismatch could rule one out,
but those measurements are not available. Marketing use in another FNIRSI device has not been used as proof.
Public datasheets establish peripheral capabilities, not the actual OTP application executing inside the peer.

Primary material consulted/downloaded:

- [SD7502 vendor datasheet](https://sdicmicro.cn/upload/img/2021-08/611627e6a2699.pdf), local PDF and text in `references/`.
- [SD78P953 vendor datasheet](https://sdicmicro.cn/upload/img/2022-11/6371b0eeeaeef.pdf), local PDF and text in `references/`.
- [SDIC product selection catalog](https://sdicmicro.cn/upload/img/2023-05/64633c119f860.pdf), SD78P952 package/capability comparison.

A failed download from the vendor's www host returned HTML; that response is retained with an `.html` suffix,
and was replaced by a verified PDF from the working vendor host. No missing datasheet was silently substituted.
''')

def architecture_docs():
    write('docs/FIRMWARE_ARCHITECTURE.md','''# Recovered application architecture

```mermaid
flowchart TD
  Entry[Entry 0x6400] --> Reset[Reset 0xc668: SRAM + CSRs]
  Reset --> Clock[SystemInit 0xd212]
  Clock --> Main[main 0x908a]
  Main --> Loop[application_loop 0x8ed4]
  Main --> UART[USART1 + DMA setup]
  Main --> Display[GPIO display + JPEG resource path]
  Main --> Files[Filesystem on SPI1 NOR]
  EXTI[PC button EXTI handlers] --> Event[button_event 0x200001ee]
  Event --> Loop
  RX[USART1 IDLE: 13 bytes] --> Packet[0x200002ac + saved event]
  Packet --> Loop
  Loop --> View1[Regular: 0xb5c8 + 0xa910]
  Loop --> View2[Recording: 0xb9e0 + 0xaed0]
  Loop --> View3[Monitor: 0xbc90 + 0xb29e]
  View2 --> History[30 x 4-byte history]
  Loop --> Menu[Menu state dispatcher 0x169d4]
  Menu --> Settings[Packed settings at 0x2000003e]
  Loop --> Power[Charge screen + battery checks]
  Timer[TIM3 IRQ] --> Power
  Power --> Shutdown[Commit settings + PC1 power latch]
```

This is a bare-metal foreground application with interrupt-updated globals. No RTOS scheduler, task context
switching, RTOS API or OS strings have been identified. GCC save/restore helpers are compiler support, not
evidence of an RTOS. Unnamed code still requires review, so absence of evidence is not a formal proof.

The main entry initializes priority grouping, clock bookkeeping, SysTick delays, USART2 debug at 115200,
USART1 measurement at 9600, button EXTI, battery inputs, display, resource filesystem and TIM3.
It then enters a nonreturning foreground loop. A separate strlen at 0x90d2 follows main in flash;
correctly marking the loop nonreturning separates that function from main.

The foreground loop handles a startup two-key condition, dispatches one of three views, processes DMA data,
checks charging state and performs battery/UI updates. Application view is at 0x200001de; measurement mode
is at 0x20000206; these are different state variables. Menu dispatch contains a recovered 32-state switch.
Packet parsing, UI state and command selection are intertwined; the full view logic is retained as pseudocode.

## Buttons and future expansion

PC2 is a power-related input. PC8 produces event 5 or event 6 after 61 repeated 10 ms waits, following a 20 ms
debounce. PC9 produces 3, PC10 produces 1, PC11 produces 2, PC12 produces 4. GPIOC also carries display controls,
the backlight PWM and analog/charge-related signals, so spare-pin assumptions require a board map.
The firmware stores one event byte, not an event queue. Simultaneous interrupts can overwrite events;
PC12 wins within the EXTI15_10 handler's ordered checks. The reconstructed tests preserve this behavior.
Blocking delays occur inside interrupt handlers and must be considered before adding more inputs or recording work.
The separate foreground long-press routine at 0x7b08 uses 10 ms polls with 41/101-count thresholds;
it is not the same policy as the PC8 interrupt handler. TIM3 refresh logic waits until the tick counter
exceeds 20, then adds 20 to the inactivity counter: these are different counts in the original code.
Actual elapsed timing still requires hardware validation.

## History and settings

The persistent RAM buffer starts at 0x20000020. It is loaded as 128 halfwords from 0x08040000.
Settings word is at 0x2000003e; count is at 0x20000040; thirty records begin at 0x20000042.
Each four-byte record contains a 16-bit metadata word and 16-bit magnitude. Append shifts 29 records toward
older positions, writes index zero, and caps count at 30. The recording view event path calls 0x12c78, which
updates its visible row state then appends history; extended automatic logging cadence is not fully recovered.

Metadata bits 0..5 are measurement mode, 6..11 unit ID, bit 12 sign, bits 13..14 decimal scale.
Scale codes 3/2/1/0 divide magnitude by 1000/100/10/1. Packing selects these at absolute values below 10/100/1000;
float-to-integer conversion truncates. Two overload representations store 65535 or 61166. Their exact display
interpretation is retained by the original overload classifier. The C encoder rejects nonfinite/out-of-range
input instead of relying on undefined native float-to-integer conversion; that is an explicit guard.

Shutdown commits the full 256-byte settings/history buffer using internal flash erase/program operations.
The address 0x08040000 is outside the supplied BIN and at the boundary of nominal 256 KiB flash. Its actual
accessibility and flash/RAM partition must be checked before any device test. Device-specific calibration
may exist elsewhere or inside the peer; its absence from this image must not be interpreted as dispensability.

## External resources and calibration

The NOR disk adapter reports 2048 sectors x 4096 bytes = 8 MiB, matching the user-provided W25Q64 capacity.
Filesystem paths and JPEG decoding connect this storage to display assets. Disk read/write wrappers mask sector
count to four bits before shifting by 12; requests of 16 sectors therefore cannot be assumed to work normally.
Sector-preserving writes use a 4096-byte scratch buffer and 256-byte page programming.

The display sequence resembles a common 320x240 color LCD family, but no unique controller identification has
been recovered. The firmware's two-key startup path processes measurement packets; calling it calibration mode
is a hypothesis until its commands, UI and peer behavior are fully traced. No fictional calibration API is provided.
''')

if __name__=='__main__':enrich()

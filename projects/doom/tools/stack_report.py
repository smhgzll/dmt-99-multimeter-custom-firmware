#!/usr/bin/env python3
"""Static worst-case stack depth of the standalone firmware.

Frames: the larger of the prologue adjustment found in the linked code (sum of
`addi sp,sp,-N` before the first restore) and the GCC -fstack-usage value.
Edges: direct calls and tail jumps between FUNC symbols, plus handle_reset ->
main (entered through mret). Indirect calls: every function whose address is
materialised in code or stored in initialised data is a callback; callbacks
are ranked in levels (bus primitives < storage/panel < decoder adapters <
rendering < application) and an indirect call made inside a level-k callback
may only reach callbacks below k. Unlisted callbacks get the top level and are
reported. Interrupts do not nest (INTSYSCR=0): bound = main path + worst
handler path.

Usage: stack_report.py ELF OBJDUMP SU_DIR [STACK_BYTES]
"""
import bisect, re, struct, subprocess, sys
from pathlib import Path

elf, objdump, su_dir = sys.argv[1:4]
limit = int(sys.argv[4]) if len(sys.argv) > 4 else None
# Names are qualified as file:function where static names repeat.
LEVELS = {
    0: 'set_pc write_pb delay buzzer_tone',
    1: 'write_screen',
    2: 'game_service',
}
FAMILIES = {
    'pump': 'game_service',
    'doom_cast': 'game_service','doom_project_sprites': 'game_service',
    'update_enemies': 'game_service','tick': 'game_service','doom_advance': 'game_service',
    'start': 'buzzer_tone',
    'doom_sound_init': 'buzzer_tone', 'doom_sound_stop': 'buzzer_tone',
    'doom_sound_update': 'buzzer_tone',
    'doom_sound_events': 'buzzer_tone',
    'write_word': 'set_pc write_pb',
    'display_panel_sequence': 'delay',
    'doom_fill': 'write_screen', 'doom_fill.part.0': 'write_screen',
    'doom_text': 'write_screen', 'doom_text.part.0': 'write_screen',
    'doom_render': 'write_screen', 'doom_menu': 'write_screen',
}
level_of = {n: k for k, names in LEVELS.items() for n in names.split()}
TOP = max(LEVELS) + 1

table = subprocess.run([objdump, '-t', elf], check=True, capture_output=True, text=True).stdout
functions, source = {}, None
raw = []
for line in table.splitlines():
    if ' df *ABS*' in line:
        source = line.split()[-1]
    m = re.match(r'^([0-9a-f]{8}) (.{7}) (\S+)\s+([0-9a-f]{8}) (\S+)$', line)
    if m and 'F' in m[2] and m[3] == '.text':
        raw.append((int(m[1], 16), m[5], int(m[4], 16), source, 'l' in m[2]))
    elif m and m[3] == '.text' and m[5] in ('handle_reset', 'fw_default_handler', 'NMI_Handler'):
        raw.append((int(m[1], 16), m[5], 0x60, source, False))
counts = {}
for a, n, size, src, local in raw:
    counts[n] = counts.get(n, 0) + 1
for a, n, size, src, local in raw:
    functions.setdefault(a, (f'{src}:{n}' if counts[n] > 1 else n, size))
name_at = {a: n for a, (n, _) in functions.items()}

dis = subprocess.run([objdump, '-d', '--no-show-raw-insn', elf], check=True, capture_output=True, text=True).stdout
lines_of = {n: [] for n, _ in functions.values()}
ranges = sorted((a, a + max(s, 2), n) for a, (n, s) in functions.items())
starts = [r[0] for r in ranges]
for line in dis.splitlines():
    m = re.match(r'^\s+([0-9a-f]+):\s', line)
    if not m:
        continue
    a = int(m[1], 16)
    i = bisect.bisect_right(starts, a) - 1
    if i >= 0 and ranges[i][0] <= a < ranges[i][1]:
        lines_of[ranges[i][2]].append(line)

def frame(lines):
    total = best = 0
    for line in lines:
        m = re.search(r'\b(?:c\.)?addi(?:16sp)?\s+sp,(?:sp,)?(-?\d+)', line)
        if m:
            total -= int(m[1])
            best = max(best, total)
            if int(m[1]) > 0:
                break
    return best

su, dynamic = {}, []
for path in Path(su_dir).glob('*.su'):
    for row in path.read_text().splitlines():
        parts = row.split('\t')
        if len(parts) >= 3:
            key = parts[0].rsplit(':', 1)[-1]
            su[key] = max(su.get(key, 0), int(parts[1]))
            if 'dynamic' in parts[2] and 'bounded' not in parts[2]:
                dynamic.append(row)
sizes = {n: max(frame(l), su.get(n, 0)) for n, l in lines_of.items()}

edges, indirect = {}, set()
for n, lines in lines_of.items():
    out = set()
    for line in lines:
        m = re.search(r'\b(?:jal|j|tail|call)\s+(?:ra,)?([0-9a-f]+) <([^>]+)>', line)
        if m and int(m[1], 16) in name_at and name_at[int(m[1], 16)] != n:
            out.add(name_at[int(m[1], 16)])
        if re.search(r'\bjalr\b', line) or re.search(r'\bjr\s+(?!ra\b)\w+', line):
            indirect.add(n)
    edges[n] = out
edges.setdefault('handle_reset', set()).add('main')

taken = set()
for n, lines in lines_of.items():
    for line in lines:
        code = line.split('#')[0]
        if 'jal' in code or re.search(r'\bj\s', code):
            continue
        for m in re.finditer(r'# ([0-9a-f]+) <([^>+]+)>', line):
            if int(m[1], 16) in name_at:
                taken.add(name_at[int(m[1], 16)])
data = Path(elf).read_bytes()
shoff = struct.unpack_from('<I', data, 0x20)[0]
shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 0x2e)
sections = [struct.unpack_from('<IIIIIIIIII', data, shoff + i * shentsize) for i in range(shnum)]
names = sections[shstrndx]
vector = set()
for sh in sections:
    sname = data[names[4] + sh[0]:data.index(b'\0', names[4] + sh[0])].decode()
    if sh[1] != 1:
        continue
    blob = data[sh[4]:sh[4] + sh[5]]
    for off in range(0, len(blob) - 3, 4):
        w = struct.unpack_from('<I', blob, off)[0]
        if w in name_at and w > 0x100:
            (vector if sname == '.vector' else taken).add(name_at[w]) if sname in ('.vector', '.text', '.data') else None
callbacks = sorted(taken - vector - {'_start', 'handle_reset', 'main'})
unranked = [c for c in callbacks if c not in level_of]

memo, cycles, soft_cycles = {}, [], []
def depth(n, ctx, stack=(), fallback=()):
    key = (n, ctx)
    if key in memo:
        return memo[key]
    if n in stack:
        i = stack.index(n)
        # A cycle closed only through level-based (unresolved) indirect edges
        # is an artefact of the over-approximation; direct cycles are real.
        (soft_cycles if any(fallback[i:]) else cycles).append(list(stack[i:]) + [n])
        return 0, [n + ' (cycle)']
    best, path = 0, []
    targets = [(t, ctx, False) for t in edges.get(n, ())]
    if n in indirect:
        base = re.sub(r'\.(constprop|isra|part|cold)\.\d+', '', n)
        family_key = next((k for k in (n, base, base.split(':')[-1]) if k in FAMILIES), None)
        if family_key is not None:
            family = FAMILIES[family_key].split()
            targets += [(c, level_of.get(c, TOP), False) for c in callbacks if c in family or c.split(':')[-1] in family]
        else:
            targets += [(c, level_of.get(c, TOP), True) for c in callbacks if level_of.get(c, TOP) < ctx]
    for t, c, via in targets:
        d, p = depth(t, c, stack + (n,), fallback + (via,))
        if d > best:
            best, path = d, p
    memo[key] = (sizes.get(n, 0) + best, [n] + path)
    return memo[key]

sys.setrecursionlimit(10000)
main_depth, main_path = depth('handle_reset', TOP + 1)
handlers = sorted(vector - {'_start'})
(irq_depth, irq_path), irq_name = max(((depth(h, TOP + 1), h) for h in handlers), key=lambda x: x[0][0])
total = main_depth + irq_depth
print(f'functions {len(lines_of)}, callbacks {len(callbacks)}, functions with indirect calls {len(indirect)}')
if unranked:
    print('unranked callbacks (top level):', ' '.join(unranked))
print(f'worst main path {main_depth} B: ' + ' > '.join(main_path))
print(f'worst handler path {irq_depth} B ({irq_name}): ' + ' > '.join(irq_path))
print(f'bound {total} B' + (f' of {limit} B stack ({limit - total} B margin)' if limit else ''))
for row in dynamic:
    print('dynamic frame:', row)
for c in cycles[:5]:
    print('call cycle:', ' > '.join(c))
for c in soft_cycles[:3]:
    print('note: cycle only through unresolved indirect edges:', ' > '.join(c))
fail = (limit is not None and total > limit) or bool(cycles) or bool(dynamic)
print('STACK', 'FAIL' if fail else 'PASS')
sys.exit(1 if fail else 0)

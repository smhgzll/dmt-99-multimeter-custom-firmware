# POTAT-99 DOOM validation

## 0.4.0 — button correction, ramped movement and visibility

18 October 2026. Center PC8 fires on debounced press. Holding it alone does
not pause. Holding PC8+down PC10 for 2 seconds opens the menu once;
fire/backward motion remain suppressed until both keys are released.
Menu confirmation does not repeat. Boot-held buttons stay unarmed until
release; POWER behavior and 32-bit clock wrap are preserved.

- Four C groups and four ASan/UBSan groups passed: core, sound, events/AI,
  controls. Preserved checks include 5,920 filtered/full ray comparisons,
  2,000 mixed actions and 6,000 pure upstream collision/ray comparisons.
  Six Python host tests passed.
- New controls checks cover short/held fire, the 2,000 ms chord threshold,
  early release, debounce, one menu confirmation, post-chord release,
  boot guards, clock wrap, small tap movement/turns, acceleration, bounded
  maximum speed, immediate stopping, reversal reset and opposing inputs.
- Movement: 40→160 units/s, acceleration 240 units/s².
  Turning: 16→80 degrees/s, acceleration 128 degrees/s².
  Q8 fractions are retained. A service interval is capped at 100 ms;
  accumulated world motion is capped at 64 units / 45 degrees.
  Drawing callbacks collect input only; world mutations occur at safe points.
- Gunfire no longer wakes an IDLE enemy through a wall. After sight loss,
  pursuit uses the last seen position for at most 50 AI ticks, never updating
  it from the hidden player's current position. Cross-wall sight barriers
  were checked for all 404 map walls. Hidden windup shots, imp projectiles
  and splash damage are blocked, and unseen targets are forgotten.
  No global route search was added.
- GOD MODE blocks all player `hurt_player` paths. SOUND off starts no target
  PWM. New Game preserves options; app init/power cycling resets god off,
  sound on and pistol. No Flash writes.
- After the death animation, corpses remain for 80 ticks (8 game seconds),
  become REMOVED, and are reinitialized by New Game. Actor arrays are fixed
  and heap-free; removal saves sprite work rather than returning RAM.

Four actual-BIN RV32 scenarios passed: **134 completed frames**, of which
**60 matched native C exactly in RGB565**.
Action, Q8-motion and tick observers replay into the native oracle without
replacing target instructions. Physical-GPIO modeling checked all four
directions, 2.3 s solo fire, the 2 s chord, pause clock, resume, weapons,
god/sound menus and POWER. An enemy attack did not lower health in the god
fixture; muted fire started no new TIM2 tone.
SRAM fixtures that position actors or shorten corpse timers are excluded
from native pixel-equality counts.
The legacy `collision_during_detour` field now represents six position
checks of an actor that remains idle behind a wall; it is not additional moving-detour evidence.

BIN **163,444 B**, physical **0x08006400–0x0802E273**;
static RAM **8,568 B**, stack **6,144 B**, gap **18,056 B**.
Static stack bound **3,274 B**; maximum observed emulator use **848 B**.
No emulated Flash/option-byte/NOR writes, ASIC TX, unexpected IRQ/fault or
panel-bounds violation occurred. The original standalone-project check
preserved 5,017 parent files and all three prior DOOM BINs.
A full object/asset rebuild produced the same BIN.
Evidence is in `docs/validation/v0.4.0/`, screenshots in `docs/screenshots/v0.4.0/`.
The release was not physically tested. Rendering remains capped at 5 FPS;
physical performance, installation and acoustic output were not measured.

## 0.3.0 — timed AI, animation and weapons

18 October 2026. AI runs independently of keys in 100 ms steps.
This deliberately changes the old turn-based AI; the 0.2.0 set of 3,000
action/AI comparisons is not credited to this version.
The **6,000** unchanged upstream ray-intersection and pure wall/circle
collision cases remain. Earlier test sources/results are retained in Git
history and older report directories; the parent project was unchanged.

- Core C: menus/resume/new game, weapon selection, **5,920** filtered/full
  ray comparisons, **2,000** timed walk/combat actions and renderer bounds.
- Event/AI fixtures: input does not immediately move NPCs; clock-driven
  movement without keys, sight/hearing, wall-blocked visibility/shots,
  wall collision/detours, timed attacks and death/respawn, HP/pain/death/
  corpses and small pickup scale.
- Six distinct weapon damage models, shotgun pellets, bursts, short range,
  rocket collision/splash and clock-driven weapon animation.
- Sound C tests; all three C groups with ASan and fail-fast UBSan.
  LeakSanitizer disabled under ptrace; address/UB checks enabled.
- Four Python host tests: deterministic CLI, pause/clock, weapon selection and PNG.

Emulation exposed short GPIO presses being lost during expensive raycasts.
Button/audio servicing now runs between ray groups, actor updates and
drawing strips. Input is applied at safe update points; service callbacks
do not directly mutate NPC/world state. The fixed eight-entry queue gives
pause priority over its final entry when full. No new IRQ or heap was added.

Sprite metadata uses actual WAD aspect ratios; pickups are 8–24 units high,
actors 56. Animation frames are actual WAD lumps.
The map remains single-height, without global pathfinding, full inventory
or every directional sprite rotation. AI uses local detours.

BIN **162,096 B**, **0x08006400–0x0802DD2F**;
static RAM **8,028 B**, stack **6,144 B**, gap **18,596 B**.
Static stack bound **3,274 B**. Rendering cap 5 FPS; physical performance unmeasured.

Four emulator scenarios passed: 77 completed frames, **40** matching native
C exactly in RGB565. The other 37 were controlled target-fixture frames.
In fixtures, an NPC sees/approaches/attacks without keys; shooting kills,
the corpse remains, walls block sight/shots, and detours do not cross walls.
Observed stack **852 B**, static bound **3,274 B**.
A complete rebuild was byte-identical; 5,017 parent files and two prior BINs were unchanged.

Machine reports: `docs/validation/v0.3.0/`.
GPIO/action and simulation-tick observers replay the same time steps into
the native oracle, without replacing target instructions.
Controlled SRAM actor fixtures are labeled separately and excluded from
native pixel-equality claims. These are not physical-device experiments.

## 0.2.0 — directional buttons and buzzer

18 October 2026. Up PC11=forward, down PC10=backward, left PC12=turn left,
right PC9=turn right; PC8 short/long and POWER behavior preserved.
Wiring directions came from the user's 0.1.0 observations; 0.2.0 was not physically tested.

BIN **81,452 B**, **0x08006400–0x0801A22B**;
static RAM **6,160 B**, stack **6,144 B**, gap **20,464 B**.
Static stack bound **2,650 B**. The 0.1.0 release and parent POTAT-99 project were preserved.

- Core C, three Python host tests and **9,000** upstream comparisons passed.
- New event fixtures covered actual steps, blocked movement, silent turns,
  hit/miss shots, bump kills, damage and death/respawn events.
- Sound tests covered four patterns, fire/kill ordering, hurt priority,
  bounded pending events, pause/stop, delayed service and 32-bit clock wrap.
- All three C tests passed Clang ASan/UBSan with the same ptrace exception.
- Four RV32 scenarios passed. Corrected directional gameplay screens matched
  native C. Kill and death/respawn fixtures observed all four sound events.
  PWM frequencies were within 1% of nominal, duty roughly 50%; buzzer stopped
  at effect end, pause and shutdown.
- Full rebuild byte-identical; 5,017 parent files and the 0.1.0 BIN unchanged.
  Observed emulator stack **864 B**.

Events originate at actual gameplay decision points; damage is not lost
when death resets health to 100. Blocked movement and turning produce no
walk sound. Gameplay mathematics remained equivalent in upstream comparisons.

Actual RV32 tests compare corrected GPIO directions against native screens.
**Isolated SRAM fixtures** trigger killing-shot and death/respawn branches
in real BIN code; no fixture setter is compiled into production firmware.
This is not a claim of naturally reached gameplay.
Checks cover PA1 AF push-pull, TIM2 CH2 enable, PSC/ARR, roughly 50% duty,
distinct frequencies and silence.

Testing showed audio service can be delayed during raycasts.
Instead of skipping the next note, it starts for its full duration; notes
may stretch without blocking gameplay. Emulation is not an electrical or
acoustic model; buzzer audibility and loudness remain untested.

Reports: `docs/validation/v0.2.0/`. Earlier 0.1.0 evidence is retained below.

## 0.1.0 validation — preserved historical results

Date: **18 October 2026**. These results concern the former standalone
`dmt_99_doom` project only. No builds/tests were run in the parent POTAT-99
tree, and its sources/releases were unchanged during that work.

### Build and memory

- RV32IMAC + Zicsr/Zifencei, ILP32, software float, ESP-IDF GCC 14.2.0,
  `-Os -ffreestanding -fno-builtin`, `-Wall -Wextra -Werror`.
- Raw BIN **79,872 bytes**, load **0x08006400**, end-exclusive **0x08019C00**.
  Alias entry **0x6400**, vectors **0x6438**, same V2.4 header words.
- Code bound alias **0x30000**; settings **0x08040000** outside the image.
- Static RAM **6,136 B**, stack reserve **6,144 B**, gap **20,488 B**.
  The size tool's BSS total includes the reserved stack section.
- Static stack bound **2,650 B**: foreground 2,634 B + trap path 16 B,
  using local callback-family definitions, GCC .su data and objdump.
- Observed emulator stack **860 B** including gameplay/fire, **712 B** in
  boot-only scenarios; not proof for all possible game states.
- Forced object/WAD-subset regeneration produced a byte-identical BIN.
- SHA-256:
  `c6c3bfed28a7fa6eba0718b9121155be706a30c8829ea06deadb1937b4a41005`.

### Tests executed

| Test | Result / scope |
|---|---|
| C core | PASS; menu/help/resume/new game, collision and bounds |
| Filtered rays / full wall scan | **5,920** comparisons; 24 directions, 50 walking positions |
| Deterministic C walk/combat | **2,000** actions; renderer at 50 positions, final full-screen coverage |
| Clang ASan + UBSan | Same C tests pass; LeakSanitizer disabled under ptrace |
| Python host suite | **3 pass**; reproducibility, pause/restart, PPM/PNG |
| Pinned upstream gameplay | Equal position/angle/health/kills/pickups after **3,000 actions** |
| Pinned upstream collision | **3,000 points** equal |
| Pinned upstream raycast | Equal wall/distance/fraction for **3,000 rays** |
| RV32 update format / ELF | PASS; header, vectors, load region, RAM and undefined symbols |
| Complete RV32 BIN / panel | **4 scenarios pass**, below |
| Parent preservation | **5,017 files** unchanged; none added/deleted/modified |

Upstream comparisons compile unchanged functions extracted from `level1.c`
with host shims for AVR memory/UART access. They do not compare AVR machine
code or id Software's original engine. The new camera plane/local renderer
are also checked by RV32/host comparisons. The upstream candidate list,
truncated at 140 walls, has capacity for all 404 here, avoiding silent loss of visible walls.

### Offline execution of the actual RV32 BIN

A **copied** parent `Ch32Emulator` was used. The original 8 MiB dump was read
into a private RAM copy. CPU instructions were not replaced with native
renderer calls. The run used Unicorn **2.1.0**, Pillow **13.0.0**;
emulated time is not measured hardware FPS.

1. **Clean boot:** menu → PC8 New Game → PC11/PC10 turning → PC9/PC12 movement
   → PC8 fire → held PC8 pause → PC2 off. Eight completed screens matched
   native C framebuffers **pixel for pixel**. These are historical 0.1.0 controls.
2. **Bootloader leftovers:** SysTick/OTG IRQ enable/pending and VTF leftovers
   were cleared by startup; the menu matched clean boot.
3. **HSE unavailable:** existing POTAT-99 HSI×18 fallback produced the same menu.
4. **POWER held at boot:** starts without immediately shutting down;
   releasing/repressing POWER drops the latch and halts the CPU.

Every scenario had **zero** faults/stray IRQs, panel-bounds violations,
ASIC TX, Flash erase/program/unlock sequences.
Settings and NOR hashes were unchanged. No Flash/SPI writer or heap function was linked.

Testing fixed a polling issue: a released movement key no longer generates
another move during debounce. Held-PC8 testing waits for the completed pause
screen rather than counting an unfinished/pending gameplay frame.

### Parent-project preservation

Baseline: `docs/reference_hashes.json`; results:
`docs/validation/reference_preservation.json`.
.cache/.history were excluded; no build/Git operations were run there.
The existing Python environment was used only as interpreter/dependencies
with bytecode writes disabled. Parent sources, original BIN, flashdump,
analysis evidence, build outputs and published final/vX.Y.Z files were retained.

### First physical trial plan

**No physical DMT99 operation was performed.**
Panel/button/power/backlight timing, performance, USB acceptance and rollback remained unverified.

1. Keep the working firmware and hash separately; first check that a charged
   meter enters its normal USB update mode.
2. Install the single BIN listed in [README](README.md) using the existing USB
   process. Do not assume acceptance: filename/CRC/version rules are unknown.
3. Check menu, New Game, five buttons, short/held PC8 and POWER release/repress.
   If a PA2 115200 console is available, record the banner or trap message.
4. Record response time, display orientation and colors. The game supplies
   no measurement/charging/V2.4 low-battery/auto-off flow; use POWER to shut down.
5. Reinstall the previous working POTAT-99 via USB; check measurement and
   retained settings. Record physical rollback separately.

No step requires RDP, option-byte or debug-programmer operations.
Because the bootloader is unavailable, emulator tests cannot guarantee erase
range or interrupted-update recovery.

Machine-readable build/stack/upstream/emulator/preservation reports are in
`docs/validation/` and local `dist/v0.1.0/`.

The historical 5,017-file parent preservation result predates consolidation.
Current monorepo verification explicitly checks 884 immutable evidence files;
see [migration notes](../../docs/WORKSPACE_MIGRATION.md).

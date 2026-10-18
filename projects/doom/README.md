# POTAT-99 DOOM 0.4.0

A small E1M1 game for DMT99 / CH32V303RCT6. In the consolidated repository,
sources are under `projects/doom/` and releases under `artifacts/firmware/doom/`.
Reports from the earlier standalone project are preserved.
Current guides: [build](../../docs/BUILDING.md),
[Windows installation and rollback](../../docs/UPDATING.md),
[risks](../../docs/LEGAL_AND_SAFETY.md).

**[Current USB BIN: DMT-99_Doom_V0.4.0_20261018.bin](dist/v0.4.0/DMT-99_Doom_V0.4.0_20261018.bin)**

In 0.4.0, pressing the center button fires immediately; holding it alone
does not open the menu. Hold **center PC8 + down PC10 together for 2 seconds**
to pause. Movement/turning are time-based: short presses make small changes,
while held buttons accelerate to bounded speeds. Gunfire does not wake
enemies through walls. After losing sight, they pursue only the last seen
position for up to 5 seconds rather than tracking the hidden player's current location.

![Menu options](docs/screenshots/v0.4.0/sound-off.png)
![Gameplay](docs/screenshots/v0.4.0/game.png)

[Emulator screenshots of the current BIN](docs/screenshots/v0.4.0/README.md).

## Features

- Walls block sight, shots and noise wake-up; pursuit uses a bounded last-seen target.
- GOD MODE and SOUND options. God prevents all player damage; muted sound
  starts no PWM effects. Power cycling resets god off and sound on.
- Fire on debounced press, repeat according to weapon cooldown while held.
  Only the 2-second PC8+PC10 chord pauses; both keys must be released before reuse.
- Movement 40→160 units/s, acceleration 240 units/s²; turning 16→80 degrees/s,
  acceleration 128 degrees/s². Release stops motion, reversal resets the ramp,
  opposing keys cancel, and Q8 retains sub-degree precision.
- Idle, chase, windup, attack, pain and death states.
- Ranged human attacks, avoidable imp fireballs and demon melee attacks,
  with attack cooldowns and enemies requiring multiple hits.
- Actual WAD walking/attack/pain/death frames. Corpses remain for 8 seconds
  after the death animation, then disappear. Fixed actor arrays use no heap;
  removal reduces drawing work rather than returning RAM.
- Pickups retain their aspect ratios and render 8–24 world units tall.
  Health pickups heal; others increment the collection counter.
- Weapon menu, distinct damage/range/speed, muzzle animation, recoil and
  walking bob; rockets/fireballs with wall/actor collision.
- Button/audio servicing during drawing and long computations; an eight-entry
  queue applies input at a safe world-update point.

This is an enhanced [doom-avr](https://github.com/Artfical/doom-avr/)
raycaster, not the complete classic DOOM engine. The map has one height;
there is no full sector/door/key system, music, complete inventory or save game.
Enemies use local detours/wall sliding, not global pathfinding.
Sprites face the camera; this small ROM does not contain all directional views.

**This release has offline/emulator tests; physical FPS, sound and installation
have not been verified for it.** Earlier button observations came from the user.

## Controls and weapons

| Button | Gameplay | Menu |
|---|---|---|
| Up / PC11 | Forward | Previous row |
| Down / PC10 | Backward | Next row |
| Left / PC12 | Turn left | Back / Resume |
| Right / PC9 | Turn right | — |
| PC8 | Fire on press, repeat while held | Select / confirm |
| PC8 + down, 2 seconds | Pause menu | Back / Resume |
| POWER / PC2 | Power off | Power off |

Choose **WEAPONS**, navigate with up/down and confirm with PC8; left returns
without selecting. In game: PC8+down for 2 seconds → **WEAPONS** → select →
**RESUME**. The world freezes in menus, weapon selection and pause.
Weapon selection remains in RAM and defaults to pistol at startup.
All six weapons are available from the start with unlimited ammunition.
New Game preserves god/sound/weapon choices; full startup restores defaults.
These options are not written to Flash.

| Weapon | Behavior |
|---|---|
| FIST | Short range, 24 damage |
| PISTOL | Single bullet, 20 damage, 400 ms cooldown |
| SHOTGUN | Five pellets ×12 damage, spread, 800 ms cooldown |
| CHAINGUN | Three-shot burst ×12 damage, 400 ms cooldown |
| CHAINSAW | Three short-range hits ×12 damage |
| ROCKET | Moving rocket, 100 center / 50 surrounding splash damage |

Range, walls and hits affect damage. A nearby rocket explosion deals
30 damage to the player. Key presses do not advance enemies one step;
the AI has an independent clock.

## Sound

PA1 / TIM2 CH2 supplies roughly 50% PWM: step 800 Hz; fire 2600→1300 Hz;
hurt 330→220 Hz; kill 1200→1800→2400 Hz.
Blocked movement and turning alone make no step sound.
Hurt has priority; a killing shot plays fire followed by kill.
Menus and shutdown silence the buzzer. These are short buzzer effects,
not music or sampled audio. The desktop simulator is silent.

## Image and RAM

| Measure | Value |
|---|---:|
| BIN | **163,444 bytes / about 159.6 KiB** |
| Physical load range | **0x08006400–0x0802E273** |
| Entry / vector alias | **0x6400 / 0x6438** |
| Image upper bound | 0x30000; 167 KiB available to the application |
| Static RAM | **8,568 B** |
| Stack reserve / static bound | **6,144 B / 3,274 B** |
| Space outside static RAM + stack | **18,056 B** |
| Display | 320×200 scene + 40-pixel HUD, 80 rays |
| Drawing strip | 1,600 B; no full target framebuffer |
| ROM assets/metadata | 129,200 B; 75 sprite frames |

The world step is 100 ms; rendering is capped at 5 FPS, not a measured
physical rate. Catch-up is capped at 500 ms per update after long stalls.
Some real time may be dropped when rendering is slow.

A reduced WAD subset is embedded in the BIN; do not copy a separate WAD or
package to the meter. The roughly 1.6 MB USB/BOOT volume is not MCU code space.
The game writes neither external Flash nor settings page 0x08040000,
and sends no ASIC commands.

## Installation and returning to measurement firmware

1. Keep a known-working POTAT-99 BIN on the computer. Preserved releases are
   under `../../artifacts/firmware/potat-99/vX.Y.Z/`.
2. Verify with `cd dist/v0.4.0 && sha256sum -c SHA256SUMS`.
3. Use the existing **POWER+MENU USB update** process on Windows and copy
   only the current game BIN. Do not copy debug files, JSON, WAD or images.
4. Remove the game by reinstalling the known-working POTAT-99 BIN the same way.
   The original application is `../../artifacts/firmware/original/DMT-99_V2.4_20240306.bin`.

This firmware has no measurement mode, V2.4 charging screen, low-battery
flow or auto-off; use POWER to turn it off. The BIN omits the bootloader
and does not itself program Flash. Because the bootloader is unread,
its erase/acceptance rules and interrupted-update recovery cannot be guaranteed.
No RDP/option-byte/programmer step is required. Build tools do not connect to hardware.

## Building and simulator

```bash
python3 tools/fetch_wad.py        # hash-checked local shareware input
make -j4 all
make test sanitize
make emulate
make verify                      # 884 immutable evidence files after consolidation
make package
python3 tools/simulator.py
python3 tools/simulator.py --script 'fwwwaaf..........' --png build/scene.png
```

Upstream sources are vendored; no submodule initialization is needed.
On desktop, W/S or arrows move/navigate, A/D or left/right turn, Space/Enter
fires/confirms, Space+down for 2 seconds or Esc pauses, and F12 saves PNG.
The GUI uses the same C button/ramp code; releasing keys or losing focus
stops motion. The main loop advances time. In headless scripts each `.`
advances 100 ms; other characters do not directly advance time.

Requires GCC/make/Python 3 and RV32IMAC ILP32 GCC.
The Makefile locates the ESP-IDF toolchain; override with `CROSS=`.
Tk is needed for the desktop window, Unicorn/Pillow for emulation.
The emulator defaults to `PYTHON`; select an environment explicitly:

```bash
python3 -m venv .venv
.venv/bin/pip install -r tools/requirements-emulation.txt
make emulate EMU_PYTHON=.venv/bin/python
```

## Sources and evidence

- `src/game.c`: timed world, actors, visibility/collision, weapons and rockets.
- `src/render.c`, `app.c`: sprite/strip rendering, HUD, menus and weapon selection.
- `src/controls.c`: shared debounce, pause chord and time-based acceleration.
- `src/sound.c`: bounded short effects; `board/board.c`: buttons, PWM, display and power.
- `board/reference/`, `third_party/wch/`: separately retained source copies.
- `tools/build_assets.py`: animation/weapon/pickup ROM data from the pinned WAD.
- `tools/test_emulator.py`: actual RV32 instructions, GPIO input and panel framebuffer.

Results: [TEST_RESULTS.md](TEST_RESULTS.md) and `docs/validation/v0.4.0/`.
Older reports and releases remain intact.
Provenance/licenses: [NOTICE.md](NOTICE.md), file hashes in `docs/copied_sources.json`.
WAD/images have separate id Software terms; local BINs and WAD are excluded from Git.

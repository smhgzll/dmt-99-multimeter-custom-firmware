# DOOM screenshots in the DMT99 emulator

18 October2026: the actual RV32 dmt99-doom.bin ran through the existing
parent tools/fw_emulator.py. It was byte-identical to the published game BIN.
Images are the emulated panel's **320×240 framebuffer**, not desktop-renderer
captures or mockups.14 completed frames matched native C in RGB565 under identical input.

| Stage | PNG |
|---|---|
| Startup menu | [01-menu.png](01-menu.png) |
| E1M1 start | [02-game.png](02-game.png) |
| Seven forward steps | [03-forward.png](03-forward.png) |
| Three left turns, visible enemies | [04-enemies.png](04-enemies.png) |
| Fire | [05-fire.png](05-fire.png) |
| Held-PC8 pause, historical controls | [06-pause.png](06-pause.png) |

![Menu](01-menu.png)
![Enemies](04-enemies.png)
![Fire](05-fire.png)
![Pause](06-pause.png)

Historical button sequence: shortPC8→PC9×7→PC10×3→shortPC8→heldPC8.
GPIO/hold sequence and BIN/emulator/frame/PNG hashes:
[capture.json](capture.json).

The existing tools/test_emulator.py also passed clean boot, bootloader
interrupt leftovers, HSE fallback and boot-held POWER:
[regression.json](regression.json).

No faults, panel overflow, ASIC TX or Flash writes were observed.
The parent project/firmware were unchanged.
This was not physical-device or USB installation testing.

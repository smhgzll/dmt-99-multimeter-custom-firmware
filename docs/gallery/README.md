# Photo and screenshot gallery

## Device photos

User-supplied photographs of the DMT99 and the research setup. These are
physical-device photos; the emulator and desktop captures are grouped separately below.
The photos show the displayed applications and hardware, without identifying
the exact installed BIN or establishing measurement accuracy.

### DOOM and the simple screen

| DOOM: corridor gameplay | DOOM: enemy encounter | POTAT-99: simple DC voltage screen |
|---|---|---|
| <img src="doom1.jpeg" width="260" alt="DMT99 displaying DOOM corridor gameplay"> | <img src="doom2.jpeg" width="260" alt="DMT99 displaying a close enemy encounter in DOOM"> | <img src="simple_theme.jpeg" width="260" alt="DMT99 simple screen with green digits and a DC voltage reading"> |

### MCU and external Flash

| CH32V303RCT6 MCU and 8 MHz crystal | Winbond W25Q64 external SPI Flash |
|---|---|
| <img src="mcu.jpeg" width="320" alt="DMT99 PCB close-up showing the CH32V303RCT6 and its 8 MHz crystal"> | <img src="flash.jpeg" width="320" alt="Close-up of the Winbond W25Q64 Flash chip on the DMT99 PCB"> |

### Debug and UART capture setup

The debug connection used by this project is two-wire **RVSWD**; the
original photo filename `jtag.jpeg` is retained.

| RVSWD debug wiring on the opened meter | Logic-analyzer capture setup |
|---|---|
| <img src="jtag.jpeg" width="280" alt="Opened DMT99 with debug wires attached to the PCB"> | <img src="logic_analyser.jpeg" width="520" alt="Opened DMT99 connected to a USB logic analyzer for signal capture"> |

## POTAT-99 / custom firmware

Existing `docs/firmware_screens/` screenshots were captured in RV32
emulation and accompany the V2.4/parity comparisons. They are not photographs
of 0.6.0 on physical hardware or evidence of its new theme screens.

| Regular measurement | Second layout, HOLD/REL |
|---|---|
| ![DC V](../firmware_screens/02-regular-dc-volts.png) | ![Theme 2](../firmware_screens/04-theme2-hold-rel.png) |

| Recording graph | Monitor |
|---|---|
| ![Graph](../firmware_screens/05-record-graph.png) | ![Monitor](../firmware_screens/06-monitor-thresholds.png) |

| Menu | History |
|---|---|
| ![Menu](../firmware_screens/07-settings-menu.png) | ![History](../firmware_screens/08-history-browser.png) |

## DOOM 0.4.0

Emulator screenshots from the previously verified 0.4.0 BIN were reused.
No long emulator session was run during consolidation. Advanced test screens
using SRAM fixtures are labeled in the
[detailed gallery](../../projects/doom/docs/screenshots/v0.4.0/README.md).

| Gameplay | Menu / sound |
|---|---|
| ![Gameplay](../../projects/doom/docs/screenshots/v0.4.0/game.png) | ![Options](../../projects/doom/docs/screenshots/v0.4.0/sound-off.png) |

## Studio theme editor and Flash Manager

These two screenshots were captured from the **actual Qt application** during
consolidation, using offscreen mode and the native C preview tool.
No hardware was connected and no live measurement was started.
Reproduce with `tools/capture_studio_gallery.py`.

![Studio theme editor](studio-theme-editor.png)

![Studio Flash Manager](studio-flash-manager.png)

Game and manufacturer images remain the property of their rights holders;
[redistribution limits apply](../LEGAL_AND_SAFETY.md).

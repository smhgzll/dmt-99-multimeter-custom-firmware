# Firmware verification (0.2.0)

What has been checked for the release (USB update image) and the standalone
image, at which level, and how to rerun it. None of it is physical hardware
testing.

## How to run

```bash
python3 -m venv .cache/venv && .cache/venv/bin/pip install -r tools/requirements-emulation.txt
make -C reconstructed firmware-all all check simulator          # L1 + L2 host suites
python3 tools/test_battery_differential.py                      # L3, needs build/librecovered.so
.cache/venv/bin/python tools/test_startup_differential.py       # L3 register level
P=reconstructed/build/firmware-update-parity/dmt99-update-parity.bin
.cache/venv/bin/python tools/test_update_image.py --version 0.2.0       # L1/L4 update format, hand-over, release vs parity
.cache/venv/bin/python tools/test_firmware_emulation.py --image $P      # L4 image vs host simulator
.cache/venv/bin/python tools/test_original_differential.py --jobs 8 --image $P   # L4 image vs V2.4 image (slow)
```

Without `--image` the emulation suites run the standalone image.

The earlier suites (`tools/test_*_differential.py`, sanitizers, unittest groups)
remain as before and still pass; they rewrite their own reports with new
library hashes.

## Level 1 — build and layout

`make firmware` fails unless `tools/firmware_report.py` confirms the layout:
for the update image a reset jump at 0x6400, the V2.4 header words (12 nops,
ebreak) and the vector table at 0x6438 with nothing linked below 0x6400; for
the standalone image the jump at 0x0 and the table at 0x4. In both cases every
vector slot points inside the image, the BIN equals the loadable image, the
image ends below 192 KiB and 0x08040000, SRAM ≤ 32 KiB, there are no undefined
symbols and no host/libc symbols (stdio, malloc, simulator). `make release`
also checks the BIN against the V2.4 update file (`tools/update_image_format.py`:
header words identical, slot 0 = 0x6400, `.fw_info` load address 0x08006400 and
version) and refuses to replace a published version. Warnings
are enabled (`-Wall -Wextra`); the image builds without warnings.

`tools/stack_report.py` bounds the stack from the linked code and GCC stack
usage: worst main path 4270 B + worst handler 512 B = 4782 B of 6144 B. It
resolves indirect calls through explicit callback families and fails on
unbounded frames or call cycles. The emulated scenarios never used more than
about 1.8 KiB.

## Level 2 — host regression

All pre-existing C tests, sanitizer builds and Python differential suites pass
in both profiles. New host tests: signed low-battery shutdown (7780),
`screen_power_on` values, factory-reset path.

## Level 3 — machine code of V2.4 versus the reconstruction

| Check | Original | Result |
|---|---|---|
| Start-up configuration registers (`test_startup_differential.py`) | main 0x908a → 0x8ed4 in the trace oracle; firmware in the emulator | 46/46 registers equal; firmware-only: RCC CTLR/INTR written by SystemInit |
| SystemInit RCC write sequence | 0xd212 | 15 writes identical |
| Panel initialisation bus words | 0x87aa incl. full-screen clear | 76 889 words identical |
| delay_ms sequence during start-up | 0x7944, 0x87aa | [300, 10, 20, 20, 120] both |
| Battery estimate (`test_battery_differential.py`) | 0x75f0 | 4096 ADC means: result and both printf doubles bit-identical |
| JEDEC capacity table | 0xd782 | 40 identifiers identical |
| printf %f behaviour | 0x1aede | truncation reproduced |

## Level 4 — the firmware image executing

`tools/fw_emulator.py` runs the BIN in Unicorn with models of PFIC/SysTick,
RCC, FLASH (settings page), GPIO/EXTI, TIM2/3/8, ADC, USART1+DMA, USART2, SPI1
with a W25Q64 holding a private copy of the dump, and an ST7789 bus model. It
can also run the V2.4 image (XW compressed loads/stores and the hardware
prologue emulated from the instruction inventory; idle loops fast-forwarded).

**Firmware image vs host simulator** (`test_firmware_emulation.py`): 15
scenarios — regular view (V, mV, A, mA, Ω, F, Hz, °C, REL, HOLD), theme 2,
record view, monitor view, NCV and LIVE (both themes), Chinese, menus,
threshold editor, history browser, all settings pages, factory reset,
charging — every frame pixel-identical, UART command lists identical, no SPI
Flash erase/program. Further checks: first boot with an erased settings page,
commit and reboot; low-battery countdown and shutdown; 5-minute auto-off;
crystal failure fallback; diagnostic-entry gate.

**Firmware image vs V2.4 image** (`test_original_differential.py`): same
scenarios on both binaries, same dump, same inputs. Results are listed in
`analysis/original_differential.json` by the test run; differences found so
far were fixed in the shared C:

1. Record view printed 0.000 in the empty Min/Max boxes at power-on — V2.4
   initialises the previous-mode byte to 0xff (`reading_power_on`).
2. Factory reset went straight to the measurement screen — V2.4 re-enters the
   application loop and shows the language selection first (`settings_entry`).

**USB update image** (`test_update_image.py`, 0.2.0):

- Format: both update images pass the V2.4 header rules; reset jump to 0x65e0,
  image 0x08006400–0x080247AF (V2.4 ends at 0x080273B7).
- Hand-over: started with left-over bootloader state (SysTick and OTG_FS
  interrupts enabled and pending, an armed fast-vector slot pointing into the
  bootloader), the release image takes no stray interrupt and shows the same
  screen and commands as a clean start. The standalone image, which does not
  clean up, faults under the same conditions, which shows that the check works.
- Release vs parity twin: all 15 scenarios give identical frames and UART
  commands outside the About label; the label differs on the About page
  (`menu_reset`, frame 2) and nowhere else.
- The parity twin, executed from 0x6400 like V2.4: 16/16 scenarios identical to
  the V2.4 image (`analysis/original_differential.json`) and the 15 simulator
  scenarios plus the five further checks pass.

## Level 5 — hardware

Not started. Procedure and pass criteria: `docs/FIRMWARE_RELEASE.md`,
section *Hardware test checklist*.

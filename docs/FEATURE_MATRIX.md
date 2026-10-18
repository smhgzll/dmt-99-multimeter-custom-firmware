# DMT99 feature completeness matrix (firmware 0.2.0)

Status of every subsystem of the V2.4 application in the firmware (USB update
image; the standalone image shares all code except the start-up layout).
Columns: original routines (V2.4 link addresses), maintained source, completion,
test evidence, physical hardware verification, remaining work.

Legend — **Complete**: all original behaviour in the valid input domain is in
the firmware. **Complete (deviation)**: same observable function, documented
safety/robustness change. **Gated**: implemented, disabled by default.
**Missing**: not implemented. Test levels: L1 build/layout, L2 host
regression, L3 machine-code differential against V2.4, L4 firmware image in
the board emulator (vs. host simulator and vs. the V2.4 image).
No row is hardware-verified yet.

| Subsystem | Original routines | Source | Status | Tests | HW | Remaining |
|---|---|---|---|---|---|---|
| Reset, vectors, memory | c668, table 6438, entry 6400 | `firmware/startup.S`, `firmware/dmt99-app.ld` | Complete (same entry 0x6400, header and table 0x6438 as V2.4; deviations: hand-over cleanup, INTSYSCR=0, M-mode) | L1 layout + V2.4 header checks, L4 boot incl. left-over bootloader state | – | first install via the bootloader |
| Clock tree | d212 SystemInit, d2fc | `firmware/system.c` | Complete (deviation: HSI PLL fallback) | L3 15 RCC writes identical; L4 HSE-failure case | – | measure 144 MHz |
| PFIC / priorities | cb8e, cb94 (old-SDK encoding) | `platform/wch_nvic.c` | Complete | L3 IPRIOR/IENR identical | – | – |
| Delays | 1a626, 1a64c | `firmware/delay.c` | Complete (deviation: free-running SysTick) | L3 delay sequence; L4 | – | – |
| Debug console | 1a682, 1a6e2, printf 1aede…1b294 | `firmware/debug.c` | Complete (own formatter) | L4 console | – | wire PA2 |
| Main / start-up order | 908a | `firmware/main.c` | Complete | L3 46 config registers identical | – | – |
| Application loop | 8ed4 | `application/application.c`, `firmware/main.c` | Complete (cooperative states for blocking loops) | L2, L3 (8ed4 contract), L4 | – | – |
| Measurement UART + DMA | c350, c468, c570, 97c4 | `platform/wch_platform.c`, `firmware/isr.c` | Complete (deviation: IRQ-masked snapshot) | L3 registers; L4 frames/commands | – | capture PA9/PA10 |
| Frame decoding | be78, 98aa, 997e, 96de, 9774, b56a | `drivers/measurement_*.c` | Complete | L3 1024 parser + 256 segment vectors; L4 | – | unknown bytes 9–12 |
| Key handling | 7944, 7b08, 7c0a, 7c42, 7cd8 | `platform/wch_runtime.c`, `ui/buttons.c` | Complete | L3 GPIO/EXTI/NVIC; L2; L4 short/long presses | – | key labels |
| Measurement controls | b5c8, b9e0, bc90, b5ae, 97f8 | `application/control.c` | Complete | L3 6712 vectors; L4 packets equal | – | IC acceptance |
| Measurement events/value | a910 (partial), aed0, b29e, measurement/* | `measurement/events.c`, `value.c`, `ui/screen.c` | Complete in effect (debug-print side effect of a910 not reproduced) | L3, L4 incl. NCV/LIVE | – | – |
| Panel bus / init | 7d6a, 7dba, 7e0a…7f18, 86f6, 876a, 87aa, 8a02 | `drivers/display*.c`, `firmware/display_port.c` | Complete | L3 76 889 init words identical; L4 frames | – | write-strobe timing |
| Backlight | 7f66, 8016 | `platform/wch_runtime.c` | Complete | L3 TIM8 registers | – | polarity check |
| Graphics, fonts, bitmaps | 8050…8672, 8a5a…8e46, 1a360, ROM tables | `ui/graphics.c`, `chinese.c`, `font_rom.c`, `bitmap_images.c` | Complete | L3 bitmap/numeric differentials; L4 | – | – |
| JPEG resources | 678c, 6708, 6734, TJpgDec | `ui/resources.c`, `vendor/tjpgd` | Complete (static pool instead of malloc) | L3 296 JPEGs; L4 | – | – |
| Regular view | 9caa, 10bd0, 109e4, 1a468, 1066c, units 17f92 | `ui/reading.c`, `numeric_view.c`, `gauge.c`, `units.c` | Complete | L3; L4 (units, themes, NCV/LIVE) | – | – |
| Record view, graph, history strip | 17cce, 12c78, extrema 13504 | `ui/graph.c`, `history_view.c` | Complete (power-on cache fix found by L4) | L3; L4 | – | – |
| Monitor view, thresholds, alarm | 12b00, 12d5a, 13264, 11086, 126d8 | `ui/monitor.c`, `measurement/thresholds.c` | Complete | L3; L4 | – | buzzer level |
| Menus / settings scenes | 169d4, 164d4, 15af6, 1598c, 18ad2…1a124, 135b0, 13c02, 153d0, 15e9e, 16242 | `ui/menu*.c`, `settings_scene.c` | Complete (deviation in the release image: About label `DMT-99 V0.2.0`; parity build keeps `V2.4`) | L3 6276 menu vectors; L4 tours; release vs parity differs only inside the label box | – | – |
| Threshold editor | 11086 etc. | `measurement/thresholds.c` | Complete | L3; L4 | – | – |
| History record/browser | da9c, dbac, 14578, 14a92 | `storage/history*.c`, `ui/history_view.c` | Complete | L2; L4 | – | – |
| Language selection | 15168 (partial) | `application/application.c` | Complete in effect (cooperative, no continuous redraw) | L4 first boot vs V2.4 | – | – |
| Settings load/apply | c248 (partial), d944…da48 | `application/application.c`, `storage/settings.c` | Complete in effect | L2; L4 persistence | – | – |
| Settings persistence | d928, d87c, dc46 | `storage/persistence.c`, `firmware/internal_flash.c` | Complete (deviation: HCLK/IRQ restored) | L3 41 trace cases; L4 commit + reboot | – | real Flash programming |
| SPI NOR driver | d38a, d442…d6bc, d782, d86c | `drivers/spi_flash.c`, `firmware/storage_port.c` | Complete | L3 registers; L4 W25Q64 model | – | 18 MHz SPI on board |
| FAT resources start-up | dd4e (fdisk, mount, setlabel), ddb0, de12 | `firmware/storage_port.c`, `ui/resource_volume.c`, FatFs R0.14b | Complete (deviation: compare-before-write) | L2 filesystem; L4 zero writes at boot | – | – |
| CP936 names | fc9e, fcfc | `storage/codepage.c` | Partial by evidence: original tables absent, fails closed | L3 codepage differential | – | none possible |
| Battery / charger | 7562, 7598, 75f0, 767a, 770a | `firmware/battery.c`, `application/battery_level.c` | Complete | L3 593 ADC values; L4 | – | divider accuracy |
| Buzzer | 780c, 78ba, 78d0, 7918, 792e | `platform/wch_runtime.c` | Complete | L3 TIM2 registers; L4 | – | loudness |
| Power timer, auto-off, low battery | 7716, 7736, 7780, 14c42 | `application/power.c`, `firmware/isr.c` | Complete (deviation: work in foreground; signed level fixed) | L2; L4 auto-off, countdown | – | – |
| Power-down / latch | c320, PC1 | `application/application.c`, `firmware/main.c` | Complete | L4 latch release + commit | – | latch circuit |
| Charging screen | 14cd6 (partial) | `application/application.c` | Complete in effect | L4 vs simulator and V2.4 | – | PA8 circuit |
| Diagnostic boot loop | 8ed4 PC10+PC11 branch, b56a, 97f8 | `application/application.c` | Gated (`DMT_FW_DIAGNOSTIC_ENTRY`) | L3; L4 gate | – | meaning of A0 |
| Fault handling | default handlers c5c2…, cb8c | `firmware/fault.c` | Complete (deviation: record + reset) | L1 | – | – |
| Watchdog | none in V2.4 | – | Not in original | – | – | optional later |
| USB | none in the V2.4 application; prefix unknown | – | Missing | – | – | own USB MSC/updater (new work) |
| Bootloader / updater | 0x08000000–0x080063FF (not available) | – (restore image only) | Missing, unrecoverable | restore image boots in L4 | – | own updater |

Library code inside V2.4 that the firmware takes from upstream instead of
translating per address: FatFs internals (de5a…fd46, e086…fb48), TJpgDec
(6838…6d9c), libgcc soft-float (1b61e…1e4f8), newlib printf/malloc
(1a720…1af02, 1e566…1e81e). 64 WCH SDK peripheral routines are compiled from
the pinned SDK; NVIC_Init is the only one whose original encoding differs.

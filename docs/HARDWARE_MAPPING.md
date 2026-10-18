# Hardware interactions

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
| ADC1 | `0x00007598`, `0x0000767a` |
| AFIO | `0x0000cb5c` |
| DMA1 | `0x0000c8c4` |
| DMA1_Channel5 | `0x0000c350`, `0x0000c468` |
| DMA2 | `0x0000c8c4` |
| DMA2_EXTEN | `0x0000c8c4` |
| EXTEN | `0x0000cc4c`, `0x0000d2fc` |
| EXTI | `0x0000c8f0`, `0x0000c95a`, `0x0000c978` |
| FLASH_R | `0x0000c982`, `0x0000c99c`, `0x0000c9aa`, `0x0000c9b2`, `0x0000c9ee`, `0x0000ca26`, `0x0000ca44`, `0x0000ca52` |
| GPIOA | `0x00007562`, `0x00007780`, `0x0000780c`, `0x000078d0`, `0x0000c350`, `0x0000d38a`, `0x0000d4b4`, `0x0000d4f8`, `0x0000d516`, `0x0000d538`, `0x0000d588`, `0x0000d656`, `0x00014cd6`, `0x0001a682` |
| GPIOB | `0x00007d6a`, `0x00007dba`, `0x000087aa` |
| GPIOC | `0x0000767a`, `0x00007944`, `0x00007b08`, `0x00007c0a`, `0x00007c42`, `0x00007cd8`, `0x00007d6a`, `0x00007dba`, `0x00007f66`, `0x000087aa`, `0x00008ed4`, `0x000097f8`, `0x0000c320`, `0x0000d588`, `0x0000d656` |
| PFIC | `0x0000cb94` |
| RCC | `0x0000cc3a`, `0x0000cc4c`, `0x0000cd52`, `0x0000cd70`, `0x0000cd8e`, `0x0000d212`, `0x0000d2fc`, `0x0000d87c` |
| SPI1 | `0x0000d38a`, `0x0000d442` |
| SysTick | `0x0001a64c` |
| TIM2 | `0x0000780c`, `0x000078d0` |
| TIM3 | `0x00007736`, `0x00007780` |
| TIM7 | `0x0001a468` |
| TIM8 | `0x00007f66` |
| USART1 | `0x000097c4`, `0x0000c350`, `0x0000c468` |
| USART2 | `0x0001a682`, `0x0001a6e2` |
| unknown_mmio | `0x0000cb94` |

## Compiled SDK continuation

`reconstructed/platform/wch_runtime.c` adds optional SDK bindings for PC button EXTI, TIM3,
USART1 IDLE/DMA, display GPIO/reset and PWM on PA1/PC6. TIM2/TIM8 use the original PWM2 mode
and enabled compare preload. UART/DMA/SPI initialization remains in `wch_platform.c`.
No binding is activated by the offline executable and no hardware connection/testing occurred.
The SDK port uses the standard compiler machine-interrupt prologue with soft-float code;
QingKe interrupt timing, priority interactions and shared-state races need actual validation.

Settings persistence is a typed unbound backend at08040000. It preserves the original128-halfword
image and program-status progression; device-specific clock/CSR/unlock operations are not silently
replaced with a working programmer. NOR writes use an injected bus,256-byte pages and4096-byte
preserved-sector updates. Host storage tests use a memory NOR model with erase/WREN/page behavior.

The original resource initialization at0xdd4e also calls0xfc66/0xdf50, which builds an MBR
partition table and writes sector0. That role is now distinguished from mounting the volumes.
This original path is not bound or executed by the reconstruction.

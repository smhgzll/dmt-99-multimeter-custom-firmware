#!/usr/bin/env python3
"""Original start-up (main 0x908a up to application_loop 0x8ed4) versus the
standalone firmware image, at peripheral-register level.

Original side: tools/rv32_trace.py executes the preserved V2.4 instructions
with a register-file MMIO model. delay_ms (0x1a64c) and the FatFs/SPI Flash
probe calls inside dd4e are recorded as calls instead of executed (their
register traffic belongs to SysTick/SPI data and is covered elsewhere).

Firmware side: tools/fw_emulator.py executes dmt99-reconstructed.bin from
reset until the first application_step.

Compared:
  1. the final value of every configuration register the original writes
     (RCC enables/prescalers, GPIO CFGLR/CFGHR, AFIO EXTICR, EXTI masks and
     triggers, PFIC enables/priorities, USART1/2, DMA1 channel 5, ADC1, TIM2/3/8,
     SPI1), excluding registers whose value is time or data dependent;
  2. the exact sequence of panel bus words (D/C, value) emitted by 0x87aa,
     including reset pulses, through the same ST7789 model;
  3. the ordered list of delay_ms durations during start-up.
Differences are printed with the address; documented deliberate differences
are listed in EXPECTED and must still occur exactly as described.
"""
import struct, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from rv32_trace import TraceMachine
from fw_emulator import Ch32Emulator, Panel, HCLK

# Registers excluded from the value comparison and why.
VOLATILE = {
    0x40013804: 'USART1 DATAR', 0x40004404: 'USART2 DATAR', 0x4001300c: 'SPI1 DATAR',
    0x4002005c: 'DMA1 CH5 CNTR (re-armed by IDLE)', 0x40020064: 'DMA1 CH5 MADDR (SRAM layout)',
    0x40010c0c: 'GPIOB OUTDR (panel data)', 0x40010c10: 'GPIOB BSHR', 0x40011010: 'GPIOC BSHR',
    0x40011014: 'GPIOC BCR', 0x40010810: 'GPIOA BSHR', 0x40010814: 'GPIOA BCR',
    0x40010414: 'EXTI INTFR (w1c)', 0x40000410: 'TIM3 INTFR (w1c)', 0x40020004: 'DMA INTFCR (w1c)',
    0x40021024: 'RCC RSTSCKR (reset flags cleared by fault report)',
    0x40011014: 'GPIOC BCR',
}
# Deliberate, documented differences: address -> (original, firmware, reason).
EXPECTED = {
    # Debug banner: the firmware prints before the original would (USART2 enabled in both).
}


class ReachedLoop(Exception):
    pass


class OriginalStartup(TraceMachine):
    def __init__(self):
        super().__init__()
        self.r[2] = 0x20010000
        self.rcc_dynamic = False
        self.mmio = {0x40021000: 0x83, 0x40022010: 0x8080}
        self.writes = []
        self.delays = []
        self.calls = []
        self.panel = Panel()
        self.gpio = {'A': 0, 'B': 0, 'C': 0}
        self.hooks[0x1a64c] = lambda m, ms, *a: m.delays.append(ms) or 0
        self.hooks[0xd782] = lambda m, *a: m.calls.append(('jedec_probe',)) or 0
        self.hooks[0xfc66] = lambda m, *a: m.calls.append(('f_fdisk',)) or 0
        self.hooks[0xf4f8] = lambda m, fs, path, opt, *a: m.calls.append(('f_mount', m.string(path), opt)) or 0
        self.hooks[0xfb48] = lambda m, path, *a: m.calls.append(('f_setlabel', m.string(path))) or 0
        def loop(m, *a):
            m.calls.append(('application_loop',))
            raise ReachedLoop()
        self.hooks[0x8ed4] = loop

    def read(self, a, n):
        a &= 0xffffffff
        if 0x1ffff000 <= a < 0x20000000:
            return 0
        if 0x40000000 <= a < 0x60000000 or a >= 0xe0000000:
            if a == 0x40021000:
                v = self.mmio.get(a, 0)
                return v | (2 if v & 1 else 0) | ((1 << 17) if v & (1 << 16) else 0) | ((1 << 25) if v & (1 << 24) else 0)
            if a == 0x40021004:
                if self.rcc_dynamic:  # SystemInit: CFGR0 is a register, SWS follows SW
                    v = self.mmio.get(a, 0)
                    return (v & ~0xc) | ((v & 3) << 2)
                return 0x003d040a  # SWS=PLL, HSE x18, APB1 /2: SystemCoreClock 144 MHz
            if a == 0x40012408:
                v = self.mmio.get(a, 0) & ~0xc
                self.mmio[a] = v
                return v
            if a == 0x40012400:
                return 0x12
            if a in (0x40013800, 0x40004400):
                return 0xc0
            if a in (0x40010808, 0x40010c08, 0x40011008):
                return 0xffff  # keys released, charger absent
            return self.mmio.get(a & ~3, 0) >> (8 * (a & 3)) & ((1 << (8 * n)) - 1)
        return super().read(a, n)

    def write(self, a, n, v):
        a &= 0xffffffff
        if 0x40000000 <= a < 0x60000000 or a >= 0xe0000000:
            self.writes.append((a, v))
            gpio = gpio_write(self, a, v)
            if not gpio:
                if 0xe000e100 <= a < 0xe000e110:
                    self.mmio[a] = self.mmio.get(a, 0) | v
                else:
                    base = a & ~3
                    if n == 4:
                        self.mmio[base] = v & 0xffffffff
                    else:
                        shift, mask = 8 * (a & 3), ((1 << (8 * n)) - 1)
                        self.mmio[base] = (self.mmio.get(base, 0) & ~(mask << shift)) | ((v & mask) << shift)
            return
        super().write(a, n, v)


def gpio_write(m, a, v):
    if not (0x40010800 <= a < 0x40011400 and (a & 0x3ff) in (0x0c, 0x10, 0x14)):
        return False
    port = 'ABC'[(a - 0x40010800) // 0x400]
    reg, cur = a & 0x3ff, m.gpio[port]
    new = v if reg == 0x0c else ((cur | (v & 0xffff)) & ~(v >> 16)) if reg == 0x10 else cur & ~v
    new &= 0xffff
    old, m.gpio[port] = cur, new
    m.mmio[0x40010800 + 0x400 * 'ABC'.index(port) + 0x0c] = new
    if port == 'C':
        if old & 0x8 and not new & 0x8:
            m.panel.commands.append(('reset',))
        if not (old >> 13) & 1 and (new >> 13) & 1 and not new & 0x10:
            m.panel.word(m.gpio['B'], bool(new & 0x20))
            m.bus.append((bool(new & 0x20), m.gpio['B']))
    return True


def original_trace():
    m = OriginalStartup()
    m.bus = []
    try:
        m.call(0x908a, limit=50_000_000)
    except ReachedLoop:
        return m
    raise RuntimeError('original start-up returned without reaching 0x8ed4')


class FirmwareStartup(Ch32Emulator):
    def __init__(self):
        super().__init__(battery_raw=2435)
        self.writes = []
        self.bus = []
        self.delays = []

    def _mmio_write(self, uc, offset, size, value, data):
        a = 0x40000000 + offset
        self.writes.append((a, value))
        before = self.gpio['C']
        super()._mmio_write(uc, offset, size, value, data)
        after = self.gpio['C']
        if not (before >> 13) & 1 and (after >> 13) & 1 and not after & 0x10:
            self.bus.append((bool(after & 0x20), self.gpio['B']))

    def _core_write(self, uc, offset, size, value, data):
        a = 0xe000e000 + offset
        if not 0xe000f000 <= a < 0xe000f100:
            self.writes.append((a, value))
        super()._core_write(uc, offset, size, value, data)


def original_system_init():
    m = OriginalStartup()
    m.bus = []
    m.rcc_dynamic = True
    m.mmio[0x40021004] = 0
    m.call(0xd212, limit=1_000_000)
    return [(a, v) for a, v in m.writes if 0x40021000 <= a < 0x40021400]


def firmware_trace():
    emu = FirmwareStartup()
    target = emu.symbols['application_init']
    from unicorn import UC_HOOK_CODE
    from unicorn.riscv_const import UC_RISCV_REG_A0
    hit = []
    emu.uc.hook_add(UC_HOOK_CODE, lambda uc, addr, size, d: (hit.append(addr), uc.emu_stop()), begin=target, end=target)
    delay = emu.symbols['fw_delay_ms']
    emu.uc.hook_add(UC_HOOK_CODE, lambda uc, addr, size, d: emu.delays.append(uc.reg_read(UC_RISCV_REG_A0)), begin=delay, end=delay)
    emu.system_init_end = len(emu.writes)
    main_entry = emu.symbols['main']
    emu.uc.hook_add(UC_HOOK_CODE, lambda uc, addr, size, d: setattr(emu, 'main_index', len(emu.writes)), begin=main_entry, end=main_entry)
    emu.run(until=lambda e: bool(hit), seconds=10)
    return emu


def final_state(writes, pfic=True):
    state = {}
    for a, v in writes:
        if a in VOLATILE or (0x40010800 <= a < 0x40011400 and (a & 0x3ff) in (0x0c, 0x10, 0x14)):
            continue
        if 0xe000e100 <= a < 0xe000e110:
            state[a] = state.get(a, 0) | v
        elif 0xe000e180 <= a < 0xe000e190:
            e = a - 0x80
            state[e] = state.get(e, 0) & ~v
        elif 0xe000f000 <= a:
            continue
        else:
            state[a] = v
    return state


def main():
    original = original_trace()
    firmware = firmware_trace()
    o_state, f_state = final_state(original.writes), final_state(firmware.writes)
    problems = []
    for a in sorted(o_state):
        if a not in f_state:
            problems.append(f'register 0x{a:08x} written by original (0x{o_state[a]:x}) but not by firmware')
        elif (o_state[a] & 0xffffffff) != (f_state[a] & 0xffffffff):
            problems.append(f'register 0x{a:08x}: original 0x{o_state[a]:x} firmware 0x{f_state[a]:x}')
    extra = sorted(a for a in f_state if a not in o_state)
    print(f'configuration registers: original {len(o_state)}, firmware {len(f_state)}, firmware-only {len(extra)}')
    for a in extra:
        print(f'  firmware-only 0x{a:08x} = 0x{f_state[a]:x}')
    o_bus, f_bus = original.bus, firmware.bus[:len(original.bus)]
    bus_ok = o_bus == f_bus
    print(f'panel bus words: original {len(o_bus)}, firmware first {len(f_bus)} identical: {bus_ok}')
    if not bus_ok:
        for i, (x, y) in enumerate(zip(o_bus, f_bus)):
            if x != y:
                problems.append(f'panel word {i}: original {x} firmware {y}')
                break
    print('original calls:', original.calls)
    print('original delays:', original.delays, 'firmware delays:', firmware.delays)
    if firmware.delays != original.delays:
        problems.append(f'delay_ms sequence differs: {original.delays} vs {firmware.delays}')
    o_rcc = original_system_init()
    f_rcc = [(a, v) for a, v in firmware.writes[:firmware.main_index] if 0x40021000 <= a < 0x40021400]
    print(f'SystemInit RCC writes: original {len(o_rcc)}, firmware {len(f_rcc)}, identical: {o_rcc == f_rcc}')
    if o_rcc != f_rcc:
        problems.append(f'SystemInit RCC sequence differs: {[(hex(a), hex(v)) for a, v in o_rcc]} vs {[(hex(a), hex(v)) for a, v in f_rcc]}')
    for p in problems:
        print('DIFF', p)
    print('PASS' if not problems else 'FAIL', f'{len(problems)} differences')
    sys.exit(1 if problems else 0)


if __name__ == '__main__':
    main()

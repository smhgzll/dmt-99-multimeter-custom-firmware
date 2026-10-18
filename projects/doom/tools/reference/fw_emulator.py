#!/usr/bin/env python3
"""Execution environment for the standalone DMT99 firmware image.

Runs the real RV32IMAC machine code of build/firmware/dmt99-reconstructed.bin
in Unicorn and models the DMT99 board around it:

  * CH32V303 core glue: vectored PFIC interrupt delivery (absolute table at
    mtvec base), SysTick (64-bit, HCLK/8), WCH CSRs 0x800/0x804 accepted.
  * RCC/EXTEN clock tree with optional HSE failure, FLASH controller with the
    0x08040000 settings page (erased halfword 0xe339, fast page erase, PG).
  * GPIOA/B/C, AFIO, EXTI (falling-edge keys PC2/8/9/10/11/12), TIM2 (buzzer),
    TIM3 (1 s update interrupt), TIM8 (backlight), ADC1 (battery on ch10).
  * USART1 + DMA1 channel 5 (13-byte circular RX, IDLE interrupt) and USART2.
  * SPI1 + a W25Q64JV model backed by a private copy of the flash dump.
  * An ST7789-style panel on the 16-bit 8080 bus (PB data, PC3 reset, PC4 CS,
    PC5 D/C, PC13 WR) with CASET/RASET/RAMWR/MADCTL handling.

Virtual time advances with executed instructions (1 instruction = 1 cycle at
144 MHz) plus a small skip on every SysTick counter read so delay loops finish
quickly. This is an emulator of documented behaviour, not of the physical
analog circuit or the measurement IC; passing here is not hardware validation.
"""
import os, struct
from collections import deque
from pathlib import Path

from unicorn import (Uc, UcError, UC_ARCH_RISCV, UC_MODE_RISCV32, UC_HOOK_INTR,
                     UC_HOOK_MEM_UNMAPPED, UC_HOOK_MEM_WRITE, UC_PROT_READ, UC_PROT_EXEC, UC_PROT_ALL)
from unicorn.riscv_const import (UC_RISCV_REG_PC, UC_RISCV_REG_MSTATUS, UC_RISCV_REG_MEPC,
                                 UC_RISCV_REG_MCAUSE, UC_RISCV_REG_SP)

ROOT = Path(__file__).resolve().parents[1]
HCLK = 144_000_000
KEY_PINS = {'PC10': 10, 'PC11': 11, 'PC9': 9, 'PC12': 12, 'PC8': 8, 'PC2': 2}
IRQ_EXTI = {2: 24, 8: 39, 9: 39, 10: 56, 11: 56, 12: 56}
IRQ_TIM3, IRQ_USART1, IRQ_DMA1_CH5 = 45, 53, 31
IRQ_SYSTICK, IRQ_OTG_FS = 12, 83


def default_image():
    """Firmware image under test: $DMT_FW_IMAGE, else the standalone build."""
    return Path(os.environ.get('DMT_FW_IMAGE') or ROOT / 'reconstructed/build/firmware/dmt99-reconstructed.bin')


def image_layout(image):
    """(load address, vector base) from the image's .fw_info record
    (firmware/version.c): 0x08000000 standalone, 0x08006400 USB update image."""
    at = image.find(b'DMT99FW1')
    load = struct.unpack_from('<I', image, at + 12)[0] if at >= 0 else 0x08000000
    return (0x6400, 0x6438) if load == 0x08006400 else (0, 4)
ERASED_HALFWORD = 0xe339


class EmulationError(RuntimeError):
    pass


class Panel:
    """16-bit 8080 ST7789-style controller as driven by 0x87aa/0x7d6a."""

    def __init__(self):
        self.reset()

    def reset(self):
        self.fb = [[0] * 320 for _ in range(240)]
        self.command = None
        self.params = []
        self.xs, self.xe, self.ys, self.ye = 0, 319, 0, 239
        self.x = self.y = 0
        self.writing = False
        self.madctl = None
        self.pixfmt = None
        self.sleep_out = self.display_on = False
        self.commands = []
        self.pixels = 0
        self.out_of_range = 0

    def word(self, value, data):
        if not data:
            self.command, self.params, self.writing = value & 0xff, [], False
            self.commands.append(self.command)
            if self.command == 0x11:
                self.sleep_out = True
            elif self.command == 0x29:
                self.display_on = True
            elif self.command == 0x2c:
                self.writing, self.x, self.y = True, self.xs, self.ys
            elif self.command == 0x01:
                self.reset()
            return
        if self.writing:
            if 0 <= self.x < 320 and 0 <= self.y < 240:
                self.fb[self.y][self.x] = value & 0xffff
            else:
                self.out_of_range += 1
            self.pixels += 1
            self.x += 1
            if self.x > self.xe:
                self.x = self.xs
                self.y += 1
                if self.y > self.ye:
                    self.y = self.ys
            return
        self.params.append(value & 0xff)
        n, p = len(self.params), self.params
        if self.command in (0x2a, 0x2b):
            # Parameters latch as they arrive; a 2-byte write moves only the start.
            if n == 2:
                start = p[0] << 8 | p[1]
                if self.command == 0x2a:
                    self.xs = start
                else:
                    self.ys = start
            elif n == 4:
                end = p[2] << 8 | p[3]
                if self.command == 0x2a:
                    self.xe = end
                else:
                    self.ye = end
        elif self.command == 0x36 and n == 1:
            self.madctl = p[0]
        elif self.command == 0x3a and n == 1:
            self.pixfmt = p[0]

    def image(self):
        from PIL import Image
        img = Image.new('RGB', (320, 240))
        px = img.load()
        for y in range(240):
            row = self.fb[y]
            for x in range(320):
                c = row[x]
                px[x, y] = ((c >> 11) << 3 | (c >> 13), ((c >> 5) & 63) << 2 | ((c >> 9) & 3), (c & 31) << 3 | ((c >> 2) & 7))
        return img

    def save(self, path):
        self.image().save(path)

    def checksum(self):
        import hashlib
        h = hashlib.sha256()
        for row in self.fb:
            h.update(struct.pack('<320H', *row))
        return h.hexdigest()


class W25Q64:
    def __init__(self, image):
        self.data = bytearray(image)
        self.wel = False
        self.cmd = []
        self.erases, self.programs = [], []
        self.busy_reads = 0

    def select(self):
        self.cmd = []

    def deselect(self):
        c = self.cmd
        if not c:
            return
        if c[0] == 0x06:
            self.wel = True
        elif c[0] == 0x04:
            self.wel = False
        elif c[0] == 0x20 and len(c) >= 4 and self.wel:
            a = (c[1] << 16 | c[2] << 8 | c[3]) & ~0xfff
            self.data[a:a + 4096] = b'\xff' * 4096
            self.erases.append(a)
            self.wel = False
        elif c[0] == 0x02 and len(c) >= 4 and self.wel:
            a = c[1] << 16 | c[2] << 8 | c[3]
            payload = c[4:4 + 256]
            for i, b in enumerate(payload):
                at = (a & ~0xff) | ((a + i) & 0xff)
                self.data[at] &= b
            self.programs.append((a, len(payload)))
            self.wel = False
        self.cmd = []

    def exchange(self, value):
        c = self.cmd
        c.append(value)
        op = c[0]
        if op == 0x9f:
            return [0, 0xef, 0x40, 0x17][min(len(c) - 1, 3)]
        if op == 0x05:
            return 0 if len(c) > 1 else 0xff
        if op == 0x03 and len(c) > 4:
            a = (c[1] << 16 | c[2] << 8 | c[3]) + len(c) - 5
            return self.data[a & 0x7fffff]
        return 0xff


class Ch32Emulator:
    def __init__(self, bin_path=None, flash_dump=None, settings=None, hse_ok=True, battery_raw=2560,
                 option_user=0xbf, tim3_warp=1, original=False, bootloader_leftovers=False, last_page=None):
        """original=True runs the manufacturer V2.4 application instead: loaded
        at 0x6400 behind the absent 25 KiB prefix, vectors at 0x6438, 64 KiB SRAM,
        WCH XW compressed loads/stores and the hardware prologue (HPE) emulated
        from the preserved instruction inventory.

        bootloader_leftovers=True starts the image the way a careless bootloader
        might hand over: global interrupts masked (MIE clear) but SysTick running
        with its interrupt enabled in the core and the PFIC, an OTG_FS interrupt
        enabled and pending, and a fast-vector (VTF) slot for it armed with an
        address inside the bootloader. An image that does not clean this up takes
        one of these interrupts as soon as it unmasks interrupts."""
        self.original = original
        if original:
            bin_path = Path(bin_path or ROOT / 'DMT-99_V2.4_20240306.bin')
            self.load_address, self.vector_base, self.ram_size = 0x6400, 0x6438, 0x10000
            if bin_path.stat().st_size == 0x6400 + 135096:   # restore image: j 0x6400 at 0x0
                self.load_address = 0
        else:
            bin_path = Path(bin_path or default_image())
        self.bin_path = bin_path
        self.symbol_sizes = {}
        self.image = bin_path.read_bytes()
        if not original:
            self.load_address, self.vector_base = image_layout(self.image)
            self.ram_size = 0x8000
        self.bootloader_leftovers = bootloader_leftovers
        dump = Path(flash_dump or ROOT / 'flashdump/dmt99_flash_1.bin').read_bytes()
        self.nor = W25Q64(dump)
        self.panel = Panel()
        self.settings = bytearray(struct.pack('<128H', *([ERASED_HALFWORD] * 128)))
        if settings is not None:
            self.settings[:] = settings
        # 0x08040100: the page after the settings page (unused by V2.4; the
        # release firmware keeps its last-mode record there). Erased unless given.
        self.last_page = bytearray(struct.pack('<128H', *([ERASED_HALFWORD] * 128)))
        if last_page is not None:
            self.last_page[:] = last_page
        self.pages = {0x08040000: self.settings, 0x08040100: self.last_page}
        self.hse_ok, self.battery_raw, self.option_user = hse_ok, battery_raw, option_user
        # TIM3 is the firmware's only notion of seconds (auto-off, battery and
        # low-battery checks). tim3_warp > 1 shortens its period for long
        # scenarios; SysTick delays keep real virtual time.
        self.tim3_warp = tim3_warp
        self.tim3_updates = 0
        self.cycles = 0
        self.st = {'start': 0, 'frozen': 0, 'init': 0, 'ack': False}
        self.regs = {}
        self.uart_tx, self.console = bytearray(), bytearray()
        self.tx_packets = []
        self.flash_ops = []
        self.events = []
        self.pressed = set()
        self.charging = False
        self.reset_requested = False
        self.halted = False
        self.fault = None
        self.rx_queue = deque()
        self.irq_log = []
        self.dma5_count = 13
        self.flash_keys = []
        self.mode_keys = []
        self.gpio = {'A': 0, 'B': 0xffff, 'C': 0}
        self._build()

    # ---------------------------------------------------------------- setup
    def _build(self):
        uc = self.uc = Uc(UC_ARCH_RISCV, UC_MODE_RISCV32)
        # RV32IMAC without F/D: the WCH XW compressed loads/stores reuse the FP
        # compressed encodings, so on this model they trap instead of silently
        # executing as FP accesses (see _on_trap).
        from unicorn.riscv_const import UC_CPU_RISCV32_SIFIVE_E31
        uc.ctl_set_cpu_model(UC_CPU_RISCV32_SIFIVE_E31)
        uc.mem_map(0, 0x40000, UC_PROT_READ | UC_PROT_EXEC)
        uc.mem_write(self.load_address, self.image)
        uc.mem_map(0x20000000, self.ram_size, UC_PROT_ALL)
        uc.mmio_map(0x08000000, 0x80000, self._flash_read, None, self._flash_write, None)
        uc.mmio_map(0x40000000, 0x30000, self._mmio_read, None, self._mmio_write, None)
        uc.mmio_map(0xe000e000, 0x2000, self._core_read, None, self._core_write, None)
        # System/information block: 0x1FFFF70C bit 14 clear = CH32V303 (not V307),
        # 0x1FFFF7E0 flash size in KiB.
        info = bytearray(0x1000)
        struct.pack_into('<I', info, 0x70c, 0x00000000)
        struct.pack_into('<H', info, 0x7e0, 256)
        uc.mem_map(0x1ffff000, 0x1000, UC_PROT_READ)
        uc.mem_write(0x1ffff000, bytes(info))
        uc.hook_add(UC_HOOK_INTR, self._on_trap)
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, self._on_unmapped)
        self.vector = struct.unpack_from('<104I', self.image, self.vector_base - self.load_address)
        self.pc = self.load_address
        self.hpe_stack = []
        self.loop_entered = False
        if self.original:
            self._install_original_hooks()
        self.regs.update({0x40021000: 0x83, 0x40021004: 0, 0x40022010: 0x8080, 0x40021024: 0x0c000000})
        if self.bootloader_leftovers:
            uc.reg_write(UC_RISCV_REG_MSTATUS, 0x1880)
            self.regs[0xe000e100] = 1 << IRQ_SYSTICK
            self.regs[0xe000e108] = 1 << (IRQ_OTG_FS - 64)
            self.regs['swpend'] = 1 << IRQ_OTG_FS
            self.regs[0xe000e050], self.regs[0xe000e060] = IRQ_OTG_FS, 0x00000101
            self.regs[0xe000f010] = 1000
            self.regs[0xe000f000] = 0x3
        self.symbols = self._load_symbols()

    def _load_symbols(self):
        """Read the ELF next to the BIN (same basename) for symbol addresses."""
        elf = self.bin_path.with_suffix('.elf')
        if not elf.exists():
            return {}
        data = elf.read_bytes()
        shoff, shentsize, shnum = struct.unpack_from('<I', data, 0x20)[0], *struct.unpack_from('<HH', data, 0x2e)
        sections = [struct.unpack_from('<IIIIIIIIII', data, shoff + i * shentsize) for i in range(shnum)]
        symbols = {}
        for sh in sections:
            if sh[1] == 2:  # SHT_SYMTAB
                strtab = sections[sh[6]]
                for off in range(sh[4], sh[4] + sh[5], 16):
                    name, value, size, info, other, shndx = struct.unpack_from('<IIIBBH', data, off)
                    end = data.index(b'\0', strtab[4] + name)
                    symbols[data[strtab[4] + name:end].decode()] = value
                    self.symbol_sizes[data[strtab[4] + name:end].decode()] = size
        return symbols

    def read(self, name, size=None, offset=0):
        address = self.symbols[name] + offset
        return bytes(self.uc.mem_read(address, size or self.symbol_sizes[name]))

    # --------------------------------------------------------------- faults
    def _on_unmapped(self, uc, access, address, size, value, data):
        self.fault = f'unmapped access 0x{address:08x} size {size} at pc 0x{uc.reg_read(UC_RISCV_REG_PC):08x}'
        return False

    def stack_unused(self):
        """Bytes of the painted stack (0x5354414b) never overwritten."""
        bottom = self.symbols.get('_susrstack', 0x20006800)
        top = self.symbols.get('_eusrstack', 0x20008000)
        data = bytes(self.uc.mem_read(bottom, top - bottom))
        n = 0
        while n + 4 <= len(data) and data[n:n + 4] == b'KATS':
            n += 4
        return n

    def _on_trap(self, uc, intno, data):
        # Unicorn reports the address after an illegal instruction as pc+4,
        # for 16-bit and 32-bit encodings alike.
        reported = uc.reg_read(UC_RISCV_REG_PC)
        if intno == 2:
            for pc in (reported - 4, reported - 2, reported):
                if self.original and pc in self.xw:
                    self._xw(pc)
                    uc.reg_write(UC_RISCV_REG_PC, pc + 2)
                    return
                raw = struct.unpack('<I', bytes(uc.mem_read(pc, 4)))[0]
                if raw & 0x7f == 0x73 and (raw >> 12) & 3 and (raw >> 20) in (0x800, 0x804, 0xbc0):
                    uc.reg_write(UC_RISCV_REG_PC, pc + 4)
                    return
        self.fault = f'trap {intno} reported at pc 0x{reported:08x}'
        uc.emu_stop()

    # ----------------------------------------------- original image support
    def _install_original_hooks(self):
        import json, re
        from unicorn import UC_HOOK_CODE
        from unicorn import riscv_const as rc
        names = 'zero ra sp gp tp t0 t1 t2 s0 s1 a0 a1 a2 a3 a4 a5 a6 a7 s2 s3 s4 s5 s6 s7 s8 s9 s10 s11 t3 t4 t5 t6'.split()
        self.reg_id = {n: getattr(rc, f'UC_RISCV_REG_X{i}') for i, n in enumerate(names)}
        self.xw = {}
        mrets = []
        for row in json.loads((ROOT / 'analysis/instructions.json').read_text()):
            text, address = row['text'], int(row['address'], 16)
            if text.startswith('qk.'):
                m = re.fullmatch(r'qk\.c\.(\w+) (\w+),(0x[0-9a-f]+)\((\w+)\)', text)
                op = m[1].removesuffix('sp')
                self.xw[address] = (op, m[2], int(m[3], 16), m[4])
            elif text == 'mret':
                mrets.append(address)

        # Unicorn decodes the XW encodings as FP/reserved compressed forms and
        # does not trap reliably, so the loaded copy carries the defined-illegal
        # halfword 0x0000 at each XW site; _on_trap emulates the original
        # operation from the inventory. Data reads use self.image (unpatched).
        for address in self.xw:
            self.uc.mem_write(address, b'\0\0')
        self.uc.hook_add(UC_HOOK_CODE, self._loop_head, begin=0x8f9c, end=0x8f9c)
        for address in mrets:
            if address == 0xc6f2:     # handle_reset: enter main in M-mode, not WCH's U-mode
                self.uc.hook_add(UC_HOOK_CODE, self._reset_mret, begin=address, end=address)
            elif address != 0xcb88:   # NMI_Handler has no prologue use
                self.uc.hook_add(UC_HOOK_CODE, self._hpe_pop, begin=address, end=address)

    def _xw_hook(self, uc, address, size, data):
        if address in self.xw:
            self._xw(address)
            uc.reg_write(UC_RISCV_REG_PC, address + 2)

    def _bus_read(self, a, n):
        if 0x40000000 <= a < 0x40030000:
            return self._mmio_read(self.uc, a - 0x40000000, n, None)
        if 0xe000e000 <= a < 0xe0010000:
            return self._core_read(self.uc, a - 0xe000e000, n, None)
        if 0x08000000 <= a < 0x08080000:
            return self._flash_read(self.uc, a - 0x08000000, n, None)
        return int.from_bytes(bytes(self.uc.mem_read(a, n)), 'little')

    def _bus_write(self, a, n, v):
        v &= (1 << (8 * n)) - 1
        if 0x40000000 <= a < 0x40030000:
            return self._mmio_write(self.uc, a - 0x40000000, n, v, None)
        if 0xe000e000 <= a < 0xe0010000:
            return self._core_write(self.uc, a - 0xe000e000, n, v, None)
        if 0x08000000 <= a < 0x08080000:
            return self._flash_write(self.uc, a - 0x08000000, n, v, None)
        self.uc.mem_write(a, v.to_bytes(n, 'little'))

    def _xw(self, address):
        op, reg, offset, base = self.xw[address]
        uc = self.uc
        a = (uc.reg_read(self.reg_id[base]) + offset) & 0xffffffff
        if op in ('lbu', 'lhu'):
            uc.reg_write(self.reg_id[reg], self._bus_read(a, 1 if op == 'lbu' else 2))
        else:
            self._bus_write(a, 1 if op == 'sb' else 2, uc.reg_read(self.reg_id[reg]))

    HPE_REGS = ('ra', 't0', 't1', 't2', 'a0', 'a1', 'a2', 'a3', 'a4', 'a5', 'a6', 'a7', 't3', 't4', 't5', 't6')

    def _hpe_pop(self, uc, address, size, data):
        if self.hpe_stack:
            for name, value in zip(self.HPE_REGS, self.hpe_stack.pop()):
                uc.reg_write(self.reg_id[name], value)

    def _reset_mret(self, uc, address, size, data):
        uc.reg_write(UC_RISCV_REG_MSTATUS, uc.reg_read(UC_RISCV_REG_MSTATUS) | 0x1800)

    # --------------------------------------------------------- flash/option
    def _flash_read(self, uc, offset, size, data):
        a = 0x08000000 + offset
        if 0x08040000 <= a < 0x08040200:
            page = self.pages[a & ~0xff]
            o = a & 0xff
            return int.from_bytes(page[o:o + size], 'little')
        if offset + size <= len(self.image):
            return int.from_bytes(self.image[offset:offset + size], 'little')
        return int.from_bytes(struct.pack('<H', ERASED_HALFWORD) * 2, 'little') & ((1 << (8 * size)) - 1)

    def _flash_write(self, uc, offset, size, value, data):
        a = 0x08000000 + offset
        ctlr = self.regs.get(0x40022010, 0x8080)
        if not (ctlr & 1) or ctlr & 0x80 or size != 2:
            self.fault = f'flash store 0x{a:08x} without PG/unlock (CTLR=0x{ctlr:x})'
            uc.emu_stop()
            return
        if not 0x08040000 <= a < 0x08040200:
            self.fault = f'flash program outside the settings/last-mode pages: 0x{a:08x}'
            uc.emu_stop()
            return
        page, o = self.pages[a & ~0xff], a & 0xff
        if struct.unpack_from('<H', page, o)[0] != ERASED_HALFWORD:
            self.flash_ops.append(('program-not-erased', a))
        struct.pack_into('<H', page, o, value & 0xffff)
        self.flash_ops.append(('program', a, value & 0xffff))
        self.regs[0x4002200c] = self.regs.get(0x4002200c, 0) | 0x20

    # ----------------------------------------------------------------- core
    # SysTick (0xE000F000): CTLR bit0 STE, bit2 STCLK (1=HCLK, 0=HCLK/8),
    # bit4 MODE (1=down), bit5 INIT (load 0 or CMP), SR bit0 CNTIF.
    def _st_div(self):
        return 1 if self.regs.get(0xe000f000, 0) & 4 else 8

    def _st_cmp(self):
        return self.regs.get(0xe000f010, 0) | self.regs.get(0xe000f014, 0) << 32

    def _st_elapsed(self):
        st = self.st
        if not self.regs.get(0xe000f000, 0) & 1:
            return st['frozen']
        return st['frozen'] + (self.cycles - st['start']) // self._st_div()

    def systick_count(self):
        down = self.regs.get(0xe000f000, 0) & 0x10
        e = self._st_elapsed()
        return max(self.st['init'] - e, 0) if down else (self.st['init'] + e) & ((1 << 64) - 1)

    def _st_reached(self):
        if self.st['ack']:
            return False
        down = self.regs.get(0xe000f000, 0) & 0x10
        return self._st_elapsed() >= self.st['init'] if down else self.systick_count() >= self._st_cmp()

    def _core_read(self, uc, offset, size, data):
        a = 0xe000e000 + offset
        if a in (0xe000f008, 0xe000f00c):
            self.cycles += 400
            cnt = self.systick_count()
            return (cnt >> (32 if a == 0xe000f00c else 0)) & 0xffffffff
        if a == 0xe000f004:
            self.cycles += 400
            return self.regs.get(a, 0) | (1 if self._st_reached() else 0)
        return self.regs.get(a, 0)

    def _core_write(self, uc, offset, size, value, data):
        a = 0xe000e000 + offset
        if a == 0xe000e048 and (value & 0xffff0000) == 0xbeef0000 and value & 0x80:
            self.reset_requested = True
            uc.emu_stop()
        if 0xe000e100 <= a < 0xe000e110:
            self.regs[a] = self.regs.get(a, 0) | value
            return
        if 0xe000e180 <= a < 0xe000e190:
            e = 0xe000e100 + (a - 0xe000e180)
            self.regs[e] = self.regs.get(e, 0) & ~value
            return
        if 0xe000e200 <= a < 0xe000e210 or 0xe000e280 <= a < 0xe000e290:   # IPSR / IPRR
            bits = value << (32 * ((a & 0xf) >> 2))
            pend = self.regs.get('swpend', 0)
            self.regs['swpend'] = pend | bits if a < 0xe000e280 else pend & ~bits
            return
        st = self.st
        if a == 0xe000f000:
            old = self.regs.get(a, 0)
            if old & 1 and not value & 1:
                st['frozen'] = self._st_elapsed()
            elif not old & 1 and value & 1:
                st['start'] = self.cycles
            self.regs[a] = value & ~0x20
            if value & 0x20:
                st.update(start=self.cycles, frozen=0, ack=False,
                          init=self._st_cmp() if value & 0x10 else 0)
            return
        if a == 0xe000f004:
            self.regs[a] = value & ~1
            if not value & 1:
                st['ack'] = self._st_reached() or st['ack']
            return
        if a in (0xe000f008, 0xe000f00c):
            cur = self.systick_count()
            new = (cur & ~0xffffffff) | value if a == 0xe000f008 else (cur & 0xffffffff) | value << 32
            st.update(start=self.cycles, frozen=0, init=new, ack=False)
            return
        if a in (0xe000f010, 0xe000f014):
            self.regs[a] = value
            st['ack'] = False
            return
        self.regs[a] = value

    def irq_enabled(self, irq):
        return bool(self.regs.get(0xe000e100 + 4 * (irq // 32), 0) >> (irq % 32) & 1)

    # ------------------------------------------------------------ peripherals
    def _mmio_read(self, uc, offset, size, data):
        a = 0x40000000 + offset
        base = a & ~0x3
        if a == 0x40021000:  # RCC CTLR ready flags
            v = self.regs.get(a, 0)
            v |= 2 if v & 1 else 0
            if v & (1 << 16) and self.hse_ok:
                v |= 1 << 17
            if v & (1 << 24):
                v |= 1 << 25
            return v
        if a == 0x40021004:
            v = self.regs.get(a, 0)
            return (v & ~0xc) | ((v & 3) << 2)
        if a == 0x4002201c:  # FLASH OBR: RDPRT clear, USER byte
            return self.option_user << 2
        if a == 0x4002200c:
            return self.regs.get(a, 0) & ~1
        if a == 0x40012400:  # ADC STATR: EOC
            return 0x12
        if a == 0x40012408:
            v = self.regs.get(a, 0) & ~0xc
            self.regs[a] = v
            return v
        if a == 0x4001244c:
            return self.battery_raw & 0xfff
        if a in (0x40010808, 0x40010c08, 0x40011008):
            return self.gpio_input('ABC'[(a - 0x40010808) // 0x400])
        if a in (0x4001080c, 0x40010c0c, 0x4001100c):
            return self.gpio['ABC'[(a - 0x4001080c) // 0x400]]
        if a == 0x40013800:  # USART1 STATR
            return self.regs.get('u1_stat', 0xc0)
        if a == 0x40013804:
            self.regs['u1_stat'] = self.regs.get('u1_stat', 0xc0) & ~0x30
            return 0
        if a == 0x40004400:
            return 0xc0
        if a == 0x40013008:  # SPI1 STATR: TXE|RXNE
            return 0x3
        if a == 0x4001300c:
            return self.regs.get('spi_rx', 0xff)
        if a == 0x4002005c:  # DMA1 CH5 CNTR
            return self.dma5_count
        if a == 0x40010414:
            return self.regs.get(a, 0)
        if a == 0x40000410:
            return self.regs.get(a, 0)
        return self.regs.get(base, 0)

    def gpio_input(self, port):
        out = self.gpio[port]
        if port == 'C':
            v = out | 0x1f04
            for name in self.pressed:
                v &= ~(1 << KEY_PINS[name])
            return v & 0xffff
        if port == 'A':
            return (out & ~0x100) | (0x100 if self.charging else 0)
        return out

    def _gpio_write(self, port, new):
        old = self.gpio[port]
        self.gpio[port] = new & 0xffff
        if port == 'C':
            if old & 0x8 and not new & 0x8:
                self.panel.reset()
            if not (old >> 13) & 1 and (new >> 13) & 1 and not new & 0x10:
                self.panel.word(self.gpio['B'], bool(new & 0x20))
        elif port == 'A':
            if old & 0x10 and not new & 0x10:
                self.nor.select()
            elif not old & 0x10 and new & 0x10:
                self.nor.deselect()

    def _mmio_write(self, uc, offset, size, value, data):
        a = 0x40000000 + offset
        if 0x40010800 <= a < 0x40011400 and (a & 0x3ff) in (0x0c, 0x10, 0x14):
            port = 'ABC'[(a - 0x40010800) // 0x400]
            reg = a & 0x3ff
            cur = self.gpio[port]
            if reg == 0x0c:
                new = value
            elif reg == 0x10:
                new = (cur | (value & 0xffff)) & ~(value >> 16)
            else:
                new = cur & ~value
            self._gpio_write(port, new)
            return
        if a == 0x40013804:  # USART1 TX
            self.uart_tx.append(value & 0xff)
            if len(self.uart_tx) >= 10:
                self.tx_packets.append(bytes(self.uart_tx[:10]))
                del self.uart_tx[:10]
            return
        if a == 0x40004404:
            self.console.append(value & 0xff)
            return
        if a == 0x4001300c:
            self.regs['spi_rx'] = self.nor.exchange(value & 0xff) if not self.gpio['A'] & 0x10 else 0xff
            return
        if a == 0x40010414:  # EXTI INTFR write-1-to-clear
            self.regs[a] = self.regs.get(a, 0) & ~value
            return
        if a == 0x40000410:  # TIM3 INTFR
            self.regs[a] = self.regs.get(a, 0) & value
            return
        if a == 0x40020004:  # DMA INTFCR
            self.regs[0x40020000] = self.regs.get(0x40020000, 0) & ~value
            return
        if a == 0x4002005c:
            self.dma5_count = value & 0xffff
            return
        if a == 0x40022004:
            self.flash_keys.append(value)
            if self.flash_keys[-2:] == [0x45670123, 0xcdef89ab]:
                self.regs[0x40022010] = self.regs.get(0x40022010, 0x8080) & ~0x80
            return
        if a == 0x40022024:
            self.mode_keys.append(value)
            if self.mode_keys[-2:] == [0x45670123, 0xcdef89ab]:
                self.regs[0x40022010] = self.regs.get(0x40022010, 0x8080) & ~0x8000
            return
        if a == 0x40022010:
            if value & 0x40 and value & 0x20000:
                if self.regs.get(0x40022010, 0x8080) & 0x8080:
                    self.fault = 'fast page erase while locked'
                else:
                    page = self.regs.get(0x40022014, 0) & ~0xff
                    self.flash_ops.append(('erase', page))
                    if page in self.pages:
                        self.pages[page][:] = struct.pack('<H', ERASED_HALFWORD) * 128
                    else:
                        self.fault = f'erase outside the settings/last-mode pages: 0x{page:08x}'
                self.regs[0x4002200c] = self.regs.get(0x4002200c, 0) | 0x20
                value &= ~0x40
            self.regs[a] = value
            return
        if a == 0x4002200c:
            self.regs[a] = self.regs.get(a, 0) & ~value
            return
        if a == 0x40021024:
            if value & (1 << 24):
                self.regs[a] = 0
            return
        self.regs[a & ~3] = value

    # ------------------------------------------------------------ interrupts
    def pending_irqs(self):
        out = []
        intfr, inten = self.regs.get(0x40010414, 0), self.regs.get(0x40010400, 0)
        for line, irq in IRQ_EXTI.items():
            if intfr & inten & (1 << line):
                out.append(irq)
        if self.regs.get(0x40000410, 0) & 1 and self.regs.get(0x4000040c, 0) & 1:
            out.append(IRQ_TIM3)
        ctl1 = self.regs.get(0x4001380c, 0)
        if self.regs.get('u1_stat', 0xc0) & 0x10 and ctl1 & 0x10:
            out.append(IRQ_USART1)
        if self.regs.get(0x40020000, 0) & (1 << 17) and self.regs.get(0x40020058, 0) & 2:
            out.append(IRQ_DMA1_CH5)
        if self.regs.get(0xe000f000, 0) & 3 == 3 and self._st_reached():
            out.append(IRQ_SYSTICK)
        out += [i for i in range(128) if self.regs.get('swpend', 0) >> i & 1]
        return sorted(set(i for i in out if self.irq_enabled(i)), key=lambda i: (self.regs.get(('prio', i), 0), i))

    def deliver(self):
        uc = self.uc
        mstatus = uc.reg_read(UC_RISCV_REG_MSTATUS)
        if not mstatus & 8:
            return False
        pending = self.pending_irqs()
        if not pending:
            return False
        irq = pending[0]
        target = self.vector[irq]
        for slot in range(4):   # armed VTF slot: PFIC jumps straight to its address
            vtf = self.regs.get(0xe000e060 + 4 * slot, 0)
            if vtf & 1 and self.regs.get(0xe000e050, 0) >> (8 * slot) & 0xff == irq:
                target = vtf & ~1
        if self.original:
            self.hpe_stack.append(tuple(uc.reg_read(self.reg_id[n]) for n in self.HPE_REGS))
        uc.reg_write(UC_RISCV_REG_MEPC, self.pc)
        uc.reg_write(UC_RISCV_REG_MCAUSE, 0x80000000 | irq)
        uc.reg_write(UC_RISCV_REG_MSTATUS, (mstatus & ~0x88) | 0x80 | 0x1800)
        self.pc = target
        self.irq_log.append((self.cycles, irq))
        return True

    # -------------------------------------------------------------- stimuli
    def _schedule(self, delay_s, action):
        self.events.append((self.cycles + int(delay_s * HCLK), action))
        self.events.sort(key=lambda e: e[0])

    def _apply_events(self):
        while self.events and self.events[0][0] <= self.cycles:
            self.events.pop(0)[1]()
        self._feed_uart()

    def press(self, key, hold=0.15):
        """Press a key (PC2/8/9/10/11/12) for hold seconds of virtual time."""
        pin = KEY_PINS[key]
        def down():
            self.pressed.add(key)
            if self.regs.get(0x4001040c, 0) & (1 << pin):
                self.regs[0x40010414] = self.regs.get(0x40010414, 0) | (1 << pin)
        def up():
            self.pressed.discard(key)
        down()
        self._schedule(hold, up)

    def frame(self, data):
        assert len(data) == 13
        self.rx_queue.append(bytes(data))

    def _feed_uart(self):
        if not self.rx_queue or self.regs.get('u1_stat', 0xc0) & 0x10:
            return
        ch = self.regs.get(0x40020058, 0)
        if not ch & 1:
            return
        maddr = self.regs.get(0x40020064, 0)
        for b in self.rx_queue.popleft():
            idx = 13 - self.dma5_count
            if 0x20000000 <= maddr + idx < 0x20008000:
                self.uc.mem_write(maddr + idx, bytes([b]))
            self.dma5_count -= 1
            if self.dma5_count == 0:
                self.dma5_count = 13
                self.regs[0x40020000] = self.regs.get(0x40020000, 0) | (1 << 17)
        self.regs['u1_stat'] = self.regs.get('u1_stat', 0xc0) | 0x10

    # ------------------------------------------------------------------ run
    def run(self, seconds=None, until=None, slice_instr=4000, max_instr=None):
        end = self.cycles + int(seconds * HCLK) if seconds is not None else None
        if max_instr is None:
            max_instr = int((seconds or 10) * HCLK * 2)
        executed = 0
        while not self.fault and not self.reset_requested:
            if until and until(self):
                return True
            if end is not None and self.cycles >= end:
                return True
            if self.halted:
                return False
            self._apply_events()
            self._tick_timers()
            self.deliver()
            self._skip = None
            start = self.pc
            try:
                self.uc.emu_start(start, 0xffffffff, count=slice_instr)
            except UcError as e:
                if not self.fault:
                    self.fault = f'{e} at pc 0x{self.uc.reg_read(UC_RISCV_REG_PC):08x}'
            self.pc = self._skip if self._skip is not None else self.uc.reg_read(UC_RISCV_REG_PC)
            self.cycles += slice_instr
            if not self.uc.reg_read(UC_RISCV_REG_MSTATUS) & 8:
                if struct.unpack('<I', bytes(self.uc.mem_read(self.pc, 4)))[0] == 0x10500073 or \
                        struct.unpack('<I', bytes(self.uc.mem_read(self.pc - 4, 4)))[0] == 0x10500073:
                    self.halted = True
            executed += slice_instr
            if self.original and self._original_idle():
                self._fast_forward(end)
            if executed > max_instr:
                raise EmulationError('instruction budget exhausted')
        if self.fault:
            raise EmulationError(self.fault)
        return False

    # The V2.4 idle loops (application loop 8ed4 with its per-view key
    # handlers, menu loop 169d4, charging loop 14cd6) poll RAM flags that only
    # interrupts change, and nothing in them reads a timer. While such a loop
    # has nothing to do, virtual time jumps to the next timer or stimulus
    # instead of emulating millions of XW-trapping iterations.
    LOOP_CODE = ((0x8ed4, 0x908a), (0xb5c8, 0xbe18), (0x169d4, 0x17ab2), (0x164d4, 0x169d4), (0x1598c, 0x15e9e))
    CHARGE_CODE = (0x14cd6, 0x14d60)

    def _ram8(self, address):
        return self.uc.mem_read(address, 1)[0]

    def _loop_head(self, uc, address, size, data):
        self.loop_entered = True

    def _original_idle(self):
        if not self.loop_entered or self.rx_queue or self.pending_irqs():
            return False
        if not self.uc.reg_read(UC_RISCV_REG_MSTATUS) & 8:
            return False
        pc, ra = self.pc, self.uc.reg_read(self.reg_id['ra'])
        inside = lambda a, r: r[0] <= a < r[1]
        charge = inside(pc, self.CHARGE_CODE) or (0xcb40 <= pc < 0xcb4a and inside(ra, self.CHARGE_CODE))
        if charge:
            return True
        if not any(inside(pc, r) for r in self.LOOP_CODE):
            return False
        event, suppressed, battery_due = self._ram8(0x200001ee), self._ram8(0x200001c4), self._ram8(0x200001ec)
        return event == 0 and suppressed != 0 and battery_due != 1 and not self.charging

    def _fast_forward(self, end):
        targets = [t for t, _ in self.events]
        if self.regs.get('tim3_due') is not None and self.regs.get(0x40000400, 0) & 1:
            targets.append(self.regs['tim3_due'])
        if end is not None:
            targets.append(end)
        if targets:
            self.cycles = max(self.cycles, min(targets))

    def _tick_timers(self):
        tim3 = self.regs.get(0x40000400, 0)
        if tim3 & 1:
            psc = self.regs.get(0x40000428, 0) + 1
            arr = self.regs.get(0x4000042c, 0) + 1
            period = max(1, psc * arr // self.tim3_warp)
            due = self.regs.get('tim3_due')
            if due is None:
                self.regs['tim3_due'] = self.cycles + period
            elif self.cycles >= due:
                self.regs[0x40000410] = self.regs.get(0x40000410, 0) | 1
                self.regs['tim3_due'] = due + period
                self.tim3_updates += 1

    def run_ticks(self, ticks, limit_seconds=60):
        """Run until TIM3 has produced `ticks` more update events."""
        target = self.tim3_updates + ticks
        return self.run(seconds=limit_seconds, until=lambda e: e.tim3_updates >= target or e.halted)

    # --------------------------------------------------------------- probes
    def text(self):
        return self.console.decode('latin1')

    def backlight_compare(self):
        return self.regs.get(0x40013434)

    def buzzer_on(self):
        return bool(self.regs.get(0x40000020, 0) & 0x10) and self.regs.get(0x40000038, 0) != 0


def main():
    import argparse
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--bin')
    p.add_argument('--seconds', type=float, default=4.0)
    p.add_argument('--png', default=None)
    args = p.parse_args()
    emu = Ch32Emulator(args.bin)
    emu.run(seconds=args.seconds)
    print(emu.text())
    print('panel commands', len(emu.panel.commands), 'pixels', emu.panel.pixels, 'tx', [x.hex() for x in emu.tx_packets])
    if args.png:
        emu.panel.save(args.png)


if __name__ == '__main__':
    main()

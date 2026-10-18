#!/usr/bin/env python3
"""Machine-code comparison for two firmware helpers added with the target build.

1. 75f0 battery estimate versus application/battery_level.c for every 12-bit
   ADC mean: returned percentage and the two values passed to printf (the
   millivolt and level floats widened to double) must be bit-identical.
2. d782 JEDEC capacity table versus spi_flash_capacity_from_jedec: the sector
   count/size globals written by the original for 40 identifiers.

The original instructions run in tools/rv32_trace.py; the C side is the host
build reconstructed/build/librecovered.so. No hardware is involved.
"""
import ctypes as C, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from rv32_trace import ROOT, TraceMachine


def main():
    lib = C.CDLL(str(ROOT / 'reconstructed/build/librecovered.so'))
    lib.battery_level_percent.argtypes = [C.c_uint16, C.POINTER(C.c_float), C.POINTER(C.c_float)]
    lib.battery_level_percent.restype = C.c_int
    lib.spi_flash_capacity_from_jedec.argtypes = [C.c_uint32]
    lib.spi_flash_capacity_from_jedec.restype = C.c_uint32
    failures = 0
    for raw in range(4096):
        m = TraceMachine()
        prints = []
        m.hooks[0x7598] = lambda mm, *a, raw=raw: raw
        m.hooks[0x1aede] = lambda mm, *a: prints.append(a) or 0
        result = m.call(0x75f0, limit=200000)
        result -= (1 << 32) if result >= 1 << 31 else 0
        mv, level = C.c_float(), C.c_float()
        native = lib.battery_level_percent(raw, C.byref(mv), C.byref(level))
        printed = [struct.unpack('<d', struct.pack('<II', p[2], p[3]))[0] for p in prints]
        if (native != result or len(printed) != 2 or printed[0] != float(mv.value)
                or printed[1] != float(level.value)):
            failures += 1
            if failures < 5:
                print('75f0 mismatch', raw, result, native, printed, mv.value, level.value)
    print(f'75f0 battery estimate: 4096 ADC values, {failures} mismatches')
    ids = [0, 0xffffffff, 0xef3011, 0xef3012, 0xef3013, 0xef3014, 0xef3015, 0xef4014, 0xef4015, 0xef4016,
           0xef4017, 0xef4018, 0xef4019, 0xef401a, 0xef6015, 0xef6016, 0xef6017, 0xef6018, 0xef6019, 0xef601a,
           0xc84017, 0xc22017, 0x1f4501, 0x123456, 0xef3010, 0xef4013, 0xef5017, 0xef7017, 0xef4001, 0xef6001,
           1, 0xef2017, 0xef4020, 0xef6020, 0xff4017, 0xef40ff, 0x00ef40, 0xef3016, 0xef4012, 0xef6014]
    bad = 0
    for jedec in ids:
        m = TraceMachine()
        m.hooks[0xd4b4] = lambda mm, *a, j=jedec: j
        m.call(0xd782, limit=10000)
        count = m.read(0x20000214, 4)
        size = m.read(0x20000218, 2)
        expected = lib.spi_flash_capacity_from_jedec(jedec)
        if count != expected >> 12 or size != (0x1000 if expected else 0):
            bad += 1
            print('d782 mismatch', hex(jedec), count, size, expected)
    print(f'd782 capacity table: {len(ids)} JEDEC identifiers, {bad} mismatches')
    sys.exit(1 if failures or bad else 0)


if __name__ == '__main__':
    main()

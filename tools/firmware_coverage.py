#!/usr/bin/env python3
"""Per-function status of the V2.4 application in the standalone firmware.

Combines analysis/functions.json (439 original functions), the existing source
ledger analysis/source_coverage.json and the target-build additions below,
and writes analysis/firmware_coverage.json plus a summary. Categories:

  native        translated C linked into the firmware
  deviation     same function with a documented robustness change
  library       upstream source compiled instead (FatFs, TJpgDec, libgcc, SDK)
  replaced      original helper not needed by the firmware design (heap,
                newlib printf internals, millicode); behaviour provided elsewhere
  gated         implemented but disabled by default
  partial       evidence-limited (CP936 tables absent in the image)
  vector-default  unused vector slot; firmware records the trap and resets

Usage: firmware_coverage.py [--write]
"""
import json, sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = {
    0x6400: ('deviation', 'firmware/startup.S', 'reset jump at 0x0 instead of 0x6400'),
    0xc668: ('deviation', 'firmware/startup.S', 'handle_reset; INTSYSCR=0, M-mode main, FPU off'),
    0xcb88: ('native', 'firmware/startup.S', 'NMI_Handler mret'),
    0xcb8e: ('native', 'platform/wch_nvic.c', 'NVIC_PriorityGroupConfig'),
    0xcb94: ('native', 'platform/wch_nvic.c', 'NVIC_Init with the image SDK encoding'),
    0xd212: ('deviation', 'firmware/system.c', 'SystemInit, plus HSI PLL fallback'),
    0xd2fc: ('native', 'firmware/system.c', 'SystemCoreClockUpdate'),
    0x1a626: ('deviation', 'firmware/delay.c', 'delay_init, free-running SysTick'),
    0x1a64c: ('deviation', 'firmware/delay.c', 'delay_ms, ISR-safe'),
    0x1a682: ('native', 'firmware/debug.c', 'USART2 TX init'),
    0x1a6e2: ('native', 'firmware/debug.c', '_write'),
    0x1aede: ('native', 'firmware/debug.c', 'printf (own formatter, truncating %f)'),
    0x7562: ('native', 'firmware/battery.c', 'PA8 charger input'),
    0x7598: ('deviation', 'firmware/battery.c', 'ADC mean; EOC wait bounded'),
    0x75f0: ('native', 'application/battery_level.c', 'battery estimate, bit-exact'),
    0x767a: ('native', 'firmware/battery.c', 'ADC1 init incl. AWDCH leftover'),
    0x770a: ('native', 'firmware/battery.c', 'battery_inputs_init'),
    0x780c: ('native', 'platform/wch_runtime.c', 'TIM2 PWM configuration'),
    0x7f66: ('native', 'platform/wch_runtime.c', 'TIM8 PWM configuration'),
    0x8016: ('native', 'storage/settings.c + platform/wch_runtime.c', 'backlight from settings'),
    0x7e0a: ('native', 'drivers/display.c', 'command+data pair (used through display_cursor/window)'),
    0x7e20: ('native', 'drivers/display.c', 'memory-write command'),
    0x87aa: ('native', 'firmware/display_port.c', 'GPIO/reset + panel sequence, 76889 words identical'),
    0x910e: ('native', 'application/format.c', 'formatter for every application format string'),
    0x969e: ('native', 'application/format.c', 'sprintf entry'),
    0x90d2: ('replaced', 'application/format.c', 'strlen used only by 0x910e'),
    0xa74e: ('native', 'ui/screen.c + ui/status_view.c', 'NCV level'),
    0xa782: ('native', 'ui/screen.c + ui/status_view.c', 'LIVE indicator'),
    0xa7a4: ('native', 'ui/screen.c', 'mode dispatch to NCV/LIVE/reading'),
    0xa7d0: ('native', 'ui/screen.c', 'AC/DC label images'),
    0xa910: ('native', 'measurement/events.c + ui/screen.c', 'regular FD events incl. "LCD_Fill" console text'),
    0xbe18: ('native', 'application/control.h', 'unit getter (state field)'),
    0xbe1e: ('native', 'application/control.h', 'value getter (state field)'),
    0xbe24: ('native', 'application/control.h', 'text getter (state field)'),
    0xbe2a: ('native', 'application/application.c', 'overload kind'),
    0xc248: ('native', 'application/application.c', 'settings load/apply, cooperative'),
    0xc320: ('native', 'application/application.c + firmware/main.c', 'power-down and halt'),
    0xd442: ('native', 'firmware/storage_port.c', 'SPI1 byte exchange, 201-poll bound'),
    0xd498: ('native', 'firmware/storage_port.c', 'send byte'),
    0xd4a4: ('native', 'firmware/storage_port.c', 'receive byte'),
    0xd782: ('native', 'drivers/spi_flash.c', 'JEDEC capacity table, 40 ids identical'),
    0xd86c: ('native', 'firmware/storage_port.c', 'SPI init + JEDEC probe'),
    0xd87c: ('deviation', 'storage/persistence.c + firmware/internal_flash.c', 'commit; HCLK/IRQ restored'),
    0xd908: ('native', 'firmware/internal_flash.c', 'halfword copy'),
    0xdca0: ('native', 'storage/filesystem.c', 'disk_status'),
    0xdca4: ('native', 'storage/filesystem.c', 'disk_initialize'),
    0xdd48: ('native', 'storage/filesystem.c', 'get_fattime'),
    0xdd4e: ('deviation', 'firmware/storage_port.c', 'fdisk/mount/setlabel/mount; compare-before-write'),
    0x14cd6: ('native', 'application/application.c', 'charging screen, cooperative'),
    0x15168: ('native', 'application/application.c + ui/status_view.c', 'language selection, cooperative'),
    0x1e57a: ('native', 'platform/memory.c', 'memcmp'),
    0x1e7f4: ('native', 'platform/memory.c', 'strchr'),
    0x1e80c: ('native', 'application/application.c', 'strcpy of the history text cache'),
}
REPLACED = {  # helpers of newlib printf/malloc and WCH millicode
    'millicode': (0x6600, 0x6660), 'printf internals': (0x1a720, 0x1b294 + 1),
    'heap': (0x1e566, 0x1e57a), 'heap tail': (0x1e650, 0x1e7f4), 'malloc locks': (0x1e81c, 0x1e820),
}
LIBRARY = {'FatFs R0.14b': (0xde5a, 0xfd46 + 1), 'libgcc soft-float': (0x1b61e, 0x1e4f8 + 1)}


def main():
    functions = json.loads((ROOT / 'analysis/functions.json').read_text())
    ledger = {int(c['address'], 16): c for c in json.loads((ROOT / 'analysis/source_coverage.json').read_text())['functions']}
    rows = []
    for f in functions:
        a = int(f['address'], 16)
        name, purpose = f['name'], f['likely_purpose']
        if a in FIRMWARE:
            status, source, note = FIRMWARE[a]
        elif a in ledger and ledger[a]['extent'] == 'full':
            kind = ledger[a].get('kind')
            status = 'library' if kind == 'upstream_library' or 'vendor/' in ledger[a]['source'] else 'native'
            source, note = ledger[a]['source'].replace('reconstructed/', ''), ledger[a].get('scope', '')
        elif a in (0xfc9e, 0xfcfc):
            status, source, note = 'partial', 'storage/codepage.c', 'CP936 tables absent in the image; fails closed'
        elif ('self-loop' in purpose.lower() or 'default' in purpose.lower() or 'to itself' in purpose
              or name.endswith(('_Handler', '_default')) or 'IRQHandler' in name):
            status, source, note = 'vector-default', 'firmware/fault.c', 'unused slot: record + reset'
        elif any(lo <= a < hi for lo, hi in LIBRARY.values()):
            status, source, note = 'library', next(k for k, (lo, hi) in LIBRARY.items() if lo <= a < hi), 'upstream source'
        elif any(lo <= a < hi for lo, hi in REPLACED.values()):
            status, source, note = 'replaced', next(k for k, (lo, hi) in REPLACED.items() if lo <= a < hi), 'not needed by the firmware'
        elif name[0].isupper() and '_' in name:
            status, source, note = 'library', 'WCH SDK (pinned)', 'peripheral driver'
        elif a in ledger:
            status, source, note = 'partial', ledger[a]['source'].replace('reconstructed/', ''), ledger[a].get('scope', '')
        else:
            status, source, note = 'unclassified', '', purpose
        rows.append({'address': f'0x{a:05x}', 'name': name, 'size': f['size'], 'status': status,
                     'source': source, 'note': note, 'hardware_validated': False})
    counts = Counter(r['status'] for r in rows)
    summary = {'functions': len(rows), 'by_status': dict(sorted(counts.items())),
               'bytes_by_status': {k: sum(r['size'] for r in rows if r['status'] == k) for k in sorted(counts)}}
    print(json.dumps(summary, indent=1))
    for r in rows:
        if r['status'] in ('unclassified', 'partial'):
            print('  ', r['status'], r['address'], r['name'], r['source'], '-', r['note'][:80])
    if '--write' in sys.argv:
        (ROOT / 'analysis/firmware_coverage.json').write_text(json.dumps(
            {'definition': __doc__.strip().splitlines()[0], 'firmware': '0.1.0-rc1', 'summary': summary,
             'functions': rows}, indent=1) + '\n')


if __name__ == '__main__':
    main()

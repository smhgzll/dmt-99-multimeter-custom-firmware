# CH32V303 integration boundary

dmt99_asic.c/.h uses C11, libc memcpy/memmove/memset and fixed buffers.
It has no HAL, MMIO, interrupts, delays, UART transmission or calibration access.
It is a transport/display interpretation module, not an analog-accuracy
validated driver for every mode.

```c
dmt99_stream rx;
/* on_record must copy and retain every FRAME/UNFRAMED/INCOMPLETE record. */
dmt99_stream_init(&rx, 12, on_record, context);
/* For EVERY received UART byte: */
dmt99_stream_feed(&rx, received_byte, uart_error_flags);
/* At actual UART IDLE, after feeding all bytes received so far: */
dmt99_stream_idle(&rx);
```

DMA count13 does not mean13 new bytes arrived. Use the actual DMA write index
or RXNE stream; do not copy a constant13 bytes at IDLE.
Binding must manage circular wrap/TC/IDLE ordering.
Errors pass unchanged to callbacks; the application decides whether
numeric output from an erroneous record is trustworthy.
RX checksums are unvalidated.

When parsed.numeric is true, displayed-unit value is mantissa/divisor;
use appropriate floating/rational arithmetic rather than integer division.
Unit codes and dmt99_unit_resource_name are firmware UI associations.
dmt99_value_si mathematically converts supported units to V/A/Ω/F/Hz and
rejects unknowns/ambiguous C/F temperature.
The physical corpus covers V/µA/A/Ω/kΩ/MΩ/nF/Hz and temperature code21;
other prefixes are firmware-resource evidence.
Open-input V/current associate ac_dc_code2 with DC and1 with AC.
Physical range, all overload conditions and analog calibration remain unverified.

Resistance Ω/kΩ/MΩ was compared against physical reference-board captures;
dmt99_resistance_ohms converts to Ω.
dmt99_is_resistance_open_pattern identifies the exact open-probe pattern,
not every overload/fault.
When numeric=false, divisor=0: do not divide.
AUTO/OL/unknown symbols are not silently zero readings.
Leading blank cells are ignored only for numeric conversion
(physical `"  1.1"`→11/10Ω); text preserves blanks.
Internal/trailing blanks and letters remain nonnumeric.
Nominal1Ω and displayed1.1Ω are different metadata fields, not proof of analog accuracy.

For dmt99_host_ui_status(&parsed,host_mode,&state), the caller must supply
the selected mode. Mode09 NCV counts dashes among four symbols(0..4);
mode13 LIVE returns1 if the first symbol is not L, otherwise0.
This reproduces screen.c's drawing decision, not a safety-rated energized-wire detector.
Physical open inputs gave NCV0/LIVE0; active levels were not tested.
Unsupported modes/non-display frames returnfalse without modifying output.
Raw unknown NCV symbols and LIVE text L1UE remain unchanged.

dmt99_build_firmware_template emits only supported literal firmware forms
and sends nothing. Most ordinary commands remain physically untested.
Legacy00/check11 is preserved; diagnosticA0 is unavailable.
Commands generated during this passive investigation were never transmitted.

During the original standalone session no files were copied into custom
firmware and no MCU was flashed. The module built as an object/static archive
using rv32imac_zicsr_zifencei/ilp32:
`make -C firmware`, or `make -C firmware CROSS=/path/to/riscv-prefix-`.
build/libdmt99_asic_rv32.a is a host-side output; the Makefile does not connect
to a target. Firmware libc/libgcc must supply mem* and soft-double operations.
Compiling an RV32 object does not validate a complete application's link/RAM/stack.
Binding and physical conversions require further corpus/hardware validation.
The historical13-byte stream form requires explicit profile selection;
its availability does not validate that wire length.

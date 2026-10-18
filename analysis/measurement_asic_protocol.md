# Measurement-side serial protocol

## Evidence and transport

High confidence: original main `0x908a` calls `0xc350` with 9600. GPIO setup selects PA9 TX and PA10 RX,
USART1 8N1, RX+TX and no flow control. DMA1 channel 5 uses byte-sized circular transfers, incrementing RAM
at `0x200002ac`, length 13. USART1 IDLE `0xc468` disables DMA, clears IDLE/reads DATAR, resets CNTR to 13,
and reenables DMA. The DMA transfer-complete handler `0xc570` only clears the interrupt flag.
No inter-packet period, electrical voltage or exact peer identity can be established without additional evidence.

## TX, MCU to peer

`measurement_send_command` at `0x97c4` transmits exactly ten bytes and waits for TC after every byte.
The template at `0x1e820` is `AA 55 05 00 00 00 00 00 00 00`; command constructors replace bytes 3 and 9.
For observed zero-payload commands: `AA 55 05 CC 00 00 00 00 00 (CC+05)`.
The second template at `0x1e82c` has byte 9 = `0x11`, used when selecting mode `0x0c`.
This is evidence for the listed commands, not proof of a general checksum over arbitrary payloads.

| ID | Supported interpretation | Construction evidence |
|---|---|---|
| 01 | PC12/button action; physical action unresolved | `0x97f8`, checksum 06 |
| 03 | Hold-related action; UI toggles hold icon on event 03 | `0x97f8`, `0xb5c8`, `0xa910`; checksum 08 |
| 08 | PC11/button action; physical action unresolved | `0x97f8`, checksum 0D |
| 09 | PC10/button action; mode/state 09 needs more interpretation | `0x97f8`, checksum 0E |
| 0A | Capacitance UI | `0xb9e0` and event `0xa910` cap.jpg; checksum 0F |
| 0B | Resistance UI | r.jpg; checksum 10 |
| 0C / 0D | DC voltage / AC voltage | dc_v.jpg / ac_v.jpg in regular view; checksum 11 / 12 |
| 0E | ol.jpg resource; physical function unresolved | `0xa910`; checksum 13 |
| 0F | Frequency | hz.jpg; checksum 14 |
| 10 / 11 | DC A / DC mA | regular-view dc_a resource and unit selection; checksum 15 / 16 |
| 12 | Temperature | temp.jpg; checksum 17 |
| 13 | State/mode requiring more interpretation | explicit constructor/checksum 18 |
| 14 | Default/automatic-view path, exact analog policy unresolved | constructor/checksum 19 |
| 15 / 16 | AC A / AC mA | ac_a resource and units; checksum 1A / 1B |

UI resource associations establish what the host displays; they do not establish every analog operation
performed by the peer. Some second-view English resource selections appear inconsistent with the regular view;
keep the original paths visible rather than silently correcting them.

## RX, peer to MCU

The normal display decoder `0xbe78` checks byte 0=`5A`, byte 1=`A5`. It decodes four LCD symbols from:
`(b[2]&F0)|(b[3]&0F)`, `(b[3]&F0)|(b[4]&0F)`, `(b[4]&F0)|(b[5]&0F)`, `(b[5]&F0)|(b[6]&0F)`.
`0x98aa` clears bit `0x10` before matching segment patterns. These are not ordinary BCD digits.

| Symbol | Patterns, after masking 0x10 |
|---|---|
| 0..9 | EB, 0A, AD, 8F, 4E, C7, E7, 8A, EF, CF |
| A,U,T,O,L,E | EE,23,65,27,61,E5 |
| minus, space, C, P, r | 04,00,E1,EC,24 |
| unknown | FF return code |

Negative sign is b[2] bit 4. Decimal markers are b[3], b[4], b[5] bit 4, in that priority order.
Bits 0/1 of b[2] produce status codes 1/2 (bit 1 has priority), otherwise FF; AC/DC labeling remains tentative.
Unit-code selection is a priority tree over bytes 6..8; the native decoder retains all original numeric unit IDs.
Byte 7 bit 7 is exported as a status flag. Meanings of bytes 9..12 and an incoming checksum rule are unresolved.

Special/event frames have b[2]=FD and event/mode ID in b[3]. ISR `0xc468` copies exactly 13 bytes into
`0x200002bc` and clears an event-consumed flag. The main loop later copies a 16-byte buffer, including three
bytes not newly copied by this ISR. The reconstruction models the known 13-byte snapshot only.
The ISR itself does not check 5AA5; the higher-level reconstructed parser adds a documented header/length guard.

The ISR suppresses a normal update if b[2] high three bits, b[3..5] low nibbles and high three bits, and b[6]
low nibble are all zero. This exact predicate is implemented independently of header validation.
No calibration command, continuous sample period, ADC register map or general error-code set is verified.

## Validation

All 256 input values to original machine-code `0x98aa` match the compiled C segment decoder.
Host tests cover literal TX vectors, packed-digit RX, events, blank suppression and invalid lengths/headers.
These validate defined wire operations; they do not validate the whole measurement subsystem or analog accuracy.

## Native policy validation continuation

`application/control.c` now implements all three observed button command policies. Differential tests
compare the exact ten-byte transmitted packets, selector resets and relative-reference state against
b5c8/b9e0/bc90. The command-zero template AA55 05 00 00 00 00 00 00 11 is preserved on the binary's
fallback/long-press/history paths; it is not normalized to a hypothetical checksum rule.

`measurement/events.c` recovers per-view FD mode/HOLD transitions, with original-entry comparisons.
`measurement/value.c` adds the original six-character text and numeric conversion, including signed-byte
unknown-segment behavior, last-symbol numeric status and decimal/unit flag priority. The original
software floating-point routines execute in these host differential tests; there is no ASIC emulation.
Relative is invalidated when reported mode/unit changes. Host selectors request modes; the actual
peer autoranging and calibration algorithms remain unknown. Diagnostic A0 is retained as uninterpreted
wire evidence and is not enabled by the native normal application.

# Comparison with original decompiled code — 2026-10-18

Command templates match. The source project was used read-only.
Reproduce with:

```bash
python3 -B tools/audit_decompiled.py
```

Results: [decompiled_audit.json](../evidence/decompiled_audit.json).
The tool executes the original project's preserved instruction inventory
and ROM data with rv32_trace in a Python RAM model.
USART/UI leaf calls are intercepted; no target access or UART transmission
occurs. This is not complete MCU/ASIC emulation.
Bytecode writes into the reference tree are disabled.
Source SHA-256 values matched before/after the run.

## Command bytes

Regular-view button handler 0xB5C8:

| Mode | CC (hex) | Last byte (hex) | Decompiled source lines |
|---|---|---|---|
| DC low current / mA UI group | 11 | 16 | B5C8:32–33 |
| DC A | 10 | 15 | B5C8:36–37 |
| AC low current / mA UI group | 16 | 1B | B5C8:40–41 |
| AC A | 15 | 1A | B5C8:44–45 |
| Temperature | 12 | 17 | B5C8:104–105 |
| NCV | 09 | 0E | B5C8:108–109 |
| LIVE | 13 | 18 | B5C8:113–114 |

Ordinary template: `AA 55 05 CC 00 00 00 00 00 LAST`.
0x97C4 sends exactly10 bytes. Event/UI resources at0xA910 support DC A/mA,
AC A/mA and temperature associations;0xA7A4 dispatches09/13 to NCV/LIVE.
IDs are not interpreted as ASIC registers or an arbitrary-payload checksum rule.

24 audit contexts yielded19 distinct literals matching both the earlier
analysis/asic_protocol_commands.json and the new catalog.
All18 serializer outputs match original bytes.
Diagnostic `AA 55 02 A0 ... A2` exists in the source but is deliberately
unavailable through the serializer.
Legacy `AA 55 05 00 ... 11` remains unchanged, not corrected to00+05.

Across25 physical sessions, **6 complete error-free MCU TX packets** were
found:14 twice,0C/0D/0B/0A once each. All match original constructors.
The last seven open-input recordings began after mode selection, so they
do not capture the seven mode-selection commands above.
Those associations are supported by code/UI context, not recorded selection TX.

## ASIC replies and interpretation differences

All256 segment inputs match original0x98AA.
For74 distinct clean5AA5 replies in the physical corpus, executing
0x997E/0xBE78 confirmed four symbols, sign, decimal position, unit,
AC/DC code and b7bit7. Every field matched.
For replies accepted as numeric by the new parser, values also matched
original float results at rel_tol1e-6/abs_tol1e-9, allowing float32 rounding.

Two distinct raw NCV replies gave level0 through0xA74E; three LIVE replies
gave active-render0 through0xA782. The context-dependent new status decoder
agrees. Active-field/cable thresholds were not tested.

0x17F92 selects units/ua.jpg for code8, units/ma.jpg for9 and units/a.jpg for10.
Thus interpreting code8 as µA under an mA **mode heading** agrees with code.
For temperature code21, the host C/F setting selects the image;0x9CAA
converts display values when Fahrenheit is preferred.
Code21 alone does not encode the host preference.

Intentional differences from stock UI text:

- Stock reformats sign/blanks/decimal; new text preserves raw symbols.
  For example stock mode0 text ` L1UE ` corresponds to raw `L1UE`.
- NCV's E4 is unknown255 in the original dictionary too. Generic stock
  signed-char behavior may yield '/', while the new parser retains '?' and
  numeric=None. NCV UI uses its separate level rendering.
- Stock may zero leading blank LCD cells in temperature/low-current modes;
  raw text preserves blanks and numeric conversion is separate.
- 0xC350/0xC468 configure/copy13 DMA bytes, but recordings show12-byte display
  bursts and10-byte AA55FD events. Buffer size is not imposed on wire data;
  no bytes are added.

RX checksum, opaque tail, physical range and analog calibration remain unresolved.

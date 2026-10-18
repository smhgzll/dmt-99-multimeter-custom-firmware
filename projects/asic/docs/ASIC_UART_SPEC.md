# DMT99 ASIC UART — experimental, evidence-bounded specification

Scope: original-firmware AUTO/startup, resistance reference-board and
open-input recordings. This does not claim recovery of calibration,
an ASIC register protocol, every measurement mode or a general checksum.
Sources: evidence/sources.json; recordings: captures/;
portable implementation: firmware/dmt99_asic.c/.h.

## Transport and boundaries

9600 baud, 8 data bits, no parity, 1 stop, LSB-first.
D0=PA9 MCU_TX, D2=PA10 ASIC_TX; analyzer inputs are passive.
AUTO recordings at 1/2 MHz match independent sigrok UART decoding.
Resistance recordings use 1 MHz. Sample index is the authoritative timestamp.

Three physical wire forms were observed:

| Direction / family | Start | Observed length | Scope |
|---|---|---|---|
| MCU command | `AA 55 05` | 10 bytes | Startup 14; transitions 0C/0D/0B and 0A |
| ASIC display | `5A A5` | 12 bytes | AUTO, resistance and open-input V/current/C/Hz/NCV/LIVE/temperature |
| ASIC event candidate | `AA 55 FD` | 10 bytes | Startup event 14 |

The MCU DMA length is **13**, a buffer configuration rather than evidence
that these 12-byte display bursts contain another byte.
A 13-byte 5AA5 profile remains as historical compatibility, not the
physically confirmed default. Missing bytes are never invented.

Display bursts occur roughly six times per second. Session analysis.json
retains independent idle-burst min/median/max statistics.
The software split threshold is four UART characters, not a proven ASIC
timing requirement. Gaps, UART errors, unknown bytes and incomplete packets remain visible.

## MCU commands

Observed AUTO startup:
`AA 55 05 14 00 00 00 00 00 19`.

The firmware contains 19 literal TX forms. For zero-payload ordinary IDs
01,03,08,09..16(hex), b9=ID+5; the legacy
`AA 55 05 00 00 00 00 00 00 11` exception is preserved.
Diagnostic A0 is uninterpreted and not exposed by the serializer.
This does not establish arbitrary-payload checksums; serialization transmits nothing.

MCU TX was silent during steady resistance captures.
A separate AUTO→resistance transition recorded 0C→0D→0B and matching event
candidate IDs, ending in the resistance open-input pattern.
This physically supports 0B's resistance-UI association.
Intermediate low-voltage displays are not AC/DC accuracy or calibration tests.

## ASIC display message: 12 bytes

| Offset | Information | Evidence / limit |
|---|---|---|
| 0..1 | `5A A5` | Physical AUTO/resistance bursts |
| 2..6 | Four packed LCD symbols | Firmware formula and display comparisons |
| 2 bit4 | Negative sign | Firmware-derived; no physical negative-reference test |
| 3/4/5 bit4 | Decimal after digit 1/2/3 | Positions checked in resistance captures; conflict priority is firmware-derived |
| 2 bit0/1 | Host AC/DC code | Open-input V/current: 2=DC, 1=AC association; bit1 priority is firmware-derived |
| 6..8 | Unit priority tree | Ω/kΩ/MΩ observed in resistance captures |
| 7 bit7 | Unnamed status flag | Physical meaning unknown |
| 9..11 | Opaque | Retained; not identified as checksum/ADC/calibration |

Symbols:
`(b2&F0)|(b3&0F)`, `(b3&F0)|(b4&0F)`,
`(b4&F0)|(b5&0F)`, `(b5&F0)|(b6&0F)`.
Mask with &EF before matching. These are not BCD.
All 256 segment inputs match reconstructed C; that does not physically validate every symbol.

Numeric conversion accepts digits with leading blank LCD cells only;
raw text remains unchanged. Example physical `"  1.1"` gives mantissa 11,
divisor 10, unit 11=Ω. Internal/trailing blanks, letters, unknowns and AUTO
are not forced into numeric values.

Resistance units under ordinary priority conditions:

| Condition | Host code | Unit |
|---|---|---|
| b7&20, neither b6&40 nor b7&04 | 11 | Ω |
| b7&20, b6&40 | 12 | kΩ |
| b7&20, no b6&40, b7&04 | 13 | MΩ |

Other conflicting flags retain original firmware priority; all conflicts
have not been physically exercised.
dmt99_resistance_ohms converts displayed units using ×1/×1000/×1000000,
without correcting to component nominal values. Physical range fields remain unresolved.

Open probes produced symbols [17,0,14,17], decimal_after=2, unit=13,
raw text `" 0.L "`. resistance_open_pattern names that exact observation;
it does not classify every overload/fault.
Numeric is false/None; C divisor=0 and Ω conversion fails.

## ASIC event candidate

Startup: `AA 55 FD 14 00 00 00 AA 01 BB`, 10 bytes.
Transitions additionally observed:
`AA 55 FD 0C 00 00 00 AA 01 B3`,
`AA 55 FD 0D 00 00 00 AA 01 B4`,
`AA 55 FD 0B 00 00 00 AA 01 B2`,
and capacitance `AA 55 FD 0A 00 00 00 AA 01 B1`.

FD is offset2, event ID offset3. Two startup observations followed AUTO TX
by roughly 164–166 ms; this is not guaranteed correlated ACK/acceptance/rejection.
The original IDLE ISR's FD test has no header guard; a 5AA5-only parser misses
this separate family. Meanings/integrity of b4..9 remain unknown.

## Integrity and evidence levels

RX checksum existence/offset/rule is unknown.
Clean AUTO captures show different b11 for the same b0..10, contradicting
a deterministic checksum solely over that prefix. This does not prove no
checksum exists. Sum/XOR scanning generates hypotheses only.
For the five observed event IDs, the last byte matches sum(b0..8)&FF,
but other fields did not vary; this remains a bounded hypothesis and is not
used as an arbitrary event-payload rule.

Display comparisons test protocol/display consistency. ST2 resistance values
are **nominal**; no independent reference measurement or tolerance was reported.
These are not analog-accuracy or calibration certificates.
Counts: evidence/resistance_comparison.json. Original .sr, transitions
and user reports remain in session metadata.
[Open-input validation](OPEN_PROBE_VALIDATION.md) covers C/buzzer/DC/AC/Hz/
current/NCV/LIVE/temperature.

NCV/LIVE are not numeric measurements. Given explicit host mode 09/13,
dmt99_host_ui_status applies screen.c's dash count or first-symbol-L decision.
Mode is not guessed from the packet. Open-input raw ` E? ` and `L1UE`
are retained; active-source thresholds/levels were not tested.
Temperature code21 and the user's °C selection are separate evidence:
display 32 matched packet value 32, but generic SI conversion rejects
code21 because C/F context is ambiguous.

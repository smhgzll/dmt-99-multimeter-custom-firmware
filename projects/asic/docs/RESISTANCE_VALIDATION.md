# Resistance captures — 2026-10-18

A nine-resistor reference board supplied with an ST2 kit was used.
The user reported nominal values; no tolerance or independent measurement
was supplied. DMT99 original firmware; passive analyzer D0=MCU_TX/D2=ASIC_TX,
1MHz/9600 8N1.

| Physical condition | User-reported display | Decoded display | Complete RX packets |
|---|---|---|---:|
| Nominal1Ω, first | 1.1Ω | 0.8–1.1Ω plus open/blank transitions | 59 |
| Nominal1Ω, repeat | 1.1Ω | 1.0Ω(17),1.1Ω(42) | 59 |
| Nominal1kΩ | 1.001kΩ | 1.000–1.001kΩ(45), open/blank transitions(13) | 58 |
| Nominal10kΩ | 9.970kΩ | Raw ` 9.97`kΩ throughout | 59 |
| Nominal100kΩ | Approximately99.80kΩ | Raw ` 99.8`kΩ throughout | 60 |
| Nominal1MΩ | 0.996 in MΩ context | 0.996MΩ throughout | 60 |
| Nominal10MΩ | 9.970MΩ | 9.96MΩ(39),9.97MΩ(20) | 59 |
| Open probes | User reported disconnected; no display text supplied | Raw ` 0.L `, nonnumeric | 60 |

Each session lasts10s. Complete display messages are **12 bytes**, header5AA5.
The first seven resistance/open sessions have no UART errors.
The10MΩ session retains two initial/ending incomplete/error candidates
and7+9-byte fragments; its59 complete packets are error-free.
Independent sigrok comparison passed for the clean excerpt.
Other clean resistance sessions were compared over complete byte streams.

Raw digits are not silently reformatted: reported9.970 and UART ` 9.97`
remain separate metadata/output fields, despite equal numeric value.
Metadata records that the1MΩ report's unit came from context;
wire flags independently yield the firmware's MΩ code.

## Corrections implemented

- Leading blanks in the physical1Ω reading `"  1.1"` previously prevented
  numeric conversion. Python/C now ignore leading blank LCD cells only,
  preserving raw text. Letters, internal/trailing blanks and unknowns are not coerced.
- Physical Ω/kΩ/MΩ patterns match unit priority.
  C converts displayed units by ×1/×1000/×1000000 without nominal-value correction.
- Exact open-input symbols[17,0,14,17]/decimal2/unit13 are separately
  recognized; no numeric zero/0Ω is produced.
  This does not solve every overload/fault condition.

## Command association

Capture `20261018T011808_auto_to_resistance_open_fa59a550` is20s/1MHz,
without UART errors. The user switched AUTO→resistance:

| t(s) | MCU command | ASIC event ID | Event-start delay |
|---|---|---|---|
| 13.073980 | `AA 55 05 0C 00 00 00 00 00 11` | 0C | 181.435ms |
| 15.091084 | `AA 55 05 0D 00 00 00 00 00 12` | 0D | 162.763ms |
| 15.892405 | `AA 55 05 0B 00 00 00 00 00 10` | 0B | 164.707ms |

Events are10-byteAA55FD. The final state shows resistance-open input.
This supports0B's resistance-UI association, not guaranteed ACK or complete
understanding of analog side effects. Both directions matched independent UART decoding.

## Files and scope

Complete raw sessions are under captures/*resistance*, *1Mohm*, *10Mohm*,
alongside CSV/JSON/text, metadata, errors and sigrok logs.
Display/unit comparison counts: evidence/resistance_comparison.json.
Unchanged real-sample regression excerpts are under tests/fixtures/,
with parent hash/sample range in each JSON.

Independent C: firmware/dmt99_asic.c/.h, with no UART/MMIO transmission or
target Flash mutation. Binding boundary: firmware/INTEGRATION.md.
Opaque b9..11, ASIC identity, calibration, physical range code, RX integrity
and untested analog modes remain unknown.
The user's subsequent scope was open-input modes only, without new components/sources.

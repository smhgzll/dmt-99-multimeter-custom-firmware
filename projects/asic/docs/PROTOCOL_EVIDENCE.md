# ASIC protocol — firmware sources and physical evidence

This is not a complete/validated wire specification.
Physical AUTO/startup findings are in [HARDWARE_FINDINGS.md](HARDWARE_FINDINGS.md).
evidence/sources.json records source paths/hashes, normalized to relative
notation for public consolidation; evidence/firmware_commands.json retains literal provenance.

| Field | Firmware-derived information | Physical validation |
|---|---|---|
| UART | 0xC350: USART1 PA9/PA10; main9600; 8N1 | 1/2-MHz AUTO samples agree with 9600 8N1 and independent decoding |
| RX DMA | DMA1 ch5, circular, byte,13; IDLE ISR0xC468 rearms | Pending |
| TX | 0x97C4: exactly10 bytes, waits TC per byte | Startup14 literal observed as10 bytes |
| TX b0..1 | AA55 | Observed in startup14 |
| TX b2 | Usually05;02 for diagnostic A0 | Pending |
| TX b3 | 17 ordinary IDs, plus legacy00/A0 | ASIC effects require separate checks |
| TX b4..8 | Zero in recovered literal forms | General payload unknown |
| TX b9 | Listed command+5 forms, legacy00/11, A0/A2 | General checksum unknown |
| RX b0..1 | 0xBE78 display guard5AA5 | AUTO display5AA5/12; event candidateAA55FD/10 |
| RX b2..6 | Four packed LCD symbols, not BCD | Resistance/open-input display comparisons; not every symbol physically tested |
| RX b2 bit4 | Negative sign | Pending |
| RX b3/b4/b5 bit4 | Decimal after1/2/3, in that priority | Three positions observed; conflicting-flag priority is firmware-derived |
| RX b2 bit0/1 | Host code1/2, bit1 priority | Open-input V/current associate1=AC,2=DC |
| RX b6..8 | Host unit priority/UI resource associations | Ω/kΩ/MΩ and open-input V/µA/A/nF/Hz/temperature; physical range unknown |
| RX NCV/LIVE | screen.c counts NCV dashes; LIVE renders active when first symbol is not L | Open-inputNCV0/LIVEfalse; active inputs untested, explicit host mode required |
| RX b7 bit7 | Unnamed status flag | Meaning unknown |
| RX b2=FD | Event snapshot, b3ID, no ISR header guard | ACK causality unproven |
| RX b9..12 | Opaque in historical13-byte DMA view | Actual12-byte packets have b9..11; meanings unknown, wire b12 unobserved |
| RX integrity | No established firmware checksum check | Existence/offset/algorithm unknown |

Packed segments:
`(b2&F0)|(b3&0F)`, `(b3&F0)|(b4&0F)`,
`(b4&F0)|(b5&0F)`, `(b5&F0)|(b6&0F)`.
0x98AA clears bit0x10 before matching.
`EB 0A AD 8F 4E C7 E7 8A EF CF` map to0..9.
Other symbols/unit priorities are differential-tested against reconstructed C;
this alone is not renewed machine-code proof of that reconstruction.

Evidence categories remain **physically observed**, **firmware-derived/plausible**
and **unknown**. Observed packet forms do not establish analog meanings.
Host probe records remain local under artifacts/local-environment/ after consolidation.
USB identification alone is not UART-format validation.

The planned physical sequence was startup/idle, resistance open/1k/10k/100k,
capacitance100n/1u/10u, DC1.5V battery, continuity open/closed and AUTO transitions.
Each capture starts only after user-prepared wiring. No mains tests.
Original raw files/metadata are preserved; the later scope was restricted to open inputs.

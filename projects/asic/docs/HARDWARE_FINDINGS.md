# Physical hardware findings — 2026-10-18

Passive GND/D0/D2 wiring, original firmware, user-reported AUTO mode.
ASIC TX was monitored on D2, MCU TX on D0. No commands were sent to the MCU.

| Capture | Rate / acquired duration | Observation |
|---|---|---|
| `20261018T003908_idle_already_powered_81d808b6` | 1 MHz / 20 s | 120 ×12-byte 5AA5 bursts; 1440 RX bytes; no UART errors |
| `20261018T004101_auto_idle_d0_d2_2mhz_9eb57894` | 2 MHz / 10 s | 61 ×12-byte 5AA5 bursts; 732 RX bytes; no UART errors |
| `20261018T004155_startup_auto_d0_d2_2mhz_998d0648` | 2 MHz / 29.429760 s | Two AUTO TX commands/two AA55FD candidates; power-transition errors retained |

All 2172 RX bytes from the first two captures match libsigrokdecode UART
(evidence/real_capture_decoder_validation.json).
This supports 9600 8N1 and observed burst lengths, not every operating mode.

## Measurement/display family

`5A A5 E4 2E 63 25 07 00 00 00 01 3F` — **12 bytes**.
Firmware segment interpretation gives AUTO.
In the first two captures b0..10 stayed constant while b11 took 3E/3F/40.
A deterministic checksum of the preceding 11 bytes therefore does not
explain b11 in these clean captures. Its physical meaning and other checksum
locations/rules remain unconfirmed.

The original firmware has a 13-byte DMA buffer. Both facts are reported
without adding a missing byte or presenting stale-buffer hypotheses as established causes.

## Separate startup family

MCU TX: `AA 55 05 14 00 00 00 00 00 19` — **10 bytes**.
ASIC RX: `AA 55 FD 14 00 00 00 AA 01 BB` — **10 bytes**, not 5AA5.

RX began 165.855 ms and 163.391 ms after TX in two observations.
FD/14 is consistent with firmware event dispatch and AUTO association,
but ACK, acceptance/rejection and guaranteed request/response semantics are unproven.
The initial 5AA5-only guard could miss this family; the parser now retains it separately.

Of 60,000,000 requested startup samples, 58,859,520 arrived.
Despite sigrok-cli return code0, the session is marked partial_or_failed.
USB transfer timeouts appear in logs; the sample shortfall's cause remains unverified.
Initial low level and power-transition framing errors are preserved.

## Validation limits

Physically observed: UART bytes, two message families, 12/10-byte bursts,
literal command14 and temporal relationships in these AUTO captures.
Firmware-derived: segments and sign/decimal/unit priority.
Later ST2 sessions covered nominal 1Ω/1kΩ/10kΩ/100kΩ/1MΩ/10MΩ and open probes:
[resistance validation](RESISTANCE_VALIDATION.md).
User-limited open-input C/buzzer/DC/AC/Hz sessions followed:
[open-input validation](OPEN_PROBE_VALIDATION.md).

C conversion implements UI prefixes. Analog calibration, physical range,
b9..11 meanings, RX checksum and guaranteed ACK remain unvalidated.
Regression excerpts are unchanged raw sample slices, not regenerated
waveforms. Their JSON files retain parent hashes, sample ranges and expected
byte/sample positions. The prior analysis directory was unchanged during acquisition.

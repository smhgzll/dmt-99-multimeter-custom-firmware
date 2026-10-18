# DMT99 ASIC UART — passive capture and analysis

In the consolidated repository, sources are under `projects/asic/` and full
captures under `artifacts/captures/asic-captures/`.
Earlier acquisition/test results below are historical evidence.
[Shared build guide](../../docs/BUILDING.md),
[evidence overview](../../docs/RESEARCH.md),
[electrical risks](../../docs/LEGAL_AND_SAFETY.md).

Working directory: `projects/asic`. The prior analysis is read-only reference
input. Python 3 standard library, sigrok-cli and FX2 analyzer runtime firmware
are sufficient for capture/analysis.

## Recorded hardware status

2026-10-18, Europe/Istanbul: USB `0925:3881` was detected; fx2lafw identified
it as Saleae Logic / 8 channels. Missing firmware was obtained from the
Nobara repository as
`sigrok-firmware-fx2lafw-0.1.7.git9fe40f8-1.fc44.noarch.rpm`.
The rpm -K signature/digest check passed; the package was extracted locally.
Runtime loading changed analyzer USB address 1.62→1.63.
No MCU/ASIC/Winbond programming or UART transmission was performed.

After PulseView was closed, analyzer access was verified and physical
AUTO/startup, ST2 resistance-reference and open-input captures were acquired.
[Resistance results](docs/RESISTANCE_VALIDATION.md),
[wire format](docs/ASIC_UART_SPEC.md).
Timestamped host probe records remain local under
`artifacts/local-environment/` after consolidation.
Traceable excerpts from real samples are in `tests/fixtures/`; synthetic
tests are labeled separately.

**Verified D0/D2 wiring:** the user confirmed channels D0 and D2.
AUTO bursts were **12 bytes** at two sampling rates; the startup ASIC event
candidate `AA 55 FD ...` was **10 bytes**.
The historical 13-byte DMA buffer is not treated as the wire length.
[Hardware findings](docs/HARDWARE_FINDINGS.md).

Long acquisitions ended early around 30 seconds twice; the cause remains
unresolved. The CLI compares actual/target sample counts and flags incomplete
sessions. Separate 10-second captures were used thereafter; gaps between them
are not presented as continuous recording.
See `evidence/acquisition_limit_observation.json`.

The user's final acquisition scope was open-input tests only. Tests requiring
new components/sources were deferred; capacitor, battery and closed-circuit
validation was not performed.

## Wiring and first capture

- Analyzer GND → DMT99 GND.
- D0 → MCU PA9 / pin 42 → ASIC (`MCU_TX`).
- D2 → MCU PA10 / pin 43 ← ASIC (`ASIC_TX`), the user's verified wiring.
- Do not connect analyzer VCC; channels are passive logic inputs.
- Run the original DMT99 firmware, initially with measurement inputs disconnected.
- Never measure mains while the analyzer is attached.

```bash
cd projects/asic
python3 tools/dmt99.py probe
python3 tools/dmt99.py scenarios
```

`probe` opens the analyzer and reads capabilities without sampling.
Normal FX2 initialization may load runtime firmware into analyzer RAM.
After verifying physical wiring:

```bash
python3 tools/dmt99.py capture --test startup_idle --seconds 30 --connected \
  --rx-channel 2 --rx-frame-bytes 12 \
  --notes 'Measurement inputs disconnected; powered on after capture started'
```

For startup acquisition, keep the meter off until the CLI reports that raw
data is being written and capture is active. This message indicates the
first USB data reached the file; OS latency applies.
The default is 1 MHz; `--samplerate 2000000` selects 2 MHz.
Duration is configurable. A 30-second/2-MHz capture needs roughly 60 MB
of raw logic plus derived outputs. Open the .sr in PulseView after capture completes.

Nobara system installation example:

```bash
sudo dnf install sigrok-cli pulseview sigrok-firmware-fx2lafw
```

Local installation without root:

```bash
bash tools/bootstrap_analyzer.sh
```

Firmware lookup is redirected through sigrok's
`XDG_DATA_HOME/sigrok-firmware` mechanism.
[Sigrok firmware documentation](https://www.sigrok.org/wiki/Firmware).

## Labels and measurement metadata

Prepare wiring before each physical change and acquire a separate session.
`scenarios` lists resistance, capacitance, battery, continuity and AUTO cases.
The following is an example; executing it samples the connected analyzer,
rather than creating a synthetic capture:

```bash
python3 tools/dmt99.py capture --test resistance_1k --connected --seconds 10 \
  --rx-channel 2 --rx-frame-bytes 12 \
  --actual '1 kohm, 1% nominal; no independent reference measurement' --mode resistance \
  --display '0.999 kΩ' --display-numeric 0.999 --display-unit kohm --tolerance 0.002
```

Add/correct the reported display and component after capture:

```bash
python3 tools/dmt99.py annotate captures/SESSION \
  --display '1.000 kΩ' --display-numeric 1.000 --display-unit kohm \
  --actual '1 kohm, 1%' --notes 'Stable region; display fluctuation ±0.002 kΩ'
```

Previous values remain in `annotation_history`.
`display_numeric` is in the **displayed unit**, without silent SI conversion.
Use `--actual` to distinguish nominal values from independently measured ones.
Record transition order and approximate capture times in notes;
one display value does not describe an entire transition recording.

## Stored outputs

Each acquisition creates a unique local-time directory:

| File | Contents |
|---|---|
| `raw.sr` | Original sigrok recording; unchanged by reanalysis |
| `sigrok.log` | Driver/USB logs, including errors |
| `metadata.json` | Test, display/component, wiring, command, duration and result |
| `logic.bin` | Ordered concatenation of raw .sr chunks |
| `decoded.json` | Every UART candidate, bits, timing, packets and errors |
| `bytes.csv` | Candidate direction, time, value and errors |
| `packets.csv` | Packet candidates, truncation, unframed fragments and fields |
| `decoded.txt` | Readable TX/RX timeline |
| `analysis.json` | Repeats, changing offsets, groups and checksum hypotheses |

```bash
python3 tools/dmt99.py analyze captures/SESSION --gap-chars 4
python3 tools/dmt99.py compare captures/SESSION1 captures/SESSION2
python3 tools/dmt99.py import-sr /path/to/capture.sr --hardware --test resistance_open
```

Capture/import defaults use verified D0/D2 and the observed 12-byte profile.
Override with `--rx-channel`/`--rx-frame-bytes`; choices are recorded.
Select the earlier D0/D1+13 hypothesis explicitly with
`--rx-channel 1 --rx-frame-bytes 13`.
Import checks channel metadata and one-byte-per-sample encoding rather than
guessing unknown wiring. D2 was initially unselected in the first recording;
FX2's raw bit2 was recovered through an explicit profile revision after
the user's D0/D2 confirmation. Earlier results remain in revisions/,
and the original .sr is unchanged.
Imports without `--hardware` do not count as physical comparisons.
Comparison output defaults to evidence/comparison.json; override with --output.

## Data and evidence policy

Decoding uses 9600 8N1, LSB-first, bit-center sampling.
Sample index is the primary timestamp; t_s is relative to capture start.
Host wall time is separate metadata, not the absolute timestamp of a UART byte.

Initial low levels, short starts/glitches, low stop bits and truncated ending
frames are flagged. Incomplete values remain null, with known bits retained
in bits_lsb_first/partial_value. All raw samples remain; edges inside a frame
are not duplicated as additional bytes. Long low levels appear in signal statistics.

Packet boundaries depend on the explicit profile: MCU AA55/10, ASIC AA55FD/10,
ASIC 5AA5/12, or the historical 13-byte hypothesis.
The default four-character idle threshold is **not a physically verified
timing requirement**. Gaps split incomplete packets while preserving
unframed data. Embedded headers are reported in embedded_header_offsets;
no silent resynchronization occurs. Every candidate belongs to exactly one
same-direction fragment; none is dropped.

TX check bytes are compared only with the 19 recovered literal forms.
Legacy 00/.../11 remains unchanged. General payload checksums are unproven.
RX checksum existence, offset and algorithm are unknown. Sum/XOR scans
produce hypotheses, not validation from coincidental constant fields,
duplicate packets or UART-error candidates.

Measurements are decoded from packed LCD segments, not assumed BCD/raw ADC.
Unknown symbols are not forced into numbers. The original numeric parser's
unknown-symbol coercion is deliberately not reused.
Firmware interpretations remain visible for erroneous UART candidates,
with errors explicit; those candidates are excluded from checksum hypotheses
and numeric display comparisons.

Temporal proximity to preceding TX does not establish ACK or causality.
`compare` counts displayed value/unit matches without automatically marking
fields proven. [Physical findings](docs/HARDWARE_FINDINGS.md) keep observed
wire forms separate from analog meanings requiring further measurements.

## Checks and next steps

```bash
python3 -m unittest discover -s tests -v
```

Tests cover all 256 byte values at two rates/two channels, chunk boundaries,
framing/truncation/glitches, lossless grouping, 19 literal TX forms, independent
sigrok decoding and 4096 field combinations against read-only reconstructed C.
These are not analog/ASIC validation.

25 physical sessions and 24 real-sample excerpts are preserved.
[Capture index](docs/CAPTURE_INDEX.md).
The recorded standalone results were 31 tests and 48 independent two-direction
sigrok comparisons. Cases include DC/AC low-current/A, idle NCV/LIVE and
temperature without an external probe.

The independent C parser/serializer/stream module is firmware/dmt99_asic.c/.h;
its boundary is [INTEGRATION.md](firmware/INTEGRATION.md).
Checks include real packets, literal serialization, synthetic numeric
interpretation and 100000-byte streams.
If GCC sanitizer runtimes are missing, install libasan/libubsan; the earlier
session used locally extracted packages, while the consolidated harness also
supports SANITIZER_CC and Clang.
SI prefix conversion covers UI V/A/Ω/F/Hz codes; analog calibration,
untested prefixes/modes and custom-firmware binding remain unvalidated.
`make -C firmware` produces an RV32 archive without connecting to a target.
The original standalone work did not modify the existing firmware project.

Extract regression samples without changing bits:

```bash
python3 tools/export_fixture.py captures/SESSION tests/fixtures/NAME \
  --start-sample 100000 --end-sample 1000000
python3 tools/validate_capture.py captures/SESSION
python3 tools/validate_fixtures.py
python3 tools/update_inventory.py
```

Each excerpt stores sample range, parent hash and metadata snapshot.
Boundary truncation/errors remain visible.
Use --display-approximate for approximate reports; without a tolerance,
exact-value evidence is not silently generated. --display-exact explicitly reverses it.

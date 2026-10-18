# Open-input mode captures — 2026-10-18

Direct original-code checks:
[DECOMPILED_COMPARISON.md](DECOMPILED_COMPARISON.md).

The user declined further capacitor/battery/component connections.
All following captures used open probes without an external source.
Results describe display/protocol state, not calibration or known-input validation.

| User-selected mode | Decoded display | Complete packets | Boundary UART errors |
|---|---|---:|---:|
| Capacitance | 0.014 nF | 59 | 0 |
| Buzzer/continuity | Raw ` .0L `, numeric=None, firmware unit6/V | 61 | 0 |
| DC V | −0.003..0.000 V | 60 | 0 |
| AC V | 0.272..0.273 V | 58 | 1 incomplete ending candidate |
| Hz | 48.40..51.57 Hz, mostly50.03..50.05 | 61 | 0 |
| DC low current, user mA mode | Raw `   0`, unit8/µA, ac_dc_code2 | 60 | 0 |
| AC low current, user mA mode | Raw `   3`, unit8/µA, ac_dc_code1 | 59 | 0 |
| DC A | 0.000 A, ac_dc_code2 | 61 | 0 |
| AC A | 0.001 A, ac_dc_code1 | 60 | 1 initial incomplete fragment |
| Wavy-cable screen consistent with NCV | Raw ` E? `, nonnumeric, host NCV level0 | 61 | 0 |
| LIVE, cable-only screen | Raw `L1UE`, nonnumeric, host active-renderfalse | 61 | 0 |
| Temperature, no external probe, °C selected | Raw `  32`, numeric32, code21; user display32°C | 58 | 0 |

Each recording is10s/1MHz, D0=MCU_TX and D2=ASIC_TX.
Display messages are12-byte5AA5.
The user initially reported capacitance0.015, then confirmed nF; this was
not silently equated to0.014. DC fluctuations and approximately50Hz were reported.
AC/buzzer display text was not separately reported; decoder output is not
written into metadata as if the user had observed it.

All60 DC V packets had ac_dc_code2; all58 complete AC V packets had1.
The original priority is b2bit1 then bit0. Low-current/A captures show
the same DC2/AC1 association. Other modes, analog signal accuracy/calibration
and conflicting flags remain separately unverified.

Resistance open-input raw `" 0.L "` uses unit13/decimal2.
The user-reported buzzer mode gives `" .0L "`, unit6/decimal1.
Both are nonnumeric. Firmware unit V is not changed to an unreported Ω.
Buzzer/diode distinctions and closed-circuit behavior remain incompletely understood.

AC open-input b9..11 are zero. Many other modes have b9=0 and b10..11 giving
BE16 values in the300s. Integer/scale/auxiliary/status meaning is unproven.
The user reported no secondary/small AC reading.
No temperature/frequency meaning is asserted; hypotheses remain in
evidence/opaque_tail_hypotheses.json.

Clean streams were compared with independent sigrok UART decoding.
The incomplete AC ending candidate is preserved, with a separate clean excerpt.
RX checksum remains unknown.

A60s sweep ended after29.532160s/29,532,160samples and was labeled incomplete.
Its end captured MCU0A, ASIC event0A and initial0.000nF messages, supporting
the0A capacitance-UI association. Subsequent modes used separate10s sessions,
without bridging gaps. Early termination remains unexplained;
USB power/control read on and runtime-suspend duration0.

Excerpts are tests/fixtures/real_*open* and real_capacitance_mode_event*.
Each JSON includes parent .sr hash, sample range and metadata snapshot.
Complete raw recordings remain under captures/.

## NCV, LIVE and temperature interpretation limits

For the first NCV request, the user did not know the mode name and described
a cable icon with waves. After one button press, only the cable remained.
The [FNIRSI manual, printed pages16–17](https://www.docs.beti.com.tr/FNIRSI/DMT-99/DMT-99_ing.pdf)
associates these with NCV/LIVE.
The original cable_wave_icon_idle_unconfirmed label is preserved;
later context was added to annotation history.
These fixed-mode recordings did not capture selection TX; command09/13
is not claimed as physically observed there.

Original reconstructed/ui/screen.c counts code16 ('-') among four symbols
for mode09 NCV. For mode13 LIVE, first symbol code14 ('L') suppresses active rendering.
C dmt99_host_ui_status and Python host_ui_status implement this only with
explicit caller-provided host mode.
Open inputs gave NCV0/LIVEfalse. Energized sources, thresholds and field
strength were not tested.
NCV's unknown E4 remains '?'; raw LIVE `L1UE` is not corrected to `LIVE`.
Neither recording is numeric.

User mA headings and packet unit8/µA are separate. Open-input low current
was observed only at0/3, without establishing mA ranges or scale transitions.
Open-input A values0.000/0.001 do not validate current calibration.

The user confirmed °C and display32. Unit21 alone does not prove C/F;
dmt99_value_si continues to reject it.
Original ui/reading.c applies ×1.8+32 for mode12 with Fahrenheit preference;
this is code evidence, not a physical Fahrenheit comparison.
Temperature b10..11=0143..0146, BE16=323..326; closeness to primary32 does
not establish opaque-tail meaning or scaling.

# DMT99 ASIC control recovery

This milestone continues from141 complete /18 partial /128 incomplete non-trap bodies.
Current coverage is **146 complete /14 partial /123 incomplete**, including partials.
The priority is a usable peer driver and original application control flow. None of
these results establish that a physical ASIC has accepted a command.

## Original functions completed

| Entry | Maintained implementation | Evidence scope |
|---|---|---|
| 8ed4 | `foreground_loop`, entry, normal and diagnostic iterations | Complete nonreturning caller through typed external interfaces; original branch/call/state comparisons |
| b56a | `measurement_diagnostic_entry` | All10 bytes of both packets, their order and200ms delay, randomized original stack |
| b5c8 | Controller + connected menu | Regular commands/selectors/relative and real menu entry/return |
| b9e0 | Controller + connected menu/history | Recording commands/Fahrenheit formatting, template-zero TX, menu and extrema reset |
| bc90 | Controller + connected menu | Monitor commands, buzzer off, menu and selector/state reset |

The8ed4 reference contains its original infinite loops. The simulator/controller uses
explicit resumable states, including a diagnostic state, and is not cycle equivalent.
Completing the caller does not complete its settings, ADC, display, delay or other
callee internals. The reference is compiled but never activated by offline main.
Original-entry credit is separate from the new API/example code, which earns none.

Normal flow dispatches buttons before RX, copies16 saved-event bytes despite the ISR
refreshing13, invalidates the shared label cache, respects HOLD, then polls charging
and deferred battery work. The connected controller now invalidates its auto-label
cache when a saved FD frame is consumed as the original does.

Diagnostic entry samples GPIOC PC10, then PC11 only if PC10 is low. Both low select
backlight `(100,4799,0)`, delay1000 and b56a. It bypasses settings load and normal
screen startup. Its loop forwards/debounces keys, skips the first pending FD frame,
then parses the current frame and calls8672 at `(100,100,100,50,32,fd40)` and c11a
at `(200,110)`. It finishes with RX suppression0, so a frame can be rendered again
without a new IRQ. This repeated-render behavior is preserved, not changed into a
new receive queue. No ASIC factory/calibration meaning is assigned to this path.

## Wire evidence

Command IDs, addresses and packet bytes below are hexadecimal; delays are decimal.

USART1 is configured at9600,8N1, TX PA9/RX PA10, no flow control. The sender97c4
writes10 bytes and waits for TC after each byte. C350 configures circular DMA1/channel5
with13 bytes. C468 handles IDLE, rearms DMA before inspecting/copying the buffer,
uses a blank-segment suppression predicate and saves FD events. It does not verify
received byte count, header, sequence or checksum. RXNE is enabled at initialization
but this ISR body services IDLE; actual interrupt/byte timing needs device evidence.

The actual wire packet boundaries, timing, voltage and peer identity remain unmeasured.
The standalone decoder requires an explicit coherent snapshot and length13, checks
5A A5, and does no invented checksum validation. The stock raw ISR/controller contract
remains separately preserved, including header-blind event capture.

### MCU to peer

Ordinary observed constructors emit `AA55 05 CC 0000000000 (CC+05)`. This describes
these zero-payload literals only. Byte2 is called a format tag here; its length/type
meaning is not proven. Byte9 is an observed check byte, not a generalized checksum API.

| ID | Host association / provenance | Check byte |
|---|---|---:|
| 01 | Diagnostic accepted key4,97f8; physical function unknown | 06 |
| 03 | HOLD-related button/event in normal UI; diagnostic key3 | 08 |
| 08 | Diagnostic accepted key2; physical function unknown | 0d |
| 09 | Normal UI NCV path; diagnostic accepted key1 | 0e |
| 0a | Capacitance UI | 0f |
| 0b | Resistance UI; first diagnostic-entry request | 10 |
| 0c /0d | Regular UI dc_v/ac_v resource associations | 11 /12 |
| 0e | Regular UI `ol.jpg`; analog meaning unproven | 13 |
| 0f | Frequency UI | 14 |
| 10 /11 | Regular DC current UI groups | 15 /16 |
| 12 | Temperature UI | 17 |
| 13 | LIVE UI path | 18 |
| 14 | Default/automatic UI request; peer ranging policy unknown | 19 |
| 15 /16 | Regular AC current UI groups | 1a /1b |
| 00 | Legacy fallback/history/relative paths: exact tag05 packet | **11** |
| a0 | Second diagnostic request: exact tag02 packet; meaning unknown | a2 |

B56A emits exactly:

```text
AA 55 05 0B 00 00 00 00 00 10
[delay200ms]
AA 55 02 A0 00 00 00 00 00 A2
```

The legacy00 packet retains11; treating it as an additive checksum and replacing
it with05 would change original wire behavior. A0 must not be encoded as tag05/checka5.
No range-register, calibration, EEPROM, raw-ADC, reset or universal error command has
been recovered. Other command values, payload fields4..8 and generalized encodings
are unsupported. Host UI associations are evidence of displayed behavior, not proof
of analog signal-path selection or ASIC identity.

`analysis/asic_protocol_commands.json` records literal packets and original constructor
addresses. The three button traces exercise actual original169d4 code; original base
and history routines are isolated as qualified external-call boundaries.

### Peer to MCU

Display frames begin5A A5; FD at byte2 selects an event with ID at byte3. A mode-like
FD event is a host notification. No request identifier or correlation proves it is an
ACK; unsolicited, repeated or delayed notifications remain possible. The driver does
not change its reported state optimistically after transmitting a request.

The four LCD symbol inputs are `(b2&f0)|(b3&0f)` through `(b5&f0)|(b6&0f)`.
The segment decoder masks10, uses original symbol tables and preserves unknown codeff.
The sign is b2 bit4; b3/b4/b5 bit4 decimal priority is first/second/third. Status bits
b2 bit1/0 produce2/1/ff in that priority. Unit selection follows the exact priority
tree over bytes6..8. Byte7 bit7 is exposed without inventing a named status meaning.
All13 raw bytes, especially unknown9..12, remain available. There is no guessed RX CRC,
sequence, low-battery/error code or conversion-ready interpretation.

The independent reading parser reproduces the original host's six visible characters,
signed-unknown-symbol `/`, final-symbol numeric flag and context-dependent zero padding.
It also preserves both NUL stores: the seventh `%c` is zero and the formatter writes
another terminator. The old native adapter omitted the second store; the extended
16-byte comparison found and corrected this difference. Display magnitude and unit
code are separate; no inferred SI scaling or calibration is applied. The legacy numeric
flag alone is not proof that every segment is a valid numerical digit.

## Published SDIC comparison

The reviewed [SD7502 SoC sheet](https://sdicmicro.cn/upload/img/2021-08/611627e6a2699.pdf),
[SD7501 SoC sheet](https://sdicmicro.cn/upload/img/2021-08/611cad9c519ad.pdf) and
[SD78P953 SoC sheet](https://sdicmicro.cn/upload/img/2022-11/6371b0eeeaeef.pdf)
describe programmable OTP MCU/ADC devices with UART. They supply no matching DMT99
AA55/5AA5 command table. UART capability alone cannot establish silicon or application
protocol identity. The existing local7502/78P953 PDFs and their hashes are preserved.

A published [SDIC-branded SD7501A6 application sheet, v0,2020/08, section8](https://www.scribd.com/document/683202907/SD7501A6-6000)
describes one-way2400-baud,8-bit/no-parity,15-byte LCD data with high-nibble position
tags1..F. That differs from DMT99's bidirectional9600-baud,10-byte TX/13-byte RX and
AA55/5AA5 headers. It is an application-sheet mirror; an authenticated vendor-hosted
copy was not obtained. It is used only as a bounded protocol comparison, not as chip
identification or source of any new driver command. Inference: custom programmed
SDIC silicon remains possible, but no published protocol match has been established.

## Independent C API and replay

`drivers/asic_driver.h` needs no graphics, filesystem, settings or application objects.
It uses three portable source files and a callback-owned UART. The button-interface
header is a plain C IO type; no UI implementation is linked.

- `asic_request_mode(io,id)` accepts only observed09..16 values.
- `asic_request_hold(io)` emits the03 button action; it does not force HOLD-on.
- `asic_forward_key(io,key)` encodes accepted diagnostic keys1..4; debounce is external.
- `asic_transmit_observed` validates19 known packet forms, including literal legacy00/A0.
- `asic_replay_diagnostic_entry` is explicit opt-in and stops on local transport failure.
- `asic_receive_idle` exposes frame metadata/raw bytes; `asic_reading_update` uses caller-owned mode/numeric/text context.

A true send result means local transport acceptance only. No response is synthesized,
no timeout/retry/queue policy is invented, and mode/HOLD acknowledgement is not asserted.
The existing original void-call diagnostic replay is separately preserved. The optional
compiled WCH callback `recovered_asic_transmit` uses the original per-byte TC polling;
offline executables never initialize MMIO or call it.

```bash
make -C reconstructed asic-test asic-sanitize build/asic-replay
reconstructed/build/asic-replay
```

The replay's UART and received frames are explicit mocks. Its minimal link excludes
the application, GUI, FatFs, JPEG and WCH objects. For simulator diagnostic reproduction:

```bash
make -C reconstructed simulator
reconstructed/build/dmt99-simulator --flash flashdump/dmt99_flash_1.bin
```

Enter these CLI commands on separate lines:

```text
diagnostic
status
button 3
reading 12.34 6
step
```

## UART traces still needed

| Capture context | Existing expected host behavior | What the capture must resolve |
|---|---|---|
| Stock cold boot, each saved view | Regular requests14; recording/monitor request0c | Startup delay, peer readiness, boot notifications, byte/frame gaps |
| Primary/current/other key cycles | Tables above; monitor has its own limited selectors | Actual analog mode/unit transitions and acceptance of each request |
| HOLD twice and mode change while held |03 requests and FD03 host toggles | Reply causality, duplicate notifications, peer/local HOLD interaction |
| Long REL and recording-history key | Legacy00/check11 remains on wire | Whether the peer accepts/ignores that packet and whether it has side effects |
| Natural autorange transitions | Host consumes new unit/decimal flags | Range policy, latency, flags and status code meaning |
| Available original diagnostic session |05/0b then02/a0 with200ms gap | A0 responses/side effects; no calibration interpretation is assumed |
| Raw uninterrupted RX/TX with timestamps | Fixed lengths are configured by host, not measured here | True IDLE boundaries, short/overlong blocks, unknown bytes9..12/check rule |

Also establish physical TX/RX routing, idle polarity/logic levels and actual9600/8N1
bit timing. Capture simultaneous directions and retain raw timestamps/bytes and input
context. No physical capture, hardware access, firmware flashing, bootloader work or
RDP work was performed in this milestone.

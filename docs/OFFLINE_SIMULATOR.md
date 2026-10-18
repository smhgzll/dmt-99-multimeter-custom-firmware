# Offline DMT99 simulator

The simulator runs the maintained C controller, recovered UI functions, configured
FatFs/TJpgDec and real resources from the acquired Winbond image. Its RGB565 canvas
is 320×240. The C PNG exporter writes deterministic RGB8 PNGs; it does not depend on
an image library. The browser interface is a local control panel for that same C
process and framebuffer.

Build and start from the repository root:

```bash
make -C reconstructed simulator
python3 tools/dmt99_simulator.py
```

Open `http://127.0.0.1:9999`. Use the button controls, explicit mode ACK and reading
inputs, or deliver a raw 13-byte packet. The page displays the active profile and
controller state and downloads the actual framebuffer PNG. It requires no remote
service. The Python viewer uses only the standard library.

For a CLI session or deterministic replay:

```bash
reconstructed/build/dmt99-simulator --flash flashdump/dmt99_flash_1.bin
reconstructed/build/dmt99-simulator --flash flashdump/dmt99_flash_1.bin --script examples/regular.sim
```

The script format is one command per line, with `#` comments:

```text
ack 0x0c
reading 12.34 6
button 2
ack 3
snapshot /tmp/dmt99-hold.png
button 6
status
powercycle
status
```

`reading` requires exactly four digits, an optional sign and decimal point after
digit 1, 2 or 3. It constructs an observed segment/flag packet and passes it through
the recovered parser. Units are 6 V, 7 mV, 8 µA, 9 mA, 10 A, 11 Ω, 12 kΩ, 13 MΩ,
15 nF, 16 µF, 17 mF, 18 Hz, 19 kHz, 20 MHz and 21 Celsius. Fahrenheit is a display
preference. No autoranging or analog measurement algorithm is synthesized.

Normal button events are 1 other modes, 2 HOLD, 3 primary modes, 4 current modes,
5 select/record and 6 menu. `button 2 long` invokes the recovered REL policy.
In menus, events 1/2 navigate, 3 enters, 4 goes back, 5 confirms and 6 exits.
An emitted command does not automatically receive a response: `ack MODE` supplies
that external event explicitly. `packet HEX26` delivers any complete raw frame.

Other commands are `step`, `tick COUNT`, `charge 0|1`, `battery 0..100`, `poweroff`,
`poweron`, `powercycle`, `status`, `help` and `quit`. `tick` means TIM3 update
events; the recovered handler performs its 21-update gating. `configure VIEW
LAYOUT ENGLISH` changes the mock boot settings and starts a fresh boot. It is a
simulator fixture, distinct from button/menu input.

Both profiles retain separate build outputs:

```bash
make -C reconstructed CONFIG=STOCK_COMPATIBLE all check simulator
make -C reconstructed CONFIG=CUSTOM_DMT99 all check simulator
python3 tools/dmt99_simulator.py --profile CUSTOM_DMT99 --state /tmp/dmt99-custom.state
```

`STOCK_COMPATIBLE` compiles out the last-mode implementation and never calls its
backend. `CUSTOM_DMT99` remembers the last acknowledged supported measurement mode.
It restores the observed mode command, suppresses the normal default command and
waits for an explicit ACK before changing the displayed mode. Selector positions
follow the restored mode. HOLD, REL, samples and unsupported FD events are not
remembered. Changing the view invalidates a mismatched record; factory reset clears
it. The independent 16-byte record has magic/version/view/mode and CRC32.

The default backends use private memory. `--state PATH` optionally stores a
simulator-owned 280-byte container: 8-byte identity, 256-byte mock settings image
and the separate 16-byte custom record. Saving uses a temporary file and rename;
existing unrelated files are rejected. Stock host-state IO preserves the custom
bytes opaquely without invoking or interpreting the feature. No placement for the
custom record in device Flash has been selected. The original `08040000` constant
is retained only as an address checked by the mock settings interface; it is not
confirmed as a device storage layout.

The acquired SPI file is opened read-only. FatFs receives no write callback.
Screenshots require `.png`, and an existing non-PNG destination is rejected.
The ELF builds remain offline link artifacts and the Makefile has no release BIN
or flashing target. The simulator does not access a meter, ESP32 or debug adapter.

The major scene functions and their primitive-call contracts are recovered.
The menu implementation preserves documented incremental `169d4` transition/cache
ordering for supported layouts and languages. Other historical-language screens and
device rendering timing remain unvalidated. The graph preserves the inherited register
input and preceding sample slot. Resistance scratch bytes that the binary reads
from its stack are explicit inputs, initialized deterministically by this host.
The CP936 fault boundary still prevents one non-ASCII filename from opening.
See [UI evidence](UI_RECOVERY.md) and [scheduling limits](SCHEDULING_SEMANTICS.md).

Example C framebuffer exports are in `docs/simulator_screenshots/`, with hashes and
replay inputs in `analysis/simulator_screenshots.json`. A temporary loopback HTTP
smoke test validated HTML, commands, controller state and the actual PNG response;
its scope/hashes are in `analysis/simulator_http_validation.json`. The C backend and
PNG decoder tests also run without any socket permission. Full browser JavaScript
automation is not claimed.

Phase4 continuation replaces whole-scene menu repaints with the original incremental
transition/cache behavior for supported UI languages/layouts. The C integration test
checks untouched regions, stable-menu no-op behavior, pending BACK, theme changes,
exit cache invalidation and view-specific extrema/selector resets. Tests still use
serialized input at cooperative yield points, not a cycle-accurate MCU model.

The replay `examples/menu-transitions.sim` exports two additional frames from the
current C renderer: [incremental cursor](simulator_screenshots/menu-incremental.png)
and [theme transition](simulator_screenshots/theme-transition.png). Earlier Phase4
screenshots and their hashes remain preserved. Run from the repository root:

```bash
reconstructed/build/dmt99-simulator --flash flashdump/dmt99_flash_1.bin --script examples/menu-transitions.sim
```

The ASIC continuation adds explicit two-key diagnostic boot to the C CLI:

```text
diagnostic
status
button 3
reading 12.34 6
step
```

This clears the panel, bypasses settings/custom restore, replays the exact0b/A0
sequence and advances1200 virtual milliseconds. Diagnostic keys1..4 forward09/08/03/01;
the loop skips its first pending FD and can redraw a frame without a new IRQ. Normal
buttons and menus retain their existing meaning outside this explicit state. All
outgoing packets pass the evidence-only driver validator; no reply is synthesized.
The command named `ack` injects an FD fixture: correlation with a request has not been
proved on the wire. Real baud/IDLE timing and physical peer side effects are outside
this simulator. See [ASIC control and capture requirements](ASIC_CONTROL_REPORT.md).

## DMT99 Studio host tools

`make -C reconstructed studio-tools` builds the simulator pair and `dmt99-jpeg-check`. They are used by
`../dmt99-studio` and are host-only; the meter firmware does not change.

- **`--trace DRAWS.jsonl`** (or the script commands `trace PATH|off` and `marker NAME`) writes one JSON
  object per draw call:
  - JPEG draws: path, position, decoded size and FatFs/TJpgDec results;
  - semantic text, bitmap, fill and border calls of the view layer.

  DMT99 Studio builds its click-to-asset screen map from this trace.
- **`dmt99-jpeg-check IMAGE.jpg [OUT.rgb565]`** decodes one file with the firmware's TJpgDec build and the
  same 0x1c00-byte pool as `resource_draw_jpeg`.
- **`--device-pty [--mcu-image APP.bin]`** serves the management protocol
  (`application/mgmt_protocol.c`, specification in `dmt99-studio/DEVICE_PROTOCOL.md`) on a pseudo
  terminal. Stdin keeps accepting simulator commands.
  - Winbond writes change only the in-memory copy.
  - MCU memory reads answer only from the given application BIN and the settings page; all other
    addresses answer FAULT.

The draw trace showed that the boot logo path `"0:/on.jpg"` opens `1:/ON.JPG`. One FATFS object serves
both drives and is remounted on `1:` right after `0:/LOGO.jpg` is tried. `0:/LOGO.jpg` on the BOOT
volume is the custom-logo override.

> Historical report. Current implementation, hardware evidence and safe test commands: [DEBUG_CAPABILITIES.md](../DEBUG_CAPABILITIES.md). Protocol 3 has no dump/reset command and disables state-changing experiments by default.

# Classic ESP32: flash and first RVSWD identification

The adapter was physically identified by the user using esptool as
**ESP32-D0WD-V3 revision 3.1**, dual-core, up to 240 MHz, 40 MHz crystal, CP2102 on
`/dev/ttyUSB0`. This supersedes the earlier S3/N16R16 assumption. The existing
project was adapted in place; the RVSWD transport remains unchanged.

## Selected pins

| Function | Classic ESP32 | DMT99 header |
| --- | --- | --- |
| SWCLK | **GPIO25** via 330 ohm series resistor | 2: PA14 |
| SWDIO | **GPIO26** via 330 ohm series resistor | 3: PA13 |
| Ground | GND | 1: GND |
| Target VREF | No connection to ESP power outputs | 4: external measurement and target-side pull-ups only |
| UART0 TX / RX | GPIO1 / GPIO3 | CP2102 only, not target |

GPIO16/17 can be tied to PSRAM on classic ESP32 modules. The reported silicon
name does not establish the module's memory wiring; software disabling PSRAM does
not disconnect those wires. GPIO25/26 avoid that conflict, Flash pins, UART0 and
strapping pins. They are exposed even on WROVER modules, but confirm your board
also exposes them and has no attached peripheral on them. See
[Espressif's module datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-wrover-e_esp32-wrover-ie_datasheet_en.pdf).

**Move both signal wires from GPIO16/17 to GPIO25/26 before probing.** Firmware
does not configure GPIO16/17. Add/check the two 4.7 kohm target-powered pull-ups
shown in the [README wiring diagram](../README.md#wiring-and-power).

## Build and flash on Nobara

Use a normal host terminal, not this agent's isolated execution environment: the
agent can see host sysfs metadata but its `/dev` has no USB serial nodes. The
previous connection attempt failed before reaching the ESP32. No claim of a real
CH32 identification response is based on that attempt.

Keep target signals disconnected during flashing/reset, and close serial monitors.
Only the ESP32 adapter is programmed by these commands; the CH32V303 is untouched.

```bash
cd projects/rvswd
export PYTHONDONTWRITEBYTECODE=1
. ${IDF_PATH}/export.sh
export ADAPTER_PORT=/dev/ttyUSB0

idf.py --version
# Must print ESP-IDF v5.5.5.
python -m esptool --chip esp32 --port "$ADAPTER_PORT" --baud 115200 chip_id
python -m esptool --chip esp32 --port "$ADAPTER_PORT" --baud 115200 flash_id
```

`flash_id` must report at least **2 MiB**. The build conservatively declares 2 MiB
instead of assuming the old 16 MiB adapter specification; larger Flash is fine.
PSRAM is disabled. The reported 40 MHz crystal is explicitly selected.

Build in a fresh target-specific build directory inside the existing project:

```bash
idf.py -B build-esp32 -D IDF_TARGET=esp32 -D SDKCONFIG=build-esp32/sdkconfig -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.uart.defaults' build
```

Do not reuse `build`, `build-uart` or `build-native` from the S3 builds. `build-esp32`
has its own `sdkconfig`; the source project and RVSWD implementation are preserved.
The project rejects other chip targets. Use generated flashing arguments rather
than old S3 raw offsets (classic ESP32's bootloader is at `0x1000`).

Once GPIO25/26 exposure and wiring have been physically checked, enable the board
gate and supply a meaningful checked-board label:

```bash
idf.py -B build-esp32 -D SDKCONFIG=build-esp32/sdkconfig menuconfig
# DMT99 read-only RVSWD diagnostic:
#   Actual board schematic/continuity confirms GPIO25 and GPIO26 are free = enabled
#   Board model/revision checked by operator = your checked board/module label
idf.py -B build-esp32 -D SDKCONFIG=build-esp32/sdkconfig build
idf.py -B build-esp32 -D SDKCONFIG=build-esp32/sdkconfig -p "$ADAPTER_PORT" -b 115200 flash
```

This enables only operator-requested identification. Both GPIOs still start in
input mode without pulls and return there after every probe. You may flash with
the gate disabled first to check the console; `info` works in either case.

If automatic ESP32 download mode fails: hold BOOT, press/release EN (RESET), release
BOOT, then run the ESP32 operation. After flashing, release BOOT and reset normally
if needed. CP2102 carries both flashing and the console at **115200, 8N1, no flow
control**. GPIO1/3 remain dedicated to that console.

If the host reports permission denied:

```bash
sudo usermod -aG dialout "$USER"
# Log out and back in before opening the port.
```

If the port is absent, inspect `ls -l /dev/serial/by-id/ /dev/ttyUSB*` and update
`ADAPTER_PORT` if its name changed. Changing baud rate cannot fix a missing device.

## First hardware identification

Check the console first; this does not require a target connection:

```bash
python tools/diag.py --port "$ADAPTER_PORT" --output captures/esp32-info-001.json info
```

The `adapter` object must include:

```json
{
  "adapter_target": "esp32",
  "console": "uart0",
  "baud": 115200,
  "console_tx": 1,
  "console_rx": 3,
  "gpio_clk": 25,
  "gpio_dio": 26,
  "board_verified": true,
  "initialized": true,
  "memory_read": false,
  "dmi_write": false
}
```

This is an expected **subset**, not a captured response. If `board_verified` is
false, finish the board-check/menuconfig step before probing. The host refuses
an old S3 or GPIO16/17 image before issuing `identify`.

Disconnect the DMT99 test leads from all circuits, disconnect its charger/USB and
power it from its own battery. Power the ESP32 from USB. Connect grounds first,
then the verified signal wires and target-powered pull-ups. Do not connect an ESP
power output to target pin 4. With an external meter, measure target pin 4 relative
to pin 1; the accepted window is 3000–3450 mV. Both boards must remain powered.

```bash
read -r -p 'Measured DMT99 pin 4 to pin 1 voltage, in mV: ' TARGET_MV
python tools/diag.py --port "$ADAPTER_PORT" --output captures/esp32-identify-001.json identify --voltage-mv "$TARGET_MV" --board-pins-checked --wiring-checked
```

This sends one bounded eight-sample probe at the existing slow timing. It stops on
the first error, releases the GPIOs, saves the complete response and does not retry
automatically. Choose a fresh filename if a capture already exists. The activated
IDF Python includes pyserial; the previously installed `.venv/bin/python` also works.

Return the complete `esp32-info-001.json` and `esp32-identify-001.json` records for
review. `result: ok` is one coherent CH32V303-family identification session, not
proof of Flash capacity or read permission. An inactive module or lack of response
does not trigger activation, unlock, erase, protection changes or a target write.
No Flash reader is enabled before stable hardware identification is established.

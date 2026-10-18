> Historical report. Current implementation, hardware evidence and safe test commands: [DEBUG_CAPABILITIES.md](../DEBUG_CAPABILITIES.md). Protocol 3 has no dump/reset command and disables state-changing experiments by default.

# Software build and test report

Date: 2026-10-17. No physical target was connected, flashed or read.

The earlier sections record the superseded S3 builds. **Use only the classic
ESP32 build recorded at the end of this file for the physically confirmed adapter.**

| Item | Result |
| --- | --- |
| SDK | ESP-IDF `v5.5.5`, commit `b774170ff46c393eeb5e495ea37936038d3f4f4f` |
| SDK path | `${IDF_PATH}` (unchanged) |
| Compiler | Xtensa GCC 14.2.0, crosstool-NG `esp-14.2.0_20260121` |
| Target | `esp32s3`, DIO/40 MHz, 16 MB Flash, PSRAM disabled |
| Default image | `build/dmt99_rvswd_diag.bin`, 169616 bytes (`0x29690`) |
| Board gate | Disabled; board name `UNVERIFIED` |
| C transport model | Passed, including 5,045 individually injected GPIO operation failures |
| Sanitizers | Clang AddressSanitizer + UndefinedBehaviorSanitizer passed; LeakSanitizer disabled under sandbox tracing |
| Python tests | All 7 passed using the installed IDF Python environment; includes a real pyserial/synthetic pseudoterminal CLI exchange |
| Upstream reference build inclusion | Zero source files from `esp32-component-rvswd/` in compile database |
| Diagnostic library exports | `rv_init`, `rv_release`, `rv_identify`, `rv_result_name`; no target-write/memory/programming API |

The supplied binary was built with the board gate disabled. A separate synthetic
board-enabled configuration was also compiled to exercise the conditional branch;
it is not a verified board profile or a firmware image to flash. The existing
Nicolai reference and SDK worktrees remained clean. The DMT99 reconstruction
repository was not modified.

Both final configurations were confirmed with `cmake --build` returning exit 0.
The last `idf.py` wrapper lingered after binary generation and size checks in the
tool environment and was stopped; the successful CMake/Ninja checks independently
confirm the final build products. Earlier full `idf.py build` runs also completed.

Final artifact SHA-256 values for this build (rebuilding may change ESP-IDF build
metadata and thus the binary hash):

```text
e6a5c0c9d8b319ea5b9af4c5ab58388e7167e243fb5856443877bb0a8d591c33  build/dmt99_rvswd_diag.bin
f5d63a7014b2e1e2992b4d1cbd1db626490e11b0ed884fb7a4fb83e54d4f426b  build/bootloader/bootloader.bin
7f00b6c042a89b15b0cac534f82ed988caf29278ff5700b0c511eb1b5bb7c820  build/partition_table/partition-table.bin
```

Software tests establish the modeled safety policy and host behavior only. Board
pin availability, electrical levels, actual RVSWD behavior on CH32V303, stable
hardware identification and permission to read Flash remain unverified. No target
Flash dump exists from this work. See [VALIDATION.md](VALIDATION.md).

## CP2102 console follow-up

The host kernel enumerates a CP2102 (`10c4:ea60`, `cp210x`) at `ttyUSB0`; no native
ESP32 USB interface was observed. The original firmware console was native USB.
The application now supports the selected IDF console, with UART0 on GPIO43/44
at 115200 baud for the connected bridge and native USB retained as an alternative.
The RVSWD implementation, GPIO interlocks and target read-only policy are unchanged.

| Image | Build result | Application size |
| --- | --- | --- |
| `build-uart/dmt99_rvswd_diag.bin` | ESP-IDF v5.5.5 CMake/Ninja exit 0 | 186736 bytes (`0x2d970`) |
| `build-native/dmt99_rvswd_diag.bin` | ESP-IDF v5.5.5 CMake/Ninja exit 0 | 169648 bytes (`0x296b0`) |

Both configurations have the board gate disabled. No unrelated test infrastructure
was added. UART image SHA-256 for this build:

```text
4bdde6c04aabe6801d0dd7a4d2801deb4a89350f312979abe1add68eb825ab7f  build-uart/dmt99_rvswd_diag.bin
```

The attempted host `info` connection failed with `ENOENT` because this execution
environment exposes no `/dev/ttyUSB0` device node. The complete connection error
is in `captures/uart-connection-001.json`. This is an access limitation, not an
RVSWD protocol failure. No first real RVSWD attempt, stable hardware identification,
memory-reader implementation, Flash acquisition or dump-hash validation occurred.
The exact host commands and required physical checks are in
[HARDWARE_SESSION.md](HARDWARE_SESSION.md).

## Classic ESP32 adaptation: current build

User-confirmed adapter: ESP32-D0WD-V3 rev 3.1, dual-core, 40 MHz crystal, CP2102
on `/dev/ttyUSB0`. The existing project now targets `esp32`, with UART0 TX/RX on
GPIO1/3 and RVSWD SWCLK/SWDIO on GPIO25/26. The new pins avoid possible PSRAM use
of GPIO16/17. The old native USB profile was removed. The default board-verification
gate remains disabled pending the GPIO25/26 board check and rewiring.

| Check | Result |
| --- | --- |
| SDK | Existing ESP-IDF v5.5.5 installation, unchanged |
| Build | `build-esp32`, CMake/Ninja exit 0, project code compiled with warnings as errors |
| Image | `build-esp32/dmt99_rvswd_diag.bin`, 166336 bytes (`0x289c0`) |
| Adapter memory settings | DIO / 40 MHz, conservative 2 MiB Flash configuration, PSRAM disabled |
| Crystal | 40 MHz explicitly selected |
| Image validation | `esptool --chip esp32 image_info`: valid checksum and validation hash |
| Flasher metadata | Chip `esp32`; bootloader offset `0x1000` |
| Existing C transport tests | Passed, including 5,045 injected GPIO failures |
| Existing Python tests | All 7 passed, including serial pseudoterminal capture |
| Sanitizers | Existing Clang AddressSanitizer / UndefinedBehaviorSanitizer option passed, leak detection disabled under tracing |
| Test changes | Existing fixture/attestation data updated for ESP32 and GPIO25/26; no new test infrastructure |

The RVSWD transport and its public header were preserved byte-for-byte. Only board,
console, configuration and host pin checks changed. Their unchanged SHA-256 values:

```text
40a414dd7554040ad18e479f1b9f0e191b4eb53f11e9e0a232b64d8acd45386c  main/rvswd_ro.c
aaec22953caadbe08ec50e16ca7592028417d27d4883e2d412f5f68b00256900  main/rvswd_ro.h
```

Current application image SHA-256:

```text
bfc9a69bdc5f3532aaa300a4609a6a94452ad2c1b262bdafd7013164a8ebcc03  build-esp32/dmt99_rvswd_diag.bin
```

The agent still cannot access `/dev/ttyUSB0` inside its execution environment.
No ESP32 upload or real CH32V303 probe was performed here. No target erase, unlock,
option-byte change, programming or memory access was added. Use the current
[hardware command guide](HARDWARE_SESSION.md) for the first real response.

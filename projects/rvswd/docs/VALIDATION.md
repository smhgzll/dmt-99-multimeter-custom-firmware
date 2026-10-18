> Historical report. Current implementation, hardware evidence and safe test commands: [DEBUG_CAPABILITIES.md](../DEBUG_CAPABILITIES.md). Protocol 3 has no dump/reset command and disables state-changing experiments by default.

# Validation state and hardware gates

This file distinguishes software verification from physical target validation.
No adapter was flashed and no WCH target was contacted during development.

## Completed without hardware

- Cross-compilation for classic ESP32 using ESP-IDF v5.5.5, with project code warnings
  treated as errors. The default board-unverified configuration builds bootloader,
  partition table, ELF and application binary.
- Scripted native C electrical model validates 100-clock synchronization, MSB-first
  addresses, read-only opcode, even parity, auxiliary ownership and terminal clock.
  The test verifies SWDIO is released for all received bits.
- Board, wiring and voltage gates generate no probe clocks. Disconnected all-HIGH
  and all-zero data, wrong/unstable ID, inactive DM, authentication failure, invalid
  status, busy abstract command, parity error, stuck lines and deadline expiry stop
  the probe. GPIO failures are injected at every call position in a successful
  three-sample probe; all must release outputs without retry.
- Host tests cover boot noise, malformed/unrelated input, errors, timeouts, stale
  request IDs, argument validation and exact-size offline SHA-256 comparisons,
  including mismatch and same-file rejection.
- The actual host CLI is exercised through pyserial against a synthetic POSIX
  pseudoterminal. It captures a tagged failure result and sends no automatic retry.
  This test requires pyserial; it ran using the installed IDF Python environment.

Run `CC=cc bash tests/run.sh` from the project root. An available Clang sanitizer
build can be run with:

```bash
ASAN_OPTIONS=detect_leaks=0 CC=clang SANITIZE=1 bash tests/run.sh
```

AddressSanitizer and UndefinedBehaviorSanitizer were used in this environment;
LeakSanitizer is disabled because the tool sandbox runs under tracing. GCC's
sanitizer shared libraries were missing here, so the sanitized run used Clang.
These tests model the documented assumptions; they cannot establish actual WCH
edge timing, voltage levels, output contention or protection behavior.

## Gate 1: verify the actual board and electrical startup

Record the board manufacturer, model/revision, module marking, schematic URL and
continuity results for IO25 and IO26. UART0/CP2102 is on GPIO1/3. GPIO16/17 may be
reserved for PSRAM and are deliberately unused. Keep driving disabled until both
selected debug pins are exposed and unused. The user confirmed ESP32-D0WD-V3 r3.1;
that silicon identity alone does not establish module wiring. Keep VREF away from
ESP power outputs.

With target signals initially disconnected, inspect reset/boot/USB-enumeration
behavior and confirm the application releases the pins without pulls. Then follow
the README power/wiring procedure. Check target rail, LOW voltage, idle levels and
rise times. Firmware input mode cannot prevent externally powered pins from
back-feeding an unpowered ESP32 through its protection structures.

## Gate 2: stable identification only

Run one explicit identify capture at the default timing. Check the trace contains
the synchronization sequence followed exclusively by allowlisted READ frames.
Observe data ownership, phase selection and terminal clock. On any error, inspect
the saved raw result before a new manual attempt. Do not infer RDP from all-HIGH,
all-zero, parity failures or lack of communication. Do not use a programmer's
unbrick/unprotect/automatic-recovery command.

Require multiple separately initiated sessions with eight parity-clean samples
each, identical 0x7f/HARTINFO/CPBR values and plausible DMSTATUS. Repeat after normal
target power cycles, reconnecting safely. Correlate the family hint with the chip
marking; the direct register is not full package/capacity identification. Save
settings, raw traces and voltage measurements alongside each capture.

`ok` only qualifies one software probe. A human must review electrical and repeated
session evidence before marking CH32V303 identification validated. No success path
in this firmware transitions to memory access.

## Gate 3: permitted read-only memory access, future revision

This stage is **not implemented** because gate 2 cannot be completed here. It is
not enabled by changing Kconfig or passing a host flag. Before adding a reader:

1. Establish the exact part/capacity and current protection state through verified,
   permitted reads and authoritative target documentation. Stop on denial, locked
   state, unknown protection semantics or missing evidence. Do not change option
   bytes or disable read protection.
2. Verify an available memory-read mechanism on this exact debug module. Standard
   system-bus access must not be assumed. The reference program-buffer approach
   requires debug/CPU writes and hart control, so it needs a separate, narrowly
   reviewed implementation and must never expose arbitrary DMI writes or execution.
   No write/erase/unlock function may become reachable in the diagnostic build.
3. Bound addresses to `0x08000000` through `0x0803ffff`, with a required total of
   262144 bytes. Stop at any address/protocol error. Do not read MMIO, option bytes,
   bootloader or other ranges as a fallback. Verify that the full 256 KiB is
   actually mapped in the existing configuration without changing it.
4. Acquire pass 1 and pass 2 as **separate target reads**, in ordered chunks (for
   example, 1024 bytes), with pass number, address, sequence, length and chunk
   integrity in the serial format. The host must reject missing/duplicate/out-of-
   order chunks and leave partial files clearly marked as incomplete. Use flow
   control/host acknowledgments and finite timeouts.
5. Hash each complete pass independently on the host. Require matching SHA-256
   values and exact lengths; retain both original files and metadata. Check that
   the content is credible rather than uniform fault data. A hash match proves
   repeatability of bytes, not authorization, identity or authenticity.

Only a later verified reader can satisfy acquisition requirements 5, 8 and 9.
The current offline comparison helper supplies the hash/length check without
creating misleading dump files or sending speculative memory commands.

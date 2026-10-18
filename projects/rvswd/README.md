# DMT99 CH32V303 read-only diagnostic probe

Unified workspace: source is `projects/rvswd/`, captures/recovery evidence is
under `artifacts/captures/rvswd-*`. The latest [recovery report](FLASH_RECOVERY_REPORT.md)
supersedes earlier identification-only claims: **zero original Flash bytes recovered**,
one 4-byte SRAM word repeated successfully. [Shared build guide](../../docs/BUILDING.md)
and [risks](../../docs/LEGAL_AND_SAFETY.md). No hardware commands are run by the workspace build.

Classic ESP32 / ESP-IDF **v5.5.5**, CP2102 UART0 at 115200 8N1.
RVSWD uses **GPIO25 SWCLK / GPIO26 SWDIO**, UART0 uses GPIO1/3.

Current results, historical hardware evidence, connection steps and safe commands
are in **[DEBUG_CAPABILITIES.md](DEBUG_CAPABILITIES.md)**. That report supersedes
the earlier identification-only README and historical build/session instructions.

The existing captures establish a CH32V303-family chip ID and active RDP. This
revision adds bounded diagnostics for DMI status, optional DPC/GPR reads on an
already halted hart, and fixed SRAM/GPIO/USART/SPI snapshots when SBA is supported.
The new diagnostic engine has been tested offline and built for ESP32; it has not
been run against the physical target in this change.

Default `capabilities` performs only DMI reads. Snapshot flags explicitly select
read-only target accesses and may write debug command/address registers. They
never write target memory or implicitly halt/resume the CPU. State-changing
experiments have a separate build gate, disabled by default, and explicit runtime
confirmation. There is no reset, dump, programming or unlock command.

```bash
CC=cc bash tests/run.sh
ASAN_OPTIONS=detect_leaks=0 CC=clang SANITIZE=1 bash tests/run.sh
python3 tools/diag.py --help
```

For hardware commands, first follow the report's wiring, measured-voltage and
protocol-3 adapter checks. Target signals must be disconnected during adapter
flashing/reset. Power the meter from its battery, connect ground first, and never
connect ESP32 power outputs to the target. Both open-drain signals need target-side
pull-ups; internal pull-ups are disabled.

The original `esp32-component-rvswd/` programmer reference remains excluded from
the build. Historical Flash reader source is compiled only with the offline-test
macro and has no UART route. Existing captures are preserved.
# Firmware recovery investigation

Current research, test/build results and the unprotected-spare prerequisite:
[FLASH_RECOVERY_REPORT.md](FLASH_RECOVERY_REPORT.md). `tools/recovery.py` audits
evidence, checks adapter compatibility, and runs the separately gated spare-only
Access Memory experiment. The normal build does not enable it. No original MCU
Flash bytes have been recovered by this experiment.


# Maintained C reconstruction, firmware and offline simulator

Current workspace guides: [build](../docs/BUILDING.md),
[Windows USB installation/rollback](../docs/UPDATING.md), [themes](../docs/THEMES.md).
New releases go to `artifacts/firmware/potat-99/`; the old `final/` path is
a relative compatibility link. Consolidation does not change application/ASIC behavior.

`make release` builds the USB update image (same format as the V2.4 update
file, installed by the meter's own bootloader) and publishes it to
[`final/vX.Y.Z/`](../final/README.md). `make firmware-all` builds every image
described in [FIRMWARE_RELEASE.md](../docs/FIRMWARE_RELEASE.md):
`build/firmware-update/` (release), `build/firmware-update-parity/` (About label
kept at V2.4 for differentials), the standalone `build/firmware/dmt99-reconstructed.*`
with its `firmware-readonly/` and `firmware-bringup/` variants, and the V2.4
restore image. `FW_LAYOUT=app|standalone` selects the layout for custom builds.
There are no programming targets.

The same application C also builds the host simulator, the host tests and
the earlier offline replay ELFs:

```bash
make -C reconstructed CONFIG=STOCK_COMPATIBLE all check simulator firmware-all
make -C reconstructed CONFIG=CUSTOM_DMT99 all check simulator
python3 tools/dmt99_simulator.py
```

Source organization:

- `application/`: controller, command policies, formatting, scheduling, power and the
  separate custom last-mode record interface. The small `foreground.c` reference remains.
- `measurement/`: frame/event/value handling, threshold editing and alarm policy.
- `ui/reading.c`, `graph.c`, `gauge.c`: recovered `9caa`, extrema, strip/ring rendering and gauges.
- `ui/screen.c`, `menu_scene.c`, `settings_scene.c`, `units.c`, `monitor.c`, `history_view.c`:
  connected base/mode/menu/settings, unit-cache, monitor and history rendering.
- `ui/resources.c`, `resource_volume.c`, `numeric_view.c`: acquired resources and both numeric paths.
- `ui/graphics.c`, `chinese.c`, `font_rom.c`, `bitmap_images.c`, `scene_rom.c`: recovered drawing and ROM assets.
- `storage/`: history/settings interfaces, shared FAT adapter and explicit CP936 fault boundary.
- `simulator/`: host-only engine, CLI and deterministic PNG exporter; excluded from the RV32 image.
- `drivers/`, `platform/`: recovered transports and the SDK peripheral set-up routines
  (UART/DMA, SPI, keys, timers, PWM, NVIC encoding of the V2.4 image).
- `firmware/`: target-only code — startup and vectors, linker scripts (`dmt99-app.ld`
  update image at 0x6400, `dmt99.ld` standalone, shared `dmt99-sections.ld`), clocks, delays,
  debug console, battery/charger, settings page backend, SPI Flash/FatFs start-up,
  panel port, interrupt handlers, fault record and the firmware `main`.
- `vendor/`: pinned, licensed official WCH, FatFs R0.14b and TJpgDec R0.01b sources.

Application IO exposes declared dependencies and unresolved original addresses;
on the target only `0xde12` (logo removal) is reached and it is bound. The
menu observer, cooperative blocking-loop states and the power-on cache values
are checked against the V2.4 image in the board emulator
(`tools/test_original_differential.py`). CP936 file names fail closed as in
the original. `STOCK_COMPATIBLE` compiles out the experimental last-mode
feature; `CUSTOM_DMT99` keeps it behind an injected backend that has no device
storage location yet. See [simulator usage](../docs/OFFLINE_SIMULATOR.md),
[UI evidence](../docs/UI_RECOVERY.md) and [firmware verification](../docs/FIRMWARE_VERIFICATION.md).

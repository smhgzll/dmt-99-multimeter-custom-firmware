# Next steps

Release 0.2.0 is complete as far as the V2.4 application is concerned and
verified up to emulator level. It installs through the meter's own USB
bootloader, so no RDP removal or debug probe is needed.

1. **First install (your decision).** Re-install the W25Q64JV and panel, confirm
   V2.4 works, then follow [final/README.md](../final/README.md) and the
   checklist in [FIRMWARE_RELEASE.md](FIRMWARE_RELEASE.md). Note what the
   bootloader does with the file (accepted name, messages, file left on the
   disk); that settles its unknown rules. A PA2 console log helps if anything
   goes wrong. Fixes found on hardware become 0.2.x releases; confirmed parity
   becomes 1.0.0.
2. **Measurement-IC capture.** The bring-up image logs every received frame and
   sent command with timestamps. Use it (and a logic analyser on PA9/PA10) to
   settle command acceptance, frame bytes 9–12, the legacy zero command and the
   monitor-view resend behaviour before any protocol changes.
3. **Electrical checks.** Panel write-strobe timing, 18 MHz SPI, ADC divider and
   the 18 MHz ADC clock inherited from V2.4, buzzer/backlight polarity, power
   latch with charger attached.
4. **USB.** Updates keep using the manufacturer bootloader. The application
   itself has no USB function, as in V2.4.
5. **Optional robustness after parity is confirmed:** watchdog, WFI in idle,
   CRC on the settings page — all deliberately absent now to stay with V2.4.

Re-run before every release: `make -C reconstructed firmware-all all check`,
`tools/test_startup_differential.py`, `tools/test_update_image.py --version <v>`,
`tools/test_firmware_emulation.py --image <parity BIN>`,
`tools/test_original_differential.py --image <parity BIN> --report analysis/original_differential.json`,
then `make -C reconstructed release`.

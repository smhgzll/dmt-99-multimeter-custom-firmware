# USB storage investigation

The subsequent [firmware dump feasibility audit](FIRMWARE_DUMP_FEASIBILITY.md) traces
the application's internal settings writes and compares WCH RDP/boot documentation.
It does not recover an updater or active MSC implementation. The candidate export
channel is USART2 TX; no USB endpoint implementation or device payload was added.

No valid device/configuration/interface descriptor set is identified in V2.4, the
surviving carved application or the8MiB SPI image. `tools/inspect_usb.py` validates
candidate descriptor lengths, USB version, endpoint0 size, configuration bounds and
interface counts; it does not classify incidental bytes as an active USB stack.
Results and the empty recovered SCSI-handler map are in `usb_descriptor_report.json`
and `usb_msc_command_map.json`.

The original function/MMIO inventory does not establish a USB MSC handler. Default
USB-related vector loops do not implement a transport. The internal region below
application origin6400 is absent from the supplied update BIN, so a separate internal
bootloader remains a plausible location, **not a confirmed finding**. The carved
application is another application build, not demonstrated bootloader code. Windows
host executables in unused SPI space likewise do not establish device USB support.
No WCH sample stack or descriptor VID/PID is assigned without executable evidence.

The reports therefore cannot attribute the reported Linux/macOS failures. Static
leads worth testing against a future bootloader/capture are: MBR type07 over FAT12,
4096-byte physical-sector units, exported READ CAPACITY block geometry, failed-command
sense data, BOT residue/direction handling and reset recovery. The original FatFs disk
adapter also truncates16-sector transfer length to0, but whether USB shares that path
is unknown. None of these is reported as a confirmed operating-system-specific bug.

The separate read-only `usb_storage_reference.py` is a **reference oracle**, with no
original function-address coverage and no claim to reconstruct the missing transport.
It models readiness, inquiry, sense, capacity, READ10, write-protect errors, removal and
sync; the transport mock checks31-byte CBWs, tags, residues and13-byte CSWs. It has no
physical USB API or image-write method. Its default4096x2048 geometry comes from the
application's block adapter; actual USB-exported geometry remains unknown. Five tests
cover real-image reads, bounds, sense/errors, malformed wrappers and read-only behavior.
It is intentionally narrower than a complete BOT endpoint/stall/reset state machine.

The reference framing follows [USB-IF Bulk-Only Transport1.0](https://www.usb.org/sites/default/files/usbmassbulk_10.pdf);
capacity/read expectations use the [T10 SBC working draft](https://t10.org/ftp/t10/document.05/05-344r0.pdf).
Actual device behavior, update file recognition and interoperability require the
missing bootloader image or physical USB captures. No hardware has been modified.

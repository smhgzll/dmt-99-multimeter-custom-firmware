# POTAT-99 management protocol, version 1

This is the link between DMT99 Studio and a device.

| Part | Where | Status |
|---|---|---|
| Protocol logic (C) | `dmt_99_analysis/reconstructed/application/mgmt_protocol.[ch]` | Implemented and tested |
| Simulated device | `dmt99-simulator --device-pty` | Implemented and tested |
| Meter firmware transport (USB) | — | **Not implemented.** Firmware 0.3.x does not answer this protocol. |

The C logic is transport independent. The simulated device serves it on a pseudo terminal. It runs the
firmware's own UI and controller code, with a memory copy of a flash image.

## Framing

All integers are little endian. `crc32` is IEEE 802.3, as in `zlib.crc32`, over every byte before it.

```
Request   D9 9A | ver u8 | cmd u8 | seq u16 | len u16 | payload[len ≤ 512] | crc32
Response  D9 9B | ver u8 | cmd u8 | seq u16 | status u8 | len u16 | payload[len ≤ 5120] | crc32
```

- **Version.** The version is 1.
- **Responses.** A response echoes the command and the sequence number of its request.
- **Bad frames.** If a request has a bad CRC, an oversized length or the wrong version, the device answers
  with status 1 (`FRAME`) and no payload, then resynchronises on the next `D9 9A`.
- **Partial frames.** A partial request is dropped after 1 s.
- **Measurement events.** These are unsolicited frames that use the response layout, with cmd `0x23` and
  their own sequence counter.

## Status codes

| Code | Name | Meaning |
|---|---|---|
| 0 | OK | |
| 1 | FRAME | CRC, length or version error |
| 2 | COMMAND | Unknown command, or not supported by this device |
| 3 | ARGUMENT | Malformed payload |
| 4 | RANGE | Address or length outside the allowed region |
| 5 | DENIED | Write outside the assets partition, program before erase, or a page crossing |
| 6 | BUSY | A write session is already open |
| 7 | IO | The flash driver reported an error or timeout |
| 8 | VERIFY | Programmed data did not read back |
| 9 | UNAVAILABLE | No measurement (charging screen, menu, no frame yet) |
| 10 | FAULT | Region not readable; nothing is returned or invented |
| 11 | SESSION | No valid write session (wrong token, closed, or timed out after 15 s) |

## Commands

| Cmd | Name | Request payload | Response payload |
|---|---|---|---|
| 0x00 | PING | Any bytes (≤512) | The same bytes |
| 0x01 | GET_DEVICE_INFO | — | TLV list (below) |
| 0x02 | GET_FIRMWARE_VERSION | — | TLV tags 1–7, 13 |
| 0x03 | GET_MEMORY_MAP | — | Region records (below) |
| 0x04 | GET_CAPABILITIES | — | `caps u32, max_read u16, max_request u16, max_program u16, protocol u8` |
| 0x10 | GET_SPI_FLASH_ID | — | `jedec u32, capacity u32` |
| 0x11 | READ_SPI_FLASH | `address u32, length u16 (≤4096)` | Data |
| 0x12 | READ_SPI_PARTITION | `index u8 (0 MBR area, 1 BOOT, 2 assets), offset u32, length u16` | Data |
| 0x13 | VERIFY_SPI_DATA | `address u32, length u32` | `crc32 u32, length u32` |
| 0x14 | WRITE_SPI_ASSET_REGION | Sub-operation (below) | Per sub-operation |
| 0x20 | GET_CURRENT_MEASUREMENT | — | Measurement record (status 9 when unavailable) |
| 0x21 | START_MEASUREMENT_STREAM | `interval_ms u16` (0 = every new frame, else 50–5000) | — |
| 0x22 | STOP_MEASUREMENT_STREAM | — | — |
| 0x23 | MEASUREMENT_EVENT | (device → host only) | Measurement record |
| 0x30 | GET_SETTINGS | — | 256-byte settings image (RAM copy) |
| 0x31 | SET_SETTINGS | `field u8, value u16` | — |
| 0x40 | READ_ACCESSIBLE_INTERNAL_FLASH | `address u32, length u16`, inside 0x08000000–0x0807FFFF | Data, or status 10 |
| 0x41 | READ_ACCESSIBLE_BOOT_REGION | `region u8, offset u32, length u16` | Data, or status 10 |
| 0x42 | EXPORT_CONFIGURATION | — | Sections `id u8, len u16, data` (below) |

### Device info TLV

Each entry is `tag u8, len u8, value`. Numbers are little endian.

| Tags | Fields |
|---|---|
| 1–4 | product, version, git, build date (strings) |
| 5–7 | load address, image size, image CRC32 |
| 8–9 | chip ID, 12-byte UID |
| 10–12 | flash KiB, JEDEC ID, SPI capacity |
| 13–16 | protocol, capabilities, max read, max request |
| 17–22 | run state, clock source, uptime (ms), charging, battery %, RDP flag |

Capability bits:

| Bit | Capability |
|---|---|
| 0 | SPI read |
| 1 | Asset write |
| 2 | Measurement |
| 3 | Stream |
| 4 | Settings get |
| 5 | Settings set |
| 6 | Internal read |
| 7 | Boot read |
| 8 | Export |

### Memory map record

A record is `space u8 (0 MCU, 1 SPI), flags u8, start u32, length u32, name_len u8, name`.

Flags:

| Bit | Meaning |
|---|---|
| 0 | Readable |
| 1 | Writable over this protocol |
| 2 | Reads are fault-guarded |

Reported regions:

| Space | Regions |
|---|---|
| MCU | `code-flash-window`, `code-flash-nominal`, `vendor-bootloader`, `application`, `settings-page`, `system-boot-area`, `esig`, `option-bytes`, `sram` (no access) |
| SPI | `winbond`, `mbr-reserved`, `boot-fat12`, `assets-fat12` (the only writable region) |

### Write session (cmd 0x14)

The first payload byte selects the sub-operation.

| Op | Payload after op | Effect |
|---|---|---|
| 1 BEGIN | `"ASET"` | Opens the session and enters maintenance: the UI stops reading the Winbond. Returns `token u32, first_sector u16 (463), sectors u16 (1585)`. |
| 2 ERASE | `token u32, sector u16` | Erases one 4 KiB sector. Only sectors 463..2047 are accepted. |
| 3 PROGRAM | `token u32, address u32, data (1..256)` | Programs within one 256-byte page of the sector erased last, then reads it back. |
| 4 END | `token u32` | Closes the session, remounts FatFs and repaints. |

The host must not trust its own copy of the device. DMT99 Studio does the following:

1. Reads the device's MBR and assets partition, and saves them as a backup verified by the device CRC.
2. Applies the edits to that data.
3. Validates the FAT12 volume.
4. Writes data sectors first, then FAT and directory sectors.
5. Checks every sector with VERIFY_SPI_DATA.

The session ends by itself after 15 s without a write command.

### Measurement record (50 bytes)

```
frame_seq u32, frame_age_ms u32,
run_state, view, layout, mode, unit, numeric, hold, relative, ac_dc, status_high, charging, battery (u8 each),
value f32, relative_value f32, text[8] (LCD text as decoded by the firmware), frame[13] (raw newest frame), pad u8
```

- **Source.** The values are the firmware's decoded controller state. There is no second ASIC parser.
- **Mode events.** `frame[2] == 0xFD` marks a mode event frame (`AA 55 FD id ...`), not a display reading.
- **Units.**

| Code | Unit | Code | Unit | Code | Unit |
|---|---|---|---|---|---|
| 6 | V | 11 | Ω | 17 | mF |
| 7 | mV | 12 | kΩ | 18 | Hz |
| 8 | µA | 13 | MΩ | 19 | kHz |
| 9 | mA | 15 | nF | 20 | MHz |
| 10 | A | 16 | µF | 21 | °C |

- **Mode IDs.**

| ID | Mode | ID | Mode | ID | Mode |
|---|---|---|---|---|---|
| 09 | NCV | 0E | continuity | 13 | LIVE |
| 0A | capacitance | 0F | frequency | 14 | AUTO |
| 0B | resistance | 10 | DC A | 15 | AC A |
| 0C | DC V | 11 | DC mA | 16 | AC mA |
| 0D | AC V | 12 | temperature | | |

- **Charging.** While the firmware shows the charging lock screen, the record is sent with status 9.

### Settings fields (cmd 0x31)

Values are written to the RAM settings image. They are stored by the firmware's normal power-off save, as
a menu change would be.

| Field | Values |
|---|---|
| 1 language | 0 Chinese, 1 English |
| 2 backlight | 0..3 |
| 3 Fahrenheit | 0/1 |
| 4 auto-off | 0 never, 1 5 min, 2 15 min, 3 30 min |
| 5 view | 1..3, applied at next start |
| 6 layout | 0/1, applied at next start |

### Boot regions (cmd 0x41)

| Region | Range |
|---|---|
| 0 | FNIRSI bootloader, 0x08000000 + 0x6400 |
| 1 | WCH system boot area, 0x1FFF8000 + 0x7000 |
| 2 | Option bytes, 0x1FFFF800 + 16 |
| 3 | ESIG, 0x1FFFF7E0 + 32 |

These reads only report bytes that the reader actually returned. The simulated device answers `FAULT` for
everything except the application BIN given with `--mcu-image` and the settings page. It never invents
bootloader bytes.

### Export sections (cmd 0x42)

| Section | Content |
|---|---|
| 1 | Device info TLV |
| 2 | Settings image |
| 3 | Option bytes (if readable) |
| 4 | ESIG (if readable) |
| 5 | Memory map |

## Simulated device

```bash
make -C ../dmt_99_analysis/reconstructed studio-tools
../dmt_99_analysis/reconstructed/build/dmt99-simulator-release --flash IMAGE.bin --device-pty \
    [--mcu-image ../dmt_99_analysis/final/v0.3.1/DMT-99_V0.3.1_20261018.bin]
```

- **Startup.** It prints `{"device_pty": "/dev/pts/N"}`. Connect to that path like a serial port.
- **Input.** Simulator commands on stdin inject packets, keys and charging state: `ack 0x0c`,
  `reading 12.34 6`, `button 3`, `charge 1`.
- **Flash safety.** Writes change only the in-memory copy; the image file is never written.

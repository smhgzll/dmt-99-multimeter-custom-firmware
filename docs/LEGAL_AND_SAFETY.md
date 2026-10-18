# Independence, usage limits and liability

This project is not affiliated with FNIRSI and is not firmware or tooling
prepared, endorsed, certified or supported by FNIRSI. Product and trademark
names identify the examined device and reference sources only.
The rights of FNIRSI, WCH, Espressif, id Software and other owners are reserved.

## Warranty and liability

The source, documents, BINs, themes and tools are experimental and supplied
**as is**. To the extent permitted by applicable law, no warranty is given
for operation, fitness for a particular purpose, measurement accuracy, data
integrity or non-infringement. Authors and contributors accept no liability
for device failure, loss of bootloader/calibration/settings/data, incorrect
measurements, property damage, business losses or direct/indirect damage
resulting from use. The decision to use, modify or install these materials
and the responsibility for backups belong to the user.

This notice does not claim to exclude rights or liabilities that cannot be
excluded by law. Third-party licenses have their own warranty provisions.
This document is not legal advice.

## Electrical and device risks

- Custom firmware does not recertify the manufacturer's safety rating or
  calibration. Do not use it for safety-related, critical or mains measurements.
- Logic analyzer, USB-UART and ESP32 GND connections can connect computer
  ground to the measurement circuit. Do not measure energized or mains
  circuits with the device open or debug wiring attached; disconnect probes.
- Do not connect 5 V or ESP32 power outputs to target pins. The analyzer is
  passive input only, with VCC disconnected. RVSWD voltage, open-drain and
  pull-up requirements are specified in the board report.
- Wrong images/addresses, interrupted power, incorrect Flash geometry or FAT
  writes can damage resources/settings/bootloader. Emulator tests cannot prevent this.
- Removing RDP can mass-erase protected Flash. No backup of the original
  first 25 KiB is available, so this is not a reversible update route.
- Historical lab commands are not blanket authorization or recommendations
  to repeat experiments. The default RVSWD build leaves experimental gates closed.

## Research and redistribution

Work only on devices/data you own or are explicitly authorized to examine.
Reverse engineering, protection mechanisms, copyright and redistribution
rules vary by jurisdiction and contract; this repository grants no blanket
permission. Do not publish calibration, ownership or identity data.

Rights in original firmware, extracted JPEGs/fonts, decompilation, game data
and patented/copyrighted components are separate from open-source code
licenses. Presence in this workspace is not permission to relicense or
redistribute those materials freely.
Consult [license scope](../LICENSE.md) and [third-party notices](../THIRD_PARTY_NOTICES.md).

DOOM adaptation code is AGPL-3.0-or-later. Applicable Corresponding Source
obligations for distribution and, where relevant, network use are described
in [GNU AGPL](https://www.gnu.org/licenses/agpl-3.0.en.html).
This code license grants no rights to id Software WAD/maps/textures/sprites.

The default `.gitignore` excludes manufacturer images, raw Flash dumps,
complete sessions, DOOM data and binary/debug outputs. Local files are retained.
This filter is not a substitute for a full legal or personal-data review.

## Evidence limits

Physical UART captures and RVSWD diagnostic sessions exist. They do not
validate every custom firmware release on hardware. A correctly linked BIN
does not prove updater erase behavior. A successful VM copy is not a universal
recovery guarantee. No successful RDP bypass or original Flash recovery is claimed.

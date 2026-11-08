# Licensing

The project's own code and documentation are released under the
[MIT license](LICENSE). Some directories and files keep their own licenses,
and material owned by third parties is not relicensed.

| Content | License / notice |
|---|---|
| POTAT-99 firmware sources (`reconstructed/`, except `vendor/`), DMT99 Studio (`projects/studio/`), ASIC parser (`projects/asic/`), `tools/`, tests and documentation | [MIT](LICENSE) |
| DOOM adaptation (`projects/doom/`) | [AGPL-3.0-or-later](projects/doom/LICENSE), [NOTICE](projects/doom/NOTICE.md); source and build scripts are kept together |
| ESP32 RVSWD project (`projects/rvswd/`) | [MIT](projects/rvswd/LICENSE); upstream attribution in [NOTICE](projects/rvswd/NOTICE.md) |
| WCH SDK (`reconstructed/vendor/wch/`) | WCH use conditions in the source headers; [example](reconstructed/vendor/wch/EVT/EXAM/SRC/Peripheral/src/ch32v30x_gpio.c) |
| FatFs / TJpgDec | ChaN notices in their source headers; [FatFs recovery](reconstructed/vendor/fatfs/RECOVERY.md), [TJpgDec recovery](reconstructed/vendor/tjpgd/RECOVERY.md) |
| FNIRSI BIN, bootloader, external Flash dump, original images/fonts and data derived from them | Rights stay with their owners; inclusion for research is not permission for unrestricted redistribution |
| DOOM WAD, maps, textures and sprites | Game data of id Software / the relevant rights holders; neither MIT nor AGPL covers it |

## Why MIT

- The firmware links the WCH SDK, whose headers restrict use to WCH
  microcontrollers. A GPL license on the firmware would conflict with that
  restriction; MIT does not.
- FatFs and TJpgDec use BSD-style notices, which MIT is compatible with.
- MIT code can be combined with the AGPL DOOM adaptation; the combined DOOM
  image is then distributed under the AGPL's terms.

## What the MIT grant does not cover

The firmware is a reconstruction of the behaviour of FNIRSI's V2.4
application. The MIT license grants only the rights the contributors hold in
their own work. It does not grant rights in FNIRSI's firmware, bootloader,
images, fonts, trademarks or other material, and it does not override the
third-party licenses listed above.

## Distributing binaries

- **POTAT-99 BIN:** keep the MIT notice and the third-party notices (WCH,
  ChaN) with the BIN.
- **DOOM BIN:** this is AGPL-3.0-or-later. Providing the BIN alone does not
  satisfy the Corresponding Source obligations; see
  [the license](https://www.gnu.org/licenses/agpl-3.0.en.html). Do not
  bundle the game data unless you have the right to.

Data with uncertain redistribution rights is excluded from Git by default.
This page is not legal advice and not a certificate of compliance.
Warranty and liability: [LEGAL_AND_SAFETY.md](docs/LEGAL_AND_SAFETY.md).
Third-party attributions: [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

# License scope

This consolidated research repository has not been assigned a new blanket
license covering every file. Moving files does not change their existing rights.

| Content | Applicable notice |
|---|---|
| DOOM adaptation code | [AGPL-3.0-or-later](projects/doom/LICENSE), [NOTICE](projects/doom/NOTICE.md); source and build scripts are retained together |
| ESP32 RVSWD project | [MIT license](projects/rvswd/LICENSE); upstream attribution in [NOTICE](projects/rvswd/NOTICE.md) |
| WCH SDK | WCH-microcontroller use conditions in the source headers; [example notice](reconstructed/vendor/wch/EVT/EXAM/SRC/Peripheral/src/ch32v30x_gpio.c) |
| FatFs / TJpgDec | ChaN notices in their source headers; [FatFs recovery](reconstructed/vendor/fatfs/RECOVERY.md), [TJpgDec recovery](reconstructed/vendor/tjpgd/RECOVERY.md) |
| Other reconstruction, Studio and ASIC sources | Any existing file-level license applies; files without an explicit grant do not automatically acquire MIT/GPL permissions |
| FNIRSI BIN, external Flash content, original images/fonts and derived data | Rights remain with their respective owners; research availability is not permission for unrestricted redistribution |
| DOOM WAD, maps, textures and sprites | Game data belonging to id Software/the relevant rights holders; the engine code's AGPL does not cover these assets |

Check the applicable source notices and data rights before distributing.
Data with uncertain redistribution rights is excluded from Git by default.
This policy is not a certificate of legal compliance.

For applicable Corresponding Source obligations when distributing AGPL code
or binaries, consult [the license itself](https://www.gnu.org/licenses/agpl-3.0.en.html).
Providing a BIN alone does not automatically satisfy those obligations.
Placing other files alongside it does not establish license compatibility;
distribution of a combined image requires separate consideration.

Warranty and liability: [LEGAL_AND_SAFETY.md](docs/LEGAL_AND_SAFETY.md).

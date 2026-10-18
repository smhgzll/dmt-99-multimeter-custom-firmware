# Flash Manager

## Winbond W25Q64JV layout (verified on the acquired dump)

| Sectors (4 KiB) | Bytes | Content | Studio |
|---|---|---|---|
| 0–62 | 0x000000–0x03EFFF | MBR (2 partitions, type 0x07) and reserved area | Read only |
| 63–462 | 0x03F000–0x1CEFFF | FAT12 "BOOT": the bootloader's USB disk; also holds the optional custom logo `LOGO.jpg` | Read only |
| 463–2047 | 0x1CF000–0x7FFFFF | FAT12 assets: 299 files, 297 of them JPEG, in `menu/ mode/ units/ num/ history/ cell/ time/ motif2/ …` | Editable |

Both volumes have the same layout:

- 4096-byte sectors, 1 sector per cluster;
- 2 FATs of 1 sector each;
- 512 root entries;
- OEM name `MSDOS5.0`.

In the acquired dump:

- The BOOT volume holds only `System Volume Information`. Its directory chain is inconsistent and 4
  clusters are lost. There are deleted entries for `LOGO.JPG` and `CH32V3~1.BIN` (the update file, deleted
  after flashing). Studio reports this as pre-existing and never writes that volume.
- The firmware rewrites the MBR and the BOOT root directory sector on every boot. This is V2.4 behaviour; the
  custom firmware skips the write when the content is identical.

## Offline functions

- **Opening.** A file is opened read-only. All edits go to a working copy in memory.
- **Partitions and files.** Partition table, BPB details, file tree with start cluster, cluster count and
  flash offset.
- **Validation:**
  - FAT copies identical;
  - media byte;
  - chain errors, cycles and cross-links;
  - chains shorter or longer than the file size;
  - lost clusters;
  - partition table equal to the DMT99 layout.
- **Export.** A single file, a partition image, or the whole working image.
- **Diff.** A report (JSON) and a binary sector patch (`DMT99SECTORPATCH`: each record is `DS` + sector
  u16 + 4096 bytes).

**Saving never overwrites the original:**

- It refuses the file the image was loaded from.
- It refuses any file whose content is the original dump (SHA-256 `fc6bf2ed…7d69`).
- It writes through a temporary file and a rename.

## Online functions (need a device with the management link)

- **Read.** A full 8 MiB backup, saved as `.bin` plus a `.json` that holds:
  - address, length and timestamp;
  - SHA-256 and CRC32;
  - the device-side CRC32 and whether it matched.
- **Compare.** Per-sector CRC between the device and the working image, listing differing sectors by
  region.
- **Upload** (assets partition only):
  1. Read the device's MBR and assets partition. Save them as a backup verified against the device CRC.
     If the CRC differs, nothing is written.
  2. Apply the edited assets on top of the *device* content, not the old dump, so history files written by
     the meter are kept. Assets that are already identical are skipped.
  3. Validate the resulting FAT12 volume. Refuse if any sector outside 463..2047 would change.
  4. Open a write session. The meter stops reading assets and shows a maintenance state.
  5. For each changed sector, data sectors before FAT and directory sectors: erase, program 256-byte pages,
     read back each page, then check the device CRC of the whole sector.
  6. Close the session. The meter remounts FatFs and repaints.
- **Restore.** Writes an assets backup back, changing only the sectors that differ.

The device enforces these rules again on its side: sector range, an open session, erase before program, no
page crossing, and a 15 s session timeout.

**Power loss during an upload** can leave the assets volume inconsistent: missing pictures, while the
measurement still works. The meter's bootloader is not affected (it lives in MCU Flash). Restore from the
backup taken in step 1.

## Not done

- **Formatting.** Formatting the Winbond or rewriting the MBR is not implemented, by design.
- **The bootloader's USB disk.** It shows only the BOOT volume. It is not a way to reach the full Winbond,
  and Studio does not use it for asset writes.

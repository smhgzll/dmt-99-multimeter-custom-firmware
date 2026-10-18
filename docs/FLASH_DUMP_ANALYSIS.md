# External Flash acquisition and filesystem evidence

`flashdump/dmt99_flash_1.bin` is 8,388,608 bytes and SHA-256
`fc6bf2ed36f578c15725a4365c4a381c98ce42657d3b8a98ff06efb1b6427d69`.
The requested second reading is absent. Independent acquisition agreement is **not verified**.
The original update BIN also matches its expected SHA-256. Evidence is opened read-only.

`file`, 7-Zip and mtools were tried before implementing the read-only parser.
7-Zip identifies the 4096-byte MBR sector convention and lists the resource volume.
It cannot open volume 0 as FAT. Tool output is preserved under `analysis/phase3/tool_reports/`.

| Region | Byte start | Byte length | Evidence |
|---|---:|---:|---|
| MBR and pre-partition area | 0 | 258048 | 55AA at510; two type07 entries; mostly erased space |
| Volume0 | 258048 (0x3f000) | 1638400 | FAT12,400 sectors,393 data clusters |
| Volume1 | 1896448 (0x1cf000) | 6492160 | FAT12,1585 sectors,1578 data clusters |

Both BPBs specify4096-byte sectors, one sector/cluster, one reserved sector,
two one-sector FAT copies,512 root entries and four root-directory sectors.
Each volume's FAT copies are byte-identical. Type07 in the MBR is inconsistent
with the FAT12 contents; sector units are4096 throughout this image, not512.
The small residue at0xff22 in the pre-partition area is inventoried as raw bytes;
it is not evidence of an executable bootloader. The BOOTMGR text is in generic
DOS boot-sector code, not a live BOOTMGR file.

Volume1 has299 live files in12 directories:297 JPEGs and two Windows indexing
metadata files. All file chains, hashes, physical extents, LFN checksums and image
dimensions are recorded in `analysis/flash_files.json`. All297 images decode in
Pillow. There are no crosslinks, missing file clusters or allocated orphan clusters
in this volume. `history/` contains image labels, not measurement records.
No calibration/configuration file has been identified. Fonts drawn by native code
remain in internal firmware; JPEG numeric glyphs are separate resources.

Volume0 has57 deleted entries and a live `System Volume Information` directory
whose start cluster2 is marked free. Four allocated clusters (3,4,7,8) have no
valid live owner. No file is claimed recovered through that damaged directory.
Deleted LFNs include CH32V303RBT61.bin, APP_DMT-99_V2.0.BIN, JT-MT01_V0.0.BIN
and JOY-IT names. Their allocation chains have been cleared. Reused start clusters
and repeated entries prevent assigning current bytes to these historical names.
Deleted entries and partial LFNs remain leads, not confirmed versions.

`tools/flash_forensics.py` extracts only valid live files to `extracted_flash/volume1/`.
It bounds volume/cluster reads, rejects cycles/free/bad clusters and verifies LFN
order/checksum. `tools/flash_candidates.py` separately carves raw JPEG and executable
extents into `extracted_flash/carved/`; these never acquire live-file status.
971 complete raw JPEG extents decode, including297 live files. There are366
nested extents (embedded previews/thumbnails),605 top-level extents and308
non-live top-level extents. Repeated assets explain much unallocated data; counts do not imply971
unique resources. Raw image carving requires contiguous physical bytes and does
not reconstruct deleted fragmented files.

A 135036-byte application survives at0x4d000. Separate PE/COFF x86 extents also
survive twice, including nested executable content. See the firmware comparison
report. None is executed. `flash_regions.json` hashes/classifies all2048 physical
4KiB sectors, including free space and the pre-partition area. No physical device
has been accessed or altered. Six forensic tests cover actual chains, extracted
hashes, truncation, bad BPBs, FAT disagreement, loops and JPEG truncation.

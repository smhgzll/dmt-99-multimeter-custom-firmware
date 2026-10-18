# Resource loading architecture

The maintained path is SPI NOR read ->4096-byte block callbacks -> authentic configured
FatFs ->FIL open/read/seek ->resource wrapper ->TJpgDec RGB565 MCU blocks ->display
window/command/data or an injected host canvas. None of these bindings initializes
MMIO or writes the acquired image by default.

Original d656/dca8 shift block addresses by12; dd00 reports2048 sectors,4096 bytes,
one erase block. Physical partitions start at63 and463 sectors. VolToPart at
SRAM200001d4 is `{0,1},{0,2}`. Filesystem init dd4e first calls f_fdisk with20/80,
mounts0:, attempts mkdir("0:BOOT"), then mounts1:. This original partition rewrite
is isolated behind an explicit write capability in `filesystem_original_partition_setup`.
Ordinary mounting is nondestructive. The original disk driver's16-sector byte-count
narrowing remains in `storage/disk.c`; the new bounded byte callback adapter accepts
full-width lengths. On-device callers would need to split transfers at that boundary.

Both original logical drives register the **same** FATFS at200012cc. Remounting1:
changes that shared object, so registered0: lookups can also use the resource volume.
This explains explicit0:/on.jpg after remount1 and relative mode/unit/menu paths.
A mount object per partition would change this observed behavior. The adapters allow
the original shared-object ordering without silently copying state.

`resource_optional_logo` (ddb0) mounts0:, tries0:/LOGO.jpg and always remounts1:.
`resource_remove_logo` (de12) mounts0:, unlinks if mounting succeeds and remounts1:.
The first volume contains only deleted logos in this acquisition. The original
1511a fallback then tries0:/on.jpg, applies brightness, waits1000ms, clears and
calls the base scene. The complete startup/base scene remains partially recovered.
Deletion requires an injected writable backend; default offline bindings reject it.

The wrapper678c returns1 only if opening fails. It allocates7168 bytes, prepares,
optionally decodes, frees and closes, then returns0 even when JPEG decoding failed.
Native `resources.c` preserves that result and exposes detailed statuses. Its workspace
is caller-owned rather than dynamically allocated; original allocation/stack timing is
not reproduced. FIL needs4136 bytes, JDEC124 and pool7168 on RV32, exceeding the
reserved2KiB offline stack if made automatic. The tests use static workspace.
No target stack-readiness claim follows from successful linking.

Input6708 reads requested bytes or seeks relative to fptr; a read error returns its
FatFs error number, and seek failure returns0. Output6734 sets an inclusive MCU
rectangle at the saved origin, emits the pixel command and writes every RGB565 word.
A host canvas alternate retains coordinates/pixels. Native numeric renderer109e4
preserves digit/O/letter resources, decimal spacing and intervening clears, with216
original-trace comparisons. The alternate10bd0 bitmap path and1a360 packed blitter
add240 comparisons and preserve3696 bytes of original digit glyph data. Resource errors do not trigger an invented fallback.

`firmware_flash_crossrefs.json` maps physical file extents/hashes to literal firmware
strings, original accessors, available C entries and tests.191 live resources have
literal firmware references. Language prefixes, underscore selection suffixes, unit
caches and formatted numeric names also form paths dynamically; absence of a direct
literal is not proof an asset is unused. Existing mode-event state policies retain
the Recording DC/AC filename reversal. JPEG EXIF previews are distinguished from
top-level resources in the raw carving report.

Settings/history serialize into the internal page08040000, independent of external
resource storage. The external `history/` files are labels. No recovered calibration
algorithm or calibration partition is claimed. Image upload/update recognition and
USB writes are not present in the supplied application evidence; write-capable FatFs
APIs are upstream functionality, not proof of a recovered device upload protocol.

Validation covers298 ASCII-named files byte-for-byte and296 JPEGs twice, plus all297
JPEGs via the independent forensic decoder. One Chinese-named resource is blocked
by the explicit CP936 boundary. Corrupt BPBs/chains, missing/invalid JPEGs and read-only
capabilities are tested. All writes use a private heap copy of the image.

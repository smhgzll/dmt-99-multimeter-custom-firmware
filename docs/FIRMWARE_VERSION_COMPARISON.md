# Surviving executable comparison

The carved application at Flash0x4d000 loads at0x6400 and jumps to reset0xc65c.
Its reset routine derives the data source0x27194, data length488 and logical
end0x2737c, giving135036 bytes. This is a bounded raw carve, not a reconstructed
FAT chain or proven historical filename. Its SHA-256 is
`bddf4261481e1c56fcd0eabb25ffbbd40122faea5c25baf77ef824e06588fe75`.
Original V2.4 is135096 bytes with reset0xc668 and initialized data at0x271d0.

`tools/flash_candidates.py` compares complete contiguous original function bodies,
excluding bodies smaller than16 bytes and noncontiguous bodies. 103 match exactly
somewhere in the carved application. Separate32-byte prefix leads are explicitly
not full-body matches. Relocated external calls, shared tails and changed bodies
require additional decoding before CFG or semantic equivalence can be claimed.
The comparison does not infer an overall firmware similarity percentage.
Code and initialized data are reported separately in `firmware_differences.json`.
The initialized data differs. Deleted directory names alone cannot establish a
V2.0/JOY-IT version for the surviving application.

Four PE/COFF extents are also carved, with two unique hashes (duplicate host files
and nested executable content). Their machine type0x14c identifies x86. Exact
raw-section bounds are reported; optional overlays are not inferred. These are
unallocated host-file residue, not alternate MCU firmware or a recovered meter
USB bootloader. None has been executed.

A later raw region contains shared runtime signatures, but no complete executable
extent has yet been established there. It is retained in the per-sector inventory
without being counted as another recovered firmware. Full normalized differential
decoding of the carved application remains open; original439-function evidence
has not been regenerated.

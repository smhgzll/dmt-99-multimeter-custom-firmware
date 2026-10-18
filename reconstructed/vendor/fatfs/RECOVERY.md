ChaN FatFs R0.14b, official archive https://elm-chan.org/fsw/ff/arc/ff14b.zip.
Source copyright, license condition and disclaimer are retained verbatim.

`ff.c`, `ff.h`, `diskio.h` are upstream. `ffconf.h` sets CP936, LFN on the stack,
two volumes, multipartition,512..4096-byte sectors and mkfs/fdisk availability.
`unicode_upper.c` is the authentic upstream uppercase routine and compressed tables.
The original CP936 functions reference an absent mapping table; the adapter reports
non-ASCII conversion requests by original address and returns0. This is an explicit
boundary, not a substituted CP936 implementation or claim of valid original behavior.

Only counterparts individually qualified in analysis/library_matches.json count
as complete source recovery. Extra compiled upstream APIs are not credited to an
original function. No physical device or evidence file is writable through default
bindings. This configuration is non-reentrant, like the observed original.

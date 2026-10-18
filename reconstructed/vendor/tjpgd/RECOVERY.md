ChaN TJpgDec R0.01b, official archive `https://elm-chan.org/fsw/tjpgd/arc/tjpgd1b.zip`.
Copyright and redistribution conditions are retained in the source.

Configuration: JD_SZBUF=512, JD_FORMAT=1 (RGB565), JD_USE_SCALE=1, JD_TBLCLIP=1.
`integer.h` supplies the upstream required integer widths on both RV32 and LP64.
The original 124-byte RV32 JDEC layout distinguishes this branch from R0.01c/d.
Further behavior and machine-code matching is recorded in analysis/library_matches.json.
Source is not described as byte-identical merely because its tables or names match.

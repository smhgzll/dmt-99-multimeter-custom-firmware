# Source and data notices

The gameplay core is adapted from Artfical/doom-avr,
Copyright (C) 2026 Talha Berk Arslan, **AGPL-3.0-or-later**.
Pinned upstream commit: `f584566ec2e18041725e1de5668c6cb1f764ab58`.
Upstream headers and the complete license are retained in the submodule and
the root [LICENSE](LICENSE). New application/renderer code uses the same license.

This is a small raycaster reimplementation, not id Software's original engine.
The DOOM name, E1M1 map, palette, textures and sprites are id Software game
data. The locally generated WAD subset and local BIN containing that subset
are **not** relicensed as AGPL. `assets/doom1.wad`, its shareware distribution
archive, generated art and `dist/` are excluded from Git. Their presence here
is for the requested local build; public redistribution requires attention to
the original game-data terms. The shareware archive is obtained from a mirror
of `ftp.idsoftware.com`, recorded in `assets/source.json`.

WCH SDK files retain Nanjing Qinheng Microelectronics' copyright and their
WCH-microcontroller use notice in each file. They are compiled only for the
CH32 target and are not included in the desktop game library. This document
does not override those notices.

Target startup/linker/display/clock code and the board emulator derive from
the user's read-only POTAT-99 reconstruction at
`.`. File provenance and original SHA-256
values are recorded in `docs/copied_sources.json`. The stack checker is a
local adaptation with callback families restricted to this port.

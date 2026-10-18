# Theme Editor

## How screens are built (from the firmware)

The DMT99 UI is a set of JPEG assets on the Winbond assets volume (`1:`), drawn at fixed coordinates, plus
primitives the firmware draws itself:

| Origin | Examples | Editable in Studio |
|---|---|---|
| JPEG asset | Frames (`motif2/electric_frame.jpg`), mode artwork (`mode/dc_v.jpg`), units (`units/v.jpg`), large digits (`num/*.jpg`, large-digit layout), menu pictures (`menu/english/*.jpg`), logo (`ON.JPG`) | Yes: replace the image. Text inside the picture is part of the image ("baked-in"). |
| Firmware text (font ROM) | Menu labels ("Mode selection", "Threshold settings"), threshold values, About version line | No; positions, fonts and colours are compiled into the firmware |
| Firmware ROM bitmaps | Classic-layout digits, small icons | No |
| Firmware primitives | Fills, borders, lines, the graph | No |

The editor gets this map from the native simulator's draw trace (`dmt99-simulator --trace`). It records
every JPEG draw (path, x, y, decoded size, decoder results) and every semantic text, bitmap or fill call of
the firmware's view layer. Clicking the preview selects the topmost visible element. The element list
labels each one by origin, using the colours from the table above.

### Catalog screens

There are 30 screens, each rendered from a fresh boot with a settings fixture:

- **System:** language selection, charging lock, low battery.
- **Regular view:** AUTO and 11 measurement modes, HOLD, REL.
- **Recording:** voltage and resistance graphs.
- **Monitor:** gauge and threshold editor.
- **Menu:** every top-level page, plus the opened pages for mode selection, thresholds, system, reset and
  About.

Every screen exists in both digit layouts and in both English and Chinese. Readings are synthetic display
packets in the observed format; they are not measurements. The asset map ("Screens" column) is built from
all four combinations. It currently covers 95 assets; assets that never appear on these screens are still
editable.

## Asset rules (enforced before a replacement is accepted)

1. **Decoder support.** The file must be accepted by the device decoder, run by `dmt99-jpeg-check`. That
   means baseline JPEG, 8-bit, exactly 3 components (no grayscale), 4:4:4, 4:2:2 or 4:2:0 sampling, and a
   decode within the 0x1c00-byte work pool.
2. **Same size.** Width and height must equal the original. The firmware draws at fixed positions, so
   another size would overlap neighbours or leave gaps.
3. **Space.** The file must fit in the free clusters of the assets volume plus the clusters it already
   owns.

**Import** converts any picture (PNG with alpha, BMP, WebP, …) to such a JPEG:

- resized by stretch, fit, fill or center;
- the original's chroma sampling is kept;
- quality defaults to the original's estimated quality (at least 75);
- no metadata segments are written.

If you import a JPEG that is already compatible and the right size, its bytes are used unchanged.

The **working copy** preview shows the pixels as the device decoder produces them (RGB565).

**Edit baked-in text.** This tool paints over the original image with a background colour and draws new
text in any installed font. The result goes through the same conversion and checks.

## Undo, restore and the working image

The working image is rebuilt from the base image plus the replacement list after every change. Because of
that:

- Undo, redo and restore are exact.
- Restoring every asset gives back the original image byte for byte.
- A replacement of the same size changes only that file's data sectors.

The boot logo is drawn by the firmware as `"0:/on.jpg"`. Because of a FatFs volume quirk inherited from
V2.4, that path actually opens `1:/ON.JPG`, and Studio maps it there. A file `LOGO.jpg` on the BOOT volume
(the bootloader's USB disk) overrides it; this is FNIRSI's custom logo feature, which the firmware keeps.

## Theme projects

A project is a folder:

```
project.json             format "dmt99-studio-theme" v1: name, notes, base image SHA-256,
                         per-asset file + SHA-256 + original SHA-256 + source, layout_overrides
assets/1/<path>          replacement JPEGs under their device names
```

When a project is opened:

- The base image must match the recorded hash, or you must confirm loading it onto another image.
- Every asset file is checked against its recorded SHA-256.

The `layout_overrides` field is stored and loaded, but the current firmware does not read it.

## Batch colour

**Batch colour** applies one colour change to a group of assets and to the firmware-drawn colours:

- **Scope:** large digits (`num/`), mode artwork (`mode/`), units, frames (`motif2/`), menu pictures,
  history/battery/time, root pictures (logo, NCV, …).
- **Controls:** hue shift (±180°), saturation, brightness.
- **Hue range:** optionally, only colours within ± a hue range of a centre hue, with a soft 15° edge.
- **Greys:** pixels below a minimum saturation (black background, white, greys) are left alone.
- **Firmware colours:** the RGB565 colours the firmware draws itself are transformed the same way and
  stored as the theme palette. They are `#fd40` orange (classic digits, minus sign, selected menu text),
  `#ffff` white, `#1c9f` blue and `#4228` grey. This needs firmware 0.5.0. The preview simulator applies
  the palette with `--palette`.
- **Base:** by default the original artwork is the base, so repeated batches do not compound. Unchecking
  the option recolours the current working images instead, including manual edits.

Images are re-encoded with the original size, chroma sampling and estimated quality. The whole batch,
images and palette together, is one undo step and is stored in the project (`palette` in `project.json`).

Note: in the classic digit layout the digits are firmware glyphs, recoloured by the palette. The decimal
point (`num/radix.jpg`) and `num/e.jpg` are images, so include "Large digits (num/)" to recolour them too.

## Installable themes (THEME.PKG, firmware 0.4.0)

**File ▸ Export THEME.PKG for the meter** packages the edited images of the current session. The package
holds:
- the theme name (≤ 24 ASCII characters);
- the files, under their assets-volume paths;
- per-file CRC32s and a whole-package CRC32.

The format is in `core/themepkg.py` and `ui/theme.h` of the firmware project. An optional `THEME.PAL`
entry carries the palette (up to 16 RGB565 pairs, firmware 0.5.0). Studio warns when the package
is larger than the free space of the bootloader disk (about 1.5 MB in the acquired dump).

What the firmware does with it at start-up (`ui/theme.c`, `ui/theme_boot.c`):

1. **Locate the package.** It finds `THEME.PKG` on the BOOT volume and reads it through that volume's FAT
   chain.
2. **Check it.** The package must have a valid header and CRC. Every path must be an existing original
   `.jpg`, and every file must start like a JPEG. There must be enough free clusters.
3. **Copy and verify.** The files are written into the first free `themes/Tnn` and read back. `THEME.INF`
   is written last; it holds the name, size, a Bloom filter of the paths and the package CRC.
4. **Activate and clean up.** The theme is activated (settings halfwords 118/119) and the package is
   deleted. A rejected package is deleted too, after its reason is shown on screen.

While a theme is active, the firmware looks up an asset in the theme folder first when the Bloom filter
allows it, and otherwise draws the original. Removing a theme deletes its folder recursively. The empty
`themes` folder goes when the last theme does, so the free space returns to the exact original value.

**Flash Manager** shows the installed themes of an image and a pending `THEME.PKG`.
`themepkg.simulate_install()` runs the firmware's own installer (simulator, memory copy) on an image. The
tests use it to check install, a second theme, persistence over a power cycle, deletion from the meter's
theme menu, rejection of damaged or foreign packages, and the out-of-space case.

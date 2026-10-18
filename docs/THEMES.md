# Preparing and installing themes

Studio's offline editor works; real-device USB management and live
measurement transport remain unfinished. Theme installation does not require
that transport: copy `THEME.PKG` manually to the existing bootloader disk.

## Editing

1. [Start Studio](BUILDING.md#studio-and-theme-editor) and select
   **File → Open original dump**. The original 8 MiB image is read-only input.
2. Select a screen/asset in **Theme Editor**. Import PNG/JPEG images,
   bake text into an image, or recolor groups with **Batch colour**.
3. Check previews rendered by the native firmware C code using real Flash
   resources. JPEG compatibility is checked with the firmware's TJpgDec build.
4. Save the editable project under `artifacts/themes/<name>/project/`.
   Use **Export THEME.PKG for the meter** to write
   `artifacts/themes/<name>/THEME.PKG`. Do not save over the original dump.

## Versions and space

| Feature | Required POTAT-99 version |
|---|---|
| JPEG theme package | ≥0.4.0 |
| `THEME.PAL` mappings for firmware-drawn colors | ≥0.5.0 |
| Image-free Simple screen | ≥0.6.0 |

A package contains only changed images and an optional palette.
Arbitrarily moving positions, fonts or code-drawn text is unsupported.
Keep the name **THEME.PKG**. BOOT is 1.5625 MiB in total; although roughly
1.5 MB free has been observed, check actual available space. Free assets-volume
space also limits installed themes; the roughly 3 MB observation is not a
guaranteed constant.

## Installing, selecting and deleting

1. Install a compatible POTAT-99 version using the [Windows USB procedure](UPDATING.md).
2. Switch off the meter, enter BOOT USB mode with **POWER + MENU**, and connect to Windows.
3. Copy only `THEME.PKG`, safely eject and start normally.
4. The application checks package bounds/CRCs, installs into
   `themes/Tnn` on the assets volume, verifies, activates and removes the
   package. After interruption, do not assume installation completed; inspect the error.
5. In **Menu → About → ENTER → Display**, OK activates a theme.
   ENTER deletes a selected custom theme after confirmation. MENU/BACK exits.

Select **Original** to return to the original resources. Original images
are not overwritten, but installing/deleting themes writes assets Flash.
Keep an 8 MiB external Flash backup distinct from an MCU firmware backup;
do not use a raw dump as a USB firmware BIN.

In 0.6.0, the same Display screen provides **Simple screen** with background,
digit, text/accent colors and digit size. It does not need a package or Studio.

## Small example

```bash
python3 tools/create_demo_theme.py
# artifacts/themes/green-demo/THEME.PKG — 78 bytes; palette 0xFD40 → 0x07E0 only
```

This example copies no manufacturer JPEG and requires ≥0.5.0.
It changes only the mapped firmware-drawn colors, not colors inside JPEGs.
The package is parsed/CRC-checked offline; physical installation was not
tested during consolidation.

Further details: [editor](../projects/studio/THEME_EDITOR.md),
[Flash Manager](../projects/studio/FLASH_MANAGER.md),
[package C implementation](../reconstructed/ui/theme_boot.c).

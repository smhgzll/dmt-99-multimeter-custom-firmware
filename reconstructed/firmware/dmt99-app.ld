/* USB update image for the manufacturer bootloader (same format as V2.4).
 *
 * The meter keeps its original 25 KiB prefix at 0x08000000..0x080063FF (the
 * USB mass-storage updater entered with POWER+MENU). That updater installs a raw
 * application BIN at Code Flash 0x08006400 and starts it there; the V2.4 file
 * DMT-99_V2.4_20240306.bin is linked to 0x6400 with this header:
 *
 *   +0x00  j handle_reset
 *   +0x04  12 x nop (0x00000013), ebreak (0x00100073)
 *   +0x38  104-slot vector table, slot 0 = image start
 *
 * This script produces the same layout from the same sources as the standalone
 * image. Nothing below 0x6400 is referenced or written. Code stays below the
 * smallest standard zero-wait size (192 KiB) and RAM within 32 KiB, as for the
 * standalone image; the settings page 0x08040000 is untouched by the layout.
 */
ENTRY(_start)
__stack_size = 6144;

MEMORY {
    FLASH (rx) : ORIGIN = 0x00006400, LENGTH = 192K - 25K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE dmt99-sections.ld

ASSERT(_start == 0x6400, "update image must start at the application origin 0x6400")
ASSERT(_vector_base == 0x6438, "vector table must sit at 0x6438 as in the V2.4 update file")

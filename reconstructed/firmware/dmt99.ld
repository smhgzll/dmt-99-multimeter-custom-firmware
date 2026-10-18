/* Standalone DMT99 replacement image for CH32V303RCT6.
 *
 * Executes from the 0x00000000 alias of Code Flash 0x08000000; the raw BIN is
 * programmed at 0x08000000. Unlike the V2.4 application (linked at 0x6400 behind
 * an unknown 25 KiB prefix), nothing outside this image is required to boot.
 *
 * Limits are chosen to work with every standard USER RAM_CODE_MOD setting of the
 * RC part (RM v2.5 table 32-4): 192/224/256/288 KiB zero-wait code and
 * 128/96/64/32 KiB SRAM. Removing RDP erases the part and the option byte state
 * afterwards is not known, so the image must not depend on more than 32 KiB SRAM
 * or more than 192 KiB of zero-wait code. The settings page used by the original
 * firmware, 0x08040000..0x080400FF, lies above the image in every case.
 */
ENTRY(_start)
__stack_size = 6144;

MEMORY {
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 192K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE dmt99-sections.ld

ASSERT(_start == 0, "reset entry must be the first word of Code Flash")
ASSERT(_vector_base == 4, "vector table must follow the reset jump")

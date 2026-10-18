/* Section layout shared by the standalone (dmt99.ld) and USB update
 * (dmt99-app.ld) images. The including script defines MEMORY, ENTRY and
 * __stack_size and checks the entry and vector addresses. */
SECTIONS {
    .init : {
        _sinit = .;
        KEEP(*(SORT_NONE(.init)))
        . = ALIGN(4);
    } >FLASH
    .vector : {
        KEEP(*(.vector))
        . = ALIGN(4);
    } >FLASH
    .text : {
        . = ALIGN(4);
        *(.text.handle_reset)
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
    } >FLASH
    /* Build identity record at a fixed offset from the end of the code; see
     * firmware/version.c. Kept in its own section so tools can find it. */
    .fw_info : {
        . = ALIGN(4);
        KEEP(*(.fw_info))
        . = ALIGN(4);
    } >FLASH
    .data : {
        . = ALIGN(4);
        _data_vma = .;
        *(.data .data.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>FLASH
    _data_lma = LOADADDR(.data);
    _image_end = _data_lma + SIZEOF(.data);
    .bss (NOLOAD) : {
        . = ALIGN(4);
        _sbss = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
    /* Survives warm resets: fault record and reset bookkeeping. */
    .noinit (NOLOAD) : {
        . = ALIGN(4);
        *(.noinit .noinit.*)
        . = ALIGN(4);
    } >RAM
    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size (NOLOAD) : {
        . = ALIGN(16);
        _susrstack = .;
        . = . + __stack_size;
        _eusrstack = .;
    } >RAM
    /DISCARD/ : { *(.comment) *(.eh_frame*) }
}

ASSERT(_image_end <= 0x30000, "image exceeds the smallest standard zero-wait code size (192 KiB)")
ASSERT(_image_end <= 0x40000, "image overlaps the settings page at 0x08040000")
ASSERT(_eusrstack <= 0x20008000, "RAM use exceeds the smallest SRAM configuration (32 KiB)")
ASSERT(.  <= 0x20008000, "RAM overflow")
ASSERT(_ebss <= ADDR(.noinit), "bss/noinit overlap")
ASSERT(ADDR(.noinit) + SIZEOF(.noinit) <= _susrstack, "static RAM collides with the stack")

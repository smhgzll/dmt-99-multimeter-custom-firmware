/* Based on official WCH Link.ld; verified application origin and SRAM size.
 * The absent region [0,0x6400) is preserved. This is an offline link harness.
 */
ENTRY(_start)
MEMORY {
    FLASH (rx) : ORIGIN = 0x00006400, LENGTH = 0x39c00
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}
SECTIONS {
    .init : { KEEP(*(.init)) . = 0x38; } > FLASH
    .vector : { KEEP(*(.vector)) } > FLASH
    .text 0x6600 : { *(.text*) *(.rodata*) . = ALIGN(4); } > FLASH
    .data : {
        . = ALIGN(4); _data_vma = .; *(.data*)
        . = ALIGN(8); PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata*) *(.srodata*) . = ALIGN(4); _edata = .;
    } > RAM AT> FLASH
    _data_lma = LOADADDR(.data);
    .bss (NOLOAD) : { . = ALIGN(4); _sbss = .; *(.sbss*) *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } > RAM
    .stack ORIGIN(RAM) + LENGTH(RAM) - 0x800 (NOLOAD) : {
        _susrstack = .; . += 0x800; _eusrstack = .;
    } > RAM
    /DISCARD/ : { *(.comment) *(.eh_frame*) }
}
ASSERT(ADDR(.vector) == 0x6438, "vector placement differs from original")
ASSERT(ADDR(.text) == 0x6600, "code placement differs from original")
ASSERT(_eusrstack == 0x20010000, "stack top differs from original")
ASSERT(_ebss <= _susrstack, "RAM overflow")

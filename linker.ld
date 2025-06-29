/*
 * Vintix OS -- linker.ld
 * A linker script for a 64-bit kernel loaded by a Multiboot2 bootloader.
 */

ENTRY(_start)

SECTIONS
{
    /* The kernel starts at 1MB */
    . = 1M;

    /* The first section must be the multiboot header */
    .text : ALIGN(4K) {
        KEEP(*(.multiboot_header))
        *(.text .text.*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4K) {
        *(.data .data.*)
    }

    .bss : ALIGN(4K) {
        *(.bss .bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}

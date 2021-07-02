/*OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

KERNEL_VIRT_BASE = 0xffffffff80000000;

SECTIONS
{
    . = 0x100000;
    _start = .;

    .text :
    {
        *(.text)
        . = ALIGN(0x1000);
    }

    .data :
    {
        *(.data)
        . = ALIGN(0x1000);
    }
    .rodata :
    {
        *(.rodata)
        . = ALIGN(0x1000);
    }
    .bss :
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(0x1000);
    }
    _kernel_end = .;
}*/

ENTRY(_start)
SECTIONS
{
    . = 0xffffffff80200000;

    _kernel_start = .;

    .stivale2hdr : {
        KEEP(*(.stivale2hdr))
    }

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(COMMON)
        *(.bss*)
    }

    _kernel_end = .;
}
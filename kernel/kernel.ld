/*OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)



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
    KERNEL_VIRTUAL_ADDR = 0xffffffff80000000;

    . = KERNEL_VIRTUAL_ADDR;
    . += 0x200000;

    _kernel_start = .;

    .stivale2hdr : AT(ADDR(.stivale2hdr) - KERNEL_VIRTUAL_ADDR)
    {
        KEEP(*(.stivale2hdr))
    }

    .text : AT(ADDR(.text) - KERNEL_VIRTUAL_ADDR)
    {
        *(.text)
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_VIRTUAL_ADDR)
    {
        *(.rodata)
    }

    .data : AT(ADDR(.data) - KERNEL_VIRTUAL_ADDR)
    {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - KERNEL_VIRTUAL_ADDR)
    {
        *(COMMON)
        *(.bss)
    }

    _kernel_end = .;
}
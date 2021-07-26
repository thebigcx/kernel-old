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

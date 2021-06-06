OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

KERNEL_VIRT_BASE = 0xffffffff80000000;

SECTIONS
{
    . = 0x100000;
    _KernelStart = .;

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
    _KernelEnd = .;
}
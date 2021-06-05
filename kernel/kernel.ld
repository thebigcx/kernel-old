OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

KERNEL_VIRT_BASE = 0xC0000000;

SECTIONS
{
    _KernelStart = .;
    .text : ALIGN(0x1000)
    {
        *(.text)
    }
    .data : ALIGN(0x1000)
    {
        *(.data)
    }
    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }
    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }
    _KernelEnd = .;
}
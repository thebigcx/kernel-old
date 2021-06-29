SECTIONS
{
    . = 0x7c00;

    .text :
    {
        *(.text)
    }

    /*.data :
    {
        *(.data)
    }
    .rodata :
    {
        *(.rodata)
    }
    .bss :
    {
        *(COMMON)
        *(.bss)
    }*/
}
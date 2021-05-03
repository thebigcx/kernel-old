#include <gdt/gdt.h>

#define GRANLONG GDTF_GRAN | GDTF_LONG

__attribute__((aligned(PAGE_SIZE)))
gdt_t def_gdt = 
{
    { 0, 0, 0, 0x0,                                                      0x0,      0 }, // Null
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_EXEC | GDTA_WRITABLE, GRANLONG, 0 }, // Kernel code seg
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_WRITABLE,             GRANLONG, 0 }, // Kernel data seg
    { 0, 0, 0, 0x0,                                                      0x0,      0 }, // User Null
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_EXEC | GDTA_WRITABLE, GRANLONG, 0 }, // Kernel code seg
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_WRITABLE,             GRANLONG, 0 }  // Kernel data seg
};
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "../drivers/graphics/graphics.h"
#include "paging/paging.h"
#include "memory/memory.h"
#include "gdt/idt.h"

// Defined in linker
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

typedef struct
{
    uint64_t mem_map_size;
    uint64_t mem_map_key;
    uint64_t mem_map_desc_size;
    uint32_t mem_map_desc_vers;
    void* mem_map;

    uint64_t fb_adr;
    uint32_t pix_per_line;
    uint32_t v_res;
    uint64_t fb_buf_sz;

    psf1_font* font;
} boot_info_t;

void _start(boot_info_t* inf)
{
    graphics_data.fb_adr = inf->fb_adr;
    graphics_data.pix_per_line = inf->pix_per_line;
    graphics_data.v_res = inf->v_res;
    graphics_data.font = inf->font;

    char buffer[100];

    page_init();
    page_alloc_init(inf->mem_map, inf->mem_map_size, inf->mem_map_desc_size);

    // Reserve memory for kernel
    uint64_t kernel_sz = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    void* kernel_start = (void*)&_KernelStart;
    uint64_t kernel_pg_cnt = kernel_sz / PAGE_SIZE + 1;

    page_alloc_m(kernel_start, kernel_pg_cnt);

    uint64_t mem_size = get_memory_size(inf->mem_map, inf->mem_map_size / inf->mem_map_desc_size, inf->mem_map_desc_size);
    for (uint64_t i = 0; i < mem_size; i += PAGE_SIZE)
    {
        map_memory((void*)i, (void*)i);
    }

    uint64_t fb_size = inf->fb_buf_sz + PAGE_SIZE;
    page_alloc_m((void*)inf->fb_adr, fb_size / PAGE_SIZE + 1);
    for (uint64_t i = inf->fb_adr; i < inf->fb_adr + fb_size; i += PAGE_SIZE)
    {
        map_memory((void*)i, (void*)i);
    }

    page_dir_t* dir = page_get_pml4();

    asm ("mov %0, %%cr3"::"r"(dir));

    idt_init();
    
    gr_clear_color(0, 0, 0, 1);
    gr_clear();
    for (int x = 0; x < 100; x++)
    for (int y = 0; y < 100; y++)
    {
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * x + 4 * y)) = 0x00ff00000;
    }
    
    while (true);
    return;
}
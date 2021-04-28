#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "../drivers/graphics/graphics.h"
#include "paging/page_alloc.h"

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

    psf1_font* font;
} boot_info_t;

void _start(boot_info_t* inf)
{
    graphics_data.fb_adr = inf->fb_adr;
    graphics_data.pix_per_line = inf->pix_per_line;
    graphics_data.v_res = inf->v_res;
    graphics_data.font = inf->font;

    const char* str = "wow\n";
    puts(str);
    char buf[10];
    puts(itoa(-29587, buf, 10));
    while (1);

    page_init();
    page_alloc_init(inf->mem_map, inf->mem_map_size, inf->mem_map_desc_size);

    // Reserve memory for kernel
    uint64_t kernel_sz = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    void* kernel_start = (void*)&_KernelStart;
    uint64_t kernel_pg_cnt = kernel_sz / PAGE_SIZE + 1;

    page_reserve_m(kernel_start, kernel_pg_cnt);
    
    //uint32_t* mem = (uint32_t)malloc(sizeof(uint32_t));
    //*mem = 0x00FF0000;
    uint32_t color = 0x00FF0000;
    map_memory(&color, &color);

    uint32_t* color_pointer = (uint32_t*)get_physaddr(&color);

    map_memory((void*)0x600000000, (void*)0x80000);

    uint32_t* test = (uint32_t*)0x600000000;
    *test = 0x00FF0000;

    gr_clear_color(0, 0, 0, 1);
    gr_clear();
    for (int x = 0; x < 100; x++)
    for (int y = 0; y < 100; y++)
    {
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * x + 4 * y)) = *test;
    }
    
    return;
}
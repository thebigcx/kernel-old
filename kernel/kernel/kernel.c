#include "stdint.h"
#include "../drivers/graphics/graphics.h"
#include "page_alloc.h"

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
} boot_info_t;

void _start(boot_info_t* inf)
{
    page_alloc_init(inf->mem_map, inf->mem_map_size, inf->mem_map_desc_size);

    graphics_data.fb_adr = inf->fb_adr;
    graphics_data.pix_per_line = inf->pix_per_line;
    graphics_data.v_res = inf->v_res;

    gr_clear_color(0, 0, 0, 1);
    gr_clear();
    for (int x = 0; x < 100; x++)
    for (int y = 0; y < 100; y++)
    {
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * x + 4 * y)) = 0x00FF0000;
    }
    //while (1);
    return;
}
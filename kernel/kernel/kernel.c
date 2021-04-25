#include "stdint.h"
#include "../drivers/graphics/graphics.h"

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

void _start()
{
    //graphics_data.fb_adr = inf.fb_adr;
    //graphics_data.pix_per_line = inf.pix_per_line;
    //graphics_data.v_res = inf.v_res;

    //gr_clear_color(1, 0, 0, 1);
    //gr_clear();
    //while (1);
}
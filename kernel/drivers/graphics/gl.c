#include <drivers/graphics/graphics.h>

void gl_swap_buffers()
{
    memcpy(graphics_data.fb_surf.buffer, graphics_data.back_buffer.buffer, graphics_data.fb_surf.width * graphics_data.fb_surf.height);
}
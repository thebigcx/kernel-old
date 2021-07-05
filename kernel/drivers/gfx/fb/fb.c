#include <drivers/gfx/fb/fb.h>
#include <util/stdlib.h>
#include <util/bmp.h>
#include <drivers/fs/vfs/vfs.h>
#include <mem/kheap.h>
#include <mem/paging.h>

vid_mode_t vidmode;
psf1_font* font;

static uint32_t rgbtopix(uint8_t r, uint8_t g, uint8_t b)
{
    return r << 16 | g << 8 | b;
}

int fb_ioctl(vfs_node_t* node, uint64_t request, void* argp)
{

}

void* fb_mmap(vfs_node_t* file, void* addr, size_t len, int prot, int flags, size_t off)
{
    /*void* ret = pmm_request();
    for (uint32_t i = 1; i < len / PAGE_SIZE_4K + 1; i++)
    {
        pmm_request();
    }*/

    
}

void video_setmode(vid_mode_t mode)
{
    vidmode = mode;
}

void video_init()
{
    vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
    node->ioctl = fb_ioctl;
    node->mmap = fb_mmap;
    node->flags = FS_BLKDEV;
    node->name = strdup("fb0");
    vfs_mount(node, "/dev/fb0");
}

void video_set_fnt(psf1_font* fnt)
{
    font = fnt;
}

const vid_mode_t* video_get_mode()
{
    return &vidmode;
}

void video_putchar(char c, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    char* face = (char*)font->glyph_buf + (c * font->header->c_size);

    for (uint64_t j = y; j < y + 16; j++)
    {
        for (uint64_t i = x; i < x + 8; i++)
        {
            if ((*face & (0b10000000 >> (i - x))) > 0)
            {
                *((uint32_t*)vidmode.fb + i + (j * vidmode.width)) = rgbtopix(r, g, b);
            }
        }
        face++;
    }
}

void video_puts(const char* str, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t xoff = 0;
    
    while (*str != 0)
    {
        video_putchar(*str, x + xoff, y, r, g, b);
        xoff += 8;
        str++;
    }
}

void video_putpix(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x > vidmode.width || x < 0 || y > vidmode.height || y < 0) return;
    
    *((uint32_t*)vidmode.fb + x + (y * vidmode.width)) = rgbtopix(r, g, b);
}

void video_draw_img(uint32_t x, uint32_t y, uint32_t w, uint32_t h, void* data)
{
    uint32_t* pixels = (uint32_t*)data;

    for (uint64_t j = y; j < y + h; j++)
    for (uint64_t i = x; i < x + w; i++)
    {
        *((uint32_t*)vidmode.fb + i + (j * vidmode.width)) = pixels[w * (j - y) + (i - x)];
    }
}
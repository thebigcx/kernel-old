#include <drivers/gfx/fb/fb.h>
#include <util/stdlib.h>
#include <util/bmp.h>
#include <drivers/fs/vfs/vfs.h>
#include <mem/kheap.h>
#include <mem/paging.h>
#include <sched/sched.h>

vid_mode_t vidmode;
psf1_font_t font;

static uint32_t rgbtopix(uint8_t r, uint8_t g, uint8_t b)
{
    return r << 16 | g << 8 | b;
}

int fb_ioctl(vfs_node_t* node, uint64_t request, void* argp)
{
    switch (request)
    {
        case 0:
        {
            fbinfo_t* inf = argp;
            inf->width = vidmode.width;
            inf->height = vidmode.height;
            inf->bpp = vidmode.depth;
            return 0;
        }
    }
}

void* fb_mmap(vfs_node_t* file, proc_t* proc, void* addr, size_t len, int prot, int flags, size_t off)
{
    // TODO: find available region
    uint64_t pgcnt = vidmode.width * vidmode.height * (vidmode.depth / 8) / PAGE_SIZE_4K + 1;
    page_map_memory(0x30000, vidmode.fbphys, pgcnt, proc->addr_space);

    return 0x30000;
}

void video_setmode(vid_mode_t mode)
{
    vidmode = mode;
}

void video_init()
{
    memset(vidmode.fb, 0, vidmode.width * vidmode.height * (vidmode.depth / 8));

    vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
    node->ioctl = fb_ioctl;
    node->mmap = fb_mmap;
    node->flags = FS_BLKDEV;
    node->name = strdup("fb");
    vfs_mount(node, "/dev/fb");

    vfs_node_t* psffont = vfs_resolve_path("/usr/font.psf", NULL);

    psf1_header_t header;
    vfs_read(psffont, &header, 0, sizeof(psf1_header_t));

    uint64_t bufsize = header.c_size * 256;

    if (header.mode == 1)
        bufsize = header.c_size * 256;

    uint8_t* buf = kmalloc(bufsize);

    vfs_read(psffont, buf, sizeof(psf1_header_t), bufsize);

    font.glyph_buf = buf;
    font.header = header;
}

/*void video_set_fnt(psf1_font* fnt)
{
    font = fnt;
}*/

const vid_mode_t* video_get_mode()
{
    return &vidmode;
}

void video_putchar(char c, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    char* face = (char*)font.glyph_buf + (c * font.header.c_size);

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
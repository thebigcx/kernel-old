#include <util/bmp.h>
#include <util/stdlib.h>

static void bmp_load_pix8(void* data, int w, int h, bmp_pal_t* pal, uint8_t* dst)
{
    uint8_t pix;
    uint8_t* ptr = dst;

    for (uint32_t i = 0; i < h; i++)
    {
        for (uint32_t j = 0; j < w; j++)
        {
            pix = ((uint8_t*)data)[i * w + j];
            *ptr = pal[pix].r; ptr++;
            *ptr = pal[pix].g; ptr++;
            *ptr = pal[pix].b; ptr++;
            *ptr = 0xff;       ptr++;
        }

        if (w % 4 != 0)
        {
            data += 4 - (w % 4);
        }
    }
}

static void bmp_load_pix24(void* data, int w, int h, uint8_t* dst)
{
    uint8_t pix[3];
    uint8_t* ptr = dst;

    for (uint32_t i = 0; i < h; i++)
    {
        for (uint32_t j = 0; j < w; j++)
        {
            memcpy(pix, data + (i * 3 * w + j * 3), 3);
            *ptr = pix[2]; ptr++;
            *ptr = pix[1]; ptr++;
            *ptr = pix[0]; ptr++;
            *ptr = 0xff;   ptr++;
        }

        if (w * 3 % 4 != 0)
        {
            data += 4 - (w * 3 % 4);
        }
    }
}

uint8_t* bmp_load(void* data, int* w, int* h)
{
    bmp_hdr_t* hdr = (bmp_hdr_t*)data;

    if (hdr->magic[0] != 'B' || hdr->magic[1] != 'M') return NULL;

    bmp_inf_hdr_t* inf = (bmp_inf_hdr_t*)(data + sizeof(bmp_hdr_t));
    bmp_pal_t* pal = NULL;

    uint32_t width = inf->width;
    uint32_t height = inf->height;

    *w = width;
    *h = height;

    // Load palette if present
    if (inf->bpp <= 8)
    {
        if (inf->bpp == 1 && inf->palette == 0) inf->palette = 2;
        if (inf->bpp == 4 && inf->palette == 0) inf->palette = 16;
        if (inf->bpp == 8 && inf->palette == 0) inf->palette = 256;

        pal = kmalloc(inf->palette * sizeof(bmp_pal_t));
        memcpy(pal, data + sizeof(bmp_hdr_t) + inf->hdr_size, sizeof(bmp_pal_t) * inf->palette);
    }

    data += hdr->off;
    
    uint8_t* buf = kmalloc(width * height * 4);
    memset(buf, 0, width * height * 4);

    switch (inf->bpp)
    {
        case 8:
            bmp_load_pix8(data, width, height, pal, buf);
            break;

        case 24:
            bmp_load_pix24(data, width, height, buf);
            break;

        default:
            console_write("[BMP] Unsupported format!\n", 255, 0, 0);
            break;
    }

    if (pal) kfree(pal);

    return buf;
}
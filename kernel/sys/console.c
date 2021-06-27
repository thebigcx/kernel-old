#include <sys/console.h>
#include <util/stdlib.h>
#include <drivers/gfx/fb/fb.h>
#include <drivers/fs/vfs/vfs.h>

uint32_t curs_x = 0;
uint32_t curs_y = 0;

void console_putchar(char c, uint8_t r, uint8_t g, uint8_t b)
{
    if (c == '\n')
    {
        curs_y++;
        curs_x = 0;
    }
    else
    {
        video_putchar(c, curs_x * 8, curs_y * 16, r, g, b);
    }

    const vid_mode_t* mode = video_get_mode();

    curs_x++;
    if (curs_x * 8 >= mode->width)
    {
        curs_x = 0;
        curs_y++;
    }

    if (curs_y * 16 >= mode->height)
    {
        console_clear();
    }
}

void console_write(const char* str, uint8_t r, uint8_t g, uint8_t b)
{
    while (*str != 0)
    {
        console_putchar(*str, r, g, b);
        str++;
    }
}

void console_clear()
{
    const vid_mode_t* mode = video_get_mode();
    memset(mode->fb, 0, mode->width * mode->height * (mode->depth / 8));

    curs_x = 0;
    curs_y = 0;
}

void console_printf(const char* format, uint8_t r, uint8_t g, uint8_t b, ...)
{
    va_list list;
    va_start(list, b);
    char str[4096];
    vsnprintf(str, format, list);
    console_write(str, r, g, b);
}

size_t conwrite(vfs_node_t* node, const void* ptr, size_t off, size_t size)
{
    for (uint32_t i = off; i < size + off; i++)
    {
        console_putchar(((char*)ptr)[i], 255, 255, 255);
    }

    return size;
}

void console_init()
{
    vfs_node_t* stdout = kmalloc(sizeof(vfs_node_t));
    stdout->write = conwrite;
    stdout->flags = FS_CHARDEV;
    stdout->name = strdup("stdout");
    vfs_mount(stdout, "/dev/stdout");
}
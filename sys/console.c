#include <sys/console.h>
#include <util/stdlib.h>
#include <drivers/gfx/fb/fb.h>
#include <fs/vfs/vfs.h>
#include <drivers/tty//serial/serial.h>

uint32_t curs_x = 0;
uint32_t curs_y = 0;
vfs_node_t* kb = NULL;

void console_putchar(char c, uint8_t r, uint8_t g, uint8_t b)
{
    const vid_mode_t* mode = video_get_mode();

    if (c == '\n')
    {
        curs_y++;
        curs_x = 0;
    }
    else if (c == '\t')
    {
        curs_x += 4;
    }
    else if (c == '\b')
    {
        curs_x--;
    }
    else
    {
        video_putchar(c, curs_x * 8, curs_y * 16, r, g, b);

        curs_x++;
        if (curs_x * 8 >= mode->width)
        {
            curs_x = 0;
            curs_y++;
        }
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

// System console stdout
size_t conwrite(vfs_node_t* node, const void* ptr, size_t off, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        console_putchar(((char*)ptr)[i], 255, 255, 255);
    }

    return size;
}

static char scancode_toascii[] =
{
    'c', '~', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't',
    'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', '~', 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '`',
    '~', '\\', 'z', 'x', 'c', 'v', 'b',
    'n', 'm', ',', '.', '/', '~', '*',
    '~', ' ', '~', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c'
};

// System console stdin
size_t conread(vfs_node_t* node, void* ptr, size_t off, size_t size)
{
    if (!kb)
    {
        kb = vfs_resolve_path("/dev/keyboard", NULL);
    }

    uint8_t* buffer = kmalloc(size);
    if (vfs_read(kb, buffer, off, size))
    {
        uint32_t read = 0;
        for (int i = 0; i < size; i++)
        {
            if (buffer[i] > 88)
                continue;
            
            *((char*)ptr) = scancode_toascii[buffer[i]];
            ptr++;
            read++;
        }

        kfree(buffer);
        return read;
    }

    kfree(buffer);
    return 0;
}

void console_init()
{
    vfs_node_t* console = kmalloc(sizeof(vfs_node_t));
    console->write = conwrite;
    console->read = conread;
    console->flags = FS_CHARDEV;
    console->name = strdup("console");
    vfs_mount(console, "/dev/console");
}

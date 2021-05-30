#include <console.h>
#include <drivers/video/video.h>

uint32_t curs_x = 0;
uint32_t curs_y = 0;

void console_putchar(char c, uint8_t r, uint8_t g, uint8_t b)
{
    if (c == '\n')
    {
        curs_y++;
        curs_x = 0;
        return;
    }

    video_putchar(c, curs_x * 8, curs_y * 16, r, g, b);

    curs_x++;
    if (curs_x * 8 >= video_get_mode().width)
    {
        curs_x = 0;
        curs_y++;
    }

    if (curs_y * 16 >= video_get_mode().height)
        curs_y = 0;
}

void console_write(const char* str, uint8_t r, uint8_t g, uint8_t b)
{
    while (*str != 0)
    {
        console_putchar(*str, r, g, b);
        str++;
    }
}
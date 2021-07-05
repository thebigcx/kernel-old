#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void _start()
{
    /*char buffer[100];
    buffer[0] = 'H';
    buffer[1] = 'e';
    buffer[2] = 'l';
    buffer[3] = 'l';
    buffer[4] = 'o';
    buffer[5] = '!';

    int file = open("/text/test.txt", 0, 0);
    write(1, "Hello, world!", 100);
    //char buffer[1024];
    read(file, buffer, 100);

    write(1, buffer, 6);*/ // stdout

    /*int keyboard = open("/dev/keyboard", 0, 0);
    
    while (1)
    {
        uint32_t key;
        if (read(keyboard, &key, 1))
        {
            char buf[3];
            write(1, itoa(key, buf, 16), 1);
        }
    }*/

    int fd = open("/dev/fb", 0, 0);
    void* fb = mmap(NULL, 0, 0, 0, fd, 0);

    for (int y = 0; y < 100; y++)
    for (int x = 0; x < 100; x++)
    {
        *((uint32_t*)fb + x + y * 768) = 0xffffffff;
    }

    for (;;);
}
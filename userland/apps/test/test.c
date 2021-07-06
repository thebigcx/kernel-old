#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void _start(int argc, char** argv)
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

    char* nl = "\n";
    write(1, argv[0], 1);
    write(1, nl, 1);
    write(1, argv[1], 1);
    write(1, nl, 1);

    int keyboard = open("/dev/keyboard", 0, 0);

    //int fd = open("/dev/fb", 0, 0);
    //void* fb = mmap(NULL, 0, 0, 0, fd, 0);
    
    while (1)
    {
        

        /*for (int y = 0; y < 1; y++)
        for (int x = 0; x < 1; x++)
        {
            *((uint32_t*)fb + x + y * 768) = 0xffffffff;
        }*/

        uint32_t key;
        if (read(keyboard, &key, 1))
        {
            char buf[3];
            write(1, itoa(key, buf, 16), 1);
        }
    }

    

    

    

    for (;;);
}
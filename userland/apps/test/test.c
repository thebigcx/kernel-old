#include <unistd.h>
#include <stdio.h>
#include <string.h>

void _start()
{
    char buffer[100];
    buffer[0] = 'H';
    buffer[1] = 'e';
    buffer[2] = 'l';
    buffer[3] = 'l';
    buffer[4] = 'o';
    buffer[5] = '!';

    //int file = open("/text/test.txt", 0, 0);
    write(1, buffer, 100);

    for (;;);
}
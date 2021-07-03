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
    //strcpy(buffer, "Hello from userspace!");

    //for (;;)
    write(1, buffer, 100);
    for (;;);
}
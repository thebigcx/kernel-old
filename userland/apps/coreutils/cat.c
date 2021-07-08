#include <unistd.h>

void _start(int argc, char** argv)
{
    int fd = open("/text/test.txt", 0, 0);
    char buffer[200];

    read(fd, buffer, 200);

    printf("%s\n", buffer);

    exit(0);
    for(;;);
}
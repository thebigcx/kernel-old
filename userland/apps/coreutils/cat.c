#include <unistd.h>

void _start(int argc, char** argv)
{
    printf("ARGV[1]: %s\n", argv[1]);
    exit(0);
    /*int fd = open("/text/test.txt", 0, 0);
    //int fd = open(argv[1], 0, 0);
    char buffer[200];

    read(fd, buffer, 200);

    printf("%s\n", buffer);

    exit(0);*/
    //for(;;);
}
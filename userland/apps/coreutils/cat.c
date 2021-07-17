#include <unistd.h>
#include <stdio.h>

void _start(int argc, char** argv)
{
    int fd;
    if ((fd = open(argv[1], 0, 0)) < 0)
    {
        printf("cat: %s: no such file or directory\n", argv[1]);
        exit(0);
    }

    char buffer[200];
    read(fd, buffer, 200);
    printf("%s\n", buffer);
    exit(0);

    /*FILE* file = fopen(argv[1], "r");
    char buffer[200];
    fread(buffer, 1, 200, file);
    printf("%s\n", buffer);
    fclose(file);
    exit(0);*/

}
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    int fd;
    if ((fd = open(argv[1], 0, 0)) < 0)
    {
        printf("cat: %s: no such file or directory\n", argv[1]);
        return 0;
    }

    char buffer[200];
    read(fd, buffer, 200);
    printf("%s\n", buffer);
    for(;;);
    return 0;

    /*FILE* file = fopen(argv[1], "r");
    char buffer[200];
    fread(buffer, 1, 200, file);
    printf("%s\n", buffer);
    fclose(file);
    return 0;*/

}
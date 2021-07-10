#include <sys/internal.h>

void _start(int argc, char** argv)
{
    libc_init();
    int ret = main(argc, argv);
    exit(ret);
}
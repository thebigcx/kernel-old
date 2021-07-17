#include <sys/internal.h>
#include <unistd.h>

extern int main(int, void**);

void _start(int argc, void** argv)
{
    libc_start_main(main, argc, argv);
}

void libc_start_main(int (*main)(), int argc, void** argv)
{
    __libc_init();

    int ret = main(argc, argv);

    exit(ret);
}
#include <stdlib.h>
#include <signal.h>

int abs(int n)
{
    if (n < 0) return -n;
    return n;
}

void abort()
{
    raise(SIGABRT);
}
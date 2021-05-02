#include <stdlib.h>

static unsigned long next = 1;

void srand(unsigned int seed)
{
    next = seed;
}

int rand()
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / ((unsigned long)RAND_MAX * 2)) % ((unsigned long)RAND_MAX + 1);
}
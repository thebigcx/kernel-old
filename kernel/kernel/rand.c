#include <rand.h>
#include <math.h>

static uint64_t next = 1;

void rand_seed(uint32_t seed)
{
    next = seed;
}

static int32_t rand_gen()
{
    next = next * 1103515245 + 12345;
    return (uint32_t)(next / ((uint64_t)RAND_MAX * 2)) % ((uint64_t)RAND_MAX + 1);
}

uint64_t rand_range(uint64_t min, uint64_t max)
{
    return (rand_gen() / (double)RAND_MAX) * max + min;
}
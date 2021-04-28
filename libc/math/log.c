#include "math.h"

double log(double x)
{
    return x > 1 ? 1 + log(x / 10) : 0;
}
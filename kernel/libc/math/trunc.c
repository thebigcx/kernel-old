#include <math.h>

double trunc(double d)
{
    return d > 0 ? floor(d) : ceil(d);
}
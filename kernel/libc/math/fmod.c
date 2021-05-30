#include <math.h>

double fmod(double x, double y)
{
    return x - trunc(x / y) * y;
}
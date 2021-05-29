#include "math.h"

double pow(double x, double y)
{
    for (int i = 0; i < y - 1; i++)
        x *= x;

    return x;
}
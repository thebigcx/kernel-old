#include "math.h"

double ceil(double x)
{
    if (x < 0) return (int)x; // (int) truncates towards 0
    return (int)x + 1;
}
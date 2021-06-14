#include "stdlib.h"
#include "string.h"
#include "math.h"

char* itoa(int value, char* str, int base)
{
    if (base < 2 || base > 32)
        return str;

    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10)
            str[i++] = 65 + (r - 10);
        else
            str[i++] = 48 + r;

        n = n / base;
    }

    if (i == 0)
        str[i++] = '0';

    if (value < 0 && base == 10)
        str[i++] = '-';

    str[i] = '\0';

    return strrev(str);
}
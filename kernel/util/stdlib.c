#include <util/stdlib.h>

int memcmp(const void* s1, const void* s2, size_t n)
{
    unsigned char* cs1 = (unsigned char*)s1;
    unsigned char* cs2 = (unsigned char*)s2;

    for (size_t i = 0; i < n; i++)
    {
        if (cs1[i] < cs2[i]) return -1;
        if (cs1[i] > cs2[i]) return 1;
    }

    return 0;
}

void* memcpy(void* dst, const void* src, size_t n)
{
    unsigned char* cdst = (unsigned char*)dst;
    unsigned char* csrc = (unsigned char*)src;

    for (size_t i = 0; i < n; i++)
        cdst[i] = csrc[i];

    return dst;
}

void* memmove(void* dst, const void* src, size_t n)
{
    unsigned char* cdst = (unsigned char*)dst;
    unsigned char* csrc = (unsigned char*)src;

    for (size_t i = 0; i < n; i++)
        cdst[i] = csrc[i];

    return dst;
}

void* memset(void* dst, unsigned char c, size_t n)
{
    unsigned char* cdst = (unsigned char*)dst;

    for (size_t i = 0; i < n; i++)
        cdst[i] = c;

    return dst;
}

int strcmp(const char* str1, const char* str2)
{
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t len = len1 < len2 ? len1 : len2;

    if (len == 0 || len1 != len2) return 1;

    for (size_t i = 0; i < len; i++)
    {
        if (str1[i] != str2[i])
        {
            return str1[i] - str2[i];
        }
    }

    return 0;
}

char* strcpy(char* dst, const char* src)
{
    return strncpy(dst, src, strlen(src));
}

char* strncpy(char* dst, const char* src, size_t n)
{
    for (size_t i = 0; i < n && src[i] != '\0'; i++)
        dst[i] = src[i];

    return dst;
}

size_t strlen(const char* str)
{
    size_t s = 0;

    while (str[s] != '\0') s++;

    return s;
}

char* strrev(char* str)
{
    int i = strlen(str) - 1, j = 0;

    char c;
    while (i > j)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
        i--;
        j++;
    }

    return str;
}

int strncmp(const char* str1, const char* str2, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (str1[i] != str2[i])
        {
            return str1[i] - str2[i];
        }
    }

    return 0;
}

char* strsep(char** str, const char* delim)
{
    char* begin, *end;
    size_t len = strlen(delim);
    int s = 0;

    begin = *str;

    while (1)
    {
        if (begin[s] == '\0') break;

        for (size_t i = 0; i < len; i++)
        {
            if (begin[s] == delim[i]) break;
        }

        s++;
    }

    end = begin + s;

    if (*end)
    {
        *(end++) = '\0';
        *str = end;
    }
    else
    {
        *str = NULL;
    }

    return begin;
}

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

double floor(double x)
{
    if (x < 0) return (int)(x - 1);
    return (int)x;
}
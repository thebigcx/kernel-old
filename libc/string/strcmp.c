#include <string.h>

int strcmp(const char* str1, const char* str2)
{
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t len = len1 < len2 ? len1 : len2;

    for (size_t i = 0; i < len; i++)
    {
        if (str1[i] != str2[i])
        {
            return str1[i] - str2[i];
        }
    }

    return 0;
}
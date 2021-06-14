#include <string.h>

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
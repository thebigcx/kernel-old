#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

int vsprintf(char* str, const char* format, va_list list)
{
    uint32_t stridx = 0;
    char buffer[100];
    size_t len = 0;
    const char* arg = NULL;

    while (*format != '\0')
    {
        if (*format == '%')
        {
            format++;
            
            switch (*format)
            {
                case '%':
                    str[stridx++] = '%';
                    break;
                case 'c':
                    str[stridx++] = va_arg(list, int);
                    break;
                case 'i':
                case 'd':
                    itoa(va_arg(list, int), buffer, 10);
                    len = strlen(buffer);

                    for (int j = 0; j < len; j++)
                        str[stridx++] = buffer[j];

                    break;
                case 's':
                    arg = va_arg(list, const char*);
                    len = strlen(arg);

                    for (int i = 0; i < len; i++)
                        str[stridx++] = arg[i];
                    
                    break;
            }
        }
        else
        {
            str[stridx++] = *format;
        }
        format++;
    }

    str[stridx] = '\0';

    return stridx;
}
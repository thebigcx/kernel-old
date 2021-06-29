#include <stdio.h>

FILE _stdin = 
{
    .fd = 0
};

FILE _stdout = 
{
    .fd = 1
};

FILE _stderr = 
{
    .fd = 2
};

FILE* stdout = &_stdout;
FILE* stdin = &_stdin;
FILE* stderr = &_stderr;
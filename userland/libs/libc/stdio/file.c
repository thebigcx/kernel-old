#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

FILE* fopen(const char* path, const char* mode)
{
    FILE* file = malloc(sizeof(FILE));
    file->fd = open(path, 0, 0);
    return file;
}

int fclose(FILE* stream)
{
    int ret = close(stream->fd);
    free(stream);
    return ret;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return read(stream->fd, ptr, size * nmemb);
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return write(stream->fd, ptr, size * nmemb);
}

int fseek(FILE* stream, long int offset, int whence)
{
    return seek(stream->fd, offset, whence);
}
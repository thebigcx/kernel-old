#include <stdio.h>

extern FILE* vfs_open(const char* path);

FILE* fopen(const char* path, const char* mode)
{
    return vfs_open(path);
}

extern FILE* vfs_close(FILE* stream);

int fclose(FILE* stream)
{
    return vfs_close(stream);
}

extern FILE* vfs_read(void* ptr, size_t size, FILE* stream);

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return vfs_read(ptr, size * nmemb, stream);
}

extern FILE* vfs_write(const void* ptr, size_t size, FILE* stream);

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return vfs_write(ptr, size * nmemb, stream);
}
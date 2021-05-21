#include <stdio.h>

extern FILE* vfs_fopen(const char* path, const char* mode);

FILE* fopen(const char* path, const char* mode)
{
    return vfs_fopen(path, mode);
}

extern FILE* vfs_fclose(FILE* stream);

int fclose(FILE* stream)
{
    return vfs_fclose(stream);
}

extern FILE* vfs_fread(void* ptr, size_t size, size_t nmemb, FILE* stream);

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return vfs_fread(ptr, size, nmemb, stream);
}

extern FILE* vfs_fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return vfs_fwrite(ptr, size, nmemb, stream);
}
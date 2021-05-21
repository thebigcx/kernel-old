#include <drivers/fs/vfs/vfs.h>

fs_file_t* vfs_fopen(const char* path, const char* format)
{
    return root_mnt_pt.fs_dri.fopen(&root_mnt_pt.fs_dri, path, format);
}

int vfs_fclose(fs_file_t* stream)
{
    return root_mnt_pt.fs_dri.fclose(&root_mnt_pt.fs_dri, stream);
}

size_t vfs_fread(void* ptr, size_t size, size_t nmemb, fs_file_t* stream)
{
    return root_mnt_pt.fs_dri.fread(&root_mnt_pt.fs_dri, ptr, size, nmemb, stream);
}

size_t vfs_fwrite(const void* ptr, size_t size, size_t nmemb, fs_file_t* stream)
{
    return root_mnt_pt.fs_dri.fwrite(&root_mnt_pt.fs_dri, ptr, size, nmemb, stream);
}
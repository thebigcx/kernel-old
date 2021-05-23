#include <drivers/fs/vfs/vfs.h>

fs_file_t* vfs_open(const char* path)
{
    return root_mnt_pt.fs_dri.fopen(&root_mnt_pt.fs_dri, path);
}

int vfs_close(fs_file_t* stream)
{
    return root_mnt_pt.fs_dri.fclose(&root_mnt_pt.fs_dri, stream);
}

size_t vfs_read(void* ptr, size_t size, fs_file_t* stream)
{
    return root_mnt_pt.fs_dri.fread(&root_mnt_pt.fs_dri, ptr, size, stream);
}

size_t vfs_write(const void* ptr, size_t size, fs_file_t* stream)
{
    return root_mnt_pt.fs_dri.fwrite(&root_mnt_pt.fs_dri, ptr, size, stream);
}
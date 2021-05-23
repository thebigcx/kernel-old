#pragma once

#include <stddef.h>
#include <stdint.h>
#include <drivers/storage/dev.h>

#define FS_TYPE_FAT32 0

#define FS_SEEK_BEGIN 0
#define FS_SEEK_CURR 1
#define FS_SEEK_END 2

typedef struct fs_file_t
{
    uint32_t id;
    uint32_t pos;
    void* priv;

} fs_file_t;

typedef struct fs_dri
{
    fs_file_t* (*fopen)(struct fs_dri* dri, const char* path);
    size_t (*fread)(struct fs_dri* dri, void* ptr, size_t size, fs_file_t* stream);
    size_t (*fwrite)(struct fs_dri* dri, const void* ptr, size_t size, fs_file_t* stream);
    int (*fclose)(struct fs_dri* dri, fs_file_t* stream);
    
    int type;
    void* priv;

} fs_dri_t;

typedef struct mount
{
    dev_t* dev;
    fs_dri_t fs_dri;
    char* mnt_pt;

} mount_t;

#define MOUNT_LST_MAX 100

typedef struct mount_lst
{
    mount_t mnts[MOUNT_LST_MAX];
    uint32_t cnt;

} mount_lst_t;

extern mount_t root_mnt_pt;
extern mount_lst_t fs_mounts;

// mount.c
int fs_get_type(dev_t* dev);
void fs_mnt_disk(dev_t* dev, mount_t* mnt);

// vfs.c
fs_file_t* vfs_open(const char* path);
size_t vfs_read(void* ptr, size_t size, fs_file_t* stream);
size_t vfs_write(const void* ptr, size_t size, fs_file_t* stream);
int vfs_close(fs_file_t* stream);
int vfs_seek(fs_file_t* file, uint64_t off, int whence);
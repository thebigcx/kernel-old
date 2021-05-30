#pragma once

#include <stddef.h>
#include <stdint.h>
#include <drivers/storage/dev.h>

#define FS_TYPE_FAT32 0

#define FS_SEEK_BEGIN 0
#define FS_SEEK_CURR 1
#define FS_SEEK_END 2

typedef struct fs_node
{
    void* priv;

    size_t (*read)(struct fs_dri* dri, void* ptr, size_t size, struct fs_file* stream);
    size_t (*write)(struct fs_dri* dri, const void* ptr, size_t size, struct fs_file* stream);

} fs_node_t;

typedef struct fs_file_t
{
    fs_node_t* node;
    uint32_t pos;

} fs_file_t;

typedef struct fs_dri
{
    fs_node_t (*find_file)(struct fs_dri* dri, fs_node_t* dir, const char* name);
    
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
    mount_t* mnts[MOUNT_LST_MAX];
    uint32_t cnt;

} mount_lst_t;

extern mount_t* root_mnt_pt;
extern mount_lst_t fs_mnts;

// mount.c
int fs_get_type(dev_t* dev);
mount_t* fs_mnt_dev(dev_t* dev, const char* mnt_pt);

// vfs.c
fs_file_t* vfs_open(fs_node_t* node);
size_t vfs_read(fs_file_t* file, void* ptr, size_t size);
size_t vfs_write(fs_file_t* file, const void* ptr, size_t size);
void vfs_close(fs_file_t* file);
int vfs_seek(fs_file_t* file, uint64_t off, int whence);
fs_node_t vfs_resolve_path(const char* path, const char* working_dir);
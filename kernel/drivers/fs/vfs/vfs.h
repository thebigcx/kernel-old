#pragma once


#include <util/types.h>
#include <dev.h>

#define FS_TYPE_FAT32   0
#define FS_TYPE_EXT2    1

#define FS_SEEK_BEGIN   0
#define FS_SEEK_CURR    1
#define FS_SEEK_END     2

#define FS_UNKNWN       0x00
#define FS_FILE         0x01
#define FS_DIR          0x02
#define FS_CHARDEV      0x04
#define FS_BLKDEV       0x08
#define FS_PIPE         0x10
#define FS_SYMLINK      0x20
#define FS_MNTPT        0x40

// File descriptor
typedef struct fs_fd
{
    struct fs_node* node;
    size_t pos;
    uint32_t flags;

} fs_fd_t;

typedef struct fs_node
{
    fs_fd_t* (*open)(struct fs_node* file, uint32_t flags);
    size_t (*read)(struct fs_node* file, void* ptr, size_t off, size_t size);
    size_t (*write)(struct fs_node* file, const void* ptr, size_t off, size_t size);
    void (*close)(struct fs_node* file);

    void* derived;
    uint32_t flags;
    char* name;
    uint32_t size;

} fs_node_t;

typedef struct fs_vol
{
    fs_node_t (*finddir)(struct fs_vol* vol, fs_node_t* dir, const char* name);
    
    int type;
    void* derived;
    char* mnt_pt;

} fs_vol_t;

#define MOUNT_LST_MAX 100

typedef struct mount_lst
{
    fs_vol_t* mnts[MOUNT_LST_MAX];
    uint32_t cnt;

} mount_lst_t;

extern fs_vol_t* root_vol;
extern mount_lst_t fs_mnts;

// mount.c
int fs_get_type(dev_t* dev);
fs_vol_t* fs_mnt_dev(dev_t* dev, const char* mnt_pt);

// vfs.c
fs_fd_t* vfs_open(fs_node_t* node, uint32_t flags);
size_t vfs_read(fs_node_t* file, void* ptr, size_t off, size_t size);
size_t vfs_write(fs_node_t* file, const void* ptr, size_t off, size_t size);
void vfs_close(fs_node_t* file);
fs_node_t vfs_resolve_path(const char* path, const char* working_dir);
void vfs_mk_dev_file(fs_node_t node, const char* path);
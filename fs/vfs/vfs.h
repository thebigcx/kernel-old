#pragma once

#include <util/types.h>
#include <util/list.h>
#include <util/tree.h>
#include <sched/sched.h>

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
    struct vfs_node* node;
    size_t pos;
    uint32_t flags;
    uint32_t mode;

} fs_fd_t;

typedef struct vfs_node
{
    fs_fd_t* (*open)(struct vfs_node* file, uint32_t flags, uint32_t mode);
    size_t (*read)(struct vfs_node* file, void* ptr, size_t off, size_t size);
    size_t (*write)(struct vfs_node* file, const void* ptr, size_t off, size_t size);
    void (*close)(struct vfs_node* file);
    struct vfs_node* (*finddir)(struct vfs_node* dir, const char* name);
    list_t* (*listdir)(struct vfs_node* dir);
    void (*mkfile)(struct vfs_node* parent, const char* name);
    void (*mkdir)(struct vfs_node* parent, const char* name);
    int (*ioctl)(struct vfs_node* file, uint64_t request, void* argp);
    void* (*mmap)(struct vfs_node* file, proc_t* proc, void* addr, size_t len, int prot, int flags, size_t off);

    void* device;
    uint32_t flags;
    char* name;
    uint32_t size;
    uint32_t inode_num;

} vfs_node_t;

typedef struct vfs_ent
{
    char* name;
    vfs_node_t* file;

} vfs_ent_t;

typedef struct vfs_path
{
    list_t* parts;

} vfs_path_t;

typedef struct vfs_stat
{
    uint64_t ino;
    uint32_t mode;
    uint64_t size;

} vfs_stat_t;

extern tree_t* vfs_tree;

// mount.c
int vfs_get_type(vfs_node_t* dev);
void vfs_mount(vfs_node_t* dev, const char* mnt_pt);
vfs_node_t* vfs_get_mountpoint(vfs_path_t* path);

// path.c
vfs_path_t* vfs_mkpath(const char* pathstr, const char* working_dir);
void vfs_destroy_path(vfs_path_t* path);
char* vfs_mk_canonpath(char* path, char* working);

// vfs.c
void vfs_init();
fs_fd_t* vfs_open(vfs_node_t* node, uint32_t flags, uint32_t mode);
size_t vfs_read(vfs_node_t* file, void* ptr, size_t off, size_t size);
size_t vfs_write(vfs_node_t* file, const void* ptr, size_t off, size_t size);
void vfs_close(fs_fd_t* file);
vfs_node_t* vfs_resolve_path(const char* path, const char* working_dir);
list_t* vfs_listdir(vfs_node_t* dir);
void vfs_mkfile(vfs_node_t* parent, const char* name);
void vfs_mkdir(vfs_node_t* parent, const char* name);
int vfs_ioctl(vfs_node_t* file, uint64_t request, void* argp);
int vfs_stat(const char* path, vfs_stat_t* stat);

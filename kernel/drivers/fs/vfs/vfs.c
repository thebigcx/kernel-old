#include <drivers/fs/vfs/vfs.h>
#include <string.h>
#include <mem/heap.h>

struct dev_node
{
    const char* path;
    fs_node_t node;
} dev_nodes[128];
uint32_t dev_node_cnt = 0;

void vfs_mk_dev_file(fs_node_t node, const char* path)
{
    dev_nodes[dev_node_cnt].path = path;
    dev_nodes[dev_node_cnt].node = node;
    dev_node_cnt++;
}

// Returns true if file doesn't exist, etc
int vfs_open(fs_node_t* file)
{
    return file->open(file);
}

void vfs_close(fs_node_t* file)
{
    file->close(file);
}

size_t vfs_read(fs_node_t* file, void* ptr, size_t off, size_t size)
{
    return file->read(file, ptr, off, size);
}

size_t vfs_write(fs_node_t* file, const void* ptr, size_t off, size_t size)
{
    return file->write(file, ptr, off, size);
}

void strsplit(char** arr, const char* str, char c, uint32_t* cnt)
{
    size_t len = strlen(str);
    int arr_len = 0;

    char* token = kmalloc(len);
    int tok_size = 0;
    for (size_t i = 0; i < len + 1; i++)
    {
        if (str[i] == c || str[i] == '\0')
        {
            if (tok_size)
            {
                token[tok_size] = '\0';
                strcpy(arr[arr_len++], token);
                tok_size = 0;
            }
            continue;
        }

        token[tok_size++] = str[i];
    }

    kfree(token);

    *cnt = arr_len;
}

fs_node_t vfs_resolve_path(const char* path, const char* working_dir)
{
    for (uint32_t i = 0; i < dev_node_cnt; i++)
    {
        if (path[0] != '/' && working_dir)
        {
            char* new_path = kmalloc(strlen(path) + strlen(working_dir) + 1);
            strcpy(new_path, working_dir);
            new_path[strlen(working_dir)] = '/';
            strcpy(new_path + strlen(working_dir) + 1, path);

            if (strcmp(dev_nodes[i].path, new_path) == 0)
                return dev_nodes[i].node;
        }
        else
        {
            if (strcmp(dev_nodes[i].path, path) == 0)
                return dev_nodes[i].node;
        }
    }

    char* parts[10];
    for (int i = 0; i < 10; i++)
        parts[i] = kmalloc(32);

    uint32_t part_cnt;

    if (working_dir && path[0] != '/')
    {
        uint32_t cnt;
        
        strsplit(parts, working_dir, '/', &cnt);
        strsplit(parts + cnt, path, '/', &part_cnt);
        
        part_cnt += cnt;
    }
    else
    {
        strsplit(parts, path, '/', &part_cnt);
    }

    fs_node_t node;

    for (uint32_t i = 0; i < part_cnt; i++)
    {
        if (i == 0)
        {
            node = root_mnt_pt->fs_dri.find_file(&root_mnt_pt->fs_dri, NULL, parts[i]);
            continue;
        }

        node = root_mnt_pt->fs_dri.find_file(&root_mnt_pt->fs_dri, &node, parts[i]);
    }

    for (int i = 0; i < 10; i++)
        kfree(parts[i]);

    return node;
}
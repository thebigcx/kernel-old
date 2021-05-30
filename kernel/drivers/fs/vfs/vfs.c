#include <drivers/fs/vfs/vfs.h>
#include <string.h>
#include <mem/heap.h>

fs_file_t* vfs_open(fs_node_t* node)
{
    fs_file_t* file = kmalloc(sizeof(fs_file_t));
    file->node = node;
    file->pos = 0;
    return file;
}

void vfs_close(fs_file_t* file)
{
    kfree(file);
}

size_t vfs_read(fs_file_t* file, void* ptr, size_t size)
{
    return file->node->read(&root_mnt_pt->fs_dri, ptr, size, file);
}

size_t vfs_write(fs_file_t* file, const void* ptr, size_t size)
{
    return file->node->write(&root_mnt_pt->fs_dri, ptr, size, file);
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

    //kfree(token);

    *cnt = arr_len;
}

fs_node_t vfs_resolve_path(const char* path, const char* working_dir)
{
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
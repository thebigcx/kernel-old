#include <drivers/fs/vfs/vfs.h>

list_t* strsplit(const char* str, char c)
{
    list_t* list = list_create();

    size_t len = strlen(str);

    char* token = kmalloc(len);
    int tok_size = 0;
    for (size_t i = 0; i < len + 1; i++)
    {
        if (str[i] == c || str[i] == '\0')
        {
            if (tok_size)
            {
                token[tok_size] = '\0';
                
                char* ele = kmalloc(tok_size);
                strcpy(ele, token);
                list_push_back(list, ele);
                
                tok_size = 0;
            }
            continue;
        }

        token[tok_size++] = str[i];
    }

    kfree(token);

    return list;
}

// Make a path struct.
// Working dir is expected to be NULL or begin with '/'
// If working dir is NULL, pathstr should begin with '/'
vfs_path_t* vfs_mkpath(const char* pathstr, const char* working_dir)
{
    char* abspath;

    if (pathstr[0] == '/' && strlen(pathstr) == 1)
    {
        vfs_path_t* path = kmalloc(sizeof(vfs_path_t));
        path->parts = list_create();

        return path;
    }
    
    if (working_dir && pathstr[0] != '/') // Relative path
    {
        abspath = kmalloc(strlen(pathstr) + strlen(working_dir) + 1);
        strcpy(abspath, working_dir + 1);
        strcpy(abspath + strlen(working_dir), pathstr);
    }
    else // Absolute path
    {
        abspath = kmalloc(strlen(pathstr));
        strcpy(abspath, pathstr + 1);
        abspath[strlen(pathstr)] = 0;
    }

    vfs_path_t* path = kmalloc(sizeof(vfs_path_t));
    path->parts = strsplit(abspath, '/');

    kfree(abspath);
    return path;
}

void vfs_destroy_path(vfs_path_t* path)
{
    list_foreach(path->parts, part)
    {
        kfree(part); // char*
    }

    list_destroy(path->parts);
    kfree(path);
}
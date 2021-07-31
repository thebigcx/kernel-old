#include <fs/vfs/vfs.h>

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
        strcpy(abspath, working_dir);
        strcpy(abspath + strlen(working_dir), pathstr);
    }
    else // Absolute path
    {
        abspath = kmalloc(strlen(pathstr) + 1);
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

// Make canonical path (shortest)
char* vfs_mk_canonpath(char* path, char* working)
{
	char* temppath;

	if (working && path[0] != '/')
	{
		temppath = kmalloc(strlen(path) + strlen(working) + 2); // Slash in between, null terminator
		strcpy(temppath, working);
		strcpy(temppath + strlen(temppath), "/");
		strcpy(temppath + strlen(temppath), path);
	}
	else
	{
		temppath = kmalloc(strlen(path) + 1);
		strcpy(temppath, path);
	}

	list_t* tokens = list_create();

	size_t finalsize = 1;
	
	char* token = strtok(temppath, "/");
	while (token != NULL)
	{
		if (token[0] == 0)
		{
			kfree(token);
		}
		else if (strcmp(token, "..") == 0)
		{
			if (tokens->cnt > 0)
			{
				char* del_token = list_pop_back(tokens);
				finalsize -= strlen(del_token) + 1;
				kfree(del_token);
			}
		}
		else if (strcmp(token, ".") == 0)
		{
			kfree(token);	
		}
		else
		{
			list_push_back(tokens, token);
			finalsize += strlen(token) + 1;
		}

		token = strtok(NULL, "/");
	}

	finalsize++; // Null-terminator
	char* final = kmalloc(finalsize);
	
	if (tokens->cnt == 0)
	{
		strcpy(final, "/");
	}
	else
	{
		list_foreach(tokens, node)
		{
			char* token = node->val;
			strcpy(final + strlen(final), "/");
			strcpy(final + strlen(final), token);
		}
	}

	uint32_t cnt = tokens->cnt;
	for (uint32_t i = 0; i < cnt; i++)
	{
		kfree(list_pop_back(tokens));
	}

	return final;
}

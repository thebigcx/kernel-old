#include "paging.h"
#include "stdlib.h"

void page_init()
{
    for (unsigned int i = 0; i < PAGE_ENTRY_CNT; i++)
    {
        memset(&(page_directory[i]), 0, sizeof(page_dir_entry_t));

        page_directory[i].user_super = true;
        page_directory[i].read_write = true;
    }
}
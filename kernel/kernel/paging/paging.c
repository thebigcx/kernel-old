#include "paging.h"
#include "string.h"

// Definitions
page_dir_entry_t page_directory[PAGE_ENTRY_CNT] __attribute__((aligned(PAGE_SIZE)));
page_table_entry_t page_table[PAGE_ENTRY_CNT] __attribute__((aligned(PAGE_SIZE)));

void page_init()
{
    for (unsigned int i = 0; i < PAGE_ENTRY_CNT; i++)
    {
        memset(&(page_directory[i]), 0, sizeof(page_dir_entry_t));

        page_directory[i].user_super = true;
        page_directory[i].read_write = true;
    }

    for (unsigned int i = 0; i < PAGE_ENTRY_CNT; i++)
    {
        memset(&(page_table[i]), 0, sizeof(page_table_entry_t));

        page_table[i].address = i * PAGE_SIZE;

        page_table[i].user_super = true;
        page_table[i].read_write = true;
        page_table[i].present = false;
    }

    page_directory[0].address = page_table;
}
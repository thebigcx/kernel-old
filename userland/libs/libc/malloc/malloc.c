#include <sys/internal.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

typedef struct heapblock
{
    struct heapblock* next;
    struct heapblock* prev;
    int free;

} heapblock_t;

static heapblock_t* heap_start;
static heapblock_t* last_block;
static void* heap_end;

void _heap_expand(size_t size)
{
    //void* addr = mmap(NULL, size, 0, 0, 0, 0);
}

void __malloc_init()
{
    //_heap_expand(0x10000);
    
}

void* malloc(size_t size)
{
    
}

void free(void* ptr)
{

}

void* realloc(void* ptr, size_t size)
{

}

void* calloc(size_t nitems, size_t size)
{

}

#include <stdlib.h>
#include <stdint.h>

typedef struct heapnode
{
    struct heapnode* next;
    uint32_t len;
    uint8_t free;

} heapnode_t;

static heapnode_t* heapstart;
static heapnode_t* heapend;

void _malloc_init()
{
    
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
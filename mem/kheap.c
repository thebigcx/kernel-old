#include <mem/kheap.h>
#include <mem/paging.h>
#include <util/types.h>
#include <util/stdlib.h>
#include <util/spinlock.h>

static void* heap_start;
static void* heap_end;
static heap_block_t* last_block;
static lock_t kheap_lock = 0;

static void heap_combine_forward(heap_block_t* block)
{
    if (block->next == NULL) return;
    if (!block->next->free) return;
    if (block->next == last_block) last_block = block;

    if (block->next->next != NULL)
        block->next->next->prev = block;

    block->len += block->next->len + sizeof(heap_block_t);
    block->next = block->next->next;
}

static void heap_combine_back(heap_block_t* block)
{
    if (block->prev != NULL && block->prev->free) heap_combine_forward(block->prev);
}

static heap_block_t* heap_split_block(heap_block_t* block, size_t len)
{
    // Must be 16-byte aligned
    if (len < 16) return NULL;
    int64_t split_block_len = block->len - len - sizeof(heap_block_t);
    if (split_block_len < 16) return NULL;

    heap_block_t* new = (heap_block_t*)((size_t)block + len + sizeof(heap_block_t));
    if (block->next)
        block->next->prev = new;
    new->next = block->next;
    block->next = new;
    new->prev = block;
    new->len = split_block_len;
    new->free = block->free;
    block->len = len;

    if (last_block == block) last_block = new;
    return new;
}

void kheap_init()
{
    // Pre-allocate 1000 pages and map them to physical pages
    void* addr = page_kernel_alloc4k(1000);
    for (uint32_t i = 0; i < 1000; i++)
    {
        void* phys = pmm_request();
        page_kernel_map_memory(addr + i * PAGE_SIZE_4K, phys, 1);
    }
    
    size_t heap_len = 1000 * PAGE_SIZE_4K;

    heap_start = addr;
    heap_end = (void*)((size_t)heap_start + heap_len);

    heap_block_t* start_block = (heap_block_t*)addr;
    start_block->len = 0;
    start_block->next = NULL;
    start_block->prev = NULL;
    start_block->free = true;

    last_block = start_block;
}

void* kmalloc(size_t n)
{
    // Must be a multiple of 16 bytes
    if (n % 16 != 0)
        n = n - (n % 16) + 16;

    acquire_lock(kheap_lock);

    heap_block_t* curr = (heap_block_t*)heap_start;
	if (!curr)
	{
		kheap_expand(n);
		return kmalloc(n);
	}

    while (1)
    {
        if (curr->free)
        {
            if (curr->len > n + sizeof(heap_block_t))
            {
                heap_split_block(curr, n);
                curr->free = false;
                release_lock(kheap_lock);
                memset((void*)((uint64_t)curr + sizeof(heap_block_t)), 0, n);
                return (void*)((uint64_t)curr + sizeof(heap_block_t));
            }
            if (curr->len == n)
            {
                curr->free = false;
                release_lock(kheap_lock);
                memset((void*)((uint64_t)curr + sizeof(heap_block_t)), 0, n);
                return (void*)((uint64_t)curr + sizeof(heap_block_t));
            }
        }

        if (curr->next == NULL) break;
        curr = curr->next;
    }
    
    release_lock(kheap_lock);

    // Expand heap and re-run kmalloc()
    kheap_expand(n);
    return kmalloc(n);
}

void kfree(void* ptr)
{
    acquire_lock(kheap_lock);

    heap_block_t* block = (heap_block_t*)ptr - 1;
    block->free = true;
    heap_combine_forward(block);
    heap_combine_back(block);

    release_lock(kheap_lock);
}

void* krealloc(void* ptr, size_t size)
{
    size_t block_sz = ((heap_block_t*)((uint64_t)ptr - sizeof(heap_block_t)))->len;
    if (block_sz < size) return ptr; // Reallocation to less than previous size is UB

    void* new = kmalloc(size);
    memcpy(new, ptr, block_sz);

    kfree(ptr);

    return new;
}

void kheap_expand(size_t n)
{
    acquire_lock(kheap_lock);

    heap_block_t* new = (uint64_t)last_block + last_block->len + sizeof(heap_block_t);

    new->free = true;
    new->prev = last_block;
    last_block->next = new;
    last_block = new;
    new->next = NULL;
    new->len = n;
    
    heap_combine_back(new);
    release_lock(kheap_lock);
}

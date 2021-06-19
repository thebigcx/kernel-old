#pragma once

#include <util/types.h>

typedef struct heap_block
{
    size_t len;
    bool free;
    struct heap_block* prev;
    struct heap_block* next;
} heap_block_t;

void heap_init();
void* kmalloc(size_t n);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t size);
void heap_expand(size_t n);
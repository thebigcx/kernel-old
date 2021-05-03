#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct heap_block
{
    size_t len;
    bool free;
    struct heap_block* prev;
    struct heap_block* next;
} heap_block_t;

void heap_init(void* addr, size_t pg_cnt);
void* _malloc(size_t n);
void _free(void* ptr);
void* _realloc(void* ptr, size_t size);
void heap_expand(size_t n);
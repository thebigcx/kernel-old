#pragma once

#include "stddef.h"

typedef struct heap_seg
{
    size_t length;
} heap_seg_t;

void heap_init();
void* _malloc(size_t n);
void _free(void* ptr);
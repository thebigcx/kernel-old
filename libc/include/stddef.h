#pragma once

#define NULL (void*)0

typedef unsigned long size_t;
typedef long ptrdiff_t;
typedef int wchar_t;

// Create a struct 'st' at address 0x0, and take the address of member 'mem'
#define offsetof(st, mem)\
    ((size_t)&((st*)0)->mem)
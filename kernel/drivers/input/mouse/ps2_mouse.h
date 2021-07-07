#pragma once


#include <util/types.h>
#include <drivers/fs/vfs/vfs.h>

#define MOUSE_BTN_LEFT  (1 << 0)
#define MOUSE_BTN_RIGHT (1 << 1)
#define MOUSE_BTN_MID   (1 << 2)

typedef struct mouse_packet
{
    int8_t btns;
    int8_t xmov;
    int8_t ymov;
    int8_t scroll;
    
} mouse_packet_t;

void mouse_init();
bool mouse_get_packet(mouse_packet_t* packet);

size_t mouse_vfs_read(vfs_node_t* file, void* ptr, size_t off, size_t size);
#pragma once


#include <util/types.h>
#include <drivers/fs/vfs/vfs.h>

#define MOUSE_BTN_LEFT  (1 << 0)
#define MOUSE_BTN_RIGHT (1 << 1)
#define MOUSE_BTN_MID   (1 << 2)

typedef struct mouse_packet
{
    int8_t buttons;
    int8_t x_mov;
    int8_t y_mov;
    int8_t vert_scroll;
    
} mouse_packet_t;

void mouse_init();
bool mouse_get_packet(mouse_packet_t* packet);

size_t mouse_vfs_read(vfs_node_t* file, void* ptr, size_t off, size_t size);
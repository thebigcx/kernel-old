#pragma once

#include <stdint.h>

typedef struct mouse_packet
{
    int8_t buttons;
    int8_t x_mov;
    int8_t y_mov;
    int8_t vert_scroll;
    
} mouse_packet_t;

void mouse_init();
void mouse_map_int();
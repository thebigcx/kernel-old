#pragma once

#include <stdbool.h>
#include <stdint.h>

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
void mouse_map_int();
bool mouse_get_packet(mouse_packet_t* packet);
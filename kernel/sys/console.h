#pragma once

#include <util/types.h>

void console_putchar(char c, uint8_t r, uint8_t g, uint8_t b);
void console_write(const char* str, uint8_t r, uint8_t g, uint8_t b);
void console_clear();
void console_printf(const char* format, uint8_t r, uint8_t g, uint8_t b, ...);
void console_init();
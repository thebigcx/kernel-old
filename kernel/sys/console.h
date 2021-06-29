#pragma once

#include <util/types.h>

#define ANSI_RED    "\033[91m"
#define ANSI_GREEN  "\033[92m"
#define ANSI_YELLOW "\033[93m"
#define ANSI_BLUE   "\033[94m"
#define ANSI_PINK   "\033[95m"
#define ANSI_CYAN   "\033[96m"
#define ANSI_WHITE  "\033[97m"
#define ANSI_BOLD   "\033[1m"
#define ANSI_RESET  "\033[0m"

void console_putchar(char c, uint8_t r, uint8_t g, uint8_t b);
void console_write(const char* str, uint8_t r, uint8_t g, uint8_t b);
void console_clear();
void console_printf(const char* format, uint8_t r, uint8_t g, uint8_t b, ...);
void console_init();
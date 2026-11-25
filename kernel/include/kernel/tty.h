#pragma once

#include <stdint.h>
#include <stddef.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

struct Char {
	uint8_t character;
	uint8_t color;
};

#ifdef DEBUG
void k_putc(char c);
void k_puts(const char* s);
void k_puti(uint64_t i);
void k_clear(void);
void k_set_color(uint8_t color);
#else
#define k_putc(c)          ((void)0)
#define k_puts(s)          ((void)0)
#define k_puti(i)          ((void)0)
#define k_clear()          ((void)0)
#define k_set_color(color) ((void)0)
#endif

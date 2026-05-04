#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5

void vga_init_and_clear();
void vga_putchar(char c);
void vga_write(const char* str);
void vga_update_cursor(int x, int y);
#endif
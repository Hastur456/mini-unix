#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void terminal_initialize();
void terminal_putchar(char c);
void terminal_write(const char* str);

#endif
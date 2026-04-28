#include <stdint.h>
#include "vga.h"

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static int row = 0;
static int column = 0;
static uint8_t color = 0x0F; // white on black

static uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void terminal_initialize() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', color);
        }
    }
}

void terminal_putchar(char c) {

    if (c == '\n') {
        column = 0;
        row++;
        return;
    }

    if (c == '\b') {
        if (column > 0) {
            column--;
        } 
        else if (row > 0) {
            row--;
            column = VGA_WIDTH - 1;
        }

        VGA_MEMORY[row * VGA_WIDTH + column] = vga_entry(' ', color);
        
        return;
    }

    VGA_MEMORY[row * VGA_WIDTH + column] = vga_entry(c, color);

    column++;

    if (column >= VGA_WIDTH) {
        column = 0;
        row++;
    }

    if (row >= VGA_HEIGHT) {
        row = 0;
    }
}

void terminal_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}

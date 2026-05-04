#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "io.h"

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static int row = 0;
static int column = 0;
static uint8_t color = 0x0F; // white on black

static uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void vga_update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(VGA_CTRL_REGISTER, 0x0F);
    outb(VGA_DATA_REGISTER, (uint8_t)(pos & 0xFF));

    outb(VGA_CTRL_REGISTER, 0x0E);
    outb(VGA_DATA_REGISTER, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_init_and_clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', color);
        }
    }

    row = 0;
    column = 0;
}

void vga_putchar(char c) {

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

    vga_update_cursor(column, row);
}

void vga_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "io.h"

static volatile uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

uint8_t color = 0x0F;

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
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
}

void vga_putchar_at(char c, int x, int y) {
    VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(c, color);
}

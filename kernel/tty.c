#include <stdint.h>
#include <stddef.h>
#include "tty.h"
#include "vga.h"

static int tty_x = 0;
static int tty_y = 0;


void tty_init(void) {
    tty_x = 0;
    tty_y = 0;

    vga_init_and_clear();
    vga_update_cursor(tty_x, tty_y);
}

void tty_putchar(char c) {

    if (c == '\n') {
        tty_x = 0;
        tty_y++;
        vga_update_cursor(tty_x, tty_y);
        return;
    }

    if (c == '\b') {
        if (tty_x > 0) {
            tty_x--;
        } 
        else if (tty_y > 0) {
            tty_y--;
            tty_x = VGA_WIDTH - 1;
        }

        vga_putchar_at(' ', tty_x, tty_y);
        vga_update_cursor(tty_x, tty_y);
        return;
    }

    vga_putchar_at(c, tty_x, tty_y);

    tty_x++;

    if (tty_x >= VGA_WIDTH) {
        tty_x = 0;
        tty_y++;
    }

    if (tty_y >= VGA_HEIGHT) {
        tty_y = 0;
    }

    vga_update_cursor(tty_x, tty_y);
}

void tty_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        tty_putchar(str[i]);
    }
}

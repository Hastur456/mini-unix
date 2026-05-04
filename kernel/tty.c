#include <stdint.h>
#include <stddef.h>
#include "tty.h"
#include "vga.h"

static size_t tty_x;
static size_t tty_y;


void tty_init(void) {
    tty_x = 0;
    tty_y = 0;

    vga_init_and_clear();
    vga_update_cursor(tty_x, tty_y);
}
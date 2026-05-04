#include "vga.h"
#include "keyboard.h"
#include "tty.h"

void kmain() {
    tty_init();

    tty_write("Mini Unix Kernel\n");
    tty_write("Hello from VGA terminal!");

    while(1) {
        int c = keyboard_getchar();

        if (c != -1) {
            tty_putchar(c);
        }
    };
}

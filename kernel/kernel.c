#include "vga.h"
#include "keyboard.h"

void kmain() {
    vga_init_and_clear();

    vga_write("Mini Unix Kernel\n");
    vga_write("Hello from VGA terminal!");

    while(1) {
        int c = keyboard_getchar();

        if (c != -1) {
            vga_putchar(c);
        }
    };
}

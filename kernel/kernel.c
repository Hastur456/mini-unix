#include "vga.h"
#include "keyboard.h"

void kmain() {
    terminal_initialize();

    terminal_write("Mini Unix Kernel\n");
    terminal_write("Hello from VGA terminal!");

    while(1) {
        int c = keyboard_getchar();

        if (c != -1) {
            terminal_putchar(c);
        }
    };
}

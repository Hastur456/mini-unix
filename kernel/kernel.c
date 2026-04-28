#include "vga.h"
#include "keyboard.h"

void kmain() {

    terminal_initialize();

    terminal_write("Mini Unix Kernel\n");
    terminal_write("Hello from VGA terminal!");

    while(1) {
        if (kb_status() & 1) {
            uint8_t scancode = kb_scancode();

            if (!(scancode & 0x80)) {
                char c = kbdus[scancode];

                if (c) {
                    terminal_putchar(c);
                }
            }
        }
    };
}

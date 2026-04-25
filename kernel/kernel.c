#include "vga.h"

void kmain() {

    terminal_initialize();

    terminal_write("Mini Unix Kernel\n");
    terminal_write("Hello from VGA terminal!");

    while(1);
}
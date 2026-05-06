#include "vga.h"
#include "keyboard.h"
#include "tty.h"
#include "terminal.h"

void kmain() {
    tty_init();

    shell_launch();
}

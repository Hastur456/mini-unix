#include "interrupts.h"
#include "keyboard.h"
#include "pic.h"
#include "terminal.h"
#include "timer.h"
#include "tty.h"

void kmain() {
    tty_init();
    interrupts_init();
    pic_init();
    timer_init(100);
    keyboard_init();
    interrupt_enable();

    shell_launch();
}
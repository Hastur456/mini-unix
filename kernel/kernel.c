#include "interrupts.h"
#include "keyboard.h"
#include "pic.h"
#include "syscalls.h"
#include "terminal.h"
#include "timer.h"
#include "tty.h"


void kmain() {
    tty_init();
    interrupts_init();
    syscalls_init();
    pic_init();
    timer_init(100);
    keyboard_init();
    interrupt_enable();

    write(1, "hello from syscall write\n", 25);

    shell_launch();
}

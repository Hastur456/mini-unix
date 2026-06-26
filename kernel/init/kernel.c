#include "arch/x86/gdt.h"
#include "arch/x86/interrupts.h"
#include "arch/x86/pic.h"

#include "dev/keyboard.h"
#include "dev/terminal.h"
#include "dev/timer.h"
#include "dev/tty.h"

#include "proc/syscalls.h"

void kmain() {
    tty_init();

    gdt_init();

    interrupts_init();
    syscalls_init();
    pic_init();
    timer_init(100);
    keyboard_init();
    interrupt_enable();

    write(1, "hello from syscall write\n", 25);

    shell_launch();
}

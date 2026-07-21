#include <lib/panic.h>
#include <dev/tty.h>


void panic(const char *msg, const char *file, int line) {
    __asm__ volatile ("cli");

    tty_write("\n!!! KERNEL PANIC !!!\n");
    tty_write("Reason: ");
    tty_write(msg);
    tty_write("\n");
    
    if (file) {
        tty_write("File: ");
        tty_write(file);
    }

    tty_write("\nSystem halted permanently.\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
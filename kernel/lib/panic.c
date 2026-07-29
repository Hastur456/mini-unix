#include <lib/panic.h>
#include <lib/print.h>


__attribute__((noreturn))
void panic(const char *expr, const char *file, int line, const char *func)
{
    __asm__ volatile ("cli");

    kprint("\n!!! KERNEL PANIC !!!\n");

    if (expr) {
        kprintf("Assertion failed: %s\n", expr);
    }

    if (file) {
        kprintf("Location: %s:%d\n", file, line);
    }

    if (func) {
        kprintf("Function: %s\n", func);
    }

    kprint("\nSystem halted permanently.\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}

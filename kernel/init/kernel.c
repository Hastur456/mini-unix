#include <arch/x86/gdt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/pic.h>

#include <dev/keyboard.h>
#include <dev/terminal.h>
#include <dev/timer.h>
#include <dev/tty.h>

#include <fs/tmpfs.h>
#include <fs/vfs.h>

#include <proc/syscalls.h>

#define HEAP_START 0x100000
#define HEAP_SIZE  0x100000

void kmain(void)
{
    heap_init((void *)HEAP_START, HEAP_SIZE);
    /*
     * Console
     */
    tty_init();

    /*
     * CPU and interrupt infrastructure
     */
    gdt_init();
    interrupts_init();
    syscalls_init();

    /*
     * Hardware
     */
    pic_init();
    timer_init(100);
    keyboard_init();

    /*
     * Virtual filesystem
     */
    vfs_init();

    int err = tmpfs_init();
    if (err < 0) {
        kprintf("tmpfs_init failed: %d\n", err);
        return;
    }

    err = vfs_mount("/", "tmpfs", NULL);
    if (err < 0) {
        kprintf("tmpfs mount failed: %d\n", err);
        return;
    }

    err = create("/hello.txt");
    if (err < 0) {
        kprintf("create failed: %d\n", err);
    }
    /*
     * Start accepting interrupts.
     */
    interrupt_enable();

    /*
     * Enter user-facing shell.
     */
    shell_launch();
}
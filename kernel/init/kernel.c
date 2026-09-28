#include <arch/x86/gdt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/pic.h>
#include <arch/x86/tss.h>

#include <dev/keyboard.h>
#include <dev/timer.h>
#include <dev/tty.h>
#include <dev/tty_device.h>

#include <fs/tmpfs.h>
#include <fs/vfs.h>

#include <proc/syscalls.h>

#include <lib/string.h>
#include <lib/print.h>

extern void user_init(void);
extern void enter_user_mode(uint32_t entry);

#define USER_SHELL_ENTRY 0x00200000

void kmain(void)
{
    /* HEAP */
    heap_init(
        (void *)HEAP_START,
        HEAP_SIZE
    );

    /* Console. */
    tty_init();

    /* CPU / interrupts. */
    gdt_init();
    tss_init(0x001FFFF0);
    interrupts_init();
    syscalls_init();

    /* Hardware. */
    pic_init();
    timer_init(100);
    keyboard_init();

    /* VFS. */
    vfs_init();

    /* TMPFS */
    int err = tmpfs_init();
    if (err < 0) {
        kprintf(
            "tmpfs_init failed: %d\n",
            err
        );

        return;
    }

    /* Mount fo Tmpfs to VFS*/
    err = vfs_mount(
        "/",
        "tmpfs",
        NULL
    );
    if (err < 0) {
        kprintf(
            "tmpfs mount failed: %d\n",
            err
        );

        return;
    }

    /* Create /tty. */
    err = tty_device_init();

    if (err < 0) {
        kprintf(
            "tty_device_init failed: %d\n",
            err
        );

        return;
    }

    /* Enable keyboard/timer interrupts. */
    interrupt_enable();

    /* Embedded shell binary -> userspace. */
    user_init();

    /* Enter ring 3. */
    enter_user_mode(USER_SHELL_ENTRY);
}

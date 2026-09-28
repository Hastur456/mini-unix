#include <unistd.h>

extern void shell_main(int tty_fd);

__attribute__((section(".text.start")))
void _start(void)
{
    int tty_fd = open("/tty", O_RDWR);

    shell_main(tty_fd);

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

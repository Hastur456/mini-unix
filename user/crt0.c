#include <unistd.h>

extern void shell_main(int fd);

__attribute__((section(".text.start")))
void _start(void)
{
    int stdin_fd = open("/tty", O_RDWR);
    int stdout_fd = open("/tty", O_RDWR);
    int stderr_fd = open("/tty", O_RDWR);

    if (stdin_fd == 0 && stdout_fd == 1 && stderr_fd == 2) {
        shell_main(stdout_fd);
    } else {
            if (stdin_fd >= 0) {
            write(stdin_fd, "TTY initialization failed\n", 26);
        }
    }

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

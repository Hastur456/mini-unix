#include <proc/syscalls.h>

#include <dev/commands.h>

#define BUFFER_SIZE 128


struct Command commands[] = {
    {"echo", cmd_echo},
    {"cat", cmd_cat},
    {"fs-write", cmd_fs_write},
    {"fs-test", cmd_fs_test},
    {"help", cmd_help},
    {"exit", cmd_exit}
};

const int commands_count = sizeof(commands) / sizeof(commands[0]);


/*
 * Print an integer using the existing terminal interface.
 *
 * We deliberately do not use printf() because Mini-Unix
 * does not provide the hosted C stdio environment.
 */
static void print_int(int value)
{
    char buffer[16];
    int i = 0;

    if (value == 0) {
        tty_putchar('0');
        return;
    }

    if (value < 0) {
        tty_putchar('-');
        value = -value;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        tty_putchar(buffer[--i]);
    }
}


/*
 * echo <text>
 */
int cmd_echo(int argc, char **argv)
{
    if (argc != 2) {
        tty_write("usage: echo <text>\n");
        return 1;
    }

    tty_write(argv[1]);
    tty_putchar('\n');

    return 0;
}


/*
 * cat <path>
 *
 * User shell
 *     ↓
 * open()
 *     ↓
 * syscall
 *     ↓
 * sys_open()
 *     ↓
 * vfs_open()
 *     ↓
 * tmpfs
 */
int cmd_cat(int argc, char **argv)
{
    if (argc != 2) {
        tty_write("usage: cat <path>\n");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        tty_write("cat: open failed\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];

    for (;;) {
        int bytes = read(
            fd,
            buffer,
            sizeof(buffer)
        );

        if (bytes < 0) {
            tty_write("cat: read failed\n");
            close(fd);
            return 1;
        }

        if (bytes == 0) {
            break;
        }

        for (int i = 0; i < bytes; i++) {
            tty_putchar(buffer[i]);
        }
    }

    tty_putchar('\n');

    close(fd);

    return 0;
}


/*
 * fs-write <path> <text>
 *
 * This command is intentionally simple.
 *
 * It does NOT create files.
 * The file must already exist in tmpfs.
 *
 *     open(O_WRONLY)
 *          ↓
 *        write()
 *          ↓
 *        close()
 */
int cmd_fs_write(int argc, char **argv)
{
    if (argc != 3) {
        tty_write("usage: fs-write <path> <text>\n");
        return 1;
    }

    int fd = open(argv[1], O_WRONLY);

    if (fd < 0) {
        tty_write("fs-write: open failed\n");
        return 1;
    }

    const char *text = argv[2];

    uint32_t len = 0;

    while (text[len] != '\0') {
        len++;
    }

    int bytes = write(
        fd,
        text,
        len
    );

    if (bytes < 0) {
        tty_write("fs-write: write failed\n");
        close(fd);
        return 1;
    }

    close(fd);

    tty_write("written ");
    print_int(bytes);
    tty_write(" bytes\n");

    return 0;
}


/*
 * fs-test <path>
 *
 * Integration test for:
 *
 * shell
 *   ↓
 * open()
 *   ↓
 * syscall
 *   ↓
 * VFS
 *   ↓
 * tmpfs
 *
 * and then:
 *
 * write()
 * read()
 * close()
 */
int cmd_fs_test(int argc, char **argv)
{
    if (argc != 2) {
        tty_write("usage: fs-test <path>\n");
        return 1;
    }

    const char *path = argv[1];

    const char *test_data =
        "Mini-Unix syscall test";

    uint32_t test_len = 0;

    while (test_data[test_len] != '\0') {
        test_len++;
    }


    /*
     * 1. OPEN
     */
    tty_write("[1] open: ");

    int fd = open(path, O_RDWR);

    if (fd < 0) {
        tty_write("FAIL\n");

        tty_write("error: ");
        print_int(fd);
        tty_putchar('\n');

        return 1;
    }

    tty_write("OK, fd=");
    print_int(fd);
    tty_putchar('\n');


    /*
     * 2. WRITE
     */
    tty_write("[2] write: ");

    int written = write(
        fd,
        test_data,
        test_len
    );

    if (written < 0) {
        tty_write("FAIL\n");

        close(fd);

        return 1;
    }

    tty_write("OK, ");
    print_int(written);
    tty_write(" bytes\n");


    /*
     * 3. CLOSE
     */
    tty_write("[3] close: ");

    if (close(fd) < 0) {
        tty_write("FAIL\n");
        return 1;
    }

    tty_write("OK\n");


    /*
     * 4. OPEN AGAIN
     *
     * Reopening is important.
     *
     * It proves that the data is stored in tmpfs
     * and not merely sitting in the previous file descriptor.
     */
    tty_write("[4] reopen: ");

    fd = open(path, O_RDONLY);

    if (fd < 0) {
        tty_write("FAIL\n");
        return 1;
    }

    tty_write("OK, fd=");
    print_int(fd);
    tty_putchar('\n');


    /*
     * 5. READ
     */
    tty_write("[5] read: ");

    char buffer[BUFFER_SIZE];

    int bytes = read(
        fd,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes < 0) {
        tty_write("FAIL\n");
        close(fd);
        return 1;
    }

    buffer[bytes] = '\0';

    tty_write("OK, ");
    print_int(bytes);
    tty_write(" bytes\n");


    /*
     * 6. VERIFY DATA
     */
    tty_write("[6] verify: ");

    int valid = 1;

    if ((uint32_t)bytes != test_len) {
        valid = 0;
    } else {
        for (uint32_t i = 0; i < test_len; i++) {
            if (buffer[i] != test_data[i]) {
                valid = 0;
                break;
            }
        }
    }

    if (!valid) {
        tty_write("FAIL\n");

        tty_write("expected: ");
        tty_write(test_data);
        tty_putchar('\n');

        tty_write("actual:   ");
        tty_write(buffer);
        tty_putchar('\n');

        close(fd);

        return 1;
    }

    tty_write("OK\n");


    /*
     * 7. CLOSE
     */
    tty_write("[7] close: ");

    if (close(fd) < 0) {
        tty_write("FAIL\n");
        return 1;
    }

    tty_write("OK\n");


    tty_write("\n");
    tty_write("filesystem syscall test: PASS\n");

    return 0;
}


/*
 * help
 */
int cmd_help(int argc, char **args)
{
    (void)argc;
    (void)args;

    tty_write("Type program names and arguments, and hit enter.\n");
    tty_write("The following are built in:\n");

    for (int i = 0; i < commands_count; i++) {
        tty_write("  ");
        tty_write(commands[i].name);
        tty_putchar('\n');
    }

    return 0;
}


/*
 * exit
 */
int cmd_exit(int argc, char **args)
{
    (void)argc;
    (void)args;

    return 1;
}
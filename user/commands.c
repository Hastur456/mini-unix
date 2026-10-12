#include <unistd.h>
#include <stdint.h>
#include <lib/string.h>
#include <kernel/errno.h>
#include "commands.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

static void print(const char *str)
{
    write(STDOUT_FILENO, str, strlen(str));
}

static void println(const char *str)
{
    print(str);
    write(STDOUT_FILENO, "\n", 1);
}

static void print_int(int value)
{
    char buffer[11];
    unsigned int magnitude;
    int i = 0;

    if (value < 0) {
        write(STDOUT_FILENO, "-", 1);
        magnitude = 0u - (unsigned int)value;
    } else {
        magnitude = (unsigned int)value;
    }

    do {
        buffer[i++] = '0' + magnitude % 10;
        magnitude /= 10;
    } while (magnitude != 0);

    while (i > 0) {
        write(STDOUT_FILENO, &buffer[--i], 1);
    }
}

int cmd_echo(int argc, char **argv)
{
    if (argc < 2) {
        println("");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (i > 1)
            println(" ");

        print(argv[i]);
    }

    println("");

    return 0;
}

int cmd_cat(int argc, char **argv)
{
    char buffer[128];

    if (argc != 2) {
        println("usage: cat <file>");
        return 0;
    }

    int fd = open(argv[1], 0);

    if (fd < 0) {
        println("cat: cannot open file");
        return 0;
    }

    int bytes;

    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0)
        println(buffer);

    close(fd);

    return 0;
}

int cmd_help(int argc, char **argv)
{
    println("Built-in commands:");
    println("  echo");
    println("  cat");
    println("  help");
    println("  exit");
    println("  clear");

    return 0;
}

int cmd_exit(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    return 1;
}

int cmd_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print("\033[2J\033[H");
    return 0;
}

int cmd_fs_write(int argc, char **argv)
{
    if (argc != 3) {
        println("usage: fs-write <path> <text>");
        return 0;
    }

    int fd = open(argv[1], O_WRONLY);

    if (fd < 0) {
        println("fs-write: open failed");
        print(argv[1]);
        return 0;
    }

    int bytes = write(fd, argv[2], strlen(argv[2]));

    if (bytes < 0) {
        println("fs-write: write failed");
        close(fd);
        return 0;
    }

    close(fd);

    print("written ");
    print_int(bytes);
    println(" bytes");

    return 0;
}

int cmd_fs_create(int argc, char **argv)
{
    if (argc != 2) {
        println("usage: fs-create <path>");
        return 0;
    }

    int err = create(argv[1]);

    if (err >= 0)
        return 0;

    print("fs-create: ");

    switch (-err) {
    case EEXIST:
        println("File already exists.");
        break;
    case ENOENT:
        println("No such file or directory.");
        break;
    case ENOTDIR:
        println("A component of the path is not a directory.");
        break;
    case EACCES:
        println("Permission denied.");
        break;
    case EROFS:
        println("Read-only file system.");
        break;
    case ENOSPC:
        println("No space left on device.");
        break;
    case ENOMEM:
        println("Cannot allocate memory.");
        break;
    case EINVAL:
        println("Invalid argument.");
        break;
    case EFAULT:
        println("Bad address.");
        break;
    default:
        print("Unknown error ");
        print_int(-err);
        println(".");
        break;
    }

    return 0;
}

struct Command commands[] = {
    {"echo", cmd_echo},
    {"cat",  cmd_cat},
    {"help", cmd_help},
    {"exit", cmd_exit},
    {"clear", cmd_clear},
    {"fs-write", cmd_fs_write},
    {"fs-create", cmd_fs_create},
};

const int commands_count =
    sizeof(commands) / sizeof(commands[0]);
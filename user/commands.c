#include <unistd.h>
#include <stdint.h>
#include <lib/string.h>
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
        write(STDOUT_FILENO, buffer, bytes);

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

struct Command commands[] = {
    {"echo", cmd_echo},
    {"cat",  cmd_cat},
    {"help", cmd_help},
    {"exit", cmd_exit},
    {"clear", cmd_clear},
};

const int commands_count =
    sizeof(commands) / sizeof(commands[0]);
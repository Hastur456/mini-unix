#ifndef COMMANDS_H
#define COMMANDS_H

#include <dev/commands.h>

typedef int (*command_func_t)(int argc, char **argv);

struct Command {
    const char *name;
    command_func_t func;
};

extern struct Command commands[];
extern const int commands_count;

int cmd_echo(int argc, char **argv);
int cmd_cat(int argc, char **argv);
int cmd_fs_write(int argc, char **argv);
int cmd_fs_test(int argc, char **argv);
int cmd_help(int argc, char **argv);
int cmd_exit(int argc, char **argv);

#endif
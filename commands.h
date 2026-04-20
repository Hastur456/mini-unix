#ifndef COMMANDS_H
#define COMMANDS_H

typedef int (*command_fn)(int argc, char **argv);

struct Command {
    const char *name;
    command_fn fn;
};

extern struct Command commands[];
extern const int commands_count;

int cmd_ls(int argc, char **argv);
int cmd_pwd(int argc, char **argv);
int cmd_cd(int argc, char **argv);
int cmd_echo(int argc, char **argv);
int cmd_cat(int argc, char **argv);

#endif

#ifndef SHELL_H
#define SHELL_H

#define COMMAND_ARG_MAX 16

struct Command {
    const char *name;
    void (*func)(int argc, char **argv);
};

extern const struct Command commands[];
extern const int commands_count;

void shell_launch(void);

#endif
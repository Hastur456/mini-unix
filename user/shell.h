#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>
#include <lib/string.h>

#include "commands.h"

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64

int shell_read_line(char *buffer, int size);
int shell_split_line(char *line, char **tokens, int size);
int shell_execute(int argc, char **args);
void shell_main(int tty_fd);

#endif
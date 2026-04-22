#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
#include "commands.h"

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"


char *shell_read_line(void) {
    int bufsize = SHELL_RL_BUFSIZE;
    int position = 0;

    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += SHELL_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);

            if (!buffer) {
                fprintf(stderr, "Buffer allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


char **shell_split_line(char *line) {
    int buffersize = SHELL_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(buffersize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffersize) {
            buffersize += SHELL_TOK_BUFSIZE;
            tokens = realloc(tokens, buffersize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "shell: tokens not allocated");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHELL_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}   


int shell_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("shell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("lsh");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 0;
}


int shell_execute(char **args) {
    int argc = 0;

    if (args[0] == NULL) {
        return 1;
    }

    while (args[argc] != NULL) {
        argc++;
    }

    for (int i = 0; i < commands_count; i++) {
        if (strcmp(args[0], commands[i].name) == 0) {
            return commands[i].fn(argc, args);
        }
    }

    fprintf(stderr, "unknown command: %s\n", args[0]);
    return shell_launch(args);
}


void shell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = shell_read_line();
        args = shell_split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);

    } while (!status);
}

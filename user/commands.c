#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "commands.h"

#define BUFFER_SIZE 1024


struct Command commands[] = {
    {"ls", cmd_ls},
    {"pwd", cmd_pwd},
    {"cd", cmd_cd},
    {"echo", cmd_echo},
    {"cat", cmd_cat},
    {"exit", cmd_exit},
    {"help", cmd_help}
};

const int commands_count = sizeof(commands) / sizeof(commands[0]);


int cmd_ls(int argc, char **argv) {
    DIR *dir;
    struct dirent *entry;
    const char *path = (argc > 1) ? argv[1]: ".";

    if (argc < 2) {
        fprintf(stderr, "usage: ls {dir}\n");
        return 1;
    }

    dir = opendir(path);

    if (dir == NULL) {
        perror("Empty directory");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0) {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}

int cmd_pwd(int argc, char **argv) {
    char buffer[FILENAME_MAX];

    if (argc > 1) {
        fprintf(stderr, "usage: pwd\n");
        return 1;
    }

    if (getcwd(buffer, (int)sizeof(buffer)) == NULL) {
        perror("pwd");
        return 1;
    }

    printf("%s\n", buffer);
    return 0;
}

int cmd_cd(int argc, char **argv) {
    const char *path;

    if (argc > 2) {
        fprintf(stderr, "usage: cd [path]\n");
        return 0;
    }

    path = argc == 2 ? argv[1] : getenv("HOME");
    if (path == NULL) {
        path = ".";
    }

    if (chdir(path) != 0) {
        perror("cd");
        return 0;
    }
    
    return 0;
}


int cmd_echo(int argc, char **argv) {
    if (argc == 2) {
        printf("%s\n", argv[1]);
        return 0;
    }

    if (argc > 2 || argv[2] == ">") {
        int fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            return 1;
        }

        write(fd, argv[1], strlen(argv[1]));

        close(fd);
    }

    return 0;
}


int cmd_cat(int argc, char **argv) {
    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        perror("cat");
        return 0;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes;

    while ((bytes = read(fd, buffer, BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes);
    }

    close(fd);
    return 0;
}


int cmd_help(int argc, char **args)
{
  int i;
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < commands_count; i++) {
    printf("  %s\n", commands[i].name);
  }

  printf("Use the man command for information on other programs.\n");
  return 0;
}


int cmd_exit(int argc, char **args)
{
  return 1;
}

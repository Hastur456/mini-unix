#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "shell.h"


int main(int argc, char **argv) {
    shell_loop();

    // if (argc < 2) {
    //     fprintf(stderr, "usage: %s <command> [args]\n", argv[0]);
    //     return 1;
    // }

    // struct Command commands[] = {
    //     {"ls", cmd_ls},
    //     {"pwd", cmd_pwd},
    //     {"cd", cmd_cd},
    //     {"echo", cmd_echo},
    //     {"cat", cmd_cat}
    // };

    // for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    //     if (strcmp(argv[1], commands[i].name) == 0) {
    //         return commands[i].fn(argc - 1, argv + 1);
    //     }
    // }

    // fprintf(stderr, "unknown command: %s\n", argv[1]);
    return EXIT_SUCCESS;
}

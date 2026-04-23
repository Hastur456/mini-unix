#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "user/commands.h"
#include "user/shell.h"


int main(int argc, char **argv) {
    shell_loop();

    fprintf(stderr, "unknown command: %s\n", argv[1]);
    return EXIT_SUCCESS;
}

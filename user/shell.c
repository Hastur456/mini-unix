#include "shell.h"

static int shell_tty_fd = -1;


static void shell_print(const char *str)
{
    if (shell_tty_fd < 0)
        return;

    write(shell_tty_fd, str, strlen(str));
}


static void shell_print_error(const char *str)
{
    if (shell_tty_fd < 0)
        return;

    write(shell_tty_fd, str, strlen(str));
}


int shell_read_line(char *buffer, int size)
{
    int position = 0;

    if (!buffer || size <= 0)
        return -1;

    while (position < size - 1) {
        char c;

        int bytes = read(shell_tty_fd, &c, 1);

        if (bytes < 0) {
            return -1;
        }

        if (bytes == 0) {
            continue;
        }

        if (bytes < 0)
            return -1;

        if (bytes == 0)
            continue;

        /*
         * tty_read() currently gives us one character.
         */
        if (c == '\n' || c == '\r') {
            buffer[position] = '\0';

            shell_print("\n");

            return position;
        }

        /*
         * Basic backspace support.
         */
        if (c == '\b' || c == 127) {
            if (position > 0) {
                position--;

                shell_print("\b \b");
            }

            continue;
        }

        /*
         * Ignore other control characters.
         */
        if ((unsigned char)c < 32)
            continue;

        buffer[position++] = c;

        /*
         * Echo typed character.
         */
        write(shell_tty_fd, &c, 1);
    }

    buffer[position] = '\0';

    return position;
}


int shell_split_line(
    char *line,
    char **tokens,
    int size
)
{
    int position = 0;
    int i = 0;

    while (line[i] != '\0' && position < size - 1) {

        while (line[i] != '\0' &&
               (line[i] == ' '  ||
                line[i] == '\t' ||
                line[i] == '\r' ||
                line[i] == '\n')) {
            i++;
        }

        if (line[i] == '\0')
            break;

        tokens[position++] = &line[i];

        while (line[i] != '\0' &&
               line[i] != ' '  &&
               line[i] != '\t' &&
               line[i] != '\r' &&
               line[i] != '\n') {
            i++;
        }

        if (line[i] != '\0') {
            line[i] = '\0';
            i++;
        }
    }

    tokens[position] = NULL;

    return position;
}


int shell_launch(char **args)
{
    (void)args;

    shell_print_error("command execution is not supported\n");

    return 0;
}


int shell_execute(int argc, char **args)
{
    if (argc == 0 || args[0] == NULL)
        return 0;

    for (int i = 0; i < commands_count; i++) {
        if (strcmp(args[0], commands[i].name) == 0)
            return commands[i].fn(argc, args);
    }

    shell_print_error("unknown command: ");
    shell_print_error(args[0]);
    shell_print_error("\n");

    return shell_launch(args);
}


void shell_main(int tty_fd)
{
    char line[SHELL_RL_BUFSIZE];
    char *args[SHELL_TOK_BUFSIZE];

    shell_tty_fd = tty_fd;

    for (;;) {
        shell_print("mini-unix$ ");

        int length = shell_read_line(
            line,
            sizeof(line)
        );

        if (length < 0)
            break;

        int argc = shell_split_line(
            line,
            args,
            SHELL_TOK_BUFSIZE
        );
        
        if (shell_execute(argc, args)) break;
    }
}

#include <dev/shell.h>

#define COMMAND_BUFFER_SIZE 128

static char buffer[COMMAND_BUFFER_SIZE];
static int len = 0;

static void shell_prompt(void)
{
    tty_write("> ");
}

static void buffer_clear(void)
{
    len = 0;
    buffer[0] = '\0';
}

static void buffer_put(char c)
{
    if (len >= COMMAND_BUFFER_SIZE - 1) {
        return;
    }

    buffer[len++] = c;
    buffer[len] = '\0';

    tty_putchar(c);
}

static void buffer_backspace(void)
{
    if (len == 0) {
        return;
    }

    len--;
    buffer[len] = '\0';

    tty_putchar('\b');
}

static int is_space(char c)
{
    return c == ' ' || c == '\t';
}

static int parse_command(char *input, char **argv)
{
    int argc = 0;

    while (*input != '\0') {
        while (is_space(*input)) {
            input++;
        }

        if (*input == '\0') {
            break;
        }

        if (argc >= COMMAND_ARG_MAX) {
            return -1;
        }

        argv[argc++] = input;

        while (*input != '\0' && !is_space(*input)) {
            input++;
        }

        if (*input == '\0') {
            break;
        }

        *input = '\0';
        input++;
    }

    return argc;
}

static const struct Command *command_find(const char *name)
{
    for (int i = 0; i < commands_count; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return &commands[i];
        }
    }

    return 0;
}

static void command_execute(void)
{
    char *argv[COMMAND_ARG_MAX];

    int argc = parse_command(buffer, argv);

    if (argc == 0) {
        return;
    }

    if (argc < 0) {
        tty_write("shell: too many arguments\n");
        return;
    }

    const struct Command *command = command_find(argv[0]);

    if (command == 0) {
        tty_write("shell: command not found: ");
        tty_write(argv[0]);
        tty_putchar('\n');
        return;
    }

    command->func(argc, argv);
}

static void command_buffer_submit(void)
{
    tty_putchar('\n');

    command_execute();

    buffer_clear();
    shell_prompt();
}

void shell_launch(void)
{
    buffer_clear();
    shell_prompt();

    for (;;) {
        int c = keyboard_getchar();

        if (c == -1) {
            continue;
        }

        switch (c) {
        case '\n':
            command_buffer_submit();
            break;

        case '\b':
            buffer_backspace();
            break;

        default:
            buffer_put((char)c);
            break;
        }
    }
}
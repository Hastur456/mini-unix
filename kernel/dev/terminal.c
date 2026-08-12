#include <dev/terminal.h>


static char buffer[COMMAND_BUFFER_SIZE];
static int len = 0;

static void put_at_buffer(char c) {
    buffer[len] = c;
    len++;
    buffer[len] = '\0';

    tty_putchar(c);
}

static void clear_buffer(void) {
    len = 0;
    buffer[len] = '\0';
}

static void shell_prompt(void) {
    tty_write("> ");
}

static void buffer_backspace(void) {
    if (len == 0) {
        return;
    }

    len--;
    buffer[len] = '\0';
    tty_putchar('\b');
}

static void print_buffer(void) {
    tty_write("\nBuffer: [");

    for (int i = 0; i < len; i++) {
        tty_putchar(buffer[i]);
    }

    tty_write("]\n");
}

static void command_execute(const char* command) {
    if (strcmp(command, "") == 0) return;

    if (strcmp(command, "help") == 0) {
        tty_write("Commands: help, clear, echo <text>");
        tty_write("\n");
        return;
    }

    if (strcmp(command, "clear") == 0) {
        tty_init();
        return;
    }

    if (strncmp(command, "echo ", 5) == 0) {
        tty_write(command + 5);
        tty_write("\n");

        return;
    }

    tty_write("Unknown command: ");
    tty_write(command);
    tty_write("\n");
}

static void command_buffer_submit(void) {
    buffer[len] = '\0';
    tty_putchar('\n');
    command_execute(buffer);
    clear_buffer();
    shell_prompt();
}

void shell_launch(void) {
    shell_prompt();

    while(1) {
        int c = keyboard_getchar();

        if (c != -1) {
            if (c == '\n') {
                command_buffer_submit();
            }
            else if (c == '\b') {
                buffer_backspace();
            }
            else {
                put_at_buffer(c);
            }
        }
    };
}

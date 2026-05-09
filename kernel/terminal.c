#include "tty.h"
#include "keyboard.h"
#include "terminal.h"


static char buffer[COMMAND_BUFFER_SIZE];
static int len = 0;

static int str_eq(const char* a, const char* b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

static int str_starts_with(const char* str, const char* prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (str[i] != prefix[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

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
    tty_write("\n> ");
}

static void buffer_backspace(void) {
    if (len == 0) {
        return;
    }

    len--;
    buffer[len] = '\0';
    tty_putchar('\b');
}

static void command_execute(const char* command) {
    if (str_eq(command, "")) return;
    if (str_eq(command, "help")) {
        tty_write("Commands: help, clear, echo <text>");
        return;
    }

    if (str_eq(command, "clear")) {
        tty_init();
        return;
    }

    if (str_starts_with(command, "echo ")) {
        tty_write(command + 5);
        return;
    }

    tty_write("Unknown command: ");
    tty_write(command);
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

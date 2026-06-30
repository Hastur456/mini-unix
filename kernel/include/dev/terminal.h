#ifndef TERMINAL_H
#define TERMINAL_H

#define COMMAND_BUFFER_SIZE 128

static int str_eq(const char* a, const char* b);
static int str_starts_with(const char* str, const char* prefix);
static void put_at_buffer(char c);
static void clear_buffer(void);
static void shell_prompt(void);
static void buffer_backspace(void);
static void print_buffer(void);
static void command_execute(const char* command);
static void command_buffer_submit(void);
void shell_launch(void);

#endif

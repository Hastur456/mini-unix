#ifndef SHELL_H
#define SHELL_H

char *shell_read_line(void);
char **shell_split_line(char *line);
int shell_execute(char **args);
void shell_loop(void);

#endif

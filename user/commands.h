#ifndef COMMANDS_H
#define COMMANDS_H

typedef int (*command_fn)(int argc, char **argv);

struct Command {
    const char *name;
    command_fn fn;
};

extern struct Command commands[];
extern const int commands_count;

int cmd_echo(int argc, char **argv);
int cmd_cat(int argc, char **argv);
int cmd_exit(int argc, char **argv);
int cmd_help(int argc, char **argv);
int cmd_clear(int argc, char **argv);

// TODO: Implement filesystem navigation commands.
// - ls: list directory entries using VFS readdir support.
// - pwd: print the current working directory using getcwd.
// - cd: change the current working directory using chdir.
// - Add per-process cwd support to the process layer.
// - Implement VFS path resolution for absolute and relative paths.
// - Add syscalls and user-space wrappers for getcwd and chdir.

// int cmd_ls(int argc, char **argv);
// int cmd_pwd(int argc, char **argv);
// int cmd_cd(int argc, char **argv);
// ...

int cmd_fs_create(int argc, char **argv);
int cmd_fs_write(int argc, char **argv);


#endif

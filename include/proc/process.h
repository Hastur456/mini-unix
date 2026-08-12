#ifndef PROC_H
#define PROC_H

#include <errno.h>
#include <stddef.h>
#include <fs/vfs.h>

#define OPEN_MAX 32

typedef struct process {
    file_t *files[OPEN_MAX];
} process_t;

void process_init(process_t *proc);
int process_alloc_fd(process_t *proc);
file_t *process_get_file(process_t *proc, int fd);
int process_close_fd(process_t *proc, int fd);

#endif

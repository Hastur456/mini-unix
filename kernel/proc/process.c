#include "proc/process.h"


void process_init(process_t *proc) {
    if (!proc) return;

    for (int i = 0; i < OPEN_MAX; i++) {
        proc->files[i] = NULL;
    }
}

int process_alloc_fd(process_t *proc) {
    if (!proc) return -EINVAL;

    for (int i = 0; i < OPEN_MAX; i++) {
        if (proc->files[i] == NULL) return i;
    }

    return -EMFILE;
}

file_t *process_get_file(process_t *proc, int fd) {
    if (!proc) return NULL;

    if (fd <= 0 || fd > OPEN_MAX) return NULL;

    return proc->files[fd];
}

int process_close_fd(process_t *proc, int fd) {
    if (!proc) return -EINVAL;

    if (fd <= 0 || fd > OPEN_MAX) return -EINVAL;

    if (proc->files[fd] == NULL) return -EBADF;

    proc->files[fd] = NULL;

    return 0;
}

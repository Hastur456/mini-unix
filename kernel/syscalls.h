#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include "interrupts.h"
#include "tty.h"
#include "proc/process.h"
#include "fs/vfs.h"

#define SYSCALL_VECTOR 0x80

#define SYS_EXIT   1
#define SYS_FORK   2
#define SYS_READ   3
#define SYS_WRITE  4
#define SYS_OPEN   5
#define SYS_CLOSE  6

typedef int syscallarg_t;
typedef int (*sys_call_t)(process_t *p, void *uap, int *retval);

struct sysent {
    int sys_narg;
    sys_call_t sys_call;
};

struct sys_read_args {
    int fd;
    void *buf;
    uint32_t len;
};

struct sys_write_args {
    int fd;
    const char *buf;
    uint32_t len;
};

struct sys_open_args {
    const char *path;
    int flags;
};

struct sys_close_args {
    int fd;
};

void syscalls_init(void);
void syscall_dispatch(struct interrupt_frame *frame);

int open(const char *path, int flags);
int close(int fd);
int read(int fd, void *buf, uint32_t len);
int write(int fd, const char *buf, uint32_t len);

#endif

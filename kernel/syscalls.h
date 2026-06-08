#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include "interrupts.h"

#define SYSCALL_VECTOR 0x80

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4


struct proc {
    int pid;
};

typedef int syscallarg_t;
typedef int (*sys_call_t)(struct proc* p, void* uap, int* retval);

struct sysent {
    int sys_narg;
    sys_call_t sys_call;
};

struct sys_write_args {
    int fd;
    const char* buf;
    uint32_t len;
};

void syscalls_init(void);
void syscall_dispatch(struct interrupt_frame* frame);
int write(int fd, const char* buf, uint32_t len);
#endif
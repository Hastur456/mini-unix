#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include "arch/x86/interrupts.h"
#include "dev/tty.h"
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

typedef struct sys_read_args {
    int fd;
    void *buf;
    uint32_t len;
} sys_read_args_t;

typedef struct sys_write_args {
    int fd;
    const char *buf;
    uint32_t len;
} sys_write_args_t;

typedef struct sys_open_args {
    const char *path;
    int flags;
} sys_open_args_t;

typedef struct sys_close_args {
    int fd;
} sys_close_args_t;

typedef union sys_args
{
    syscallarg_t raw[6];

    sys_close_args_t close;
    sys_open_args_t open;
    sys_write_args_t write;
    sys_read_args_t read;

} sys_args_t;


static inline long syscall1(long num, long a1) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "b"(a1) : "memory");
    return ret;
}
static inline long syscall2(long num, long a1, long a2) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "b"(a1), "c"(a2) : "memory");
    return ret;
}
static inline long syscall3(long num, long a1, long a2, long a3) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "b"(a1), "c"(a2), "d"(a3) : "memory");
    return ret;
}
static inline long syscall4(long num, long a1, long a2, long a3, long a4) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "b"(a1), "c"(a2), "d"(a3), "S"(a4) : "memory");
    return ret;
}
static inline long syscall5(long num, long a1, long a2, long a3, long a4, long a5) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5) : "memory");
    return ret;
}

void syscalls_init(void);
void syscall_dispatch(struct interrupt_frame *frame);

int open(const char *path, int flags);
int close(int fd);
int read(int fd, void *buf, uint32_t len);
int write(int fd, const char *buf, uint32_t len);

#endif

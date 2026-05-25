#include "syscalls.h"
#include "interrupts.h"
#include "tty.h"


static struct proc kernel_proc = {
    .pid = 0,
};

static int sys_nosys(struct proc* p, void* uap, int* retval) {
    (void)p;
    (void)uap;

    *retval = -1;
    return -1;
}

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

static int sys_write(struct proc* p, void* uap, int* retval) {
    struct sys_write_args* args = (struct sys_write_args*)uap;

    (void)p;

    if (args->fd != 1 && args->fd != 2) {
        *retval = -1;
        return -1;
    }

    if (args->buf == 0) {
        *retval = -1;
        return -1;
    }

    for (uint32_t i=0; i < args->len; i++) {
        tty_putchar(args->buf[i]);
    }

    *retval = (int)args->len;
    return 0;
}

static struct sysent sysent[] = {
    {0, sys_nosys},  // 0
    {1, sys_nosys},  // 1 = exit
    {0, sys_nosys},  // 2 = fork
    {3, sys_nosys},  // 3 = read
    {3, sys_write},  // 4 = write
};

void syscall_dispatch(struct interrupt_frame* frame) {
    uint32_t number = frame->eax;
    int retval = 0;

    if (number >= sizeof(sysent) / sizeof(sysent[0])) {
        frame->eax = (uint32_t)-1;
        return;
    }

    struct sys_write_args args;

    args.fd = (int)frame->ebx;
    args.buf = (const char*)frame->ecx;
    args.len = frame->edx;

    int error = sysent[number].sys_call(&kernel_proc, &args, &retval);

    if (error) {
        frame->eax = (uint32_t)-1;
    } else {
        frame->eax = (uint32_t)retval;
    }
}

void syscalls_init(void) {
    interrupt_register_handler(SYSCALL_VECTOR, syscall_dispatch);
}


int write(int fd, const char* buf, uint32_t len) {
    return (int)syscall3(SYS_WRITE, fd, (long)buf, len);
}

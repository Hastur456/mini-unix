#include "proc/syscalls.h"

static int sys_nosys(process_t* p, void* uap, int* retval) {
    (void)p;
    (void)uap;
    *retval = -1;
    return -1;
}

static int sys_write(process_t *p, void *uap, int *retval);
static int sys_read(process_t *p, void *uap, int *retval);
static int sys_open(process_t *p, void *uap, int *retval);
static int sys_close(process_t *p, void *uap, int *retval);

static process_t kernel_proc;

static struct sysent sysent[] = {
    [0]         = {0, sys_nosys},
    [SYS_EXIT]  = {1, sys_nosys},
    [SYS_FORK]  = {0, sys_nosys},
    [SYS_READ]  = {3, sys_read},
    [SYS_WRITE] = {3, sys_write},
    [SYS_OPEN]  = {2, sys_open},
    [SYS_CLOSE] = {1, sys_close},
};

void syscall_dispatch(struct interrupt_frame *frame) {
    uint32_t number = frame->eax;
    sys_args_t args;
    int retval = 0;

    if (number >= sizeof(sysent) / sizeof(sysent[0])) {
        frame->eax = (uint32_t)-1;
        return;
    }

    if (sysent[number].sys_narg < 0 || sysent[number].sys_narg > 6) {
        frame->eax = (uint32_t)-1;
        return;
    }

    args.raw[0] = frame->ebx;
    args.raw[1] = frame->ecx;
    args.raw[2] = frame->edx;
    args.raw[3] = frame->esi;
    args.raw[4] = frame->edi;
    args.raw[5] = frame->ebp;

    int error = sysent[number].sys_call(&kernel_proc, &args, &retval);

    if (error) {
        frame->eax = (uint32_t)error;
    } else {
        frame->eax = (uint32_t)retval;
    }
}

void syscalls_init(void) {
    interrupt_register_handler(SYSCALL_VECTOR, syscall_dispatch);
}

static int sys_write(process_t *p, void *uap, int *retval) {
    sys_args_t *args = (sys_args_t *)uap;
    int fd = (int)args->write.fd;
    const char *buf = (const char *)args->write.buf;
    uint32_t len = (uint32_t)args->write.len;
    ssize_t written;

    (void)p;

    if (buf == NULL) {
        *retval = -1;
        return -EFAULT; 
    }

    written = vfs_write(p, fd, buf, len);

    if (written < 0) {
        *retval = -1;
        return (int)written;
    }

    *retval = (int)written;
    return 0;
}

static int sys_read(process_t *p, void *uap, int *retval) {
    sys_args_t *args = (sys_args_t *)uap;
    int fd = (int)args->read.fd;
    void *buf = args->read.buf;
    uint32_t len = (uint32_t)args->read.len;

    if (buf == NULL) {
        *retval = -1;
        return -EFAULT;
    }

    ssize_t bytes_read = vfs_read(p, fd, buf, len);

    if (bytes_read < 0) {
        *retval = -1;
        return (int)bytes_read;
    }

    *retval = (int)bytes_read;
    return 0;
}

static int sys_open(process_t *p, void *uap, int *retval) {
    sys_args_t *args = (sys_args_t *)uap;
    const char *path = args->open.path;
    int flags = args->open.flags;

    if (path == NULL) {
        *retval = -1;
        return -EFAULT;
    }

    int fd = vfs_open(p, path, flags);
    
    if (fd < 0) {
        *retval = -1;
        return fd;
    }

    *retval = fd;
    return 0;
}

static int sys_close(process_t *p, void *uap, int *retval) {
    sys_args_t *args = (sys_args_t *)uap;
    int fd = args->close.fd;

    int err = vfs_close(p, fd);
    
    if (err < 0) {
        *retval = -1;
        return err;
    }

    *retval = 0;
    return 0;
}

int close(int fd) {
    return (int)syscall1(SYS_CLOSE, fd);
}

int open(const char* path, int flags) {
    return (int)syscall2(SYS_OPEN, (long)path, flags);
}

int write(int fd, const char* buf, uint32_t len) {
    return (int)syscall3(SYS_WRITE, fd, (long)buf, len);
}

int read(int fd, void* buf, uint32_t len) {
    return (int)syscall3(SYS_READ, fd, (long)buf, len);
}
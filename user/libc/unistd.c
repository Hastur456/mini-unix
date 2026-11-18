#include <unistd.h>

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

int create(const char *path) {
    return (int)syscall1(SYS_CREATE, (long)path);
}

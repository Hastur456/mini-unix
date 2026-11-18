#ifndef UNISTD_H
#define UNISTD_H

#include <stdint.h>
#include <proc/syscalls.h>
#include <proc/syscalls_numbers.h>

int open(const char *path, int flags);
int close(int fd);
int read(int fd, void *buf, uint32_t len);
int write(int fd, const char *buf, uint32_t len);
int create(const char *path);

#endif

#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#define O_RDONLY    0x00
#define O_WRONLY    0x01
#define O_RDWR      0x02
#define O_ACCMODE   0x03

typedef struct file file_t;

struct vnode;

struct file {
    struct vnode *node;

    uint32_t offset;

    uint32_t flags;

    int refcount;
};

struct file *file_create(struct vnode *node, uint32_t flags);
void file_destroy(struct file *file);

#endif

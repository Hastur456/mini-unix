#ifndef FILE_H
#define FILE_H

#include <stdint.h>

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

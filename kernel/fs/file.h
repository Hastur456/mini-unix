#ifndef FILE_H
#define FILE_H


struct file {
    struct vnode *node;

    uint32_t offset;

    uint32_t flags;

    int refcount;
};

#endif

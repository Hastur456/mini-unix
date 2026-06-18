#ifndef TEMPFS_H
#define TEMPFS_H

#include <stdint.h>

typedef struct tmpfs_inode {
    uint32_t inode;

    vnode_type_t type;

    char name[VFS_MAX_NAME + 1];

    struct tmpfs_inode *parent;

    union {
        struct {
            struct tmpfs_inode **children;
            uint32_t child_count;
        } dir;

        struct {
            void *data;
            uint32_t size;
        } file;
    };
} tmpfs_inode_t;



#endif

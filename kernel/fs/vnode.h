#ifndef VNODE_H
#define VNODE_H

#include <stdint.h>
#include "mount.h"

struct vnode;

typedef enum {
    VNODE_FILE,
    VNODE_DIR,
    VNODE_CHARDEV,
    VNODE_BLOCKDEV
} vnode_type_t;


typedef struct vnode_ops {
    int (*open)(struct vnode *node);
    int (*close)(struct vnode *node);

    int (*read)(
        struct vnode *node,
        uint32_t offset,
        void *buffer,
        uint32_t size
    );

    int (*write)(
        struct vnode *node,
        uint32_t offset,
        const void *buffer,
        uint32_t size
    );

    int (*lookup)(
        struct vnode *dir,
        const char *name,
        struct vnode **result
    );

    int (*create)(
        struct vnode *dir,
        const char *name,
        struct vnode **result
    );
} vnode_ops_t;


typedef struct vnode {
    vnode_type_t type;

    uint32_t inode;
    uint32_t refcount;

    vnode_ops_t *ops;
    void *private_data;

    mount_t *mounted_here;
    struct vnode *parent;
} vnode_t;


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

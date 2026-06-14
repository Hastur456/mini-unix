#ifndef VNODE_H
#define VNODE_H

#include <stdint.h>
#include "mount.h"


typedef enum {
    VNODE_FILE,
    VNODE_DIR,
    VNODE_CHARDEV,
    VNODE_BLOCKDEV
} vnode_t;


struct vnode_ops {
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
};

typedef struct vnode_ops vnode_ops_t;

struct vnode {
    vnode_t type;
    uint32_t inode;
    uint32_t refcount;
    vnode_ops_t *ops;
    void *private_data;
    
    mount_t *mounted_here;
    vnode_t *parent;
};

struct mount *mounted_here;

#endif

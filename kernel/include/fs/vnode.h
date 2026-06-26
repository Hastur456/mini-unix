#ifndef VNODE_H
#define VNODE_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

struct vnode;
typedef struct mount mount_t;

#define VFS_MAX_NAME 255

typedef enum {
    VNODE_FILE,
    VNODE_DIR,
    VNODE_CHARDEV,
    VNODE_BLOCKDEV
} vnode_type_t;


typedef struct vnode_ops {
    int (*open)(struct vnode *node);
    int (*close)(struct vnode *node);

    ssize_t (*read)(
        struct vnode *node,
        size_t offset,
        void *buffer,
        size_t size
    );

    ssize_t (*write)(
        struct vnode *node,
        size_t offset,
        const void *buffer,
        size_t size
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
    char name[VFS_MAX_NAME + 1];

    uint32_t inode;
    uint32_t refcount;

    vnode_ops_t *ops;
    void *private_data;

    mount_t *mounted_here;
    struct vnode *parent;
} vnode_t;

#endif

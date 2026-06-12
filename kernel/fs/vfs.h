#ifndef VFS_H
#define VFS_H

#include "vnode.h"
#include "mounts.h"
#include "file.h"

#define MAX_FILESYSTEMS 16


typedef struct filesystem_type filesystem_t;

struct filesystem_type {
    const char *name;

    int (*mount)(
        filesystem_t *fs,
        struct vnode **root
    );
};


int vfs_register_fs(filesystem_t *fs);
filesystem_t *vfs_find_fs(char *name);

#endif

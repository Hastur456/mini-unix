#ifndef VFS_H
#define VFS_H

#include "vnode.h"
#include "mount.h"
#include "file.h"
#include "../proc/process.h"

#define ENOMEM 12
#define ENODEV 19
#define EINVAL 22
#define ENOENT 2
#define EEXIST 17
#define ENOSPC 28
#define ENOTDIR 20

#define MAX_FILESYSTEMS 16

typedef struct filesystem_type filesystem_t;

struct filesystem_type {
    const char *name;

    int (*mount)(
        void *device,
        struct vnode **root
    );
};


void vfs_init(void);
int vfs_register_fs(filesystem_t *fs);
filesystem_t *vfs_find_fs(const char *name);
int vfs_lookup(const char *path, vnode_t **result);
int vfs_open(process_t *current_proc, const char *path, int flags);
int vfs_close(process_t *current_proc, int fd);
#endif

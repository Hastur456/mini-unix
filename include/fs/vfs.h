#ifndef VFS_H
#define VFS_H

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#include <kernel/errno.h>

#include <fs/vnode.h>
#include <fs/mount.h>
#include <fs/file.h>
#include <proc/process.h>
#include <mm/heap.h>
#include <lib/string.h>

#define MAX_FILESYSTEMS 16

struct process;
typedef struct process process_t;

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
ssize_t vfs_read(process_t *proc, int fd, void *buf, size_t count);
ssize_t vfs_write(process_t *proc, int fd, const void *buf, size_t count);
vnode_t *vnode_alloc(void);
#endif

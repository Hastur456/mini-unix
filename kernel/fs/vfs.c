#include <stddef.h>
#include <stdint.h>

#include "vfs.h"
#include "../heap.h"
#include "../proc/process.h"
#include "file.h"

static filesystem_t *registered_fs[MAX_FILESYSTEMS];
static vnode_t root_vnode;

static int str_eq(const char *a, const char *b) {
    size_t i = 0;

    if (!a || !b) {
        return 0;
    }

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

static const char *next_component(const char *path, char *name) {
    size_t len = 0;

    while (*path == '/') {
        path++;
    }

    if (*path == '\0') {
        return NULL;
    }

    while (*path != '\0' && *path != '/') {
        if (len < VFS_MAX_NAME) {
            name[len++] = *path;
        }
        path++;
    }

    name[len] = '\0';
    return path;
}

static vnode_t *mounted_root(vnode_t *node) {
    if (node && node->mounted_here) {
        return node->mounted_here->root;
    }

    return node;
}

void vfs_init(void) {
    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        registered_fs[i] = NULL;
    }

    root_vnode.type = VNODE_DIR;
    root_vnode.name[0] = '/';
    root_vnode.name[1] = '\0';
    root_vnode.inode = 1;
    root_vnode.refcount = 1;
    root_vnode.ops = NULL;
    root_vnode.private_data = NULL;
    root_vnode.mounted_here = NULL;
    root_vnode.parent = &root_vnode;
}

int vfs_register_fs(filesystem_t *fs) {
    if (!fs || !fs->name || !fs->mount) {
        return -EINVAL;
    }

    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (registered_fs[i] && str_eq(registered_fs[i]->name, fs->name)) {
            return -EEXIST;
        }
    }

    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (!registered_fs[i]) {
            registered_fs[i] = fs;
            return 0;
        }
    }

    return -ENOSPC;
}

filesystem_t *vfs_find_fs(const char *name) {
    if (!name) {
        return NULL;
    }

    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (registered_fs[i] && str_eq(registered_fs[i]->name, name)) {
            return registered_fs[i];
        }
    }

    return NULL;
}

int vfs_lookup(const char *path, vnode_t **result) {
    vnode_t *current;
    const char *next;
    char name[VFS_MAX_NAME + 1];

    if (!path || !result || path[0] != '/') {
        return -EINVAL;
    }

    current = mounted_root(&root_vnode);
    next = path;

    while ((next = next_component(next, name)) != NULL) {
        vnode_t *child = NULL;

        current = mounted_root(current);
        if (!current->ops || !current->ops->lookup) {
            return -ENOTDIR;
        }

        if (current->ops->lookup(current, name, &child) < 0) {
            return -ENOENT;
        }

        current = child;
    }

    *result = mounted_root(current);
    return 0;
}

mount_t *alloc_mount(filesystem_t *fs,
                     vnode_t *mountpoint,
                     vnode_t *root,
                     void *device) {
    mount_t *mnt = kmalloc(sizeof(mount_t));

    if (!mnt) {
        return NULL;
    }

    mnt->fs = fs;
    mnt->root = root;
    mnt->mountpoint = mountpoint;
    mnt->private_data = NULL;
    mnt->device = device;

    return mnt;
}

int vfs_mount(const char *path, const char *fs_name, void *device) {
    filesystem_t *fs = vfs_find_fs(fs_name);
    vnode_t *mountpoint = NULL;
    vnode_t *root = NULL;
    mount_t *mnt;
    int err;

    if (!fs) {
        return -ENODEV;
    }

    err = vfs_lookup(path, &mountpoint);
    if (err < 0) {
        return err;
    }

    err = fs->mount(device, &root);
    if (err < 0) {
        return err;
    }

    if (!root) {
        return -EINVAL;
    }

    mnt = alloc_mount(fs, mountpoint, root, device);
    if (!mnt) {
        return -ENOMEM;
    }

    root->parent = mountpoint;
    mountpoint->mounted_here = mnt;

    return 0;
}

int vfs_open(process_t *current_proc, const char *path, int flags) {
    vnode_t *vnode;
    int err;

    err = vfs_lookup(path, &vnode);
    if (err < 0) return err;

    int fd = process_alloc_fd(current_proc);
    if (fd < 0) return fd;

    file_t f = file_create(vnode, flags);
    if (!f) return -ENOMEM;

    current_proc->files[fd] = f;

    return fd;
}

int vfs_close(process_t *current_proc, int fd) {
    file_t *f = process_get_file(current_proc, fd);
    if (!f) return -ENOENT;

    f->refcount--;
    if (f->refcount == 0) file_destroy(f);

    return 0;
}   

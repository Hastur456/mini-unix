#ifndef MOUNT_H
#define MOUNT_H


struct mount {
    struct vnode *mountpoint;
    struct vnode *root;

    struct filesystem_type *fs;

    void *private_data;
    void *device;
};

typedef struct mount mount_t;

mount_t *alloc_mount(filesystem_t *fs,
                     vnode_t *mountpoint,
                     vnode_t *root);
int vfs_mount(const char *path, const char *fs_name, void *device);

#endif

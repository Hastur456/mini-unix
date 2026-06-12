#ifndef MOUNT_H
#define MOUNT_H


struct mount {
    struct vnode *mountpoint;
    struct vnode *root;

    struct filesystem_type *fs_type;

    void *private_data;
};


int vfs_mount(
    const char *path,
    const char *fs_name
);

#endif

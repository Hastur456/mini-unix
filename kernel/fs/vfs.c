#include "vfs.h"
#include "vnode.h"
#include "file.h"
#include "mounts.h"


static filesystem_t *registered_fs[MAX_FILESYSTEMS];
static vnode_t root_vnode;
static vnode_ops_t root_stub_ops = {
    .open = NULL,
    .close = NULL,
    .read = NULL,
    .write = NULL,
    .lookup = root_lookup_stub,
    .create = NULL,
};


static int root_lookup_stub(
    vnode_t *dir,
    const char *name,
    vnode_t **result
) {
    (void)dir;
    (void)name;

    *result = NULL;

    return -1;
}


int str_eq(const char* a, const char* b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}


static const char *next_component(const char *path, char *name) {
    while (*path == '/') {
        path++;
    }

    if (*path == '\0') {
        return NULL;
    }

    while (*path && *path != '/') {
        *name = *path;
        name++;
        path++;
    }

    *name = '\0';

    return path;
}


void vfs_init(void) {
    root_vnode.type = VNODE_DIR;
    root_vnode.inode = 1;
    root_vnode.refcount = 1;
    root_vnode.ops = &root_stub_ops;
    root_vnode.private_data = NULL;
    root_vnode.mounted_here = NULL;
    root_vnode.parent = &root_vnode;
}


int vfs_register_fs(filesystem_t *fs) {
    if (!fs || !fs->name) {
        return -1;
    }

    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (registered_fs[i] && str_eq(registered_fs[i]->name, fs->name)) {
            return -2;
        }
    }

    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (registered_fs[i] == NULL) {
            registered_fs[i] = fs;
            return 0;
        }
    }

    return -3
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


vnode_type_t *vfs_lookup(const char *path, vnode_t **result) {
    vnode_t *current;
    char *name[VFS_MAX_NAME + 1]
    
    if (path[0] != '/') {
        return -1;
    }

    if (*path == '/') {
        current = root_vnode;
    }

    while ((p = next_component(path, name)) != NULL) {
        vnode_t *next = NULL;

        if (p->parent->ops.lookup(current, name, &next) < 0) {
            return -1;
        }

        current = next;
    }

    *result = current
    return 0;
}


// int vfs_mount(const char *path, const char *fs_name) {

// }

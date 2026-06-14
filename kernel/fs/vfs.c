#include "vfs.h"
#include "vnode.h"
#include "file.h"
#include "mounts.h"


filesystem_t *registered_fs[MAX_FILESYSTEMS];


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


static char *next_component(const char *path, const char *name) {
    while (*path == '/') {
        path++;
    }

    if (*path == '\0') {
        return NULL;
    }

    while (*path && *path != '/') {
        *name++ = *path++;
    }

    *name = '\0';

    return path;
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


vnode_type_t *vfs_lookup(const char *path, const char *name) {
    vnode_t *current;

    if (*path == '/') {
        *current.
    }
}


// int vfs_mount(const char *path, const char *fs_name) {

// }

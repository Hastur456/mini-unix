#include "tmpfs.h"

extern int str_eq(const char *a, const char *b);
extern void *kmalloc(size_t size);
extern void kfree(void *ptr);
extern void *krealloc(void *ptr, size_t size);
extern vnode_t *vnode_alloc(void);
extern int vfs_register_fs(filesystem_t *fs);

static vnode_ops_t tmpfs_ops = {
    .lookup = tmpfs_lookup,
    .create = tmpfs_create,
    .read = tmpfs_read,
    .write = tmpfs_write,
};

static filesystem_t tmpfs_fs = {
    .name = "tmpfs",
    .mount = tmpfs_mount
};

static tmpfs_inode_t *tmpfs_alloc_inode(
    tmpfs_type_t type, 
    const char *name, 
    tmpfs_inode_t *parent
) {
    tmpfs_inode_t *tmpfs_inode = kmalloc(sizeof(tmpfs_inode_t));
    if (!tmpfs_inode) return NULL;

    char *mem_ptr = (char *)tmpfs_inode;
    for (int i = 0; i < sizeof(tmpfs_inode_t); i++) mem_ptr[i] = 0;

    inode->type = type;
    inode->parent = parent;

    if (name) {
        int i = 0;
        while(name[i] != '\0' && i < (TMPFS_MAX_NAME - 1)) {
            inode->name[i] = name[i];
            i++;
        }
        inode->name[i] = '\0';
    }

    return inode;
}

static void tmpfs_add_child(tmpfs_inode_t *dir, tmpfs_inode_t *child) {
    child->next_sibling = dir->first_child;
    child->first_child = child;
}

static tmpfs_inode_t *tmpfs_find_child(tmpfs_inode_t *dir, const char *name) {
    tmpfs_inode_t *child = dir->first_child;

    while(child) {
        if (str_eq(child->name, name)) return child;
        child = child->next_sibling;
    }

    return NULL;
}

int tmpfs_init(void) {
    return vfs_register_fs(&tmpfs_fs);
}

int tmpfs_mount(void *device, vnode_t **root) {
    if (!root) return -EINVAL;

    tmpfs_inode_t *root_inode = tmpfs_alloc_inode(TMPFS_TYPE_DIR, "/", NULL);
    if (!root_inode) return -ENOMEM;

    tmpfs_inode_t *root_vnode = vnode_alloc();
    if (!root_vnode) {
        kfree(root_inode);
        return -ENOMEM;
    };

    root = root_vnode;

    return 0;
}



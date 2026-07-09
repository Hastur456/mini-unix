#include "fs/tmpfs.h"
#include "fs/vfs.h"
#include "string.h"

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
    for (size_t i = 0; i < sizeof(tmpfs_inode_t); i++) mem_ptr[i] = 0;

    tmpfs_inode->type = type;
    tmpfs_inode->parent = parent;

    if (name) {
        int i = 0;
        while(name[i] != '\0' && i < (TMPFS_MAX_NAME - 1)) {
            tmpfs_inode->name[i] = name[i];
            i++;
        }
        tmpfs_inode->name[i] = '\0';
    }

    return tmpfs_inode;
}

static vnode_t *tmpfs_create_vnode(tmpfs_inode_t *inode) {
    vnode_t *vn = vnode_alloc();
    if (!vn) return NULL;
    
    vn->ops = &tmpfs_ops;
    vn->private_data = inode;
    vn->type = (inode->type == TMPFS_TYPE_DIR) ? VNODE_DIR : VNODE_FILE;
    for (int i = 0; inode->name[i] != '\0' && i < VFS_MAX_NAME; i++) {
        vn->name[i] = inode->name[i];
        vn->name[i + 1] = '\0';
    }
    inode->vnode = vn;
    
    return vn;
}

static void tmpfs_add_child(tmpfs_inode_t *dir, tmpfs_inode_t *child) {
    child->next_sibling = dir->first_child;
    dir->first_child = child;
}

static tmpfs_inode_t *tmpfs_find_child(tmpfs_inode_t *dir, const char *name) {
    tmpfs_inode_t *child = dir->first_child;

    while(child) {
        if (strcmp(child->name, name) == 0)
            return child;
        child = child->next_sibling;
    }

    return NULL;
}

int tmpfs_init(void) {
    return vfs_register_fs(&tmpfs_fs);
}

int tmpfs_mount(void *device, vnode_t **root) {
    (void)device;

    if (!root) return -EINVAL;

    tmpfs_inode_t *root_inode = tmpfs_alloc_inode(TMPFS_TYPE_DIR, "/", NULL);
    if (!root_inode) return -ENOMEM;

    vnode_t *root_vnode = tmpfs_create_vnode(root_inode);
    if (!root_vnode) {
        kfree(root_inode);
        return -ENOMEM;
    }

    root_vnode->parent = root_vnode;
    *root = root_vnode;

    return 0;
}

int tmpfs_lookup(vnode_t *dir, const char *name, vnode_t **result) {
    if (!dir || !name || !result) return -EINVAL;

    tmpfs_inode_t *dir_inode = (tmpfs_inode_t *)dir->private_data;
    if (!dir_inode) return -EINVAL;

    if (strcmp(name, ".") == 0) {
        *result = dir;
        return 0;
    }

    if (strcmp(name, "..") == 0) {
        *result = dir_inode->parent ? dir_inode->parent->vnode : dir;
        return 0;
    }
    tmpfs_inode_t *child = tmpfs_find_child(dir_inode, name);
    if (!child) return -ENOENT;

    *result = child->vnode;

    return 0;
}

int tmpfs_create(vnode_t *dir, const char *name, vnode_t **result) {
    if (!dir || !name || !result) return -EINVAL;

    tmpfs_inode_t *inode = (tmpfs_inode_t *)dir->private_data;
    if (!inode) return -EINVAL;
    if (dir->type != VNODE_DIR) return -EINVAL;

    if (tmpfs_find_child(inode, name)) return -EEXIST;

    tmpfs_inode_t *new_inode = tmpfs_alloc_inode(TMPFS_TYPE_FILE, name, inode);
    if (!new_inode) return -ENOMEM;

    vnode_t *new_vnode = tmpfs_create_vnode(new_inode);
    if (!new_vnode) {
        kfree(new_inode);
        return -ENOMEM;
    }

    tmpfs_add_child(inode, new_inode);

    *result = new_vnode;
    return 0;
}

ssize_t tmpfs_read(vnode_t *vn, size_t offset, void *buf, size_t count) {
    if (!vn || !buf) return -EINVAL;
    if (count == 0) return 0;

    tmpfs_inode_t *inode = (tmpfs_inode_t *)vn->private_data;
    if (inode->type != TMPFS_TYPE_FILE) return -EINVAL;

    if (offset >= inode->size) {
        return 0;
    }

    size_t available = inode->size - offset;
    size_t to_read = (count > available) ? available : count;

    char *dst = (char *)buf;
    char *src = (char *)inode->data + offset;
    
    for (size_t i = 0; i < to_read; i++) {
        dst[i] = src[i];
    }

    return to_read;
}

ssize_t tmpfs_write(vnode_t *vn, size_t offset, const void *buf, size_t count) {
    if (!vn || !buf) return -EINVAL;
    if (count == 0) return 0;

    tmpfs_inode_t *inode = (tmpfs_inode_t *)vn->private_data;
    if (inode->type != TMPFS_TYPE_FILE) return -EINVAL;

    size_t required_size = offset + count;

    if (required_size > inode->size) {
        void *new_data = krealloc(inode->data, required_size);
        if (!new_data) {
            return -ENOSPC; 
        }
        inode->data = new_data;
        
        if (offset > inode->size) {
            char *hole_ptr = (char *)inode->data + inode->size;
            size_t hole_size = offset - inode->size;
            
            for (size_t i = 0; i < hole_size; i++) {
                hole_ptr[i] = 0;
            }
        }
        
        inode->size = required_size;
    }

    char *dst = (char *)inode->data + offset;
    const char *src = (const char *)buf;
    
    for (size_t i = 0; i < count; i++) {
        dst[i] = src[i];
    }

    return count;
}

#include <fs/file.h>

struct file *file_create(struct vnode *node, uint32_t flags) {
    struct file *file;

    if (!node) {
        return 0;
    }

    file = kmalloc(sizeof(struct file));
    if (!file) {
        return 0;
    }

    file->node = node;
    file->offset = 0;
    file->flags = flags;
    file->refcount = 1;

    node->refcount++;

    return file;
}


void file_destroy(struct file *file) {
    if (!file) {
        return;
    }

    file->refcount--;
    if (file->refcount > 0) {
        return;
    }

    if (file->node && file->node->refcount > 0) {
        file->node->refcount--;
    }

    kfree(file);
}

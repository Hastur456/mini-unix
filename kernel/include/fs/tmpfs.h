#ifndef TEMPFS_H
#define TEMPFS_H

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

#include "./vnode.h" 
#include "./mount.h"

#define ENOMEM 12
#define ENODEV 19
#define EINVAL 22
#define ENOENT 2
#define EEXIST 17
#define ENOSPC 28
#define ENOTDIR 20

#define TMPFS_MAX_NAME 64

typedef enum {
    TMPFS_TYPE_FILE,
    TMPFS_TYPE_DIR
} tmpfs_type_t;

typedef struct tmpfs_inode {
    tmpfs_type_t type;
    char name[TMPFS_MAX_NAME];
    size_t size;
    void *data;
    
    struct tmpfs_inode *parent;
    struct tmpfs_inode *first_child;
    struct tmpfs_inode *next_sibling;
    
    vnode_t *vnode;
} tmpfs_inode_t;

int tmpfs_init(void);
int tmpfs_mount(void *device, vnode_t **root);
int tmpfs_lookup(vnode_t *dir, const char *name, vnode_t **result);
int tmpfs_create(vnode_t *dir, const char *name, vnode_t **result);
ssize_t tmpfs_read(vnode_t *vn, size_t offset, void *buf, size_t count);
ssize_t tmpfs_write(vnode_t *vn, size_t offset, const void *buf, size_t count);

#endif

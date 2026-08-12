#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <lib/assert.h>
#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <lib/string.h>
#include <kernel/errno.h>


static uint32_t next_inode = 1;

void *kmalloc(size_t size) {
    return calloc(1, size);
}

void kfree(void *ptr) {
    free(ptr);
}

static int memfs_lookup(vnode_t *dir, const char *name, vnode_t **result);

static vnode_ops_t memfs_dir_ops = {
    .open = NULL,
    .close = NULL,
    .read = NULL,
    .write = NULL,
    .lookup = memfs_lookup,
    .create = NULL,
};

typedef struct test_node {
    vnode_t vnode;
    struct test_node *children[8];
    size_t child_count;
} test_node_t;

static test_node_t *make_node(const char *name, vnode_type_t type) {
    test_node_t *node = calloc(1, sizeof(test_node_t));
    ASSERT(node);

    node->vnode.type = type;
    strncpy(node->vnode.name, name, VFS_MAX_NAME);
    node->vnode.inode = next_inode++;
    node->vnode.refcount = 1;
    node->vnode.ops = type == VNODE_DIR ? &memfs_dir_ops : NULL;
    node->vnode.private_data = node;
    node->vnode.parent = &node->vnode;

    return node;
}

static void add_child(test_node_t *parent, test_node_t *child) {
    ASSERT(parent->child_count < 8);
    parent->children[parent->child_count++] = child;
    child->vnode.parent = &parent->vnode;
}

static int memfs_lookup(vnode_t *dir, const char *name, vnode_t **result) {
    test_node_t *node = dir->private_data;

    for (size_t i = 0; i < node->child_count; i++) {
        if (strcmp(node->children[i]->vnode.name, name) == 0) {
            *result = &node->children[i]->vnode;
            return 0;
        }
    }

    return -ENOENT;
}

typedef struct test_device {
    test_node_t *root;
} test_device_t;

static int memfs_mount(void *device, vnode_t **root) {
    test_device_t *dev = device;

    *root = &dev->root->vnode;
    return 0;
}

static filesystem_t memfs = {
    .name = "memfs",
    .mount = memfs_mount,
};

static void test_register_and_find_fs(void) {
    vfs_init();

    ASSERT(vfs_register_fs(NULL) == -EINVAL);
    ASSERT(vfs_find_fs("memfs") == NULL);
    ASSERT(vfs_register_fs(&memfs) == 0);
    ASSERT(vfs_find_fs("memfs") == &memfs);
    ASSERT(vfs_register_fs(&memfs) == -EEXIST);
    ASSERT(vfs_find_fs("missing") == NULL);
}

static void test_mount_root_and_lookup_nested_path(void) {
    test_node_t *root = make_node("/", VNODE_DIR);
    test_node_t *etc = make_node("etc", VNODE_DIR);
    test_node_t *conf = make_node("conf", VNODE_FILE);
    test_device_t dev = { .root = root };
    vnode_t *found = NULL;

    add_child(root, etc);
    add_child(etc, conf);

    vfs_init();
    ASSERT(vfs_register_fs(&memfs) == 0);
    ASSERT(vfs_mount("/", "memfs", &dev) == 0);

    ASSERT(vfs_lookup("/", &found) == 0);
    ASSERT(found == &root->vnode);

    ASSERT(vfs_lookup("/etc/conf", &found) == 0);
    ASSERT(found == &conf->vnode);

    ASSERT(vfs_lookup("//etc///conf", &found) == 0);
    ASSERT(found == &conf->vnode);
}

static void test_lookup_errors(void) {
    test_node_t *root = make_node("/", VNODE_DIR);
    test_node_t *file = make_node("file", VNODE_FILE);
    test_device_t dev = { .root = root };
    vnode_t *found = NULL;

    add_child(root, file);

    vfs_init();
    ASSERT(vfs_register_fs(&memfs) == 0);
    ASSERT(vfs_mount("/", "memfs", &dev) == 0);

    ASSERT(vfs_lookup("relative", &found) == -EINVAL);
    ASSERT(vfs_lookup("/missing", &found) == -ENOENT);
    ASSERT(vfs_lookup("/file/child", &found) == -ENOTDIR);
}

static void test_mount_errors(void) {
    test_node_t *root = make_node("/", VNODE_DIR);
    test_device_t dev = { .root = root };

    vfs_init();

    ASSERT(vfs_mount("/", "memfs", &dev) == -ENODEV);
    ASSERT(vfs_register_fs(&memfs) == 0);
    ASSERT(vfs_mount("/missing", "memfs", &dev) < 0);
}

int main(void) {
    test_register_and_find_fs();
    test_mount_root_and_lookup_nested_path();
    test_lookup_errors();
    test_mount_errors();

    puts("test_vfs: ok");
    return 0;
}

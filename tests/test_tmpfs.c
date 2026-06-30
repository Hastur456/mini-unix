#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs/tmpfs.h"
#include "fs/vfs.h"

void *kmalloc(size_t size) {
    return calloc(1, size);
}

void *krealloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

void kfree(void *ptr) {
    free(ptr);
}

static vnode_t *mount_tmpfs_root(void) {
    vnode_t *root = NULL;

    vfs_init();
    assert(tmpfs_init() == 0);
    assert(vfs_mount("/", "tmpfs", NULL) == 0);
    assert(vfs_lookup("/", &root) == 0);

    return root;
}

static void test_mount_and_special_lookup(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *found = NULL;

    assert(root != NULL);
    assert(root->type == VNODE_DIR);
    assert(strcmp(root->name, "/") == 0);
    assert(root->ops != NULL);
    assert(root->ops->lookup != NULL);
    assert(root->ops->create != NULL);

    assert(vfs_lookup("/.", &found) == 0);
    assert(found == root);

    assert(vfs_lookup("/..", &found) == 0);
    assert(found == root);
}

static void test_create_lookup_and_duplicates(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *file = NULL;
    vnode_t *found = NULL;

    assert(root->ops->create(root, "hello", &file) == 0);
    assert(file != NULL);
    assert(file->type == VNODE_FILE);
    assert(strcmp(file->name, "hello") == 0);

    assert(vfs_lookup("/hello", &found) == 0);
    assert(found == file);

    assert(root->ops->create(root, "hello", &found) == -EEXIST);
    assert(file->ops->create(file, "child", &found) == -EINVAL);
    assert(vfs_lookup("/missing", &found) == -ENOENT);
}

static void test_read_write_file(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *file = NULL;
    char buf[8];

    assert(root->ops->create(root, "note", &file) == 0);

    memset(buf, 0x7f, sizeof(buf));
    assert(tmpfs_read(file, 0, buf, sizeof(buf)) == 0);

    assert(tmpfs_write(file, 0, "abc", 3) == 3);
    memset(buf, 0, sizeof(buf));
    assert(tmpfs_read(file, 0, buf, sizeof(buf)) == 3);
    assert(memcmp(buf, "abc", 3) == 0);

    assert(tmpfs_write(file, 5, "xy", 2) == 2);
    memset(buf, 0x7f, sizeof(buf));
    assert(tmpfs_read(file, 0, buf, 7) == 7);
    assert(memcmp(buf, "abc\0\0xy", 7) == 0);
    assert(tmpfs_read(file, 7, buf, sizeof(buf)) == 0);
}

static void test_operation_errors(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *file = NULL;
    char buf[4];

    assert(tmpfs_mount(NULL, NULL) == -EINVAL);
    assert(root->ops->create(root, "bad", NULL) == -EINVAL);
    assert(tmpfs_lookup(NULL, "bad", &file) == -EINVAL);
    assert(tmpfs_lookup(root, NULL, &file) == -EINVAL);
    assert(tmpfs_read(root, 0, buf, sizeof(buf)) == -EINVAL);
    assert(tmpfs_write(root, 0, "x", 1) == -EINVAL);
}

int main(void) {
    test_mount_and_special_lookup();
    test_create_lookup_and_duplicates();
    test_read_write_file();
    test_operation_errors();

    puts("test_tmpfs: ok");
    return 0;
}

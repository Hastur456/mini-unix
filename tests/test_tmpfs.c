#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <lib/assert.h>
#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <lib/string.h>
#include <kernel/errno.h>


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
    ASSERT(tmpfs_init() == 0);
    ASSERT(vfs_mount("/", "tmpfs", NULL) == 0);
    ASSERT(vfs_lookup("/", &root) == 0);

    return root;
}

static void test_mount_and_special_lookup(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *found = NULL;

    ASSERT(root != NULL);
    ASSERT(root->type == VNODE_DIR);
    ASSERT(strcmp(root->name, "/") == 0);
    ASSERT(root->ops != NULL);
    ASSERT(root->ops->lookup != NULL);
    ASSERT(root->ops->create != NULL);

    ASSERT(vfs_lookup("/.", &found) == 0);
    ASSERT(found == root);

    ASSERT(vfs_lookup("/..", &found) == 0);
    ASSERT(found == root);
}

static void test_create_lookup_and_duplicates(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *file = NULL;
    vnode_t *found = NULL;

    ASSERT(root->ops->create(root, "hello", &file) == 0);
    ASSERT(file != NULL);
    ASSERT(file->type == VNODE_FILE);
    ASSERT(strcmp(file->name, "hello") == 0);

    ASSERT(vfs_lookup("/hello", &found) == 0);
    ASSERT(found == file);

    ASSERT(root->ops->create(root, "hello", &found) == -EEXIST);
    ASSERT(file->ops->create(file, "child", &found) == -EINVAL);
    ASSERT(vfs_lookup("/missing", &found) == -ENOENT);
}

static void test_read_write_file(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *file = NULL;
    char buf[8];

    ASSERT(root->ops->create(root, "note", &file) == 0);

    memset(buf, 0x7f, sizeof(buf));
    ASSERT(tmpfs_read(file, 0, buf, sizeof(buf)) == 0);

    ASSERT(tmpfs_write(file, 0, "abc", 3) == 3);
    memset(buf, 0, sizeof(buf));
    ASSERT(tmpfs_read(file, 0, buf, sizeof(buf)) == 3);
    ASSERT(memcmp(buf, "abc", 3) == 0);

    ASSERT(tmpfs_write(file, 5, "xy", 2) == 2);
    memset(buf, 0x7f, sizeof(buf));
    ASSERT(tmpfs_read(file, 0, buf, 7) == 7);
    ASSERT(memcmp(buf, "abc\0\0xy", 7) == 0);
    ASSERT(tmpfs_read(file, 7, buf, sizeof(buf)) == 0);
}

static void test_operation_errors(void) {
    vnode_t *root = mount_tmpfs_root();
    vnode_t *file = NULL;
    char buf[4];

    ASSERT(tmpfs_mount(NULL, NULL) == -EINVAL);
    ASSERT(root->ops->create(root, "bad", NULL) == -EINVAL);
    ASSERT(tmpfs_lookup(NULL, "bad", &file) == -EINVAL);
    ASSERT(tmpfs_lookup(root, NULL, &file) == -EINVAL);
    ASSERT(tmpfs_read(root, 0, buf, sizeof(buf)) == -EINVAL);
    ASSERT(tmpfs_write(root, 0, "x", 1) == -EINVAL);
}

int main(void) {
    test_mount_and_special_lookup();
    test_create_lookup_and_duplicates();
    test_read_write_file();
    test_operation_errors();

    puts("test_tmpfs: ok");
    return 0;
}

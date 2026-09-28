#include <dev/tty_device.h>
#include <proc/process.h>
#include <fs/vnode.h>


static int tty_open(struct vnode *vnode, int flags) {
    (void)vnode;
    (void)flags;
    return 0;
}

static int tty_close(struct vnode *vnode) {
    (void)vnode;
    return 0;
}

static ssize_t tty_read(
    struct vnode *vnode,
    size_t offset,
    void *buf,
    size_t count
)
{
    (void)vnode;
    (void)offset;

    if (!buf || count == 0)
        return 0;

    int c;

    while ((c = keyboard_getchar()) == -1){
        __asm__ volatile ("sti; hlt");
    }

    ((char *)buf)[0] = (char)c;
    return 1;
}

static ssize_t tty_dev_write(
    struct vnode *vnode,
    size_t offset,
    const void *buf,
    size_t count
) {    
    (void)vnode;
    (void)offset;
    const char *cbuf = (const char *)buf;
    for (size_t i = 0; i < count; i++) {
        tty_putchar(cbuf[i]);
    }
    return (ssize_t)count;
}

struct vnode_ops tty_dev_ops = {
    .open  = tty_open,
    .close = tty_close,
    .read  = tty_read,
    .write = tty_dev_write,
    .unlink = NULL,
    .create = NULL,
};

int tty_device_init(void) {
    vnode_t *tty_vnode;
    int err;

    err = vfs_create(NULL, "/tty", &tty_vnode);
    if (err < 0)
        return err;

    tty_vnode->ops = &tty_dev_ops;
    tty_vnode->type = VNODE_CHARDEV;

    return 0;
}

#include <dev/tty_device.h>


static int tty_open(struct vnode *vnode, int flags) {
    (void)vnode;
    (void)flags;
    return 0;
}

static int tty_close(struct vnode *vnode) {
    (void)vnode;
    return 0;
}

static ssize_t tty_read(struct vnode *vnode, void *buf, size_t count, off_t offset) {
    (void)vnode;
    (void)offset;

    char *cbuf = (char *)buf;
    size_t i;
    for (i = 0; i < count; i++) {
        int c = keyboard_getchar();
        if (c == -1) {
            if (i == 0)
                return -EIO;
            break;
        }
        cbuf[i] = (char)c;
        
        break;
    }
    return (ssize_t)i;
}

static ssize_t tty_write(struct vnode *vnode, const void *buf, size_t count, off_t offset) {
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
    .write = tty_write,
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
    tty_vnode->type = VNODE_CHAR;

    return 0;
}

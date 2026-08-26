#ifndef TTY_DEVICE_H
#define TTY_DEVICE_H

#include <dev/tty_device.h>
#include <dev/tty.h>
#include <dev/keyboard.h>
#include <fs/vnode.h>
#include <fs/vfs.h>
#include <errno.h>
#include <string.h>

extern struct vnode_ops tty_dev_ops;

int tty_device_init(void);

#endif

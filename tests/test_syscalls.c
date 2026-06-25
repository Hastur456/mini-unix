#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define EFAULT 14

#include "syscalls.h"

static interrupt_handler_t registered_handler;
static uint8_t registered_vector;

void interrupt_register_handler(uint8_t vector, interrupt_handler_t handler)
{
    registered_vector = vector;
    registered_handler = handler;
}

static int vfs_last_fd;
static const void *vfs_last_write_buf;
static void *vfs_last_read_buf;
static size_t vfs_last_len;
static const char *vfs_last_path;
static int vfs_last_flags;

static ssize_t vfs_write_result;
static ssize_t vfs_read_result;
static int vfs_open_result;
static int vfs_close_result;

ssize_t vfs_write(process_t *p, int fd, const void *buf, size_t count)
{
    (void)p;
    vfs_last_fd = fd;
    vfs_last_write_buf = buf;
    vfs_last_len = count;
    return vfs_write_result;
}

ssize_t vfs_read(process_t *p, int fd, void *buf, size_t count)
{
    (void)p;
    vfs_last_fd = fd;
    vfs_last_read_buf = buf;
    vfs_last_len = count;
    return vfs_read_result;
}

int vfs_open(process_t *p, const char *path, int flags)
{
    (void)p;
    vfs_last_path = path;
    vfs_last_flags = flags;
    return vfs_open_result;
}

int vfs_close(process_t *p, int fd)
{
    (void)p;
    vfs_last_fd = fd;
    return vfs_close_result;
}


#include "syscalls.c"

#define ASSERT_EQ_INT(expected, actual) \
    do { \
        if ((int)(expected) != (int)(actual)) { \
            return __LINE__; \
        } \
    } while (0)

#define ASSERT_EQ_U32(expected, actual) \
    do { \
        if ((uint32_t)(expected) != (uint32_t)(actual)) { \
            return __LINE__; \
        } \
    } while (0)

#define ASSERT_PTR_EQ(expected, actual) \
    do { \
        if ((const void *)(expected) != (const void *)(actual)) { \
            return __LINE__; \
        } \
    } while (0)

#define ASSERT_EQ_SIZE(expected, actual) \
    do { \
        if ((size_t)(expected) != (size_t)(actual)) { \
            return __LINE__; \
        } \
    } while (0)

static void reset_vfs_state(void)
{
    vfs_last_fd = 0;
    vfs_last_write_buf = NULL;
    vfs_last_read_buf = NULL;
    vfs_last_len = 0;
    vfs_last_path = NULL;
    vfs_last_flags = 0;
    vfs_write_result = 0;
    vfs_read_result = 0;
    vfs_open_result = 0;
    vfs_close_result = 0;
}

static int test_syscalls_init_registers_dispatcher(void)
{
    registered_vector = 0;
    registered_handler = NULL;

    syscalls_init();

    ASSERT_EQ_U32(SYSCALL_VECTOR, registered_vector);
    ASSERT_PTR_EQ(syscall_dispatch, registered_handler);
    return 0;
}

static int test_dispatch_rejects_unknown_syscall(void)
{
    struct interrupt_frame frame;

    memset(&frame, 0, sizeof(frame));
    frame.eax = 99;

    syscall_dispatch(&frame);

    ASSERT_EQ_U32((uint32_t)-1, frame.eax);
    return 0;
}

static int test_write_success(void)
{
    sys_args_t args;
    int retval = 0;
    const char buf[] = "abc";
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));
    vfs_write_result = 3;

    args.write.fd = 1;
    args.write.buf = buf;
    args.write.len = 3;

    error = sys_write(NULL, &args, &retval);

    ASSERT_EQ_INT(0, error);
    ASSERT_EQ_INT(3, retval);
    ASSERT_EQ_INT(1, vfs_last_fd);
    ASSERT_PTR_EQ(buf, vfs_last_write_buf);
    ASSERT_EQ_SIZE(3, vfs_last_len);
    return 0;
}

static int test_write_rejects_null_buffer(void)
{
    sys_args_t args;
    int retval = 0;
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));

    args.write.fd = 1;
    args.write.buf = NULL;
    args.write.len = 3;

    error = sys_write(NULL, &args, &retval);

    ASSERT_EQ_INT(-EFAULT, error);
    ASSERT_EQ_INT(-1, retval);
    ASSERT_EQ_SIZE(0, vfs_last_len);
    return 0;
}

static int test_read_success(void)
{
    sys_args_t args;
    int retval = 0;
    char buf[8];
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));
    vfs_read_result = 5;

    args.read.fd = 4;
    args.read.buf = buf;
    args.read.len = sizeof(buf);

    error = sys_read(NULL, &args, &retval);

    ASSERT_EQ_INT(0, error);
    ASSERT_EQ_INT(5, retval);
    ASSERT_EQ_INT(4, vfs_last_fd);
    ASSERT_PTR_EQ(buf, vfs_last_read_buf);
    ASSERT_EQ_SIZE(sizeof(buf), vfs_last_len);
    return 0;
}

static int test_read_rejects_null_buffer(void)
{
    sys_args_t args;
    int retval = 0;
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));

    args.read.fd = 4;
    args.read.buf = NULL;
    args.read.len = 8;

    error = sys_read(NULL, &args, &retval);

    ASSERT_EQ_INT(-EFAULT, error);
    ASSERT_EQ_INT(-1, retval);
    ASSERT_EQ_SIZE(0, vfs_last_len);
    return 0;
}

static int test_open_success(void)
{
    sys_args_t args;
    int retval = 0;
    const char path[] = "/dev/tty";
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));
    vfs_open_result = 7;

    args.open.path = path;
    args.open.flags = 2;

    error = sys_open(NULL, &args, &retval);

    ASSERT_EQ_INT(0, error);
    ASSERT_EQ_INT(7, retval);
    ASSERT_PTR_EQ(path, vfs_last_path);
    ASSERT_EQ_INT(2, vfs_last_flags);
    return 0;
}

static int test_open_rejects_null_path(void)
{
    sys_args_t args;
    int retval = 0;
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));

    args.open.path = NULL;
    args.open.flags = 2;

    error = sys_open(NULL, &args, &retval);

    ASSERT_EQ_INT(-EFAULT, error);
    ASSERT_EQ_INT(-1, retval);
    ASSERT_PTR_EQ(NULL, vfs_last_path);
    return 0;
}

static int test_close_success(void)
{
    struct interrupt_frame frame;

    reset_vfs_state();
    memset(&frame, 0, sizeof(frame));

    frame.eax = SYS_CLOSE;
    frame.ebx = 9;

    syscall_dispatch(&frame);

    ASSERT_EQ_U32(0, frame.eax);
    ASSERT_EQ_INT(9, vfs_last_fd);
    return 0;
}

static int test_vfs_errors_are_returned(void)
{
    sys_args_t args;
    int retval = 0;
    char buf[4];
    int error;

    reset_vfs_state();
    memset(&args, 0, sizeof(args));
    vfs_read_result = -5;

    args.read.fd = 3;
    args.read.buf = buf;
    args.read.len = sizeof(buf);

    error = sys_read(NULL, &args, &retval);

    ASSERT_EQ_INT(-5, error);
    ASSERT_EQ_INT(-1, retval);
    return 0;
}

int main(void)
{
    int result;

    result = test_syscalls_init_registers_dispatcher();
    if (result != 0) {
        return result;
    }

    result = test_dispatch_rejects_unknown_syscall();
    if (result != 0) {
        return result;
    }

    result = test_write_success();
    if (result != 0) {
        return result;
    }

    result = test_write_rejects_null_buffer();
    if (result != 0) {
        return result;
    }

    result = test_read_success();
    if (result != 0) {
        return result;
    }

    result = test_read_rejects_null_buffer();
    if (result != 0) {
        return result;
    }

    result = test_open_success();
    if (result != 0) {
        return result;
    }

    result = test_open_rejects_null_path();
    if (result != 0) {
        return result;
    }

    result = test_close_success();
    if (result != 0) {
        return result;
    }

    result = test_vfs_errors_are_returned();
    if (result != 0) {
        return result;
    }

    puts("test_syscalls: ok");
    return 0;
}

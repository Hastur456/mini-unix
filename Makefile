CC := gcc
ASM := nasm
LD := ld
OBJCOPY := objcopy

QEMU := qemu-system-i386
QEMU_DISPLAY := -display gtk
QEMU_FLAGS :=

BUILD_DIR := build
KERNEL_DIR := kernel
USER_DIR := user
USER_LIBC_DIR := user/libc

KERNEL_BIN := $(BUILD_DIR)/kernel.bin

# ----------------------------------------------------------------------
# User program
# ----------------------------------------------------------------------

USER_BUILD_DIR := $(BUILD_DIR)/user
USER_ELF := $(USER_BUILD_DIR)/shell.elf
USER_BIN := $(USER_BUILD_DIR)/shell.bin
USER_IMAGE_OBJ := $(BUILD_DIR)/user/shell_image.o

USER_LIBC_SRCS := $(shell find $(USER_LIBC_DIR) -name '*.c')
USER_LIBC_OBJS := \
	$(patsubst \
		$(USER_LIBC_DIR)/%.c, \
		$(BUILD_DIR)/user/libc/%.o, \
		$(USER_LIBC_SRCS))

USER_SRCS := \
	$(USER_DIR)/shell.c \
	$(USER_DIR)/commands.c

USER_OBJS := \
	$(BUILD_DIR)/user/crt0.o \
	$(BUILD_DIR)/user/shell.o \
	$(BUILD_DIR)/user/commands.o \
	$(USER_LIBC_OBJS) 

# ----------------------------------------------------------------------
# Kernel sources
# ----------------------------------------------------------------------

C_SRCS := $(shell find $(KERNEL_DIR) -name '*.c')
ASM_SRCS := $(shell find $(KERNEL_DIR) -name '*.asm')

BOOT_SRC := $(KERNEL_DIR)/arch/x86/boot.asm
BOOT_OBJ := $(BUILD_DIR)/arch/x86/boot.o

ASM_SRCS := $(filter-out $(BOOT_SRC),$(ASM_SRCS))

C_OBJS := \
	$(patsubst \
		$(KERNEL_DIR)/%.c, \
		$(BUILD_DIR)/%.o, \
		$(C_SRCS))

ASM_OBJS := \
	$(patsubst \
		$(KERNEL_DIR)/%.asm, \
		$(BUILD_DIR)/%_asm.o, \
		$(ASM_SRCS))

# Embedded user image is linked into the kernel.
OBJS := \
	$(BOOT_OBJ) \
	$(C_OBJS) \
	$(ASM_OBJS) \
	$(USER_IMAGE_OBJ) \
	$(BUILD_DIR)/user/init/user_init.o 

DEPS := $(C_OBJS:.o=.d)

# ----------------------------------------------------------------------
# Compiler / linker flags
# ----------------------------------------------------------------------

CFLAGS := \
	-m32 \
	-std=gnu99 \
	-ffreestanding \
	-fno-pie \
	-fno-stack-protector \
	-Wall \
	-Wextra \
	-MMD \
	-MP \
	-Iinclude \
	-Dkmain=kernel_main

USER_CFLAGS := \
	-m32 \
	-std=gnu99 \
	-ffreestanding \
	-fno-pie \
	-fno-stack-protector \
	-nostdlib \
	-nodefaultlibs \
	-nostartfiles \
	-Wall \
	-Wextra \
	-Iinclude \
	-Iuser

TEST_CFLAGS := \
	-std=c99 \
	-Wall \
	-Wextra \
	-Iinclude

ASFLAGS := -f elf32

LDFLAGS := \
	-m elf_i386 \
	-T link.ld \
	-nostdlib

USER_LDFLAGS := \
	-m elf_i386 \
	-T user/link.ld \
	-nostdlib

# ----------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------

TEST_BINS := \
	$(BUILD_DIR)/test_vfs \
	$(BUILD_DIR)/test_tmpfs \
	$(BUILD_DIR)/test_syscalls \
	$(BUILD_DIR)/test_list \
	$(BUILD_DIR)/test_bitmap

# ----------------------------------------------------------------------
# Phony targets
# ----------------------------------------------------------------------

.PHONY: all user run test clean dirs

all: $(KERNEL_BIN)

user: $(USER_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

# ----------------------------------------------------------------------
# Boot
# ----------------------------------------------------------------------

$(BOOT_OBJ): $(BOOT_SRC)
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

# ----------------------------------------------------------------------
# Kernel compilation
# ----------------------------------------------------------------------

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_asm.o: $(KERNEL_DIR)/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

# ----------------------------------------------------------------------
# User compilation
# ----------------------------------------------------------------------

$(BUILD_DIR)/user/%.o: $(USER_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/user/libc/%.o: $(USER_LIBC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/user/init/user_init.o: user/init/user_init.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------
# User linker
# ----------------------------------------------------------------------

$(USER_ELF): $(USER_OBJS) user/link.ld
	mkdir -p $(dir $@)
	$(LD) $(USER_LDFLAGS) -o $@ $(USER_OBJS)

# ----------------------------------------------------------------------
# ELF -> flat binary
# ----------------------------------------------------------------------

$(USER_BIN): $(USER_ELF)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

# ----------------------------------------------------------------------
# Embed user binary into kernel
# ----------------------------------------------------------------------

$(USER_IMAGE_OBJ): $(USER_BIN)
	mkdir -p $(dir $@)
	$(OBJCOPY) \
		-I binary \
		-O elf32-i386 \
		-B i386 \
		$< $@

# ----------------------------------------------------------------------
# Kernel linker
# ----------------------------------------------------------------------

$(KERNEL_BIN): $(OBJS) link.ld
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# ----------------------------------------------------------------------
# Run
# ----------------------------------------------------------------------

run: all
	$(QEMU) $(QEMU_DISPLAY) $(QEMU_FLAGS) -kernel $(KERNEL_BIN)

# ----------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------

$(BUILD_DIR)/test_vfs: \
	tests/test_vfs.c \
	kernel/fs/vfs.c \
	kernel/fs/file.c \
	kernel/proc/process.c \
	tests/assert.c \
	kernel/arch/x86/io.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) \
		tests/test_vfs.c \
		kernel/fs/vfs.c \
		kernel/fs/file.c \
		kernel/proc/process.c \
		tests/assert.c \
		kernel/arch/x86/io.c \
		-o $@

$(BUILD_DIR)/test_tmpfs: \
	tests/test_tmpfs.c \
	kernel/fs/vfs.c \
	kernel/fs/file.c \
	kernel/proc/process.c \
	kernel/fs/tmpfs/tmpfs.c \
	tests/assert.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) \
		tests/test_tmpfs.c \
		kernel/fs/vfs.c \
		kernel/fs/file.c \
		kernel/proc/process.c \
		kernel/fs/tmpfs/tmpfs.c \
		tests/assert.c \
		-o $@

$(BUILD_DIR)/test_list: \
	tests/test_list.c \
	kernel/lib/list.c \
	tests/assert.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) \
		tests/test_list.c \
		kernel/lib/list.c \
		tests/assert.c \
		-o $@

$(BUILD_DIR)/test_bitmap: \
	tests/test_bitmap.c \
	kernel/lib/bitmap.c \
	tests/assert.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) \
		tests/test_bitmap.c \
		kernel/lib/bitmap.c \
		tests/assert.c \
		-o $@

$(BUILD_DIR)/test_syscalls: tests/test_syscalls.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) \
		tests/test_syscalls.c \
		-o $@

test: $(TEST_BINS)
	@for test in $(TEST_BINS); do $$test; done

# ----------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

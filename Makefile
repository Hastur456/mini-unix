CC := gcc
ASM := nasm
LD := ld

QEMU := qemu-system-i386
QEMU_DISPLAY := -display gtk
QEMU_FLAGS :=

BUILD_DIR := build
KERNEL_DIR := kernel

KERNEL_BIN := $(BUILD_DIR)/kernel.bin

KERNEL_C_SRCS := $(wildcard $(KERNEL_DIR)/*.c)
KERNEL_FS_C_SRCS := $(wildcard $(KERNEL_DIR)/fs/*.c)
KERNEL_TMPFS_C_SRCS := $(wildcard $(KERNEL_DIR)/fs/tmpfs/*.c)
KERNEL_PROC_C_SRCS := $(wildcard $(KERNEL_DIR)/proc/*.c)
KERNEL_ASM_SRCS := $(wildcard $(KERNEL_DIR)/*.asm)

BOOT_OBJ := $(BUILD_DIR)/boot.o
C_OBJS := $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SRCS))
FS_C_OBJS := $(patsubst $(KERNEL_DIR)/fs/%.c,$(BUILD_DIR)/fs/%.o,$(KERNEL_FS_C_SRCS))
TMPFS_C_OBJS := $(patsubst $(KERNEL_DIR)/fs/tmpfs/%.c,$(BUILD_DIR)/fs/tmpfs/%.o,$(KERNEL_TMPFS_C_SRCS))
PROC_C_OBJS := $(patsubst $(KERNEL_DIR)/proc/%.c,$(BUILD_DIR)/proc/%.o,$(KERNEL_PROC_C_SRCS))
ASM_OBJS := $(patsubst $(KERNEL_DIR)/%.asm,$(BUILD_DIR)/%_asm.o,$(filter-out $(KERNEL_DIR)/boot.asm,$(KERNEL_ASM_SRCS)))
OBJS := $(BOOT_OBJ) $(C_OBJS) $(PROC_C_OBJS) $(FS_C_OBJS) $(TMPFS_C_OBJS) $(ASM_OBJS)
DEPS := $(C_OBJS:.o=.d) \
        $(FS_C_OBJS:.o=.d) \
        $(TMPFS_C_OBJS:.o=.d) \
        $(PROC_C_OBJS:.o=.d)
TEST_BINS := $(BUILD_DIR)/test_vfs $(BUILD_DIR)/test_tmpfs

CFLAGS := -m32 -std=gnu99 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra -MMD -MP -I$(KERNEL_DIR) -Dkmain=kernel_main
TEST_CFLAGS := -std=c99 -Wall -Wextra -I$(KERNEL_DIR) -I$(KERNEL_DIR)/fs
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T link.ld -nostdlib

.PHONY: all dirs run test clean

all: $(KERNEL_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

$(BOOT_OBJ): $(KERNEL_DIR)/boot.asm | dirs
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/fs/%.o: $(KERNEL_DIR)/fs/%.c | dirs
	mkdir -p $(BUILD_DIR)/fs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/fs/tmpfs/%.o: $(KERNEL_DIR)/fs/tmpfs/%.c | dirs
	mkdir -p $(BUILD_DIR)/fs/tmpfs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/proc/%.o: $(KERNEL_DIR)/proc/%.c | dirs
	mkdir -p $(BUILD_DIR)/proc
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_asm.o: $(KERNEL_DIR)/%.asm | dirs
	$(ASM) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJS) link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: all
	$(QEMU) $(QEMU_DISPLAY) $(QEMU_FLAGS) -kernel $(KERNEL_BIN)

$(BUILD_DIR)/test_vfs: tests/test_vfs.c $(KERNEL_DIR)/fs/vfs.c $(KERNEL_DIR)/fs/file.c $(KERNEL_DIR)/proc/process.c | dirs
	$(CC) $(TEST_CFLAGS) \
		tests/test_vfs.c \
		kernel/fs/vfs.c \
		kernel/fs/file.c \
		kernel/proc/process.c \
		-o $@

$(BUILD_DIR)/test_tmpfs: tests/test_tmpfs.c $(KERNEL_DIR)/fs/vfs.c $(KERNEL_DIR)/fs/file.c $(KERNEL_DIR)/proc/process.c $(KERNEL_DIR)/fs/tmpfs/tmpfs.c | dirs
	$(CC) $(TEST_CFLAGS) \
		tests/test_tmpfs.c \
		kernel/fs/vfs.c \
		kernel/fs/file.c \
		kernel/proc/process.c \
		kernel/fs/tmpfs/tmpfs.c \
		-o $@

test: $(TEST_BINS)
	@for test in $(TEST_BINS); do $$test; done

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

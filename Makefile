CC := gcc
ASM := nasm
LD := ld

QEMU := qemu-system-i386
QEMU_DISPLAY := -display gtk
QEMU_FLAGS :=

BUILD_DIR := build
KERNEL_DIR := kernel

KERNEL_BIN := $(BUILD_DIR)/kernel.bin

C_SRCS := $(shell find $(KERNEL_DIR) -name '*.c')
ASM_SRCS := $(shell find $(KERNEL_DIR) -name '*.asm')

BOOT_SRC := $(KERNEL_DIR)/arch/x86/boot.asm
BOOT_OBJ := $(BUILD_DIR)/arch/x86/boot.o

ASM_SRCS := $(filter-out $(BOOT_SRC),$(ASM_SRCS))

C_OBJS := $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))
ASM_OBJS := $(patsubst $(KERNEL_DIR)/%.asm,$(BUILD_DIR)/%_asm.o,$(ASM_SRCS))

OBJS := $(BOOT_OBJ) $(C_OBJS) $(ASM_OBJS)
DEPS := $(C_OBJS:.o=.d)

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

TEST_CFLAGS := \
	-std=c99 \
	-Wall \
	-Wextra \
	-Iinclude

ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T link.ld -nostdlib

TEST_BINS := \
	$(BUILD_DIR)/test_vfs \
	$(BUILD_DIR)/test_tmpfs \
	$(BUILD_DIR)/test_syscalls \
	$(BUILD_DIR)/test_list \
	$(BUILD_DIR)/test_bitmap

.PHONY: all run test clean dirs

all: $(KERNEL_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

$(BOOT_OBJ): $(BOOT_SRC)
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_asm.o: $(KERNEL_DIR)/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJS) link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: all
	$(QEMU) $(QEMU_DISPLAY) $(QEMU_FLAGS) -kernel $(KERNEL_BIN)

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

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
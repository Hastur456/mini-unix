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
KERNEL_ASM_SRCS := $(wildcard $(KERNEL_DIR)/*.asm)

BOOT_OBJ := $(BUILD_DIR)/boot.o
C_OBJS := $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SRCS))
ASM_OBJS := $(patsubst $(KERNEL_DIR)/%.asm,$(BUILD_DIR)/%_asm.o,$(filter-out $(KERNEL_DIR)/boot.asm,$(KERNEL_ASM_SRCS)))
OBJS := $(BOOT_OBJ) $(C_OBJS) $(ASM_OBJS)
DEPS := $(C_OBJS:.o=.d)

CFLAGS := -m32 -std=gnu99 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra -MMD -MP -I$(KERNEL_DIR) -Dkmain=kernel_main
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T link.ld -nostdlib

.PHONY: all dirs run clean

all: $(KERNEL_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

$(BOOT_OBJ): $(KERNEL_DIR)/boot.asm | dirs
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_asm.o: $(KERNEL_DIR)/%.asm | dirs
	$(ASM) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJS) link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: all
	$(QEMU) $(QEMU_DISPLAY) $(QEMU_FLAGS) -kernel $(KERNEL_BIN)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

CC := gcc
ASM := nasm
LD := ld
QEMU := qemu-system-i386

BUILD_DIR := build
KERNEL_DIR := kernel

KERNEL_BIN := $(BUILD_DIR)/kernel.bin
OBJS := \
	$(BUILD_DIR)/boot.o \
	$(BUILD_DIR)/kernel.o \
	$(BUILD_DIR)/vga.o

CFLAGS := -m32 -std=gnu99 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra -I$(KERNEL_DIR) -Dkmain=kernel_main
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T link.ld -nostdlib

.PHONY: all dirs run clean

all: $(KERNEL_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

	$(BUILD_DIR)/boot.o: $(KERNEL_DIR)/boot.asm | dirs
		$(ASM) $(ASFLAGS) $< -o $@

	$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c $(KERNEL_DIR)/vga.h | dirs
		$(CC) $(CFLAGS) -c $< -o $@

	$(BUILD_DIR)/vga.o: $(KERNEL_DIR)/vga.c $(KERNEL_DIR)/vga.h | dirs
		$(CC) $(CFLAGS) -c $< -o $@

	$(KERNEL_BIN): $(OBJS) link.ld
		$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: all
	$(QEMU) -kernel $(KERNEL_BIN)

clean:
	rm -rf $(BUILD_DIR)

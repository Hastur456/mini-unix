CC = gcc
ASM = nasm
LD = ld

BUILD_DIR = build

CFLAGS = -m32 -ffreestanding -c
LDFLAGS = -m elf_i386 -T link.ld

all: dirs $(BUILD_DIR)/kernel.bin

dirs:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/bootloader.o:
	$(ASM) -f elf32 kernel/boot/bootloader.asm -o $(BUILD_DIR)/bootloader.o

$(BUILD_DIR)/entry.o:
	$(ASM) -f elf32 kernel/entry.asm -o $(BUILD_DIR)/entry.o

$(BUILD_DIR)/kernel.o:
	$(CC) $(CFLAGS) kernel/kernel.c -o $(BUILD_DIR)/kernel.o

$(BUILD_DIR)/main.o:
	$(CC) $(CFLAGS) kernel/main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/kernel.bin: \
	$(BUILD_DIR)/bootloader.o \
	$(BUILD_DIR)/entry.o \
	$(BUILD_DIR)/kernel.o \
	$(BUILD_DIR)/main.o

	$(LD) $(LDFLAGS) \
	-o $(BUILD_DIR)/kernel.bin \
	$(BUILD_DIR)/bootloader.o \
	$(BUILD_DIR)/entry.o \
	$(BUILD_DIR)/kernel.o \
	$(BUILD_DIR)/main.o

run: all
	qemu-system-x86_64 -kernel $(BUILD_DIR)/kernel.bin

clean:
	rm -rf build
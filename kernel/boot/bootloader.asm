[BITS 16]
[ORG 0x7C00]

KERNEL_LOAD_SEGMENT equ 0x1000
KERNEL_LOAD_OFFSET  equ 0x0000
KERNEL_SECTORS      equ 9
KERNEL_START_SECTOR equ 2

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov si, boot_msg
    call print_string

    mov ax, KERNEL_LOAD_SEGMENT
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, KERNEL_SECTORS
    mov ch, 0x00
    mov cl, KERNEL_START_SECTOR
    mov dh, 0x00
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

disk_error:
    mov si, error_msg
    call print_string

hang:
    cli
    hlt
    jmp hang

print_string:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp print_string
.done:
    ret

boot_drive db 0
boot_msg   db "Booting kernel...", 0
error_msg  db "Disk read error!", 0

times 510 - ($ - $$) db 0
dw 0xAA55

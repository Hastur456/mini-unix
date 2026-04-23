[BITS 16]
[GLOBAL start]
[EXTERN kmain]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000
    sti

    call kmain

hang:
    cli
    hlt
    jmp hang

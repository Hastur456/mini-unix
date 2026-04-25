section .multiboot
align 4
dd 0x1BADB002
dd 0
dd -(0x1BADB002)

section .text
extern _start
global loader

loader:
    call _start

.hang:
    cli
    hlt
    jmp .hang
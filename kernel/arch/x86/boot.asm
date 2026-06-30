section .multiboot
align 4
dd 0x1BADB002
dd 0
dd -(0x1BADB002)

section .text
global loader
extern kernel_main

loader:
    mov esp, stack_top
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
resb 8192
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits

global _start
extern kernel_main

section .text

_start:
    mov esp, stack_top
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

section .bss
resb 8192
stack_top:
global inb

section .text

inb:
    mov dx, [esp - 4]
    in al, dx
    movzx eax, al
    ret
ENTRY(_start)

SECTIONS
{
    . = 0x00200000;

    .text :
    {
        KEEP(*(.text.start))
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }
}
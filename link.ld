ENTRY(loader)

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 1M;

    kernel_start = .;

    .text : ALIGN(4096)
    {
        *(.multiboot)
        *(.text*)
    } :text

    .rodata : ALIGN(4096)
    {
        *(.rodata*)
    } :text

    .data : ALIGN(4096)
    {
        *(.data*)
    } :data

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss*)
    } :data

    . = ALIGN(4096);
    kernel_end = .;
}

#include "gdt.h"


gdt_entry_t gdt[8];
gdt_ptr_t gdt_ptr;


void gdt_flush(gdt_ptr_t *gdtr_pointer) {
    __asm__ __volatile__ (
        "lgdt (%[gdtr]);"          // Load the GDT pointer
        "movw $0x10, %%ax;"        // 0x10 is the offset to the kernel data segment in GDT
        "movw %%ax, %%ds;"         // Reload DS
        "movw %%ax, %%es;"         // Reload ES
        "movw %%ax, %%fs;"         // Reload FS
        "movw %%ax, %%gs;"         // Reload GS
        "movw %%ax, %%ss;"         // Reload SS
        "ljmp $0x08, $1f;"         // Far jump to update CS (0x08 is the code segment offset)
        "1:"                       // Local label used for the jump target
        :
        : [gdtr] "r" (gdtr_pointer) // Input operand: register holding the pointer
        : "ax", "memory"           // Clobbers: EAX and memory modifications
    );
}


void gdt_set_gate(
    int num,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t flags
) {
    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].limit_high = (limit >> 16) & 0x0F;

    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].access = access;
    gdt[num].flags = flags & 0x0F;
}


void gdt_init(void) {
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint32_t)gdt;

    // Null descriptor
    gdt_set_gate(
        GDT_NULL_SEGMENT,
        0,
        0,
        0,
        0
    );

    // Kernel code
    gdt_set_gate(
        GDT_KCODE_SEGMENT,
        0,
        0xFFFFF,
        0x9A,
        0xC
    );

    // Kernel data
    gdt_set_gate(
        GDT_KDATA_SEGMENT,
        0,
        0xFFFFF,
        0x92,
        0xC
    );

    // Kernel stack
    gdt_set_gate(
        GDT_KSTACK_SEGMENT,
        0,
        0xFFFFF,
        0x92,
        0xC
    );

    // User code
    gdt_set_gate(
        GDT_UCODE_SEGMENT,
        0,
        0xFFFFF,
        0xFA,
        0xC
    );

    // User data
    gdt_set_gate(
        GDT_UDATA_SEGMENT,
        0,
        0xFFFFF,
        0xF2,
        0xC
    );

    // User stack
    gdt_set_gate(
        GDT_USTACK_SEGMENT,
        0,
        0xFFFFF,
        0xF2,
        0xC
    );

    // TSS
    gdt_set_gate(
        GDT_TSS_SEGMENT,
        0,
        0,
        0,
        0
    );

    gdt_flush(&gdt_ptr);
}

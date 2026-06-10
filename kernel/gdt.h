#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_NULL_SEGMENT 0
#define GDT_KCODE_SEGMENT 1
#define GDT_KDATA_SEGMENT 2
#define GDT_KSTACK_SEGMENT 3
#define GDT_UCODE_SEGMENT 4
#define GDT_UDATA_SEGMENT 5
#define GDT_USTACK_SEGMENT 6
#define GDT_TSS_SEGMENT 7
#define GDT_ENTRIES_COUNT 8

#define GDT_NULL_SELECTOR 0x00
#define GDT_KCODE_SELECTOR 0x08
#define GDT_KDATA_SELECTOR 0x10
#define GDT_KSTACK_SELECTOR 0x18
#define GDT_KTSS_SELECTOR 0x38
#define GDT_UCODE_SELECTOR 0x23
#define GDT_UDATA_SELECTOR 0x2b
#define GDT_USTACK_SELECTOR 0x33
#define GDT_UTSS_SELECTOR 0x3b

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  limit_high : 4;
    uint8_t  flags : 4;
    uint8_t  base_high;
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr_type {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct gdt_ptr_type gdt_ptr_t;

void gdt_set_gate(
    int num,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t flags
);

void gdt_init(void);
void gdt_flush(gdt_ptr_t *gdtr_pointer);

#endif

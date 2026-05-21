#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

#define INTERRUPT_VECTOR_COUNT 256
#define IRQ_BASE 32

struct interrupt_frame {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t vector;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

typedef void (*interrupt_handler_t)(struct interrupt_frame* frame);

void interrupts_init(void);
void interrupt_register_handler(uint8_t vector, interrupt_handler_t handler);
void interrupt_dispatch(struct interrupt_frame* frame);
void interrupt_enable(void);
void interrupt_disable(void);

#endif
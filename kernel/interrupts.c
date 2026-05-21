#include <stdint.h>
#include "interrupts.h"
#include "idt.h"
#include "pic.h"
#include "tty.h"

static interrupt_handler_t handlers[INTERRUPT_VECTOR_COUNT];

static void write_hex32(uint32_t value) {
    const char* hex = "0123456789ABCDEF";

    tty_write("0x");
    for (int i = 28; i >= 0; i -= 4) {
        tty_putchar(hex[(value >> i) & 0xF]);
    }
}

static void write_dec(uint32_t value) {
    char buf[11];
    int pos = 0;

    if (value == 0) {
        tty_putchar('0');
        return;
    }

    while (value > 0 && pos < 10) {
        buf[pos++] = '0' + (value % 10);
        value /= 10;
    }

    while (pos > 0) {
        tty_putchar(buf[--pos]);
    }
}

static void halt_kernel(void) {
    interrupt_disable();
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void interrupts_init(void) {
    for (int i = 0; i < INTERRUPT_VECTOR_COUNT; i++) {
        handlers[i] = 0;
    }

    idt_init();
}

void interrupt_register_handler(uint8_t vector, interrupt_handler_t handler) {
    handlers[vector] = handler;
}

void interrupt_dispatch(struct interrupt_frame* frame) {
    uint32_t vector = frame->vector;

    if (vector < INTERRUPT_VECTOR_COUNT && handlers[vector]) {
        handlers[vector](frame);
    } else if (vector < IRQ_BASE) {
        tty_write("\nUnhandled CPU exception ");
        write_dec(vector);
        tty_write(" error=");
        write_hex32(frame->error_code);
        tty_write(" eip=");
        write_hex32(frame->eip);
        tty_write("\n");
        halt_kernel();
    }

    if (vector >= IRQ_BASE && vector < IRQ_BASE + PIC_IRQ_COUNT) {
        pic_send_eoi((uint8_t)(vector - IRQ_BASE));
    }
}

void interrupt_enable(void) {
    __asm__ volatile ("sti");
}

void interrupt_disable(void) {
    __asm__ volatile ("cli");
}
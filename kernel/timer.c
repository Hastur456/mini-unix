#include <stdint.h>
#include "interrupts.h"
#include "io.h"
#include "pic.h"
#include "timer.h"

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_BASE_FREQUENCY 1193182

static volatile uint32_t ticks = 0;

static void timer_irq_handler(struct interrupt_frame* frame) {
    (void)frame;
    ticks++;
}

void timer_init(uint32_t frequency) {
    if (frequency == 0) {
        frequency = 100;
    }

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    if (divisor == 0) {
        divisor = 1;
    }

    interrupt_register_handler(IRQ_BASE, timer_irq_handler);

    outb(PIT_COMMAND_PORT, 0x36);
    outb(PIT_CHANNEL0_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF);

    pic_unmask_irq(0);
}

uint32_t timer_ticks(void) {
    return ticks;
}

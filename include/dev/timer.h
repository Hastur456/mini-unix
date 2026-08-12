#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/io.h>
#include <arch/x86/pic.h>

void timer_init(uint32_t frequency);
uint32_t timer_ticks(void);

#endif

#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stddef.h>
#include "tty.h"
#include "vga.h"

void tty_init();
void tty_putchar(char c);
void tty_write(const char* str);

#endif
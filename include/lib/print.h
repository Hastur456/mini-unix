#ifndef PRINT_H
#define PRINT_H

#include <dev/tty.h>
#include <lib/stdarg.h>

void kprint(const char *str);

void vkprintf(const char *fmt, va_list args);

void kprintf(const char *fmt, ...);

#endif
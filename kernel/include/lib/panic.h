#ifndef PANIC_H
#define PANIC_H

#include <lib/print.h>
#include <lib/stdarg.h>

void panic(const char *expr, const char *file, int line, const char *func);

#endif
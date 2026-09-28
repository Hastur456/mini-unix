#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_START 0x00100000
#define HEAP_SIZE  0x00100000

void heap_init(void *start, size_t size);

void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t size);
#endif
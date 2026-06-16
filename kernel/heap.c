#include <stdint.h>
#include <stddef.h>
#include "heap.h"


static uint8_t *heap_start;
static uint8_t *heap_current;
static uint8_t *heap_end;


void heap_init(void *start, size_t size) {
    heap_start = start;
    heap_end = heap_start + size;
    heap_current = heap_start;
}


void *kmalloc(size_t size) {
    size = (size + 7) & ~(size_t)7;

    if (heap_current + size >= heap_end) return NULL;

    void *ptr = heap_current;

    heap_current += size;

    return ptr;
}


void kfree(void *ptr) {
    (void)ptr;
}

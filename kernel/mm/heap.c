#include <mm/heap.h>

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

    if (heap_current + size >= heap_end) {
        return NULL;
    }

    void *ptr = heap_current;
    heap_current += size;

    return ptr;
}


void kfree(void *ptr) {
    (void)ptr;
}


void *krealloc(void *ptr, size_t size) {
    if (!ptr)
        return kmalloc(size);

    if (size == 0) {
        kfree(ptr);
        return NULL;
    } 

    size_t old_size = *((size_t *)ptr - 1);

    void *new_ptr = kmalloc(size);
    if (!new_ptr)
        return NULL;

    size_t copy_size = old_size < size ? old_size : size;

    char *src = (char *)ptr;
    char *dst = (char *)new_ptr;

    for (size_t i = 0; i < copy_size; i++) {
        dst[i] = src[i];
    }

    kfree(ptr);

    return new_ptr;
}

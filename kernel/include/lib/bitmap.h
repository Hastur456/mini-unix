#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>

#define BITMAP_ERROR SIZE_MAX

struct bitmap {
    uint8_t *data;
    size_t bits;
};

size_t bitmap_size(size_t bits);
void bitmap_init(struct bitmap *bitmap, uint8_t *data, size_t bits);
void bitmap_fill(struct bitmap *bitmap);
void bitmap_zero(struct bitmap *bitmap);
void bitmap_set(struct bitmap *bitmap, size_t index);
void bitmap_clear(struct bitmap *bitmap, size_t index);
int bitmap_test(const struct bitmap *bitmap, size_t index);

void bitmap_assign(struct bitmap *bitmap, size_t index, int value);
void bitmap_toggle(struct bitmap *bitmap, size_t index);
int bitmap_test_clear(const struct bitmap *bitmap, size_t index);
void bitmap_set_range(struct bitmap *bitmap, size_t start, size_t count);
void bitmap_clear_range(struct bitmap *bitmap, size_t start, size_t count);

size_t bitmap_find_first_clear(const struct bitmap *bitmap);
size_t bitmap_find_first_set(const struct bitmap *bitmap);
size_t bitmap_find_next_set(const struct bitmap *bitmap, size_t start);
size_t bitmap_find_next_clear(const struct bitmap *bitmap, size_t start);
size_t bitmap_find_zero_area(const struct bitmap *bitmap, size_t count);
size_t bitmap_count_set(const struct bitmap *bitmap);

int bitmap_test_range(const struct bitmap *bitmap, size_t start, size_t count);
int bitmap_test_range_clear(const struct bitmap *bitmap, size_t start, size_t count);

size_t bitmap_count_clear(const struct bitmap *bitmap);
static inline size_t bitmap_bits(const struct bitmap *bitmap) { return bitmap->bits; }

int bitmap_empty(const struct bitmap *bitmap);
int bitmap_full(const struct bitmap *bitmap);

#endif

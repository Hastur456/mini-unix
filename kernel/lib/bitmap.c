#include <lib/bitmap.h>

#define BYTE_IDX(bit) ((bit) / 8)
#define BIT_OFFSET(bit) ((bit) % 8)

size_t bitmap_size(size_t bits) {
    return (bits + 7) / 8;
}

void bitmap_init(
    struct bitmap *bitmap,
    uint8_t *data,
    size_t bits
) {
    if (!bitmap) return;

    bitmap->data = data;
    bitmap->bits = bits;
}

void bitmap_zero(struct bitmap *bitmap) {
    if (!bitmap || !bitmap->data || bitmap_bits(bitmap) == 0) return;

    size_t bytes = bitmap_size(bitmap_bits(bitmap));

    for (size_t i = 0; i < bytes; i++) {
        bitmap->data[i] = 0;
    }
}

void bitmap_fill(struct bitmap *bitmap) {
    if (!bitmap || !bitmap->data || bitmap_bits(bitmap) == 0) return;

    size_t bytes = bitmap_size(bitmap_bits(bitmap));

    for (size_t i = 0; i < bytes; i++) {
        bitmap->data[i] = 0xFF;
    }

    size_t remainder = bitmap_bits(bitmap) % 8;
    if (remainder != 0) {
        bitmap->data[bytes-1] &= (1 << remainder) - 1;
    }
}

void bitmap_set(struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap_bits(bitmap) == 0) return;
    if (bitmap_bits(bitmap) <= index) return;
    bitmap->data[BYTE_IDX(index)] |= (1 << BIT_OFFSET(index));
}

void bitmap_clear(struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap_bits(bitmap) == 0) return;
    if (bitmap_bits(bitmap) <= index) return;
    bitmap->data[BYTE_IDX(index)] &= ~(1 << BIT_OFFSET(index));
}

int bitmap_test(const struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap_bits(bitmap) == 0) return 0;
    if (bitmap_bits(bitmap) <= index) return 0;
    return bitmap->data[BYTE_IDX(index)] & (1 << BIT_OFFSET(index));
}

void bitmap_assign(struct bitmap *bitmap, size_t index, int value) {
    if (value) {
        bitmap_set(bitmap, index);
    } else {
        bitmap_clear(bitmap, index);
    }
}

void bitmap_toggle(struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap_bits(bitmap) == 0) return;
    if (index >= bitmap_bits(bitmap)) return;

    bitmap->data[BYTE_IDX(index)] ^= (1 << BIT_OFFSET(index));
}

int bitmap_test_clear(const struct bitmap *bitmap, size_t index) {
    int val = bitmap_test(bitmap, index);
    bitmap_clear((struct bitmap *)bitmap, index);
    return val;
}

void bitmap_set_range(struct bitmap *bitmap, size_t start, size_t count) {
    for (size_t i = 0; i < count; i++) {
        bitmap_set(bitmap, start + i);
    }
}

void bitmap_clear_range(struct bitmap *bitmap, size_t start, size_t count) {
    for (size_t i = 0; i < count; i++) {
        bitmap_clear(bitmap, start + i);
    }
}

size_t bitmap_find_first_clear(const struct bitmap *bitmap) {
    if (!bitmap || !bitmap->data) return BITMAP_ERROR;

    for (size_t i = 0; i < bitmap_bits(bitmap); i++) {
        if (!bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap_bits(bitmap);
}

size_t bitmap_find_first_set(const struct bitmap *bitmap) {
    if (!bitmap || !bitmap->data) return BITMAP_ERROR;

    for (size_t i = 0; i < bitmap_bits(bitmap); i++) {
        if (bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap_bits(bitmap);
}

size_t bitmap_find_next_clear(const struct bitmap *bitmap, size_t start) {
    if (!bitmap || !bitmap->data) return BITMAP_ERROR;

    for (size_t i = start; i < bitmap_bits(bitmap); i++) {
        if (!bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap_bits(bitmap);
}

size_t bitmap_find_next_set(const struct bitmap *bitmap, size_t start) {
    if (!bitmap || !bitmap->data) return BITMAP_ERROR;

    for (size_t i = start; i < bitmap_bits(bitmap); i++) {
        if (bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap_bits(bitmap);
}

size_t bitmap_find_zero_area(const struct bitmap *bitmap, size_t count) {
    if (!bitmap || !bitmap->data || count == 0 || count > bitmap_bits(bitmap)) {
        return BITMAP_ERROR;
    }

    size_t current_zeros = 0;
    size_t start_idx = 0;

    for (size_t i = 0; i < bitmap_bits(bitmap); i++) {
        if (!bitmap_test(bitmap, i)) {
            if (current_zeros == 0) {
                start_idx = i;
            }
            current_zeros++;
            if (current_zeros == count) {
                return start_idx;
            }
        } else {
            current_zeros = 0;
        }
    }
    
    return bitmap_bits(bitmap);
}

size_t bitmap_count_set(const struct bitmap *bitmap) {
    if (!bitmap || !bitmap->data) return BITMAP_ERROR;
    
    size_t count = 0;
    for (size_t i = 0; i < bitmap_bits(bitmap); i++) {
        if (bitmap_test(bitmap, i)) count++;
    }

    return count;
}

int bitmap_test_range(const struct bitmap *bitmap, size_t start, size_t count) {
    if (!bitmap || start + count > bitmap_bits(bitmap)) return 0;
    for (size_t i = 0; i < count; i++) {
        if (!bitmap_test(bitmap, start + i)) {
            return 0;
        }
    }
    return 1;
}

int bitmap_test_range_clear(const struct bitmap *bitmap, size_t start, size_t count) {
    if (!bitmap || start + count > bitmap->bits) return 0;
    for (size_t i = 0; i < count; i++) {
        if (bitmap_test(bitmap, start + i)) {
            return 0;
        }
    }
    return 1;
}

size_t bitmap_count_clear(const struct bitmap *bitmap) {
    if (!bitmap) return BITMAP_ERROR;
    return bitmap_bits(bitmap) - bitmap_count_set(bitmap);
}

int bitmap_empty(const struct bitmap *bitmap) {
    if (!bitmap) return 1;
    return bitmap_count_set(bitmap) == 0;
}

int bitmap_full(const struct bitmap *bitmap) {
    if (!bitmap) return 0;
    return bitmap_count_clear(bitmap) == 0;
}

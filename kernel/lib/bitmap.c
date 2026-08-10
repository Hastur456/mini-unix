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
    if (!bitmap || !bitmap->data || bitmap->bits == 0) return;

    size_t bytes = bitmap_size(bitmap->bits);

    for (size_t i = 0; i < bytes; i++) {
        bitmap->data[i] = 0;
    }
}

void bitmap_fill(struct bitmap *bitmap) {
    if (!bitmap || !bitmap->data || bitmap->bits == 0) return;

    size_t bytes = bitmap_size(bitmap->bits);

    for (size_t i = 0; i < bytes; i++) {
        bitmap->data[i] = 0xFF;
    }

    size_t remainder = bitmap->bits % 8;
    if (remainder != 0) {
        bitmap->data[bytes-1] &= (1 << remainder) - 1;
    }
}

void bitmap_set(struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap->bits == 0) return;
    if (bitmap->bits <= index) return;
    bitmap->data[BYTE_IDX(index)] |= (1 << BIT_OFFSET(index));
}

void bitmap_clear(struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap->bits == 0) return;
    if (bitmap->bits <= index) return;
    bitmap->data[BYTE_IDX(index)] &= ~(1 << BIT_OFFSET(index));
}

int bitmap_test(const struct bitmap *bitmap, size_t index) {
    if (!bitmap || !bitmap->data || bitmap->bits == 0) return 0;
    if (bitmap->bits <= index) return 0;
    return bitmap->data[BYTE_IDX(index)] & (1 << BIT_OFFSET(index));
}

size_t bitmap_find_first_clear(const struct bitmap *bitmap) {
    if (!bitmap) return bitmap->bits;

    for (size_t i = 0; i < bitmap->bits; i++) {
        if (!bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap->bits;
}

size_t bitmap_find_first_set(const struct bitmap *bitmap) {
    if (!bitmap) return bitmap->bits;

    for (size_t i = 0; i < bitmap->bits; i++) {
        if (bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap->bits;
}

size_t bitmap_find_next_clear(const struct bitmap *bitmap, size_t start) {
    if (!bitmap) return 0;

    for (size_t i = start; i < bitmap->bits; i++) {
        if (!bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap->bits;
}

size_t bitmap_find_next_set(const struct bitmap *bitmap, size_t start) {
    if (!bitmap) return 0;

    for (size_t i = start; i < bitmap->bits; i++) {
        if (bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return bitmap->bits;
}

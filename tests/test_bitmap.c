#include <stdint.h>
#include <lib/assert.h>
#include <lib/bitmap.h>

#define TEST_BITS 13
#define TEST_BYTES ((TEST_BITS + 7) / 8)

static void test_bitmap_size(void)
{
    ASSERT(bitmap_size(0) == 0);
    ASSERT(bitmap_size(1) == 1);
    ASSERT(bitmap_size(7) == 1);
    ASSERT(bitmap_size(8) == 1);
    ASSERT(bitmap_size(9) == 2);
    ASSERT(bitmap_size(13) == 2);
    ASSERT(bitmap_size(16) == 2);
    ASSERT(bitmap_size(17) == 3);
}

static void test_bitmap_init(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);

    ASSERT(bitmap.data == data);
    ASSERT(bitmap.bits == TEST_BITS);
}

static void test_bitmap_zero(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);

    data[0] = 0xFF;
    data[1] = 0xFF;

    bitmap_zero(&bitmap);

    ASSERT(data[0] == 0x00);
    ASSERT(data[1] == 0x00);

    for (size_t i = 0; i < TEST_BITS; i++) {
        ASSERT(bitmap_test(&bitmap, i) == 0);
    }
}

static void test_bitmap_fill(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_fill(&bitmap);

    ASSERT(data[0] == 0xFF);
    ASSERT(data[1] == 0x1F);

    for (size_t i = 0; i < TEST_BITS; i++) {
        ASSERT(bitmap_test(&bitmap, i) != 0);
    }
}

static void test_bitmap_set(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, 0);
    ASSERT(bitmap_test(&bitmap, 0) != 0);

    bitmap_set(&bitmap, 3);
    ASSERT(bitmap_test(&bitmap, 3) != 0);

    bitmap_set(&bitmap, 7);
    ASSERT(bitmap_test(&bitmap, 7) != 0);

    bitmap_set(&bitmap, 8);
    ASSERT(bitmap_test(&bitmap, 8) != 0);

    bitmap_set(&bitmap, 12);
    ASSERT(bitmap_test(&bitmap, 12) != 0);

    ASSERT(data[0] == 0x89);
    ASSERT(data[1] == 0x11);
}

static void test_bitmap_clear(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_fill(&bitmap);

    bitmap_clear(&bitmap, 0);
    ASSERT(bitmap_test(&bitmap, 0) == 0);

    bitmap_clear(&bitmap, 3);
    ASSERT(bitmap_test(&bitmap, 3) == 0);

    bitmap_clear(&bitmap, 8);
    ASSERT(bitmap_test(&bitmap, 8) == 0);

    bitmap_clear(&bitmap, 12);
    ASSERT(bitmap_test(&bitmap, 12) == 0);

    ASSERT(data[0] == 0xF6);
    ASSERT(data[1] == 0x0E);
}

static void test_bitmap_test(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, 2);
    bitmap_set(&bitmap, 5);
    bitmap_set(&bitmap, 10);

    ASSERT(bitmap_test(&bitmap, 0) == 0);
    ASSERT(bitmap_test(&bitmap, 1) == 0);
    ASSERT(bitmap_test(&bitmap, 2) != 0);
    ASSERT(bitmap_test(&bitmap, 3) == 0);
    ASSERT(bitmap_test(&bitmap, 4) == 0);
    ASSERT(bitmap_test(&bitmap, 5) != 0);
    ASSERT(bitmap_test(&bitmap, 6) == 0);
    ASSERT(bitmap_test(&bitmap, 7) == 0);
    ASSERT(bitmap_test(&bitmap, 8) == 0);
    ASSERT(bitmap_test(&bitmap, 9) == 0);
    ASSERT(bitmap_test(&bitmap, 10) != 0);
    ASSERT(bitmap_test(&bitmap, 11) == 0);
    ASSERT(bitmap_test(&bitmap, 12) == 0);
}

static void test_bitmap_find_first_clear(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, 0);
    bitmap_set(&bitmap, 1);
    bitmap_set(&bitmap, 2);

    ASSERT(bitmap_find_first_clear(&bitmap) == 3);

    bitmap_set(&bitmap, 3);
    ASSERT(bitmap_find_first_clear(&bitmap) == 4);

    bitmap_fill(&bitmap);
    ASSERT(bitmap_find_first_clear(&bitmap) == bitmap.bits);
}

static void test_bitmap_find_first_set(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    ASSERT(bitmap_find_first_set(&bitmap) == bitmap.bits);

    bitmap_set(&bitmap, 5);
    ASSERT(bitmap_find_first_set(&bitmap) == 5);

    bitmap_set(&bitmap, 2);
    ASSERT(bitmap_find_first_set(&bitmap) == 2);

    bitmap_set(&bitmap, 0);
    ASSERT(bitmap_find_first_set(&bitmap) == 0);
}

static void test_bitmap_find_next_clear(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, 2);
    bitmap_set(&bitmap, 5);
    bitmap_set(&bitmap, 9);
    bitmap_set(&bitmap, 12);

    ASSERT(bitmap_find_next_clear(&bitmap, 0) == 0);
    ASSERT(bitmap_find_next_clear(&bitmap, 2) == 3);
    ASSERT(bitmap_find_next_clear(&bitmap, 5) == 6);
    ASSERT(bitmap_find_next_clear(&bitmap, 9) == 10);
    ASSERT(bitmap_find_next_clear(&bitmap, 12) == bitmap.bits);
}

static void test_bitmap_find_next_set(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, 2);
    bitmap_set(&bitmap, 5);
    bitmap_set(&bitmap, 9);
    bitmap_set(&bitmap, 12);

    ASSERT(bitmap_find_next_set(&bitmap, 0) == 2);
    ASSERT(bitmap_find_next_set(&bitmap, 3) == 5);
    ASSERT(bitmap_find_next_set(&bitmap, 6) == 9);
    ASSERT(bitmap_find_next_set(&bitmap, 10) == 12);
    ASSERT(bitmap_find_next_set(&bitmap, 12) == 12);
    ASSERT(bitmap_find_next_set(&bitmap, 13) == bitmap.bits);
}

static void test_bitmap_boundaries(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, TEST_BITS);
    ASSERT(data[0] == 0);
    ASSERT(data[1] == 0);

    bitmap_clear(&bitmap, TEST_BITS);
    ASSERT(data[0] == 0);
    ASSERT(data[1] == 0);

    ASSERT(bitmap_test(&bitmap, TEST_BITS) == 0);
}

static void test_bitmap_find_not_found(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);

    bitmap_fill(&bitmap);

    ASSERT(bitmap_find_first_clear(&bitmap) == bitmap.bits);
    ASSERT(bitmap_find_next_clear(&bitmap, 0) == bitmap.bits);

    bitmap_zero(&bitmap);

    ASSERT(bitmap_find_first_set(&bitmap) == bitmap.bits);
    ASSERT(bitmap_find_next_set(&bitmap, 0) == bitmap.bits);
}

int main(void)
{
    test_bitmap_size();
    test_bitmap_init();
    test_bitmap_zero();
    test_bitmap_fill();
    test_bitmap_set();
    test_bitmap_clear();
    test_bitmap_test();
    test_bitmap_find_first_clear();
    test_bitmap_find_first_set();
    test_bitmap_find_next_clear();
    test_bitmap_find_next_set();
    test_bitmap_boundaries();
    test_bitmap_find_not_found();

    return 0;
}

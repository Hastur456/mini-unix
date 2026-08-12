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

static void test_bitmap_assign(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_assign(&bitmap, 3, 1);
    ASSERT(bitmap_test(&bitmap, 3) != 0);

    bitmap_assign(&bitmap, 3, 0);
    ASSERT(bitmap_test(&bitmap, 3) == 0);

    bitmap_assign(&bitmap, 10, 1);
    ASSERT(bitmap_test(&bitmap, 10) != 0);
}

static void test_bitmap_toggle(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_toggle(&bitmap, 3);
    ASSERT(bitmap_test(&bitmap, 3) != 0);

    bitmap_toggle(&bitmap, 3);
    ASSERT(bitmap_test(&bitmap, 3) == 0);

    bitmap_toggle(&bitmap, 12);
    ASSERT(bitmap_test(&bitmap, 12) != 0);

    bitmap_toggle(&bitmap, 12);
    ASSERT(bitmap_test(&bitmap, 12) == 0);
}

static void test_bitmap_test_clear(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set(&bitmap, 4);
    bitmap_set(&bitmap, 9);

    ASSERT(bitmap_test_clear(&bitmap, 4) != 0);
    ASSERT(bitmap_test(&bitmap, 4) == 0);

    ASSERT(bitmap_test_clear(&bitmap, 9) != 0);
    ASSERT(bitmap_test(&bitmap, 9) == 0);

    ASSERT(bitmap_test_clear(&bitmap, 5) == 0);
    ASSERT(bitmap_test(&bitmap, 5) == 0);
}

static void test_bitmap_set_range(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set_range(&bitmap, 2, 4);

    ASSERT(bitmap_test(&bitmap, 0) == 0);
    ASSERT(bitmap_test(&bitmap, 1) == 0);
    ASSERT(bitmap_test(&bitmap, 2) != 0);
    ASSERT(bitmap_test(&bitmap, 3) != 0);
    ASSERT(bitmap_test(&bitmap, 4) != 0);
    ASSERT(bitmap_test(&bitmap, 5) != 0);
    ASSERT(bitmap_test(&bitmap, 6) == 0);

    bitmap_set_range(&bitmap, 10, 3);

    ASSERT(bitmap_test(&bitmap, 10) != 0);
    ASSERT(bitmap_test(&bitmap, 11) != 0);
    ASSERT(bitmap_test(&bitmap, 12) != 0);
}

static void test_bitmap_clear_range(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_fill(&bitmap);

    bitmap_clear_range(&bitmap, 2, 4);

    ASSERT(bitmap_test(&bitmap, 0) != 0);
    ASSERT(bitmap_test(&bitmap, 1) != 0);
    ASSERT(bitmap_test(&bitmap, 2) == 0);
    ASSERT(bitmap_test(&bitmap, 3) == 0);
    ASSERT(bitmap_test(&bitmap, 4) == 0);
    ASSERT(bitmap_test(&bitmap, 5) == 0);
    ASSERT(bitmap_test(&bitmap, 6) != 0);

    bitmap_clear_range(&bitmap, 10, 3);

    ASSERT(bitmap_test(&bitmap, 10) == 0);
    ASSERT(bitmap_test(&bitmap, 11) == 0);
    ASSERT(bitmap_test(&bitmap, 12) == 0);
}

static void test_bitmap_find_zero_area(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    ASSERT(bitmap_find_zero_area(&bitmap, 1) == 0);
    ASSERT(bitmap_find_zero_area(&bitmap, 3) == 0);
    ASSERT(bitmap_find_zero_area(&bitmap, TEST_BITS) == 0);
    ASSERT(bitmap_find_zero_area(&bitmap, TEST_BITS + 1) == BITMAP_ERROR);
    ASSERT(bitmap_find_zero_area(&bitmap, 0) == BITMAP_ERROR);

    bitmap_set(&bitmap, 0);
    bitmap_set(&bitmap, 1);

    ASSERT(bitmap_find_zero_area(&bitmap, 2) == 2);

    bitmap_set(&bitmap, 2);
    bitmap_set(&bitmap, 3);

    ASSERT(bitmap_find_zero_area(&bitmap, 2) == 4);

    bitmap_set_range(&bitmap, 4, 4);

    ASSERT(bitmap_find_zero_area(&bitmap, 3) == 8);

    bitmap_fill(&bitmap);

    ASSERT(bitmap_find_zero_area(&bitmap, 1) == bitmap_bits(&bitmap));
}

static void test_bitmap_count_set(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    ASSERT(bitmap_count_set(&bitmap) == 0);

    bitmap_set(&bitmap, 0);
    ASSERT(bitmap_count_set(&bitmap) == 1);

    bitmap_set(&bitmap, 3);
    bitmap_set(&bitmap, 7);
    bitmap_set(&bitmap, 12);

    ASSERT(bitmap_count_set(&bitmap) == 4);

    bitmap_fill(&bitmap);

    ASSERT(bitmap_count_set(&bitmap) == TEST_BITS);
}

static void test_bitmap_count_clear(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    ASSERT(bitmap_count_clear(&bitmap) == TEST_BITS);

    bitmap_set(&bitmap, 0);
    ASSERT(bitmap_count_clear(&bitmap) == TEST_BITS - 1);

    bitmap_set(&bitmap, 5);
    bitmap_set(&bitmap, 12);

    ASSERT(bitmap_count_clear(&bitmap) == TEST_BITS - 3);

    bitmap_fill(&bitmap);

    ASSERT(bitmap_count_clear(&bitmap) == 0);
}

static void test_bitmap_test_range(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    bitmap_set_range(&bitmap, 2, 4);

    ASSERT(bitmap_test_range(&bitmap, 2, 4) != 0);
    ASSERT(bitmap_test_range(&bitmap, 3, 2) != 0);
    ASSERT(bitmap_test_range(&bitmap, 1, 4) == 0);
    ASSERT(bitmap_test_range(&bitmap, 6, 1) == 0);

    ASSERT(bitmap_test_range(&bitmap, 0, 0) != 0);
}

static void test_bitmap_test_range_clear(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);
    bitmap_zero(&bitmap);

    ASSERT(bitmap_test_range_clear(&bitmap, 0, TEST_BITS) != 0);

    bitmap_set_range(&bitmap, 2, 4);

    ASSERT(bitmap_test_range_clear(&bitmap, 0, 2) != 0);
    ASSERT(bitmap_test_range_clear(&bitmap, 6, 7) != 0);
    ASSERT(bitmap_test_range_clear(&bitmap, 1, 3) == 0);
    ASSERT(bitmap_test_range_clear(&bitmap, 2, 4) == 0);
}

static void test_bitmap_empty(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);

    bitmap_zero(&bitmap);
    ASSERT(bitmap_empty(&bitmap) != 0);

    bitmap_set(&bitmap, 0);
    ASSERT(bitmap_empty(&bitmap) == 0);

    bitmap_zero(&bitmap);
    ASSERT(bitmap_empty(&bitmap) != 0);
}

static void test_bitmap_full(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);

    bitmap_zero(&bitmap);
    ASSERT(bitmap_full(&bitmap) == 0);

    bitmap_fill(&bitmap);
    ASSERT(bitmap_full(&bitmap) != 0);

    bitmap_clear(&bitmap, 12);
    ASSERT(bitmap_full(&bitmap) == 0);
}

static void test_bitmap_invalid(void)
{
    uint8_t data[TEST_BYTES];
    struct bitmap bitmap;

    bitmap_init(&bitmap, data, TEST_BITS);

    ASSERT(bitmap_find_first_clear(NULL) == BITMAP_ERROR);
    ASSERT(bitmap_find_first_set(NULL) == BITMAP_ERROR);
    ASSERT(bitmap_find_next_clear(NULL, 0) == BITMAP_ERROR);
    ASSERT(bitmap_find_next_set(NULL, 0) == BITMAP_ERROR);
    ASSERT(bitmap_find_zero_area(NULL, 1) == BITMAP_ERROR);
    ASSERT(bitmap_count_set(NULL) == BITMAP_ERROR);

    bitmap.data = NULL;

    ASSERT(bitmap_find_first_clear(&bitmap) == BITMAP_ERROR);
    ASSERT(bitmap_find_first_set(&bitmap) == BITMAP_ERROR);
    ASSERT(bitmap_find_next_clear(&bitmap, 0) == BITMAP_ERROR);
    ASSERT(bitmap_find_next_set(&bitmap, 0) == BITMAP_ERROR);
    ASSERT(bitmap_find_zero_area(&bitmap, 1) == BITMAP_ERROR);
    ASSERT(bitmap_count_set(&bitmap) == BITMAP_ERROR);
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
    test_bitmap_assign();
    test_bitmap_toggle();
    test_bitmap_test_clear();
    test_bitmap_set_range();
    test_bitmap_clear_range();
    test_bitmap_find_first_clear();
    test_bitmap_find_first_set();
    test_bitmap_find_next_clear();
    test_bitmap_find_next_set();
    test_bitmap_find_zero_area();
    test_bitmap_count_set();
    test_bitmap_count_clear();
    test_bitmap_test_range();
    test_bitmap_test_range_clear();
    test_bitmap_empty();
    test_bitmap_full();
    test_bitmap_boundaries();
    test_bitmap_find_not_found();
    test_bitmap_invalid();

    int num_failures = get_g_assert_failures();

    if (num_failures) {
        printf("test_bitmap: number of failures: %i\n", num_failures);
    }
    else {
        printf("test_bitmap: ok.\n");
    }

    return 0;
}

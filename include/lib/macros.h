#ifndef MACROS_H
#define MACROS_H

#define ARRAY_SIZE(arr) \
    (sizeof(arr) / sizeof((arr)[0]))

#define MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MAX(a, b) \
    ((a) > (b) ? (a) : (b))

#define CLAMP(x, lo, hi) \
    (MIN(MAX((x), (lo)), (hi)))

#define ROUND_UP(x, y) \
    ((((x) + ((y) - 1)) / (y)) * (y))

#define ROUND_DOWN(x, y) \
    (((x) / (y)) * (y))

#define ALIGN_UP(x, a) \
    (((x) + ((a) - 1)) & ~((a) - 1))

#define ALIGN_DOWN(x, a) \
    ((x) & ~((a) - 1))

#endif

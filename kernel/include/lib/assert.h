#ifndef ASSERT_H
#define ASSERT_H

#include <lib/panic.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void assert_failure(const char *expr,
                    const char *file,
                    int line,
                    const char *func);

#ifdef __cplusplus
}
#endif

#ifndef NDEBUG

#define ASSERT(expr)                                      \
    do {                                                  \
        if (!(expr)) {                                    \
            assert_failure(#expr, __FILE__, __LINE__, __func__); \
        }                                                 \
    } while (0)
#else

#define ASSERT(expr) ((void)0)

#endif

#endif /* ASSERT_H */
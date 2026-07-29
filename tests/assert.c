#include <lib/assert.h>
#include <lib/print.h>
#include <stdlib.h>
#include <stdio.h>


int g_assert_failures = 0;

__attribute__((weak))
// __attribute__((noreturn))
void assert_failure(
    const char *expr,
    const char *file,
    int line,
    const char *func)
{
    ++g_assert_failures;

    kprint("\n=== ASSERTION FAILED ===\n");

    if (expr)
        fprintf(stderr, "Expression: %s\n", expr);

    if (file)
        fprintf(stderr, "Location: %s:%d\n", file, line);

    if (func)
        fprintf(stderr, "Function: %s\n", func);

    fprintf(stderr, "========================\n");
}

int get_g_assert_failures(void) {
    if g_assert_failures > 0:
        fprintf(stderr,
                "\n%d assertion(s) failed\n",
                g_assert_failures);
    return g_assert_failures;
}

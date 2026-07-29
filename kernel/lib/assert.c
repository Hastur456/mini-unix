#include <lib/assert.h>
#include <lib/panic.h>


// __attribute__((weak))
__attribute__((noreturn))
void assert_failure(
    const char *expr,
    const char *file,
    int line,
    const char *func
) {
    (void)func;
    panic(expr, file, line, func);

    __builtin_unreachable();
}

#ifndef STDARG_H
#define STDARG_H

/*
 * ISO C <stdarg.h>
 *
 * Variable argument handling.
 */

/*
 * GCC provides the underlying variable argument list type.
 */
#ifndef __GNUC_VA_LIST
#define __GNUC_VA_LIST

typedef __builtin_va_list __gnuc_va_list;

#endif


/*
 * Variable argument list type.
 */
typedef __gnuc_va_list va_list;


/*
 * Variable argument access.
 */

#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_end(ap)         __builtin_va_end(ap)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

#endif /* STDARG_H */
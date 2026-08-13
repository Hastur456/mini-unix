#ifndef STDDEF_H
#define STDDEF_H

/*
 * ISO C <stddef.h>
 *
 * Common definitions.
 */


/*
 * size_t
 *
 * Unsigned integer type returned by sizeof.
 */
typedef __SIZE_TYPE__ size_t;


/*
 * ptrdiff_t
 *
 * Signed integer type used for the result of
 * subtracting two pointers.
 */
typedef __PTRDIFF_TYPE__ ptrdiff_t;


/*
 * wchar_t
 *
 * Integer type capable of representing wide characters.
 */
typedef __WCHAR_TYPE__ wchar_t;


/*
 * NULL
 *
 * Null pointer constant.
 */
#ifndef NULL
#define NULL ((void *)0)
#endif


/*
 * offsetof
 *
 * Offset in bytes of a structure member.
 *
 * GCC/Clang provide __builtin_offsetof().
 */
#define offsetof(type, member) __builtin_offsetof(type, member)


/*
 * max_align_t
 *
 * Type whose alignment requirement is at least as strict
 * as that of every scalar type.
 *
 * C11.
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

typedef struct {
    long long __ll;
    long double __ld;
} max_align_t;

#endif /* C11 */


#endif /* STDDEF_H */
#include <lib/string.h>

/*
 * Memory operations
 */

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    if (d == s || n == 0)
        return dest;

    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }

    return dest;
}

void *memset(void *dest, int c, size_t n) {
    unsigned char *d = dest;
    unsigned char ch = c;

    for (size_t i = 0; i < n; i++) {
        d[i] = ch;
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }

    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const unsigned char *p = s;

    for (size_t i = 0; i < n; i++) {
        if (p[i] == (unsigned char)c) {
            return (void *)(p + i);
        }
    }

    return NULL;
}

/*
 * String copy / concatenation
 */

char *strcpy(char *dest, const char *src) {
    size_t i = 0;

    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';

    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;

    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    while (i < n) {
        dest[i] = '\0';
        i++;
    }

    return dest;
}

/*
 * String examination
 */

size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len] != '\0')
        len++;

    return len;
}

int strcmp(const char *lhs, const char *rhs) {
    size_t i = 0;

    while (lhs[i] == rhs[i] && lhs[i] != '\0') i++;

    return (unsigned char)lhs[i] - (unsigned char)rhs[i];
}

int strncmp(const char *lhs, const char *rhs, size_t n) {
    size_t i = 0;

    while (i < n && lhs[i] == rhs[i] && lhs[i] != '\0') i++;

    if (i == n) return 0;

    return (unsigned char)lhs[i] - (unsigned char)rhs[i];
}

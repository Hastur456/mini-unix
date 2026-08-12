#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <kernel/errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Memory operations
 */

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);

/*
 * String copy / concatenation
 */

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);

// char *strcat(char *dest, const char *src);
// char *strncat(char *dest, const char *src, size_t n);

/*
 * String examination
 */

size_t strlen(const char *str);

int strcmp(const char *lhs, const char *rhs);
int strncmp(const char *lhs, const char *rhs, size_t n);

/*
 * String search
 */

// char *strchr(const char *str, int ch);
// char *strrchr(const char *str, int ch);
// char *strstr(const char *str, const char *substr);

// size_t strspn(const char *str, const char *accept);
// size_t strcspn(const char *str, const char *reject);
// char *strpbrk(const char *str, const char *accept);

/*
 * Tokenization
 */

// char *strtok(char *str, const char *delim);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */
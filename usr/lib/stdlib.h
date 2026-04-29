#ifndef CYLIBC_STDLIB_H
#define CYLIBC_STDLIB_H

#include <stdint.h>

char *getenv(const char *key);
int setenv(const char *key, const char *value, int overwrite);
int unsetenv(const char *key);

int atoi(const char *s);

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

void qsort(void *base, size_t count, size_t size,
           int (*cmp)(const void *, const void *));

#endif  // CYLIBC_STDLIB_H

#ifndef CYLIBC_STRING_H
#define CYLIBC_STRING_H

#include <stdint.h>

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);
size_t strcspn(const char *s, const char *reject);
char *strtok(char *str, const char *delim);

#endif  // CYLIBC_STRING_H

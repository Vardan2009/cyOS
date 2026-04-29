#ifndef CY_STRING_H
#define CY_STRING_H

#include <stdint.h>

void *memset(void *dest, char val, uint32_t count);
void *memcpy(void *dest, const void *src, uint32_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint32_t n);
int memcmp(const void *ptr1, const void *ptr2, uint32_t num);
char *strchr(const char *str, int c);
uint32_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, uint32_t n);
char *strcat(char *dest, const char *src);
char *strsep(char **str, const char *delim);
char *strncat(char *dest, const char *src, uint32_t n);

#endif  // CY_STRING_H
